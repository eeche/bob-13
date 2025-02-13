#ifndef DOT11_H
#define DOT11_H

#include <cstdint>

// Radiotap 헤더 구조체
#pragma pack(push, 1)
struct ieee80211_radiotap_header {
    uint8_t  it_version;
    uint8_t  it_pad;
    uint16_t it_len;
    uint32_t it_present;
};
#pragma pack(pop)

// 802.11 Beacon 프레임 헤더 구조체
#pragma pack(push, 1)
struct ieee80211_header {
    uint8_t  frame_control[2];
    uint8_t  duration[2];
    uint8_t  desaddr[6];       // 목적지 MAC (수신자)
    uint8_t  sourceaddr4[6];   // 송신자 MAC (AP BSSID)
    uint8_t  bssid[6];
    uint8_t  seq_ctrl[2];
    // (간략화를 위해 다른 필드는 생략)
};
#pragma pack(pop)

#pragma pack(push, 1)
struct beacon_frame_fixed {
    uint8_t timestamp[8];
    uint16_t beacon_interval;
    uint16_t cap_info;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct info_element {
    uint8_t id;
    uint8_t length;
    uint8_t data[];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct csa_ie {
    uint8_t tag_number;         // 0x25
    uint8_t tag_length;         // 3
    uint8_t channel_switch_mode;// 0x01 (즉시 전환)
    uint8_t new_channel;        // 새 채널
    uint8_t channel_switch_count; // ex) 3
};
#pragma pack(pop)

#endif // DOT11_H

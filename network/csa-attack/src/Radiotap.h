#ifndef RADIOTAP_H
#define RADIOTAP_H

#include <cstdint>

#pragma pack(push, 1)

// Minimal radiotap header (we only parse the length)
struct ieee80211_radiotap_header {
    uint8_t  it_version;
    uint8_t  it_pad;
    uint16_t it_len;
    uint32_t it_present;
    // more fields may follow depending on it_present
};

#pragma pack(pop)

#endif // RADIOTAP_H

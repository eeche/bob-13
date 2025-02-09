#ifndef DOT11_H
#define DOT11_H

#include <cstdint>

#pragma pack(push, 1)
struct Dot11Hdr {
    uint16_t frameControl;
    uint16_t duration;
    uint8_t  addr1[6];
    uint8_t  addr2[6];
    uint8_t  addr3[6];
    uint16_t seqCtrl;
};
#pragma pack(pop)

// 서브타입(Beacon=0x80)
static const uint16_t FC_SUBTYPE_BEACON = 0x0080;

#endif

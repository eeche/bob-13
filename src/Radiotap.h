#ifndef RADIOTAP_H
#define RADIOTAP_H

#include <cstdint>

// 간단 버전 Radiotap Header (8바이트)
#pragma pack(push, 1)
struct RadioTapHdr {
    uint8_t  revision;
    uint8_t  pad;
    uint16_t length;
    uint32_t presentFlags;
};
#pragma pack(pop)

// Radiotap 관련 함수 원형
// 예: radiotapParseLen(), radiotapCreateHeader(), etc.

// radiotapParseLen: packet에서 RadiotapHdr를 보고 전체 길이를 반환 (유효성 검사 포함)
int radiotapParseLen(const uint8_t* packet, int packetLen);

// radiotapCreateHeader: 8바이트짜리 기본 RadiotapHdr를 만들어, outBuf에 써줌
void radiotapCreateHeader(uint8_t* outBuf, int outLen);

#endif

#include "Radiotap.h"
#include <cstring>
#include <iostream>

// 간단히 "8바이트"만 신뢰하는 버전
int radiotapParseLen(const uint8_t* packet, int packetLen) {
    if (packetLen < 8) {
        return -1; // too short
    }
    // cast
    const RadioTapHdr* rth = reinterpret_cast<const RadioTapHdr*>(packet);
    int len = rth->length; // 리틀엔디안 환경에서 바로 대입 - 실제로는 엔디안 check 필요
    if (len < 8 || len > packetLen) {
        return -1; // invalid
    }
    return len;
}

// 8바이트짜리 기본 Radiotap
void radiotapCreateHeader(uint8_t* outBuf, int outLen) {
    if (outLen < 8) return;
    RadioTapHdr rth;
    memset(&rth, 0, sizeof(rth));
    rth.length = sizeof(RadioTapHdr); // 8
    // presentFlags=0
    // copy
    memcpy(outBuf, &rth, 8);
}

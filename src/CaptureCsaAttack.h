#ifndef CAPTURECSAATTACK_H
#define CAPTURECSAATTACK_H

#include <pcap.h>
#include "MacAddr.h"

// startCaptureCsa: 라이브 캡처 + CSA 변조 + 재전송
//   stationMac=nullptr => broadcast
void startCaptureCsa(pcap_t* handle, const MacAddr& apMac, const MacAddr* stationMac);

#endif

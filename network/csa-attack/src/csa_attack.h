#ifndef CSA_ATTACK_H
#define CSA_ATTACK_H

#include <pcap.h>
#include <cstdint>
#include <cstddef> // size_t

// CSA 공격을 수행하기 위해 필요한 함수들
void send_beacon_with_csa(pcap_t* handle,
                          const uint8_t* orig_packet,
                          size_t packet_len,
                          int ie_offset,
                          uint8_t new_channel);

#endif // CSA_ATTACK_H

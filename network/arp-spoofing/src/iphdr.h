#include <stdint.h>

struct ipv4_header {
	uint8_t ip_hl : 4, ip_v : 4;
	uint8_t tos;
	uint16_t total_packet_len;
	uint16_t identifier;
	uint16_t flags : 3, fragment_offset : 13;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint8_t src_ip[4];
	uint8_t dst_ip[4];
};
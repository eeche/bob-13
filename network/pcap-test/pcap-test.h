// 1. Ethernet Header의 src mac / dst mac
// 2. IP Header의 src ip / dst ip
// 3. TCP Header의 src port / dst port
// 4. Payload(Data)의 hexadecimal value(최대 20바이트까지만)

struct eth_header {
	uint8_t dst_mac[6];
	uint8_t src_mac[6];
    uint16_t type;
};

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

struct tcp_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint8_t offset : 4, reserved : 4;
    uint8_t flags;
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_ptr;
};

struct payload {
    uint8_t data[20];
};
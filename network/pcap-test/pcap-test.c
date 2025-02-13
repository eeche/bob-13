#include <pcap.h>
#include <stdbool.h>
#include <stdio.h>
#include "pcap-test.h"
#include <stdint.h>

void usage() {
	printf("syntax: pcap-test <interface>\n");
	printf("sample: pcap-test wlan0\n");
}

typedef struct {
	char* dev_;
} Param;

Param param = {
	.dev_ = NULL
};

bool parse(Param* param, int argc, char* argv[]) {
	if (argc != 2) {
		usage();
		return false;
	}
	param->dev_ = argv[1];
	return true;
}

void eth_header(const uint8_t* packet) {
	struct eth_header* eth = (struct eth_header*)packet;
	struct ipv4_header* ip = (struct ipv4_header*)(packet + sizeof(struct eth_header));

	if (ip->protocol != 0x06) {
		printf("Not TCP\n");
		return;
	}

	printf("----------------------------\n");
	printf("---ethernet header---\n");
	printf("src mac: ");
	for (int i = 0; i < 6; i++) {
		printf("%02x", eth->src_mac[i]);
		if (i != 5) printf(":");
	}
	printf("\n");
	printf("dst mac: ");
	for (int i = 0; i < 6; i++) {
		printf("%02x", eth->dst_mac[i]);
		if (i != 5) printf(":");
	}
	printf("\n");
	ipv4_header(packet + sizeof(struct eth_header));
}

void ipv4_header(const uint8_t* packet) {
	struct ipv4_header* ip = (struct ipv4_header*)packet;
	printf("---ipv4 header---\n");
	printf("src ip: ");
	for (int i = 0; i < 4; i++) {
		printf("%d", ip->src_ip[i]);
		if (i != 3) printf(".");
	}
	printf("\n");
	printf("dst ip: ");
	for (int i = 0; i < 4; i++) {
		printf("%d", ip->dst_ip[i]);
		if (i != 3) printf(".");
	}
	printf("\n");
	// ip header는 가변적이므로 ip header의 길이를 구해서(ip header의 길이는 ip_hl * 4) tcp header로 넘겨준다.
	tcp_header(packet + sizeof(struct ipv4_header));
}

// tcp header의 크기를 구하기 위해서 caplen에서 ip header의 길이를 빼주면 스위치를 통과하면 tag가 붙어서 이렇게 하면 안됨
// ip header에서 total packet length에서 ip header의 길이를 빼주면 tcp header의 길이가 나온다.
void tcp_header(const uint8_t* packet) {
	struct tcp_header* tcp = (struct tcp_header*)packet;
	printf("---tcp header---\n");
	printf("src port: %d\n", ntohs(tcp->src_port));
	printf("dst port: %d\n", ntohs(tcp->dst_port));
	payload(packet + sizeof(struct tcp_header));
}

void payload(const uint8_t* packet) {
	struct payload* data = (struct payload*)packet;
	printf("---payload---\n");
	for (int i = 0; i < 20; i++) {
		printf("%02x ", data->data[i]);
		if (i == 9) printf("\n");
	}
	printf("\n");
}

int main(int argc, char* argv[]) {
	if (!parse(&param, argc, argv))
		return -1;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* pcap = pcap_open_live(param.dev_, BUFSIZ, 1, 1000, errbuf);
	if (pcap == NULL) {
		fprintf(stderr, "pcap_open_live(%s) return null - %s\n", param.dev_, errbuf);
		return -1;
	}
	
	int cnt = 1;
	while (true) {
		struct pcap_pkthdr* header;
		const uint8_t* packet;
		int res = pcap_next_ex(pcap, &header, &packet);
		if (res == 0) continue;
		if (res == PCAP_ERROR || res == PCAP_ERROR_BREAK) {
			printf("pcap_next_ex return %d(%s)\n", res, pcap_geterr(pcap));
			break;
		}
		eth_header(packet);
	}

	pcap_close(pcap);
}

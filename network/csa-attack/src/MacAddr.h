#ifndef MACADDR_H
#define MACADDR_H

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

inline bool parseMac(const std::string& macStr, uint8_t mac[6]) {
    std::istringstream iss(macStr);
    int val;
    char sep;

    for (int i = 0; i < 6; i++) {
        if (!(iss >> std::hex >> val)) {
            return false;
        }
        mac[i] = static_cast<uint8_t>(val);
        if (i < 5) {
            if (!(iss >> sep)) {
                return false;
            }
        }
    }
    return true;
}

#endif // MACADDR_H

#ifndef MACADDR_H
#define MACADDR_H

#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>

class MacAddr {
public:
    static const size_t LENGTH = 6;

private:
    uint8_t addr[LENGTH] = {0,};

public:
    MacAddr() {}
    MacAddr(const uint8_t* target) { memcpy(addr, target, LENGTH); }

    const uint8_t* data() const { return addr; }

    operator std::string() const {
        char buf[18];
        sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        return std::string(buf);
    }

    friend std::ostream& operator<<(std::ostream& os, const MacAddr& m) {
        os << (std::string)m;
        return os;
    }
};

#endif

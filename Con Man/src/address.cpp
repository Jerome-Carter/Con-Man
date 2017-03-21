//
//  address.cpp
//  Con Man
//
//  Created by James Carter on 3/17/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "address.h"

namespace Con_Man {
    namespace Socket {
        Address::Address(const struct sockaddr_in& address) : m_Address(address) {}
        Address::Address(const std::string& ip, const unsigned short& port) {
            struct sockaddr_in address;
            memset((void*)&address, 0, sizeof(address));
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            m_Address = address;
        }
    }
}

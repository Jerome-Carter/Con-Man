//
//  recepient.h
//  Con Man
//
//  Created by James Carter on 3/16/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include "../easylogging/easylogging++.h"

#include <string>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Con_Man {
    namespace Socket {
        struct Address {
        private:
            struct sockaddr_in m_Address;
            std::string m_IP;
            unsigned short m_Port;
        public:
            Address(const struct sockaddr_in& address);
            Address(const std::string& ip, const unsigned short& port);
            inline sockaddr_in getAddress() const { return m_Address; }
            inline std::string getIp() const { return m_IP; }
            inline unsigned short getPort() const { return m_Port; }
            inline void setPort(const unsigned short& port) { m_Address.sin_port = htons(port); }
            inline std::string getAddressInfo() const { return getIp() + ":" + std::to_string(getPort()); }
        };
    }
}

//
//  udp_socket.h
//  Con Man
//
//  Created by James Carter on 3/18/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include "socket.h"
#include "address.h"

#include "easylogging++.h"

#include <thread>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Con_Man {
    namespace Adapters {
        namespace Socket {
            class UDP : Interfaces::Socket {
            private:
                int m_FD;
                bool m_Open = false;
                bool m_Listening = false;
                ::Con_Man::Socket::Address* m_Address;
            public:
                UDP(::Con_Man::Socket::Address& address);
                UDP(const std::string& ip, const unsigned short& port);
                ~UDP();
                bool open();
                
                bool open(const std::string& ip, const unsigned short& port) override;
                void disable(const unsigned int& level) override;
                void close() override;
                void send(const char*& data) const override;
                void receive(const std::function<void(char*)>& call) const override;
                void listen(const std::function<void(char*)> &call) override;
                inline void ignore() override { m_Listening = false; };
                inline bool isOpen() const override { return m_Open; };
                inline bool isListening() const override { return m_Listening; };
                inline std::string getIp() const override { return m_Address->getIp(); };
                inline unsigned short getPort() const override { return m_Address->getPort(); };
            };
        }
    }
}

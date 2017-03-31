//
//  tcp.h
//  Con Man
//
//  Created by James Carter on 3/25/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include "../socket.h"
#include "../../address.h"

#include "../../../easylogging/easylogging++.h"

#include <thread>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CONNECTIONS 20

namespace Con_Man {
    namespace Adapters {
        namespace Socket {
            class TCP : public Interfaces::Socket {
            private:
                int m_FD;
                std::map<int, ::Con_Man::Socket::Address*> m_ConnectionAddressList;
                bool m_Open = false;
                bool m_Listening = false;
                std::thread* m_Listener;
                ::Con_Man::Socket::Address* m_Address;
                std::vector<char*> m_ReceivedMessages;
            public:
                TCP(::Con_Man::Socket::Address& address);
                TCP(const std::string& ip, const unsigned short& port);
                ~TCP();
                bool create();
                void listen(const std::function<void(::Con_Man::Socket::Address)> &call);
                void deafen();
                void connect(const ::Con_Man::Socket::Address& address);

                void disable(const unsigned int& level) override;
                void close() override;
                void send(const char*& data) const override {};
                void receive(const std::function<void(char*)>& call) override {};
                void listen(const std::function<void(char*)> &call) override {};
                void ignore() override {};
                inline bool isOpen() const override { return m_Open; }
                inline bool isListening() const override { return m_Listening; }
                inline std::string getIp() const override { return m_Address->getIp(); }
                inline unsigned short getPort() const override { return m_Address->getPort(); }

            private:
                bool open(const std::string& ip, const unsigned short& port) override;
            };
        }
    }
}

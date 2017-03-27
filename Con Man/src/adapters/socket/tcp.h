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

namespace Con_Man {
    namespace Adapters {
        namespace Socket {
            class TCP : public Interfaces::Socket {
            private:
                int m_FD;
                std::map<int, ::Con_Man::Socket::Address> m_ConnectionAddressList;
                bool m_Open = false;
                bool m_Listening = false;
                ::Con_Man::Socket::Address* m_Address;
                std::vector<char*> m_ReceivedMessages;
            public:
                TCP(::Con_Man::Socket::Address& address);
                TCP(const std::string& ip, const unsigned short& port);
                ~TCP();
                bool create();

                void disable(const unsigned int& level) override;
                void close() override;
                inline void send(const char*& data) const override {};
                inline void receive(const std::function<void(char*)>& call) override {};
                inline void listen(const std::function<void(char*)> &call) override {};
                inline void ignore() override { m_Listening = false; }
                inline bool isOpen() const override { return m_Open; }
                inline bool isListening() const override { return m_Listening; }
                inline std::string getIp() const override { return m_Address->getIp(); }
                inline unsigned short getPort() const override { return m_Address->getPort(); }
                //inline void getInfo() const override { system(std::string(std::string("lsof -i :") + std::to_string(getPort())).c_str()); }

            private:
                bool open(const std::string& ip, const unsigned short& port) override;
            };
        }
    }
}

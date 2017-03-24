//
//  udp_socket.h
//  Con Man
//
//  Created by James Carter on 3/18/17.
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
            class UDP : Interfaces::Socket {
            private:
                int m_FD;
                bool m_Open = false;
                bool m_Listening = false;
                ::Con_Man::Socket::Address* m_Address;
                std::vector<char*> m_ReceivedMessages;
                std::vector<::Con_Man::Socket::Address> m_Recipients;
            public:
                UDP(::Con_Man::Socket::Address& address);
                UDP(const std::string& ip, const unsigned short& port);
                ~UDP();
                bool open();

                inline char* getMessage(const unsigned int& ID) const { return m_ReceivedMessages[ID]; }

                inline void addRecipient(const ::Con_Man::Socket::Address& address) { m_Recipients.push_back(address); }
                inline void addRecipient(const std::string& ip, const unsigned short& port) {m_Recipients.push_back(*new ::Con_Man::Socket::Address(ip, port)); }
                inline ::Con_Man::Socket::Address getRecipient(const unsigned int& ID) const { return m_Recipients.at(ID); }
                ::Con_Man::Socket::Address getRecipient(const std::string& ip, const unsigned short& port) const;
                bool recipientExists(const std::string& ip, const unsigned short& port) const;

                inline void receive_from(const unsigned int& ID, const std::function<void(char*)>& call) { receive_from(getRecipient(ID), call); }
                void receive_from(const ::Con_Man::Socket::Address& sender, const std::function<void(char*)>& call);
                void receive_anon(const std::function<void(char*)>& call);
                ::Con_Man::Socket::Address receive_unknown(const std::function<void(char*)>& call);

                inline void send(const unsigned int& ID, const char*& data) const { send(m_Recipients.at(ID), data); }
                void send(const ::Con_Man::Socket::Address& recipient, const char*& data) const;

                inline void listen_to(const unsigned int& ID, const std::function<void(char*)> &call) { listen_to(m_Recipients.at(ID), call); }
                void listen_to(const ::Con_Man::Socket::Address& sender, const std::function<void(char*)> &call);
                
                bool open(const std::string& ip, const unsigned short& port) override;
                void disable(const unsigned int& level) override;
                void close() override;
                void send(const char*& data) const override;
                inline void receive(const std::function<void(char*)>& call) override { receive_unknown(call); }
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

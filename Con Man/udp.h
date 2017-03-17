//
//  udp.h
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include <vector>
#include <thread>
#include <iostream>
#include <unistd.h>

#include "socket.h"
#include "easylogging++.h"

#define TERMINATION_CHAR "|0"

// TODO: Add apropriate getters and setters
// TODO: get anon sender info
// TODO: implement multiple recipients
// TODO: move recipient to separate struct

namespace Con_Man {
    class UDP : public Socket {
    private:
        bool m_Running = false;
        bool m_Listening = false;
        int m_FileDescriptor;
        struct sockaddr_in m_Address;
        struct sockaddr_in m_Recipient;
        std::vector<char*> m_ReceivedMessages;
    public:
        UDP(const std::string& ip, const unsigned short& port);
        bool open() override;
        void close() override;
        void disable(const int& level) override;
        void send(const char*& data) const override;
        void receive(const std::function<void(char*)>& call) override;
        void listen(const std::function<void(char*)> &call);
        inline void ignore() { m_Listening = false; }
        void setRecepient(const std::string& ip, const unsigned short& port);
    public:
        inline bool getState() const { return m_Running; }
        inline std::string getMessage(unsigned int id) const { return m_ReceivedMessages.at(id); }
        inline std::string getAddress() const { return std::to_string(*inet_ntoa(m_Address.sin_addr)) + ":" + std::to_string(ntohs(m_Address.sin_port)); }
        inline std::string getRecepientAddress() const { return std::to_string(*inet_ntoa(m_Recipient.sin_addr)) + ":" + std::to_string(ntohs(m_Recipient.sin_port)); }
    };
}

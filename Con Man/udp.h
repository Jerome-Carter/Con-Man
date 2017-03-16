//
//  udp.h
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include <vector>
#include <iostream>
#include <unistd.h>

#include "socket.h"
#include "easylogging++.h"

namespace Con_Man {
    class UDP : public Socket {
    private:
        bool m_Status;
        int m_FileDescriptor;
        struct sockaddr_in m_Address;
        struct sockaddr_in m_Recipient;
        std::vector<char*> m_ReceivedMessages;
    public:
        UDP(std::string ip, unsigned short port);
        bool open() override;
        void close() override;
        void send(const char* data) const override;
        void receive(const std::function<void(char*)>& call) const override;
    };
}

//
//  udp.cpp
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "udp.h"

namespace Con_Man {
    UDP::UDP(std::string ip, unsigned short port) {
        memset((void*)&m_Address, 0, sizeof(m_Address));
        m_Address.sin_family = AF_INET;
        m_Address.sin_addr.s_addr = inet_addr(ip.c_str());
        m_Address.sin_port = htons(port);
    }
    bool UDP::open() {
        if ((m_FileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
            LOG(ERROR) << "Socket creation failed: " << strerror(errno);
            return false;
        }
        LOG(DEBUG) << "Socket created. ( File Descriptor = " << m_FileDescriptor << " )";
        if ((bind(m_FileDescriptor, (struct sockaddr*)&m_Address, sizeof(m_Address))) < 0) {
            LOG(ERROR) << "Failed to bind socket: " << strerror(errno);
            return false;
        }
        LOG(DEBUG) << "Socket bound";
        unsigned int addr_length = sizeof(m_Address);
        if (getsockname(m_FileDescriptor, (struct sockaddr *)&m_Address, &addr_length) < 0) {
            LOG(ERROR) << "Failed to bind socket: " << strerror(errno);
            return false;
        }
        LOG(DEBUG) << "Socket now open on " << inet_ntoa(m_Address.sin_addr) << ":" << ntohs(m_Address.sin_port);
        return true;
    }
    void UDP::close() {
        if (m_Status) {
            m_Status = false;
            if (::close(m_FileDescriptor) < 0)
                LOG(ERROR) << "Failed to close socket!";
            else
                LOG(INFO) << "Socket closed";
        }
    }
    void UDP::send(const char* data) const {

    }
    void UDP::receive(const std::function<void (char *)> &call) const {
        
    }
}

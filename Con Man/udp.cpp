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
        LOG(DEBUG) << "Socket now open on " << hostAndPort();
        m_Running = true;
        return true;
    }
    void UDP::close() {
        if (m_Running) {
            m_Running = false;
            if (m_Listening) m_Listening = false;
            if (::close(m_FileDescriptor) < 0)
                LOG(ERROR) << "Failed to close socket!";
            else
                LOG(DEBUG) << "Socket at " << hostAndPort() << " closed";
        }
    }
    void UDP::disable(const int& level) {
        if (level > -1 && level < 3) {
            shutdown(m_FileDescriptor, level);
            if (level == 0)
                LOG(DEBUG) << "Socket at " << hostAndPort() << " may no longer receive data";
            else if (level == 1)
                LOG(DEBUG) << "Socket at " << hostAndPort() << " may no longer send data";
            else if (level == 2)
                LOG(DEBUG) << "Socket at " << hostAndPort() << " may no longer send or receive data";
        } else LOG(WARNING) << "";
    }
    void UDP::send(const char*& data) const {
        if (m_Running) {
            std::thread tSend([this, data] () {
                char* msg = (char*)std::string(std::string(data) + TERMINATION_CHAR).c_str();
                long bytes_sent;
                unsigned long len = std::strlen(msg);
                if ((bytes_sent = sendto(m_FileDescriptor, msg, len, 0, (struct sockaddr*)&m_Recipient, sizeof(struct sockaddr))) < 0) {
                    if (m_Running) {
                        LOG(ERROR) << "Failed to send data: " << strerror(errno);
                    }
                }
            });
            tSend.detach();
        }
    }
    void UDP::receive(const std::function<void(char*)> &call) {
        if (m_Running) {
            char data[1024];
            long bytes_recved;
            socklen_t addr_len = sizeof(m_Recipient);
            if ((bytes_recved = recvfrom(m_FileDescriptor, data, sizeof(data), 0, (struct sockaddr*)&m_Recipient, &addr_len)) < 0) {
                if (m_Running) {
                    LOG(ERROR) << "Failed to receive data: " << strerror(errno);
                }
            }
            m_ReceivedMessages.push_back(data);
            call(data);
            bzero(data, 1024);
        }
    }
    void UDP::listen(const std::function<void(char*)>& call) {
        if (m_Running && !m_Listening) {
            m_Listening = true;
            LOG(INFO) << "Now listening at " << hostAndPort();
            std::thread tListen([this, &call](){
                while (m_Listening) {
                    receive(call);
                }
            });
            tListen.detach();
        } else {
            LOG(ERROR) << "Failed to initialize socket listener at " << hostAndPort();
            LOG(INFO) << "Ensure that the socket is running and not already listening! ( R = " << m_Running << "; L = " << m_Listening << " )";
        }
    }
    void UDP::ignore() {
        m_Listening = false;
    }
    std::string UDP::hostAndPort() const {
        return std::to_string(*inet_ntoa(m_Address.sin_addr)) + ":" + std::to_string(ntohs(m_Address.sin_port));
    }
}

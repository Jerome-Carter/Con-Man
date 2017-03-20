//
//  udp_socket.cpp
//  Con Man
//
//  Created by James Carter on 3/18/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "udp_socket.h"

namespace Con_Man {
    namespace Adapters {
        namespace Socket {
            UDP::UDP(const std::string& ip, const unsigned short& port) {
                m_Address = new ::Con_Man::Socket::Address(ip, port);
            }
            UDP::~UDP() {
                delete m_Address;
            }
            bool UDP::open() {
                return open(m_Address->getIp(), m_Address->getPort());
            }
            bool UDP::open(const std::string& ip, const unsigned short& port) {
                struct sockaddr_in address = m_Address->getAddress();
                if ((m_FD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
                    LOG(ERROR) << "Socket creation failed: " << strerror(errno);
                    return false;
                }
                LOG(DEBUG) << "Socket created. ( File Descriptor = " << m_FD << " )";
                if ((bind(m_FD, (struct sockaddr*)&address, sizeof(address))) < 0) {
                    LOG(ERROR) << "Failed to bind socket: " << strerror(errno);
                    return false;
                }
                LOG(DEBUG) << "Socket bound";
                unsigned int addr_length = sizeof(address);
                if (getsockname(m_FD, (struct sockaddr *)&address, &addr_length) < 0) {
                    LOG(ERROR) << "Failed to bind socket: " << strerror(errno);
                    return false;
                }
                m_Address->setPort(ntohs(address.sin_port));
                LOG(INFO) << ip << ":" << port;
                LOG(INFO) << m_Address->getIp() << ":" << m_Address->getPort();
                LOG(INFO) << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port);
                m_Open = true;
                return true;
            }
            void UDP::close() {
                if (m_Open) {
                    m_Open = false;
                    if (m_Listening) m_Listening = false;
                    if (::close(m_FD) < 0)
                        LOG(ERROR) << "Failed to close socket!";
                    else
                        LOG(DEBUG) << "Socket at " << getIp() << ":" << getPort() << " closed";
                }
            }
            void UDP::disable(const unsigned int& level) {
                if (level > -1 && level < 3) {
                    if ((level == 1 || level == 2) && m_Listening) m_Listening = false;
                    shutdown(m_FD, level);
                    if (level == 0)
                        LOG(DEBUG) << "Socket at " << getIp() << ":" << getPort() << " may no longer receive data";
                    else if (level == 1)
                        LOG(DEBUG) << "Socket at " << getIp() << ":" << getPort() << " may no longer send data";
                    else if (level == 2)
                        LOG(DEBUG) << "Socket at " << getIp() << ":" << getPort() << " may no longer send or receive data";
                } else LOG(ERROR) << "Socket at " << getIp() << ":" << getPort() << " couldn't be disabled (invalid level provided)";
            }
// TODO: Implement recipient management
            void UDP::send(const char*& data) const {

            }
            void UDP::receive(const std::function<void(char*)>& call) const {

            }
            void UDP::listen(const std::function<void(char*)> &call) {
                if (m_Open && !m_Listening) {
                    m_Listening = true;
                    LOG(DEBUG) << "Now listening at " << getIp() << ":" << getPort();
                    std::thread tListen([this, &call](){
                        while (m_Listening) {
                            receive(call);
                        }
                    });
                    tListen.detach();
                } else {
                    LOG(ERROR) << "Failed to initialize socket listener at " << getIp() << ":" << getPort();
                    LOG(WARNING) << "Ensure that the socket is running and not already listening! ( R = " << m_Open << "; L = " << m_Listening << " )";
                }
            }
        }
    }
}

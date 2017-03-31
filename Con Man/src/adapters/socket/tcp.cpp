//
//  tcp.cpp
//  Con Man
//
//  Created by James Carter on 3/26/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "tcp.h"

namespace Con_Man {
    namespace Adapters {
        namespace Socket {
            TCP::TCP(::Con_Man::Socket::Address& address) {
                m_Address = &address;
            }
            TCP::TCP(const std::string& ip, const unsigned short& port) {
                m_Address = new ::Con_Man::Socket::Address(ip, port);
            }
            TCP::~TCP() {
                close();
                delete m_Address;
                delete m_Listener;
            }
            bool TCP::create() {
                return open(m_Address->getIp(), m_Address->getPort());
            }
            bool TCP::open(const std::string &ip, const unsigned short &port) {
                struct sockaddr_in address = m_Address->getAddress();
                if ((m_FD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                    LOG(ERROR) << "Socket creation failed: " << strerror(errno);
                    return false;
                }
                if ((bind(m_FD, (struct sockaddr*)&address, sizeof(address))) < 0) {
                    LOG(ERROR) << "Failed to bind socket: " << strerror(errno);
                    return false;
                }
                unsigned int addr_length = sizeof(address);
                if (getsockname(m_FD, (struct sockaddr *)&address, &addr_length) < 0) {
                    LOG(ERROR) << "Failed to get socket name: " << strerror(errno);
                    return false;
                }
                if (port == 0) m_Address->setPort(ntohs(address.sin_port));
                LOG(INFO) << "Socket created at " << m_Address->getIp() << ":" << m_Address->getPort();
                m_Open = true;
                return true;
            }
            void TCP::close() {
                if (m_Open) {
                    m_Open = false;
                    if (m_Listening) deafen();
                    if (::close(m_FD) < 0) LOG(ERROR) << "Failed to close socket: " << strerror(errno);
                }
            }
            void TCP::disable(const unsigned int& level) {
                if (level > -1 && level < 3) {
                    if ((level == 0 || level == 2) && m_Listening) m_Listening = false;
                    shutdown(m_FD, level);
                } else LOG(ERROR) << "Socket at " << getIp() << ":" << getPort() << " couldn't be disabled (invalid level provided)";
            }
            void TCP::connect(const ::Con_Man::Socket::Address& address) {
                struct sockaddr_in addr = address.getAddress();
                if (::connect(m_FD, (struct sockaddr*)&addr, sizeof(addr)) < 0) LOG(ERROR) << getIp() << ":" << getPort() << "->connect(" << address.getAddressInfo() << ") failed: " << strerror(errno);
            }
            void TCP::listen(const std::function<void(::Con_Man::Socket::Address)> &call) {
                if (m_Open && !m_Listening) {
                    m_Listening = true;
                    if (::listen(m_FD, 10) < 0) LOG(ERROR) << "Failed to listen at " << getIp() << ":" << getPort() << " : " << strerror(errno);
                    LOG(INFO) << "Now listening at " << getIp() << ":" << getPort();
                    m_Listener = new std::thread([this, &call](){
                        while (m_Listening) {
                            int connector_fd;
                            struct sockaddr_in connector_address;
                            socklen_t address_len = sizeof(connector_address);
                            // TODO: Deal with connection limit and call param
                            if ((connector_fd = accept(m_FD, (struct sockaddr*)&connector_address, &address_len)) < 0)
                                LOG(ERROR) << "Socket at " << getIp() << ":" << getPort() << " failed to accept connection: " << strerror(errno);
                            else {
                                ::Con_Man::Socket::Address *address = new ::Con_Man::Socket::Address(connector_address);
                                LOG(INFO) << "Listener received a connection request from " << address->getIp() << ":" << address->getPort();
                                call(*address);
                                m_ConnectionAddressList[connector_fd] = address;
//                                for (std::map<int, ::Con_Man::Socket::Address*>::iterator it = m_ConnectionAddressList.begin(); it != m_ConnectionAddressList.end(); ++it)
//                                    LOG(INFO) << it->second->getAddressInfo();
                            }
                        }
                    });
                } else{
                    LOG(ERROR) << "Failed to initialize socket listener at " << getIp() << ":" << getPort();
                    LOG(WARNING) << "Ensure that the socket is running and not already listening! ( STATUS: " << (m_Open ? "Open" : "Closed") << "; " << (m_Listening ? "Listening" : "Not Listening") << " )";
                }
            }
            void TCP::deafen() {
                if (m_Listening) {
                    m_Listening = false;
                    m_Listener->join();
                }
            }
        }
    }
}

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
                return true;
            }
            void TCP::close() {
                if (m_Open) {
                    m_Open = false;
                    if (m_Listening) m_Listening = false;
                    if (::close(m_FD) < 0) LOG(ERROR) << "Failed to close socket: " << strerror(errno);
                }
            }
            void TCP::disable(const unsigned int& level) {
                if (level > -1 && level < 3) {
                    if ((level == 0 || level == 2) && m_Listening) m_Listening = false;
                    shutdown(m_FD, level);
                } else LOG(ERROR) << "Socket at " << getIp() << ":" << getPort() << " couldn't be disabled (invalid level provided)";
            }
//            void TCP::listen(const std::function<void(char*)> &call) {
//                if (!m_Open) {
//                    m_Open = true;
//                    ::listen(m_FD, 10);
//                    struct sockaddr_in address;
//                    socklen_t addr_len = sizeof(address);
//                    // TODO: Deal with connection limit and call param
//                    m_ConnectionAddressList[accept(m_FD, (struct sockaddr*)&address, &addr_len)] = *new ::Con_Man::Socket::Address(address);
//                }
//            };
        }
    }
}

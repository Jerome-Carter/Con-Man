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
            UDP::UDP(::Con_Man::Socket::Address& address) {
                m_Address = &address;
            }
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
                LOG(INFO) << "Socket now open at " << m_Address->getIp() << ":" << m_Address->getPort();
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
            void UDP::send(const char*& data) const {
                if (m_Open) {
                    for (unsigned int ID = 0; ID < m_Recipients.size(); ID++) {
                        send(ID, data);
                    }
                }
            }
            void UDP::send(const ::Con_Man::Socket::Address& recipient, const char*& data) const {
                if (m_Open) {
                    std::thread tSend([this, recipient, data] () {
                        char* msg = (char*)data;
                        long bytes_sent;
                        unsigned long len = std::strlen(msg);
                        sockaddr_in to = recipient.getAddress();
                        if ((bytes_sent = sendto(m_FD, msg, len, 0, (struct sockaddr*)&to, sizeof(struct sockaddr))) < 0) {
                            if (m_Open) {
                                perror("Failed to send data");
                            }
                        }
                    });
                    tSend.detach();
                }
            }
            void UDP::receive_from(const ::Con_Man::Socket::Address& sender, const std::function<void(char*)>& call) {
                if (m_Open) {
                    char data[1024];
                    long bytes_recved;
                    sockaddr_in address = sender.getAddress();
                    socklen_t addr_len = sizeof(address);
                    if ((bytes_recved = recvfrom(m_FD, data, sizeof(data), 0, (struct sockaddr*)&address, &addr_len)) < 0) {
                        if (m_Open) {
                            perror("Failed to receive data");
                        }
                    }
                    m_ReceivedMessages.push_back(data);
                    call(data);
                    bzero(data, 1024);
                }
            }
            void UDP::receive_anon(const std::function<void(char*)>& call) {
                if (m_Open) {
                    char data[1024];
                    ssize_t bytes_recved;
                    sockaddr_in from;
                    socklen_t addr_len = sizeof(from);
                    if ((bytes_recved = recvfrom(m_FD, data, sizeof(data), 0, (struct sockaddr*)&from, &addr_len)) < 0) {
                        if (m_Open) {
                            perror("Failed to receive data");
                        }
                    }
                    m_ReceivedMessages.push_back(data);
                    call(data);
                    bzero(data, 1024);
                }
            }
            ::Con_Man::Socket::Address UDP::receive_unknown(const std::function<void(char*)>& call) {
                if (m_Open) {
                    char data[1024];
                    ssize_t bytes_recved;
                    sockaddr_in from;
                    socklen_t addr_len = sizeof(from);
                    if ((bytes_recved = recvfrom(m_FD, data, sizeof(data), 0, (struct sockaddr*)&from, &addr_len)) < 0) {
                        if (m_Open) {
                            perror("Failed to receive data");
                        }
                    }
                    m_ReceivedMessages.push_back(data);
                    call(data);
                    bzero(data, 1024);
                    // if recip exists log and ret
                    ::Con_Man::Socket::Address address(from);
                    LOG(DEBUG) << "Data received from " << address.getAddressInfo();
                    if (recipientExists(address.getIp(), address.getPort())) {
                        return getRecipient(address.getIp(), address.getPort());
                    }
                    addRecipient(address);
                    return address;
                }
                LOG(ERROR) << "The socket at " << getIp() << ":" << getPort() << " isn't open!!!";
                return *new ::Con_Man::Socket::Address(NULL, NULL);
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
                    LOG(WARNING) << "Ensure that the socket is running and not already listening! ( STATUS: " << (m_Open ? "Open" : "Closed") << "; " << (m_Listening ? "Listening" : "Not Listening") << " )";
                }
            }
            void UDP::listen_to(const ::Con_Man::Socket::Address& sender, const std::function<void(char*)> &call) {
                if (m_Open && !m_Listening) {
                    m_Listening = true;
                    LOG(DEBUG) << "Now listening at " << getIp() << ":" << getPort();
                    std::thread tListen([this, sender, &call](){
                        while (m_Listening) {
                            receive_from(sender, call);
                        }
                    });
                    tListen.detach();
                } else{
                    LOG(ERROR) << "Failed to initialize socket listener at " << getIp() << ":" << getPort();
                    LOG(WARNING) << "Ensure that the socket is running and not already listening! ( STATUS: " << (m_Open ? "Open" : "Closed") << "; " << (m_Listening ? "Listening" : "Not Listening") << " )";
                }
            }
            ::Con_Man::Socket::Address UDP::getRecipient(const std::string& ip, const unsigned short& port) const {
                for (unsigned int ID = 0; ID < m_Recipients.size(); ID++) {
                    if ((m_Recipients[ID].getIp() == ip) && (m_Recipients[ID].getPort() == port)) {
                        return m_Recipients[ID];
                    }
                }
                LOG(ERROR) << "No recipients matching address " << ip << ":" << port;
                return *new ::Con_Man::Socket::Address(NULL, NULL);
            }
            bool UDP::recipientExists(const std::string& ip, const unsigned short& port) const {
                for (unsigned int ID = 0; ID < m_Recipients.size(); ID++) {
                    if ((m_Recipients[ID].getIp() == ip) && (m_Recipients[ID].getPort() == port)) {
                        return true;
                    }
                }
                LOG(DEBUG) << "No recipients matching address " << ip << ":" << port;
                return false;
            }
        }
    }
}

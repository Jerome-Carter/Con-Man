//
//  socket_adapter.h
//  Con Man
//
//  Created by James Carter on 3/17/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include <iostream>

namespace Con_Man {
    namespace Adapters {
        namespace Interfaces {
            class Socket {
            public:
                virtual bool open(const std::string& ip, const unsigned short& port) = 0;
                virtual void disable(const unsigned int& level) = 0;
                virtual void close() = 0;
                virtual void send(const char*& data) const = 0;
                virtual void receive(const std::function<void(char*)>& call) const = 0;
                virtual void listen(const std::function<void(char*)> &call) = 0;
                virtual void ignore() = 0;
                virtual std::string getIp() const = 0;
                virtual unsigned short getPort() const = 0;
            };
        }
    }
}

//
//  socket.h
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#pragma once

#include <functional>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Con_Man {
    class Socket {
    public:
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual void disable(const int& level) = 0;
        virtual void send(const char*& data) const = 0;
        virtual void receive(const std::function<void(char*)>& call) = 0;
    };
}

//
//  main.cpp
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "address.h"
#include "udp_socket.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace Con_Man;

int main(int argc, const char * argv[]) {
    LOG(INFO) << "Con Man started!";
    Adapters::Socket::UDP udp(*new Socket::Address("0", 0));
    udp.open();
    std::cin.get();
    udp.close();
    return 0;
}

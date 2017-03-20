//
//  main.cpp
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "udp_socket.h"
#include "address.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

// using namespace Con_Man;

int main(int argc, const char * argv[]) {
    LOG(INFO) << "Con Man started!";
    Con_Man::Adapters::Socket::UDP udp("0", 1722);
    udp.open();
    std::cin.get();
    udp.close();
    return 0;
}

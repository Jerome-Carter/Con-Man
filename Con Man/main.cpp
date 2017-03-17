//
//  main.cpp
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "udp.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace Con_Man;

int main(int argc, const char * argv[]) {
    LOG(INFO) << "Con Man started!";
    Con_Man::UDP udp_socket("0", 0);
    udp_socket.open();
    udp_socket.listen([](char* data){
        LOG(INFO) << "Data received: " << data;
    });
    std::cin.get();
    udp_socket.close();
    return 0;
}

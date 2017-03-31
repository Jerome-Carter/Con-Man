//
//  main.cpp
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "src/address.h"
#include "src/adapters/socket/tcp.h"
#include "src/adapters/socket/udp.h"

#include "easylogging/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace Con_Man;

void getSockets() {
    system(std::string("lsof -Pan -p " + std::to_string(getpid()) + " -i").c_str());
}

int main(int argc, const char * argv[]) {
    LOG(INFO) << "Con Man started!";
    Adapters::Socket::TCP tcp(*new Socket::Address("0", 0));
    tcp.create();
    std::string p;
    std::cin >> p;
    if (p == "l") {
        tcp.listen([](::Con_Man::Socket::Address client){
            getSockets();
        });
    } else {
        Socket::Address address("0", std::stoi(p));
        tcp.connect(address);
    }
    getSockets();
    std::cin.get();
    return 0;
}

//
//  main.cpp
//  Con Man
//
//  Created by James Carter on 3/15/17.
//  Copyright © 2017 James Carter. All rights reserved.
//

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, const char * argv[]) {
    LOG(INFO) << "Con Man started!";
    return 0;
}

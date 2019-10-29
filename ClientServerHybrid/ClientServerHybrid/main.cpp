//
//  main.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkApp.hpp"

#include <iostream>

int main(int argc, const char * argv[]) {
    
    NetworkApp app("127.0.0.1", 3000, 3030);
    
    std::cout << "Hello, World!\n";
    return 0;
}

//
//  AsyncTCPServer.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPServer.hpp"

AsyncTCPServer::AsyncTCPServer(unsigned short port_num) : NetworkInterface("0.0.0.0", port_num) {
    
}

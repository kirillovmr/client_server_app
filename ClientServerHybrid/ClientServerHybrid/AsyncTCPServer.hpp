//
//  AsyncTCPServer.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include "NetworkInterface.hpp"

class AsyncTCPServer : public NetworkInterface {
public:
    AsyncTCPServer(unsigned short port_num);
};

#endif /* AsyncTCPServer_hpp */

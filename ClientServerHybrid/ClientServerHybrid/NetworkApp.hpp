//
//  NetworkApp.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef NetworkApp_hpp
#define NetworkApp_hpp

#include "NetworkInterface.hpp"
#include "AsyncTCPClient.hpp"
#include "AsyncTCPServer.hpp"

#include <string>
#include <memory>
#include <boost/asio.hpp>

typedef unsigned int uint;

class NetworkApp {
private:
    AsyncTCPClient m_client;
    AsyncTCPServer m_server;
    NetworkInterface *m_workingInstance;

public:
    NetworkApp(std::string dClIp, uint dClPort, uint dSePort);
    
    void write(std::string &line);
};

#endif /* NetworkApp_hpp */

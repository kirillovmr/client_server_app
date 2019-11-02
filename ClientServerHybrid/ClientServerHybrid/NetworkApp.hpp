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
public:
    typedef enum {ClientInstance, ServerInstance} instanceType;
    
private:
    AsyncTCPClient m_client;
    AsyncTCPServer m_server;
    NetworkInterface *m_workingInstance;
    NetworkApp::instanceType m_instanceType;

public:
    NetworkApp(std::string clientIP, uint clientPort, uint serverPort);
    
    void start();
    void write(std::string &line);
    
    void setReadCallback(void (*handler)(std::string res));
    
    void switchTo(NetworkApp::instanceType type);
    void disconnect() {
//        m_workingInstance->m_socket.cancel();
//        m_workingInstance->disconnect();
    }
    
//    virtual ~NetworkApp() = default;
};

#endif /* NetworkApp_hpp */

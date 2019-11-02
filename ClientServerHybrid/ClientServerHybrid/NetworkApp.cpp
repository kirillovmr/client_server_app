//
//  NetworkApp.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkApp.hpp"

NetworkApp::NetworkApp(std::string clientIP, uint clientPort, uint serverPort) : m_client(clientIP, clientPort), m_server(serverPort) {
    m_instanceType = NetworkApp::ClientInstance;
    m_workingInstance = &m_client;
}

void NetworkApp::start() {
    m_workingInstance->connect();
}

void NetworkApp::write(std::string &line) {
    m_workingInstance->write(line);
}

void NetworkApp::setReadCallback(void (*handler)(std::string res)) {
    m_server.setReadCallback(handler);
    m_client.setReadCallback(handler);
}

void NetworkApp::switchTo(NetworkApp::instanceType type) {
    m_instanceType = type;
    if (type == NetworkApp::ServerInstance)
        m_workingInstance = &m_server;
    else
        m_workingInstance = &m_client;
    m_workingInstance->connect();
}

//NetworkApp::~NetworkApp() {
//   m_workingInstance->~NetworkInterface();
//}

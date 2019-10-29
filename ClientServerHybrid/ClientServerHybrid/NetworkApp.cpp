//
//  NetworkApp.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkApp.hpp"

NetworkApp::NetworkApp(std::string dClIp, uint dClPort, uint dSePort) : m_client(dClIp, dClPort), m_server(dSePort) {
    m_workingInstance = &m_client;
}

void NetworkApp::write(std::string &line) {
    m_workingInstance->write(line);
}

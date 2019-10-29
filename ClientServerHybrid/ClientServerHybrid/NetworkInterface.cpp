//
//  NetworkInterface.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkInterface.hpp"

using namespace std;
using namespace boost;

NetworkInterface::NetworkInterface(std::string ip, unsigned int port) : m_defaultSettings(ip, port), m_socket(m_ios) {
    
}

void NetworkInterface::read() {
    cout << "Reading\n";
}

void NetworkInterface::write(std::string line) {
    asio::write(m_socket, asio::buffer(line));
}

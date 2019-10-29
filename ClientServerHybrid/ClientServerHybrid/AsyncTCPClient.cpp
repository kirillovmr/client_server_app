//
//  AsyncTCPClient.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPClient.hpp"

#include <iostream>

using namespace std;
using namespace boost;

AsyncTCPClient::AsyncTCPClient(std::string def_ip_address, unsigned short def_port_num) : NetworkInterface(def_ip_address, def_port_num) {
    m_endpoint = boost::asio::ip::tcp::endpoint(asio::ip::address::from_string(m_defaultSettings.m_ip), m_defaultSettings.m_port);
    
    m_socket.async_connect(m_endpoint, [this](error_code error) {
        if (!error) {
            cout << "Connected to " << m_endpoint << endl;
    
            m_socket.shutdown(asio::socket_base::shutdown_send);
        }
        else {
            cout << "Error connecting: " << error.message() << endl;
        }
    });
    m_ios.run();
}

//
//  NetworkInterface.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef NetworkInterface_hpp
#define NetworkInterface_hpp

#include <string>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>

struct DefaultSettings {
    std::string m_ip;
    unsigned int m_port;
    
    DefaultSettings() = default;
    DefaultSettings(std::string ip, unsigned int port) {
        m_ip = ip;
        m_port = port;
    }
};


class NetworkInterface {
protected:
    DefaultSettings m_defaultSettings;
    
    boost::asio::io_service m_ios;
    boost::asio::ip::tcp::socket m_socket;
    
public:
    NetworkInterface() = default;
    NetworkInterface(std::string ip = "0.0.0.0", unsigned int port = 3000);
    
    void read();
    void write(std::string line);
};

#endif /* NetworkInterface_hpp */

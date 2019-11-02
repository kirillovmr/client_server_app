//
//  AsyncTCPClient.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef AsyncTCPClient_hpp
#define AsyncTCPClient_hpp

#include "NetworkInterface.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>

class AsyncTCPClient : public NetworkInterface {
private:
    boost::asio::ip::tcp::endpoint m_endpoint;
    
public:
    AsyncTCPClient(std::string raw_ip_address, unsigned short port_num);
    
    void connect() override;
    void disconnect() override {
        
    };
};

#endif /* AsyncTCPClient_hpp */

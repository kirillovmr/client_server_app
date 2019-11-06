//
//  AsyncTCPClient.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef AsyncTCPClient_hpp
#define AsyncTCPClient_hpp

#include "IHybrid.hpp"
#include "Session.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <mutex>

class AsyncTCPClient: public IHybrid, public boost::asio::noncopyable {
public:
    // Initializes parent
    AsyncTCPClient(unsigned char num_of_threads);
    
    // Connects to specified endpoint
    unsigned int connect(const std::string &raw_ip, unsigned short port_num, Callback callback, ReadHandler handler = nullptr);
    
    // Notifies server to transmit all the data
    void transmit(unsigned int session_id, std::string data);
    
    // Disconnects clients
    ~AsyncTCPClient();
};

#endif /* AsyncTCPClient_hpp */

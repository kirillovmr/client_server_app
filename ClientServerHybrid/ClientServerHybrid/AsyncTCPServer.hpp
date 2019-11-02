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

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>

typedef boost::asio::ip::tcp::endpoint endpoint;
typedef boost::asio::ip::tcp::acceptor acceptor;

class AsyncTCPServer : public NetworkInterface {
private:
    endpoint m_endpoint;
    std::shared_ptr<acceptor> m_acceptor;
    
    // The size of the queue containing the pending connection requests.
    const int m_BACKLOG_SIZE = 30;
    
public:
    AsyncTCPServer(unsigned short port_num);
    
    void connect() override;
    void disconnect() override {
        m_socket.shutdown(boost::asio::socket_base::shutdown_send, m_ec);
    };
};

#endif /* AsyncTCPServer_hpp */

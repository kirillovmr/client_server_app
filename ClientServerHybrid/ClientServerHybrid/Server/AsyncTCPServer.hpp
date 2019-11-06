//
//  AsyncTCPServer.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include "IHybrid.hpp"
#include "Session.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <atomic>
#include <memory>
#include <string>
#include <mutex>

class AsyncTCPServer: public IHybrid, public boost::asio::noncopyable {
private:
    bool m_isStarted;
    std::atomic<bool> m_isStopped;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
    
    // Values to init each session
    unsigned short m_port_num;
    Callback m_callback;
    ReadHandler m_readHandler;
    
    // Acceptor accepts connection, creates session
    void initAccept();
    
    // Starts reading from session, calls initAccept()
    void onAccept(const boost::system::error_code &ec, std::shared_ptr<Session> session);
    
public:
    // Initializes parent
    AsyncTCPServer(unsigned char num_of_threads);
    
    // Run initAccept(), can be run only once
    void start(unsigned short port_num, Callback callback, ReadHandler handler = nullptr);
    
    // Transmits the data to all the sockets connected to the server
    void transmit(std::string &data, unsigned int excludeId = -1);
    
    // Disconnects clients, stop asyncs
    ~AsyncTCPServer();
};

#endif /* AsyncTCPServer_hpp */

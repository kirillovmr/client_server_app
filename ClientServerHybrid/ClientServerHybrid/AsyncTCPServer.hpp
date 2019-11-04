//
//  AsyncTCPServer.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include "Session.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <list>

class ServerSession;


class AsyncTCPServer: public boost::asio::noncopyable {
private:
    boost::asio::io_service m_ios;
    std::mutex m_active_sessions_guard;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::list<std::unique_ptr<std::thread>> m_thread_pool;
    std::map<int, std::shared_ptr<ServerSession>> m_active_sessions;
    
    std::atomic<unsigned int> sessionCounter;
    unsigned short m_port_num;
    
    void initAccept();
    void onAccept(const boost::system::error_code &ec, std::shared_ptr<ServerSession> session);
    
public:
    AsyncTCPServer(unsigned short port_num, unsigned char num_of_threads);
    
    void start();
    void stop();
};

#endif /* AsyncTCPServer_hpp */

//
//  AsyncTCPClient.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef AsyncTCPClient_hpp
#define AsyncTCPClient_hpp

#include "Session.hpp"

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <list>

class AsyncTCPClient: public boost::asio::noncopyable {
private:
    boost::asio::io_service m_ios;
    std::mutex m_active_sessions_guard;
    std::list<std::unique_ptr<std::thread>> m_thread_pool;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::map<int, std::shared_ptr<Session>> m_active_sessions;
    
    unsigned int sessionCounter; // MAKE ATOMIC
    
    void onRequestComplete(std::shared_ptr<Session> session);
    
public:
    AsyncTCPClient(unsigned char num_of_threads);
    
    unsigned int connect(const std::string &raw_ip_address, unsigned short port_num, Callback callback, ReadHandler handler = nullptr);
    void write(unsigned int session_id, std::string &data);
    void disconnect(unsigned int session_id);
    
    void close();
};

#endif /* AsyncTCPClient_hpp */

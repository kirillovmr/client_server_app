//
//  IHybrid.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/4/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef IHybrid_hpp
#define IHybrid_hpp

#include "Session.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <list>

class IHybrid {
protected:
    const bool m_debug = true;
    boost::asio::io_service m_ios;
    std::mutex m_active_sessions_guard;
    std::list<std::unique_ptr<std::thread>> m_thread_pool;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::map<int, std::shared_ptr<Session>> m_active_sessions;
    
    std::atomic<unsigned int> sessionCounter;
    
    // Disconnects socket, removes it from session list
    void onRequestComplete(std::shared_ptr<Session> session);
    
public:
    IHybrid(unsigned char num_of_threads);
    
    // Writes to session id
    void write(unsigned int session_id, std::string &data);
    
    // Disconnects session(s) by id
    void disconnectAll();
    void disconnect(unsigned int session_id);
    
    // Binds to session object, calls onRequestComplete() when needed
    void callOnRequestComplete(unsigned int session_id);
    
    virtual ~IHybrid();
};

#endif /* IHybrid_hpp */

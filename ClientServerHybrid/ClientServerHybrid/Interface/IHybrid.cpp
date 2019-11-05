//
//  IHybrid.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/4/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "IHybrid.hpp"
using namespace std;
using namespace boost;

IHybrid::IHybrid(unsigned char num_of_threads): sessionCounter(0) {
    m_work.reset(new boost::asio::io_service::work(m_ios));

    for (unsigned char i = 0; i < num_of_threads; i++) {
        std::unique_ptr<std::thread> th( new std::thread([this](){
            m_ios.run();
        }));

        m_thread_pool.push_back(std::move(th));
    }
}


void IHybrid::onRequestComplete(std::shared_ptr<Session> session) {
    boost::system::error_code ignored_ec, ec;
    session->getSocket().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);

    // Remove session form the map of active sessions.
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        auto it = m_active_sessions.find(session->getId());
        if (it != m_active_sessions.end())
        m_active_sessions.erase(it);
    lock.unlock();
    
    if (m_debug)
        cout << "Session " << session->getId() << " was removed from list.\n";

    if (!session->m_ec && session->m_was_cancelled)
        ec = asio::error::operation_aborted;
    else
        ec = session->m_ec;

    // Call the callback provided by the user.
    session->m_callback(session->getId(), session->m_response, ec);
};

void IHybrid::callOnRequestComplete(unsigned int session_id) {
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        auto it = m_active_sessions.find(session_id);
    lock.unlock();
    
    if (it != m_active_sessions.end())
        onRequestComplete(it->second);
}


void IHybrid::write(unsigned int session_id, std::string &data) {
    
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        auto it = m_active_sessions.find(session_id);
    lock.unlock();
    
    if (it != m_active_sessions.end())
        it->second->write(data);
    else
        cout << "Session " << session_id << " was not found\n";
}


void IHybrid::disconnectAll() {
    for (auto &s: m_active_sessions) {
        disconnect(s.second->getId());
    }
}

void IHybrid::disconnect(unsigned int session_id) {
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        auto it = m_active_sessions.find(session_id);
    lock.unlock();
    
    if (it != m_active_sessions.end()) {
        if (!it->second->connected)
            return;
        
        std::unique_lock<std::mutex> cancel_lock(it->second->m_cancel_guard);
        it->second->m_was_cancelled = true;
        it->second->getSocket().cancel();
        
        if (m_debug)
            cout << "Session " << session_id << " was manually disconnected.\n";
    }
}


IHybrid::~IHybrid() {
    // Destroy work object. I/O threads exit the event loop
    // when there are no more pending asynchronous operations.
    m_work.reset(NULL);

    // Waiting for the I/O threads to exit.
    for (auto &thread : m_thread_pool)
        thread->join();
}

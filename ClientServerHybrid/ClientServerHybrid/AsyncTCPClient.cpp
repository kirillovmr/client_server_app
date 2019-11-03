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

AsyncTCPClient::AsyncTCPClient(unsigned char num_of_threads): sessionCounter(0) {
    m_work.reset(new boost::asio::io_service::work(m_ios));

    for (unsigned char i = 1; i <= num_of_threads; i++) {
        std::unique_ptr<std::thread> th( new std::thread([this](){
            m_ios.run();
        }));

        m_threads.push_back(std::move(th));
    }
}


unsigned int AsyncTCPClient::connect(const string &raw_ip, unsigned short port_num, Callback callback, ReadHandler handler) {
    
    std::shared_ptr<Session> session = std::shared_ptr<Session>( new Session(m_ios, raw_ip, port_num, "request", sessionCounter, callback));
    if(handler)
        session->readHandler = handler;
    session->getSocket().open(session->getEndpoint().protocol());
    
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    m_active_sessions[sessionCounter++] = session;
    lock.unlock();
    
    session->getSocket().async_connect(session->getEndpoint(), [this, session](const system::error_code& ec) {
        if (ec) {
            session->m_ec = ec;
            onRequestComplete(session);
            return;
        }

        std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);
        if (session->m_was_cancelled) {
            onRequestComplete(session);
            return;
        }
        
        cout << "Session " << session->getId() << ": Connected\n";
        
        session->startRead();
    });
    
    return session->getId();
}


void AsyncTCPClient::write(unsigned int session_id, std::string &data) {
    
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    auto it = m_active_sessions.find(session_id);
    
    if (it != m_active_sessions.end())
        it->second->write(data);
    else
        cout << "Session " << session_id << " was not found\n";
}


void AsyncTCPClient::disconnect(unsigned int session_id) {
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);

    auto it = m_active_sessions.find(session_id);
    if (it != m_active_sessions.end()) {
        std::unique_lock<std::mutex> cancel_lock(it->second->m_cancel_guard);

        it->second->m_was_cancelled = true;
        it->second->getSocket().cancel();
        cout << "Session " << session_id << " was manually disconnected\n";
    }
}

void AsyncTCPClient::onRequestComplete(std::shared_ptr<Session> session) {
    boost::system::error_code ignored_ec, ec;
    session->getSocket().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);

    // Remove session form the map of active sessions.
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    auto it = m_active_sessions.find(session->getId());
    if (it != m_active_sessions.end())
        m_active_sessions.erase(it);
    lock.unlock();

    if (!session->m_ec && session->m_was_cancelled)
        ec = asio::error::operation_aborted;
    else
        ec = session->m_ec;

    // Call the callback provided by the user.
    session->m_callback(session->getId(), session->m_response, ec);
};

void AsyncTCPClient::close() {
    // Destroy work object. This allows the I/O threads to
    // exit the event loop when there are no more pending
    // asynchronous operations.
    m_work.reset(NULL);

    // Waiting for the I/O threads to exit.
    for (auto& thread : m_threads)
        thread->join();
}

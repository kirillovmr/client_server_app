//
//  AsyncTCPClient.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPClient.hpp"

using namespace std;
using namespace boost;

AsyncTCPClient::AsyncTCPClient(unsigned char num_of_threads): IHybrid(num_of_threads) {}


unsigned int AsyncTCPClient::connect(const string &raw_ip, unsigned short port_num, Callback callback, ReadHandler handler) {
    std::shared_ptr<Session> session = std::shared_ptr<Session>( new Session(m_ios, raw_ip, port_num, "request", sessionCounter.load(), callback));
    
    // Bind handlers
    session->m_callOnRequestComplete = bind(&IHybrid::callOnRequestComplete, this, placeholders::_1);
    if(handler) { session->m_readHandler = handler; }
    
    session->m_sock.open(session->m_ep.protocol());
    
    // Store session
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        m_active_sessions[sessionCounter++] = session;
    lock.unlock();
    
    session->m_sock.async_connect(session->m_ep, [this, session](const system::error_code& ec) {
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
        
        cout << "Session " << session->m_id << ": Connected\n";
        session->connected.store(true);
        
        session->startRead();
    });
    
    return session->m_id;
}


AsyncTCPClient::~AsyncTCPClient() {
    // Disconnect all clients
    disconnectAll();
}

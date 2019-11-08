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

AsyncTCPClient::AsyncTCPClient(unsigned char num_of_threads): IHybrid(num_of_threads) {
    m_instanceType = InstanceType::ClientInstance;
}


unsigned int AsyncTCPClient::connect(const string &raw_ip, unsigned short port_num, Callback callback, ReadHandler handler, OnConnect onConnect) {
    std::shared_ptr<Session> session = std::shared_ptr<Session>( new Session(m_ios, raw_ip, port_num, "request", sessionCounter.load(), callback, onConnect));
    session->m_instanceType = m_instanceType;
    
    // Bind handlers
    session->m_callOnRequestComplete = bind(&IHybrid::callOnRequestComplete, this, placeholders::_1);
    if(handler) { session->m_readHandler = handler; }
    
    session->m_sock.open(session->m_ep.protocol());
    
    // Store session
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        m_active_sessions[sessionCounter++] = session;
    lock.unlock();
    
    session->connect();
    
    return session->m_id;
    
//    session->m_sock.async_connect(session->m_ep, [this, session](const system::error_code& ec) {
//        if (ec == boost::asio::error::connection_refused) {
//            cout << "Reconnecting..." << endl;

//            session->m_ec = ec;
//            onRequestComplete(session);
//            return;
//        }
//        else if (ec) {
//            session->m_ec = ec;
//            onRequestComplete(session);
//            return;
//        }
//
//        std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);
//        if (session->m_was_cancelled) {
//            onRequestComplete(session);
//            return;
//        }
//
//        cout << "Session " << session->m_id << ": Connected\n";
//        session->connected.store(true);
//
//        session->startRead();
//    });
//
//    return session->m_id;
}


void AsyncTCPClient::transmit(unsigned int session_id, std::string data) {
    data.insert(0, 1, '\t');
    write(session_id, data);
}


AsyncTCPClient::~AsyncTCPClient() {
    // Disconnect all clients
    disconnectAll();
}

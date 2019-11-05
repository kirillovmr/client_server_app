//
//  AsyncTCPServer.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPServer.hpp"

using namespace std;
using namespace boost;

using acceptor = boost::asio::ip::tcp::acceptor;
using endpoint = boost::asio::ip::tcp::endpoint;

void AsyncTCPServer::initAccept() {
    std::shared_ptr<Session> session = std::shared_ptr<Session>( new Session(m_ios, "127.0.0.1", m_port_num, "request", sessionCounter, m_callback));
    
    // Bind handlers
    session->m_callOnRequestComplete = bind(&IHybrid::callOnRequestComplete, this, placeholders::_1);
    if(m_readHandler) { session->m_readHandler = m_readHandler; }
    
    // Store session
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        m_active_sessions[sessionCounter++] = session;
    lock.unlock();
    
    // Initializes acceptor on first run
    if(!m_isStarted) {
        m_acceptor.reset( new acceptor(m_ios, endpoint(boost::asio::ip::address_v4::any(), m_port_num)) );
        m_isStarted = true;
    }
    
    m_acceptor->listen();
    if (m_debug)
        cout << "Listen started.\n";
    
    m_acceptor->async_accept(session->m_sock, [this, session]( const boost::system::error_code &error) {
        if (m_debug)
            cout << "New connection: " << session->m_sock.remote_endpoint() << endl;

        onAccept(error, session);
    });
    
}

void AsyncTCPServer::onAccept(const boost::system::error_code &ec, std::shared_ptr<Session> session) {
    if (!ec) {
        session->connected.store(true);
        session->startRead();
    }
    else {
        if (m_debug)
            cout << "onAccept() Error occured! Code: " << ec.value() << ". Message: " << ec.message() << endl;
/**/        // REMOVE SESSION
    }

    if (!m_isStopped.load()) {
        // Init next async accept operation
        // if acceptor has not been stopped yet.
        initAccept();
    }
    else {
        // Stop accepting incoming connections,
        // free allocated resources.
        m_acceptor->close();
    }
}


AsyncTCPServer::AsyncTCPServer(unsigned char num_of_threads): IHybrid(num_of_threads), m_isStarted(false), m_isStopped(false) {}


void AsyncTCPServer::start(unsigned short port_num, Callback callback, ReadHandler handler) {
    if(!m_isStarted) {
        m_port_num = port_num;
        m_callback = callback;
        m_readHandler = handler;
        initAccept();
    }
}


AsyncTCPServer::~AsyncTCPServer() {
    // Disconnect all clients
    disconnectAll();
    
    // Stop accepting incoming connection requests.
    m_isStopped.store(true);

    // Cancel all acceptor asynchronous operations
    m_acceptor->cancel();
}
//
//  AsyncTCPServer.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPServer.hpp"

class ServerSession: public Session {
public:
    std::atomic<bool> m_isStopped;
    
    ServerSession(boost::asio::io_service &ios, const std::string& raw_ip_address, unsigned short port_num, const std::string& request, unsigned int id, Callback callback) :
        Session(ios, raw_ip_address, port_num, request, id, callback)
    {
        
    }
};

using namespace std;
using namespace boost;

AsyncTCPServer::AsyncTCPServer(unsigned short port_num, unsigned char num_of_threads):
    m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port_num)),
    sessionCounter(0), m_port_num(port_num)
{
    m_work.reset(new boost::asio::io_service::work(m_ios));

    for (unsigned char i = 0; i < num_of_threads; i++) {
        std::unique_ptr<std::thread> th( new std::thread([this](){
            m_ios.run();
        }));

        m_thread_pool.push_back(std::move(th));
    }
}


void AsyncTCPServer::initAccept() {
    std::shared_ptr<ServerSession> session = std::shared_ptr<ServerSession>( new ServerSession(m_ios, "127.0.0.1", m_port_num, "request", sessionCounter, nullptr));
    
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    m_active_sessions[sessionCounter++] = session;
    lock.unlock();
    
    m_acceptor.listen();
    
    m_acceptor.async_accept(session->getSocket(), [this, session]( const boost::system::error_code &error) {
        std::cout << "Connected\n";
        
        onAccept(error, session);
    });
    
}

void AsyncTCPServer::onAccept(const boost::system::error_code &ec, std::shared_ptr<ServerSession> session) {
    if (!ec) {
        session->startRead();
    }
    else {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }

    // Init next async accept operation if
    // acceptor has not been stopped yet.
    if (!session->m_isStopped.load()) {
        initAccept();
    }
    else {
        // Stop accepting incoming connections
        // and free allocated resources.
        m_acceptor.close();
    }
}


void AsyncTCPServer::start() {
    initAccept();
}

void AsyncTCPServer::stop() {
    m_work.reset(NULL);

    // Waiting for the I/O threads to exit.
    for (auto &thread : m_thread_pool)
        thread->join();
}

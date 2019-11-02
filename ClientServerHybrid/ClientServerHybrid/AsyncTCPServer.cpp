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

AsyncTCPServer::AsyncTCPServer(unsigned short port_num) : NetworkInterface("0.0.0.0", port_num) {
    m_endpoint = endpoint(asio::ip::address_v4::any(), port_num);
}

void AsyncTCPServer::connect() {
    // Creating and opening an acceptor socket.
    m_acceptor = make_shared<acceptor>( acceptor(m_ios, m_endpoint.protocol()) );
    
    // Binding the acceptor socket.
    m_acceptor->bind(m_endpoint, m_ec);
    
    // Failed to bind the acceptor socket. Breaking execution.
    if (m_ec) {
        cout << "Failed to bind the acceptor socket." << "Error code = " << m_ec.value() << ". Message: " << m_ec.message();
        return;
    }
    
    // Starting to listen for incoming connection requests.
    m_acceptor->listen(m_BACKLOG_SIZE);
    
    cout << "Server running on " << m_endpoint << endl;
    
    // Step 7. Processing the next connection request and
    //          connecting the active socket to the client.
    m_acceptor->accept(m_socket);
    
    cout << m_socket.remote_endpoint() << " is connected." << endl;
    
    startReadingThread();
}

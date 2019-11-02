//
//  NetworkInterface.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkInterface.hpp"

using namespace std;
using namespace boost;

NetworkInterface::NetworkInterface(std::string ip, unsigned int port): m_defaultSettings(ip, port), m_socket(m_ios) {
    
    // Setting default handler
    onRead = [](string res){
        cout << "Readed: " << res << endl;
    };
}

void NetworkInterface::startReadingThread() {
    auto readFunc = [this](){
        cout << "Reading thread started\n";

        while(true) {
            asio::streambuf buffer;
            std::size_t n = asio::read_until(m_socket, buffer, '\r');
            asio::streambuf::const_buffers_type bufs = buffer.data();
            std::string line( asio::buffers_begin(bufs), asio::buffers_begin(bufs)+n);

            onRead(line);
        }
    };
    m_readingThread = make_shared<thread>( thread(readFunc) );
    
//    cout << "Reading thread started\n";
    
//    m_readingThread = make_shared<thread>( thread(readFunc) );
}

void NetworkInterface::readFunc() {
    asio::async_read_until(m_socket, m_response_buf, '\r', [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
            m_ec = ec;
        } else {
           std::istream strm(&m_response_buf);
           std::getline(strm, m_response);
            onRead(m_response);
            readFunc();
        }
    });
}

void NetworkInterface::setReadCallback(void (*handler)(std::string res)) {
    onRead = handler;
}

void NetworkInterface::write(std::string &line) {
    asio::write(m_socket, asio::buffer(line));
}

NetworkInterface::~NetworkInterface() {
    m_socket.shutdown(asio::socket_base::shutdown_send, m_ec);
    if (m_ec) {
        cout << "Error ~NetworkInterface: " << m_ec.message() << endl;
    }
//    else
//        cout << "OK" << endl;
    m_socket.close();
}

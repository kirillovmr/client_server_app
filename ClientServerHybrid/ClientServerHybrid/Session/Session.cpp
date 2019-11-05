//
//  Session.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/3/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "Session.hpp"

Session::Session(boost::asio::io_service& ios, const std::string& raw_ip_address, unsigned short port_num, const std::string& request, unsigned int id, Callback callback) :
        m_sock(ios), m_ep(boost::asio::ip::address::from_string(raw_ip_address), port_num),
        m_request(request), m_id(id), m_callback(callback), m_was_cancelled(false), connected(false)
{
    m_readHandler = std::bind(&Session::defaultReadHandler, this, std::placeholders::_1);
}

void Session::startRead() {
    boost::asio::async_read_until(m_sock, m_response_buf, '\r', std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2) );
}

void Session::handleRead(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec) {
        m_ec = ec;
        std::cout << "'handleRead': Socket was disconnected\n";
        m_callOnRequestComplete(m_id);
    } else {
        std::istream strm(&m_response_buf);
        std::getline(strm, m_response);
        
        std::unique_lock<std::mutex> lock(m_request_guard);
        if (m_response != m_request) { // Wrap with mutex only if
            m_readHandler(m_response);
        }
        lock.unlock();
        
        startRead();
    }
}

void Session::write(std::string &data) {
    std::unique_lock<std::mutex> lock(m_request_guard); // MAKE ATOMIC
        m_request = data + "\r";
    lock.unlock();
    
    boost::asio::async_write(m_sock, boost::asio::buffer(data + "\r"),
                  [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
        if (ec) {
            m_ec = ec;
            std::cout << "Writing error\n";
            return;
        }

        std::unique_lock<std::mutex> cancel_lock(m_cancel_guard);
        if (m_was_cancelled) {
            std::cout << "Writing calcelled\n";
            return;
        }
    });
}

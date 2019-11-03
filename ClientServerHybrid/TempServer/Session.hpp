//
//  Session.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/3/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef Session_hpp
#define Session_hpp

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <mutex>

typedef void (*Callback) (unsigned int request_id, const std::string& response, const boost::system::error_code& ec);
typedef void (*readHandler) (std::string data);

class Session {
private:
    unsigned int m_id;
    
    boost::asio::ip::tcp::socket m_sock;
    boost::asio::ip::tcp::endpoint m_ep;
    std::string m_request;

public:
    boost::asio::streambuf m_response_buf;
    std::string m_response;

    boost::system::error_code m_ec;

    Callback m_callback;

    bool m_was_cancelled;
    std::mutex m_cancel_guard;
    std::mutex m_request_guard;
    
    std::function<void(std::string)> readHandler;
    
    void defaultReadHandler(std::string) {
        std::cout << "Readed: " << m_response << std::endl;
    }
    
public:
    Session(boost::asio::io_service& ios, const std::string& raw_ip_address, unsigned short port_num, const std::string& request, unsigned int id, Callback callback);
    
    void startRead();
    void handleRead(const boost::system::error_code &ec, std::size_t bytes_transferred);
    void write(std::string &data);
    
    unsigned int getId() { return m_id; }
    boost::asio::ip::tcp::socket &getSocket() { return m_sock; }
    boost::asio::ip::tcp::endpoint &getEndpoint() { return m_ep; }
};

#endif /* Session_hpp */

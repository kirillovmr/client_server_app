//
//  NetworkInterface.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef NetworkInterface_hpp
#define NetworkInterface_hpp

#include <string>
#include <memory>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>

class NetworkInterface {

    struct DefaultSettings {
        std::string m_ip;
        unsigned int m_port;
        
        DefaultSettings(std::string ip, unsigned int port): m_ip(ip), m_port(port) {}
    };
    
protected:
    DefaultSettings m_defaultSettings;
    
public: //remove later
    boost::asio::io_service m_ios;
    boost::asio::ip::tcp::socket m_socket;
    boost::system::error_code m_ec;
    
    boost::asio::streambuf m_response_buf;
    std::string m_response;
    
    std::shared_ptr<std::thread> m_readingThread;
    void readFunc();
    void (*onRead)(std::string res);
    
public:
    NetworkInterface(std::string ip, unsigned int port);
    
    void setReadCallback(void (*handler)(std::string res));
    
    // Starts a thread, calls a callback on result
    void startReadingThread();
    
    // 
    void write(std::string &line);
    
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual ~NetworkInterface();
};

#endif /* NetworkInterface_hpp */

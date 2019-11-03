//
//  main.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPClient.hpp"
#include "AsyncTCPServer.hpp"
#include <boost/asio.hpp>

#include <iostream>
#include <string>

using namespace std;

void handler(unsigned int request_id, const std::string& response, const boost::system::error_code& ec) {
    if (!ec) {
        std::cout << "Request #" << request_id << " has completed. Response: " << response << std::endl;
    } else if (ec == boost::asio::error::operation_aborted) {
        std::cout << "Request #" << request_id << " has been cancelled by the user." << std::endl;
    } else {
        std::cout << "Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << std::endl;
    }

    return;
}

class Test {
private:
    string res;
    AsyncTCPClient client;
    
public:
    Test() : res("lol"), client(4) {}
    
    void newDataHandler(string data) {
        cout << "Default Readed: " << data << endl;
        res = data;
        printData();
    }
    
    void start() {
        auto readHandler = std::bind(&Test::newDataHandler, this, std::placeholders::_1);
        int id1 = client.connect("127.0.0.1", 3000, handler, readHandler);
        
        string in;
        cin >> in;
        
        client.disconnect(id1);
        client.close();
    }
    
    void printData() {
        cout << "From printData(): " << res << endl;
    }
};

int main(int argc, const char * argv[]) {
    cout << "Main started\n";
    string input;
    
//    Test test;
//    test.start();
//    return 0;
    
    AsyncTCPClient client(4);

    unsigned int id1 = client.connect("127.0.0.1", 3333, handler);
//    unsigned int id2 = client.connect("127.0.0.1", 3000, handler);

    while(true) {
        cin >> input;
        if(input == "x")
            break;

        cout << "INPUT: " << input << endl;
        client.write(id1, input);
    }

    client.disconnect(id1);
//    client.disconnect(id2);
    client.close();
    
//    AsyncTCPServer server;
    
    return 0;
}

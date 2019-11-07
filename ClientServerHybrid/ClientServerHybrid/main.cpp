//
//  main.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "AsyncTCPClient.hpp"
#include "AsyncTCPServer.hpp"
#include "NetworkChessApp.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
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
//        client.destroy();
    }
    
    void printData() {
        cout << "From printData(): " << res << endl;
    }
};

int main(int argc, const char * argv[]) {
    cout << "Main started\n";
    string input;
    string ip = "127.0.0.1";
    
    NetworkChessApp app(ip, 3000);
    
    vector<string> boardVals {"A120", "A221", "B416"};
    app.serializeBoard(boardVals);
    this_thread::sleep_for(1s);
    app.runServer();
    this_thread::sleep_for(1s);
    app.join(0);
    
    cin >> input;
    
//    Test test;
//    test.start();
//    return 0;
    
    
//    if(0) {
//        AsyncTCPClient client(4);
//        unsigned int id1 = client.connect("127.0.0.1", 3000, handler);
////        unsigned int id2 = client.connect("127.0.0.1", 3000, handler);
//        while(true) {
//            cin >> input;
//            if(input == "x")
//                break;
//
//            cout << "INPUT: " << input << endl;
//            client.write(id1, input);
//        }
//    }
//    else {
//        AsyncTCPServer server(4);
//        server.start(3000, handler);
//        while(true) {
//            cin >> input;
//            if(input == "x")
//                break;
//
//            cout << "INPUT: " << input << endl;
//            server.write(0, input);
//        }
//    }
    
    return 0;
}


// 1. Debug mode pass to session.
// 2. Refactor session struct
// 3. Check memory leakage
// 4. Test
// 5. Test

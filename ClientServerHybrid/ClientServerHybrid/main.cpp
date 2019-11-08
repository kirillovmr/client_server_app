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
    int num;
    string ip = "127.0.0.1";
    string gip = "10.0.0.204";
    
    
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);

    std::string h = boost::asio::ip::host_name();
    std::cout << "Host name is " << h << '\n';
    std::cout << "IP addresses are: \n";
    std::for_each(resolver.resolve({h, ""}), {}, [](const auto &re) {
        std::cout << re.endpoint().address().to_string() << '\n';
    });
    
    
//    cin >> input;
    NetworkChessApp app;
    
    while(true) {
        cin >> input;
        
        if(input == "x") {
            break;
        }
        else if(input == "serv") {
            app.runServer();
        }
        else if(input == "stop") {
            app.stopServer();
        }
        else if(input == "conn") {
            cin >> input;
            cin >> num;
            app.connect(input, num);
        }
        else if(input == "disc"){
            app.disconnect();
        }
    }
    
    //    vector<string> boardVals {"A120", "A221", "B416"};
    //    app.serializeBoard(boardVals);
    
    return 0;
}


// 1. Debug mode pass to session.
// 2. Refactor session struct
// 3. Check memory leakage
// 4. Test
// 5. Test

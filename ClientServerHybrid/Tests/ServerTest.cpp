//
//  ServerTest.cpp
//  Tests
//
//  Created by Viktor Kirillov on 11/5/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>

#include "AsyncTCPClient.hpp"
#include "AsyncTCPServer.hpp"
#include "catch.hpp"

#define IP "127.0.0.1"
#define PORT 3010
#define sleep .2s

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

TEST_CASE("1. Basic, Connection") {
//    return;
    AsyncTCPServer server(2);
    AsyncTCPClient client(2);
    
    SECTION("Just create instances") {
        //
        // Intentionally empty.
    }
    
    SECTION("Start server") {
        server.start(PORT, handler);
    }
    
    SECTION("Start server, sleep") {
        server.start(PORT, handler);
        this_thread::sleep_for(sleep);
    }
    
    SECTION("Accept connection") {
        server.start(PORT, handler);
        this_thread::sleep_for(sleep);
        client.connect(IP, PORT, handler);
    }
    
    SECTION("Accept connection, sleep") {
        server.start(PORT, handler);
        this_thread::sleep_for(sleep);
        int id = client.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        REQUIRE(id == 0);
    }
    
    SECTION("Accept connection, sleep, disconnect") {
        server.start(PORT, handler);
        this_thread::sleep_for(sleep);
        int id = client.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        client.disconnect(id);
    }
    
    SECTION("Accept connection, sleep, disconnect, sleep") {
        server.start(PORT, handler);
        this_thread::sleep_for(sleep);
        int id = client.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        client.disconnect(id);
        this_thread::sleep_for(sleep);
    }
    this_thread::sleep_for(sleep*2);
}

TEST_CASE("2. Advanced connection") {
//    return;
    AsyncTCPServer server(4);
    AsyncTCPClient client1(2);
    AsyncTCPClient client2(2);
    
    server.start(PORT, handler);
    this_thread::sleep_for(sleep);
    
    SECTION("Accept 2 connections") {
        int id1 = client1.connect(IP, PORT, handler);
        int id2 = client2.connect(IP, PORT, handler);
        REQUIRE(id1 == 0);
        REQUIRE(id2 == 0);
    }
    
    SECTION("Accept 2 connections, sleep") {
        client1.connect(IP, PORT, handler);
        client2.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
    }
    
    SECTION("Accept 2 connections, sleep, disconnect") {
        int id1 = client1.connect(IP, PORT, handler);
        int id2 = client2.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        client1.disconnect(id1);
        this_thread::sleep_for(sleep);
        client2.disconnect(id2);
    }
    this_thread::sleep_for(sleep*2);
}

TEST_CASE("3. Communication") {
//    return;
    AsyncTCPServer server(4);
    AsyncTCPClient client(2);
    
    vector<string> messages {"Hello", "MSG from client"};
    vector<string> messages2 {"test1", "test2", "lol"};
    shared_ptr<vector<string>> resp(new vector<string>);
    
    SECTION("Reading") {
        server.start(PORT, handler, [resp](string res){
            resp->push_back(res);
        });
        this_thread::sleep_for(sleep);
        
        int id = client.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        
        for (auto &m: messages) {
            client.write(id, m);
            this_thread::sleep_for(sleep);
        }
        
        REQUIRE(resp.get()->size() == messages.size());
        for (int i=0; i<resp.get()->size(); i++) {
            REQUIRE(resp.get()->at(i) == messages[i]);
        }
    }
    
    SECTION("Reading, writing, reading") {
        server.start(PORT, handler, [resp](string res){
            resp->push_back(res);
        });
        this_thread::sleep_for(sleep);
        
        int id = client.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        
        client.write(id, messages[0]);
        this_thread::sleep_for(sleep);
        server.write(0, messages2[0]);
        this_thread::sleep_for(sleep);
        
        for (int i=1; i<messages.size(); i++) {
            client.write(id, messages[i]);
            this_thread::sleep_for(sleep);
        }
        
        REQUIRE(resp.get()->size() == messages.size());
        for (int i=0; i<resp.get()->size(); i++) {
            REQUIRE(resp.get()->at(i) == messages[i]);
        }
    }
    this_thread::sleep_for(sleep*2);
}

TEST_CASE("4. Advanced communication") {
    AsyncTCPServer server(4);
    AsyncTCPClient client1(2);
    AsyncTCPClient client2(2);
    AsyncTCPClient client3(2);

    vector<string> messages {"test1", "test2", "lol", "kek", "krot zdes", "aloha", "nihao", "it's me"};
    shared_ptr<vector<string>> resp(new vector<string>);

    SECTION("Reading 3 clients") {
        server.start(PORT, handler, [resp](string res){
            resp->push_back(res);
        });
        this_thread::sleep_for(sleep);

        int id1 = client1.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        int id2 = client2.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);
        int id3 = client3.connect(IP, PORT, handler);
        this_thread::sleep_for(sleep);

        for (int i=0; i<messages.size(); i++) {
            if (i%3==0)
                client1.write(id1, messages[i]);
            else if (i%3==1)
                client2.write(id2, messages[i]);
            else if (i%3==2)
                client3.write(id3, messages[i]);
            this_thread::sleep_for(sleep);
        }

        REQUIRE(resp.get()->size() == messages.size());
        for (int i=0; i<resp.get()->size(); i++) {
            REQUIRE(resp.get()->at(i) == messages[i]);
        }
    }
    
    SECTION("Server transmit") {

        shared_ptr<vector<string>> resp2(new vector<string>);

        server.start(PORT, handler, [resp, &server](string res){
            resp->push_back(res);
        });
        this_thread::sleep_for(sleep);

        int id1 = client1.connect(IP, PORT, handler, [resp](string res){
            resp->push_back(res);
        });
        this_thread::sleep_for(sleep);

        int id2 = client2.connect(IP, PORT, handler, [resp2](string res){
            resp2->push_back(res);
        });
        this_thread::sleep_for(sleep);

        for (int i=0; i<messages.size(); i++) {
            server.transmit(messages[i]);
            this_thread::sleep_for(sleep);
        }

        REQUIRE(resp.get()->size() == messages.size());
        REQUIRE(resp2.get()->size() == messages.size());
        for (int i=0; i<resp.get()->size(); i++) {
            REQUIRE(resp.get()->at(i) == messages[i]);
            REQUIRE(resp2.get()->at(i) == messages[i]);
        }
    }
    
    SECTION("Client transmit") {
        
        shared_ptr<vector<string>> resp2(new vector<string>);
        shared_ptr<vector<string>> resp3(new vector<string>);
        
        server.start(PORT, handler, [resp, &server](string res){
            resp->push_back(res);
        });
        this_thread::sleep_for(sleep);
        
        int id1 = client1.connect(IP, PORT, handler, [resp2](string res){
            resp2->push_back(res);
        });
        this_thread::sleep_for(sleep);
        
        int id2 = client2.connect(IP, PORT, handler, [resp3](string res){
            resp3->push_back(res);
        });
        this_thread::sleep_for(sleep);
        
        for (int i=0; i<messages.size(); i++) {
            client1.transmit(id1, messages[i]);
            this_thread::sleep_for(sleep);
        }
        
        REQUIRE(resp.get()->size() == messages.size());
        REQUIRE(resp2.get()->size() == 0);
        REQUIRE(resp3.get()->size() == messages.size());
        for (int i=0; i<resp.get()->size(); i++) {
            REQUIRE(resp.get()->at(i) == messages[i]);
            REQUIRE(resp3.get()->at(i) == messages[i]);
        }
    }
    this_thread::sleep_for(sleep*2);
}

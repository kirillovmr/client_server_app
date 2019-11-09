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
#include <iomanip> // setw
#include <thread>
#include <string>

using namespace std;

int main(int argc, const char * argv[]) {
    cout << "Main started\n";
    string input;
    int n1,n2;
    
    AsyncTCPServer s(2);
//    s.start(3020, [](auto a, auto b, auto c){});
//    return 1;
    
    NetworkChessApp app([](const int board[64]){
        cout << endl;
        for(int i=0; i<64; i++) {
            if (i%8 == 0 && i != 0)
                cout << endl;
            cout << setw(3) << board[i];
        }
        cout << endl << endl;
    });
    
    while(true) {
        cout << "Your input: ";
        cin >> input;
        
        if(input == "x") {
            break;
        }
        else if(input == "serv") {
            string ip = app.runServer();
            if (ip.length() > 0)
                cout << "Server started, ip: " << ip << endl;
        }
        else if(input == "stop") {
            app.stopServer();
        }
        else if(input == "conn") {
            cin >> input;
            app.connect(input, 3030);
        }
        else if(input == "canc") {
            app.cancel();
        }
        else if(input == "disc"){
            app.disconnect();
        }
        else if(input == "mov"){
            cin >> n1 >> n2;
            app.makeMove(n1, n2);
        }
    }
    
    return 0;
}

// Add conenction handlers to server side
// Client seems working

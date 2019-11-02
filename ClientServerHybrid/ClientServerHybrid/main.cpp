//
//  main.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 10/28/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkApp.hpp"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, const char * argv[]) {
    
    NetworkApp app("127.0.0.1", 3000, 3030);
    app.setReadCallback([](string res){
        cout << "Custom read cb: " << res << endl;
    });
    app.start();
    
    string input;
    while (true) {
        cin >> input;
        
        if (input[0] == 'w')
            app.write(input);
        else if (input[0] == 'd')
            app.disconnect();
        else if (input[0] == 's')
            app.switchTo(NetworkApp::ServerInstance);
        else if (input[0] == 'c') {
            if(input.length() > 1)
                app.switchTo(NetworkApp::ClientInstance);
            
            app.switchTo(NetworkApp::ClientInstance);
        }
        else if (input[0] == 'x')
            break;
    }
    
    return 0;
}

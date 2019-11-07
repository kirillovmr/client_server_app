//
//  NetworkChessApp.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/6/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef NetworkChessApp_hpp
#define NetworkChessApp_hpp

#include "IHybrid.hpp"
#include "AsyncTCPServer.hpp"
#include "AsyncTCPClient.hpp"

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

class NetworkChessApp {
public:
    
private:
    enum NetworkState { ConnToMatchingServer, ConnToGameServer, RunningServer, Connecting };
    std::string msgStartServer = "startServer";
    std::string msgStopServer = "stopServer";
    std::string msgRoomNumber = "room ";
    std::string msgJoin = "join ";
    
private:
    std::unique_ptr<AsyncTCPServer> m_server;
    std::unique_ptr<AsyncTCPClient> m_client;
    
    const unsigned int m_serverPoolSize = 4;
    const unsigned int m_clientPoolSize = 4;
    
    std::string m_matchingServerIP;
    unsigned short int m_matchingServerPort;
    unsigned short int m_currentServerPort = 3030;
    
    // Holds current network state
    NetworkState m_networkState;
    
    // Holds id of session connected to matching server
    unsigned int m_matchingServerId = -1;
    
    // Room number assigned by matching server
    unsigned int m_roomNumber = -1;
    
    // Holds current board state
    std::string m_serializedBoard;
    
    void readHandler(std::string data);
    
public:
    
    NetworkChessApp(std::string matchingServerIP, unsigned short int matchingServerPort);
    
    void runServer();
    void stopServer();
    
    void join(int roomNum);
    
    void serializeBoard(const std::vector<std::string> &boardValues);
};

#endif /* NetworkChessApp_hpp */

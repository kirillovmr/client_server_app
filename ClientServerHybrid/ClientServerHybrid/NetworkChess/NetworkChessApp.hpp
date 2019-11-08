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
#include <atomic>
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
    std::string msgIP = "ip ";
    
private:
    std::unique_ptr<AsyncTCPServer> m_server;
    std::unique_ptr<AsyncTCPClient> m_client;
    
    const unsigned int m_serverPoolSize = 4;
    const unsigned int m_clientPoolSize = 4;
    
    // Matching server
    unsigned short int m_currentServerPort = 3030;
    
    
    // Joining server
    std::string m_gameServerIP;
    unsigned short int m_gameServerPort;
    std::atomic<bool> connectingToGameServer;
    
    // Holds current network state
    NetworkState m_networkState;
    
    // Holds id of sessions connected to matching server / game server
    short int m_matchingServerId = -1;
    short int m_gameServerId = -1;
    
    // Room number assigned by matching server
    short int m_roomNumber = -1;
    
    // Room number that we did last connection to
    short int m_lastGameRoomNum = -1;
    
    // Holds current board state
    std::string m_serializedBoard;
    
    // Async invokers
    void gameServerOnConnect();
    
    // Handlers
    void connGameServerHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec);
    void readGameServerHandler(std::string data);
    
public:
    
    NetworkChessApp();
    
    void runServer();
    void stopServer();
    
    // Join room
    void connect(std::string ip, unsigned short int port);
    // Leave room
    void disconnect();
    
    void serializeBoard(const std::vector<std::string> &boardValues);
};

#endif /* NetworkChessApp_hpp */

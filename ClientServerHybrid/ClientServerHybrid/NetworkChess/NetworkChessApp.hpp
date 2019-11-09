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

typedef std::function<void(const int board[64])> OnBoardChange;

class NetworkChessApp {
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Instantiation
// - - - - - -
private:
    enum NetworkState { ConnToMatchingServer, ConnToGameServer, RunningServer, Connecting };
    
    std::unique_ptr<AsyncTCPServer> m_server;
    std::unique_ptr<AsyncTCPClient> m_client;
    
    const unsigned int m_serverPoolSize = 4;
    const unsigned int m_clientPoolSize = 4;
    
public:
    
    NetworkChessApp(OnBoardChange onBoardChange = nullptr);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Client Side
// - - - - - -
private:
    std::string m_gameServerIP;
    unsigned short int m_gameServerPort;
    std::atomic<bool> m_connectingToGameServer;
    std::atomic<bool> m_cancelConnectionSent;
    
    // Id of game server session
    short int m_gameServerId = -1;
    
    // Handlers
    void readGameServerHandler(std::string data);
    void connGameServerHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec);
    
    // Async invokers
    void gameServerOnConnect();
    
public:
    
    // Join game room
    void connect(std::string ip, unsigned short int port);
    
    // Cancel joining game room
    void cancel();
    
    // Leave game room
    void disconnect();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Server Side
// - - - - - -
private:
    unsigned short int m_serverPort = 3030;
    
    // Handlers
    void serverReadHandler(std::string data);
    void serverConnectionHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec);
    
    // Async invokers
    void serverOnNewConnection();

public:
    
    std::string runServer();
    
    void stopServer();
    
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Game - related
// - - - - - -
private:
    int m_board[64];
    
    // Holds current board state
    std::string m_serializedBoard;
    
    // Initializes board
    void initStartBoard();
    
    void serializeBoard();
    
    // Decodes string to board array
    void deserializeBoard(const std::string &data);
    
    // On change handler
    OnBoardChange m_onBoardChange;
    
    std::string msgboardSenderHeader = "B|";
    
public:
    
    enum ChessType { wPawn = 11, wKnight, wBishop, wRock, wQueen, wKing,
                    bPawn = 21, bKnight, bBishop, bRock, bQueen, bKing };
    
    void makeMove(int idxFrom, int idxTo);
};

#endif /* NetworkChessApp_hpp */

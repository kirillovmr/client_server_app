//
//  NetworkChessApp.hpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/6/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#ifndef NetworkChessApp_hpp
#define NetworkChessApp_hpp

#include "AsyncTCPServer.hpp"
#include "AsyncTCPClient.hpp"

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <string>

// Ahead definition needed for State class
class NetworkChessApp;

// Handler type
typedef std::function<void(const int board[64])> OnBoardChange;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Holds current program state
// - - - - - -
class State {
private:
    friend class NetworkChessApp;
    NetworkChessApp *m_app;
    
    std::atomic_bool serverRunning;
    std::atomic_bool serverOppConnected;
    std::atomic_bool serverSpecConnected;
    
    std::atomic_bool clientRunning;
    std::atomic_bool clientConnecting;
    std::atomic_bool clientConnected;
    
    // Id of game server session
    std::atomic_short gameServerId;
    
    // Holds current board state
    std::string serializedBoard;
    
    int board[64];
    
public:
    // Initializes board to 0
    State(NetworkChessApp *app);
    
    // Setting all server related states to false
    void serverStop();
    
    // Setting all related client states to false
    void clientShutdown();
    
};


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main network wrapper
// - - - - - -
class NetworkChessApp {
private:
//->friend class State;
    
    State m_state;
    
    std::unique_ptr<AsyncTCPServer> m_server;
    std::unique_ptr<AsyncTCPClient> m_client;
    
    const unsigned int m_serverPoolSize = 4;
    const unsigned int m_clientPoolSize = 4;
    
    std::string m_gameServerIP;
    unsigned short int m_gameServerPort;
    unsigned short int m_serverPort = 3030;
    
    boost::asio::io_service m_ios;
    std::unique_ptr<boost::asio::steady_timer> m_timer;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::list<std::unique_ptr<std::thread>> m_thread_pool;
    
public:
    // Init worker and timer, creates work thread
    NetworkChessApp(OnBoardChange onBoardChange);
    
    // Destroys worker, joins thread(s)
    ~NetworkChessApp();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Client Side
// - - - - - -
private:
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
    void disconnect(bool bypass = false);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Server Side
// - - - - - -
private:
    // Handlers
    void serverReadHandler(std::string data);
    void serverConnectionHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec);
    
    // Async invokers
    void serverOnNewConnection();

public:
    // Start server, return ip where it was started
    std::string runServer(bool createNewBoard = true);
    
    // Stops server, destroys server instance
    void stopServer();
    
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Game - related
// - - - - - -
private:
    // Initializes board, saves into state
    void initStartBoard();
    
    // Encodes board array into string, saves into states
    void serializeBoard();
    
    // Decodes string to board array, saves into state
    void deserializeBoard(const std::string &data);
    
    // Called whenever any updates to board made
    OnBoardChange m_onBoardChange;
    
    // Message Headers needed for communication
    std::string msgboardSenderHeader = "B|";
    
public:
    // Chess types. 11-16 white, 21-26 black, 0 is an empty tile
    enum ChessType { wPawn = 11, wKnight, wBishop, wRock, wQueen, wKing,
                    bPawn = 21, bKnight, bBishop, bRock, bQueen, bKing };
    
    // Move chess @ idxFrom -> idxTo, reflects changes in state
    void makeMove(int idxFrom, int idxTo);
};

#endif /* NetworkChessApp_hpp */

//
//  NetworkChessApp.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/6/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkChessApp.hpp"

using namespace std;
using namespace std::placeholders;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// STATE
// - - - - - -
State::State(NetworkChessApp *app): serverRunning(false), serverOppConnected(false), serverSpecConnected(false),
    clientRunning(false), clientConnecting(false), clientConnected(false), m_app(app)
{
    for(int i=0; i<64; i++)
        board[i] = 0;
}

void State::serverStop() {
    serverRunning.store(false);
    serverOppConnected.store(false);
    serverSpecConnected.store(false);
}

void State::clientShutdown() {
    clientRunning.store(false);
    clientConnecting.store(false);
    clientConnected.store(false);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main network wrapper
// - - - - - -
NetworkChessApp::NetworkChessApp(OnBoardChange onBoardChange): m_serverPort(3030),
    m_onBoardChange(onBoardChange), m_state(this)
{
    m_work.reset(new boost::asio::io_service::work(m_ios));
    m_timer.reset(new boost::asio::steady_timer(m_ios));
    
    std::unique_ptr<std::thread> th( new std::thread([this](){
        m_ios.run();
    }));
    m_thread_pool.push_back(std::move(th));
}

NetworkChessApp::~NetworkChessApp() {
    m_work.reset(NULL);
    for (auto &thread : m_thread_pool)
        thread->join();
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Client Side
// private
// - - - - - -
void NetworkChessApp::readGameServerHandler(std::string data) {
    string cdata(data.begin()+2, data.end());
    
    cout << "Readed from GS: " << data << endl;
    
    // If got serialized board
    if (data.compare(0,2,msgboardSenderHeader) == 0) {
        m_state.serializedBoard = cdata;
        deserializeBoard(cdata);
        m_onBoardChange(m_state.board);
    }
}

void NetworkChessApp::connGameServerHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec) {
    if (ec == boost::asio::error::operation_aborted) {
        cout << "!> GS: Request #" << request_id << " has been cancelled by the user." << endl;
    }
    else if (ec == boost::asio::error::eof) {
        cout << "!> GS: connection broken. Reconnect..." << endl;
        
        m_timer->expires_after(std::chrono::seconds(3));
        m_timer->async_wait([this](const boost::system::error_code &error){
            if (!m_state.clientConnected.load()) {
                cout << "Reconnect deadline, switching to server\n";
                
                string ip = runServer(false);
                if (ip.length() > 0)
                    cout << "Server started, ip: " << ip << endl;
            }
        });
            
        m_state.clientConnected.store(false);
        connect(m_gameServerIP, m_gameServerPort);
    }
    else if (ec == boost::asio::error::connection_refused) {
        if (m_state.clientConnecting.load()) {
            cout << "!> GS: connection cancelled." << endl;
            m_state.clientConnecting.store(false);
        }
        else {
            cout << "!> GS: connection refused, Reconnect..." << endl;
            connect(m_gameServerIP, m_gameServerPort);
        }
    }
    else {
        cout << "!> GS: Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << endl;
    }
}

void NetworkChessApp::gameServerOnConnect() {
    m_state.clientConnecting.store(false);
    m_state.clientConnected.store(true);
    cout << "-> Connected to Game Server" << endl;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Client Side
// public
// - - - - - -
void NetworkChessApp::connect(string ip, unsigned short int port) {
    if (!m_server) {
        if (!m_client) {
            m_client.reset(new AsyncTCPClient(m_clientPoolSize));
            m_state.clientRunning.store(true);
        }
        
        m_gameServerIP = ip;
        m_gameServerPort = port;
        
        cout << "-> Joining gameServer at " << ip << ":" << port << endl;
        m_state.clientConnecting.store(true);
        
        short gameId = m_client->connect(ip, port,
            bind(&NetworkChessApp::connGameServerHandler, this, _1, _2, _3),
            bind(&NetworkChessApp::readGameServerHandler, this, _1),
            bind(&NetworkChessApp::gameServerOnConnect, this));
        m_state.gameServerId.store(gameId);
    }
    else
        cout << "Server is running. Stop server before opening a connection." << endl;
}

void NetworkChessApp::cancel() {
    if (m_state.clientConnecting.load())
        m_client->disconnect(m_state.gameServerId.load());
    else
        cout << "Cannot cancel connecting to Game Server because we are not connecting." << endl;
}

void NetworkChessApp::disconnect(bool bypass) {
    if (m_state.clientConnected.load() || bypass) {
        m_client->disconnect(m_state.gameServerId.load());
        m_state.clientConnected.store(false);
    }
    else
        cout << "Can not disconenct from Game Server because we are not connected." << endl;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Server Side
// private
// - - - - - -
void NetworkChessApp::serverReadHandler(std::string data) {
    string cdata(data.begin()+2, data.end());
    
    cout << "Readed from Client: " << data << endl;
    
    // If got serialized board
    if (data.compare(0,2,msgboardSenderHeader) == 0) {
        m_state.serializedBoard = cdata;
        deserializeBoard(cdata);
        m_onBoardChange(m_state.board);
    }
}

void NetworkChessApp::serverConnectionHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec) {
    if (ec == boost::asio::error::operation_aborted) {
        cout << "!> Client: Request #" << request_id << " has been cancelled by the user." << endl;
    } else if (ec == boost::asio::error::eof) {
        cout << "!> Client: connection broken." << endl;
    } else if (ec == boost::asio::error::connection_refused) {
        cout << "!> Client: connection refused." << endl;
    } else {
        cout << "!> Client: Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << endl;
    }
    m_state.serverOppConnected.store(false);
}

void NetworkChessApp::serverOnNewConnection() {
    cout << "-> New client connected." << endl;
    string data(msgboardSenderHeader + m_state.serializedBoard);
    m_server->transmit(data);
    
    if (m_state.serverOppConnected.load())
        m_state.serverSpecConnected.store(true);
    else
        m_state.serverOppConnected.store(true);
    
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Server Side
// public
// - - - - - -
string NetworkChessApp::runServer(bool createNewBoard) {
    if (!m_state.serverRunning.load()) {
        
        // Shutdown client
        if (m_state.clientRunning.load()) {
            m_client->disconnectAll();
            m_client.reset(nullptr);
            m_state.clientShutdown();
        }
    
        // Init board
        if (createNewBoard) {
            initStartBoard();
            serializeBoard();
            m_onBoardChange(m_state.board);
        }
    
        // Starting server
        m_server.reset(new AsyncTCPServer(m_serverPoolSize));
        m_server->start(m_serverPort,
                bind(&NetworkChessApp::serverConnectionHandler, this, _1, _2, _3),
                bind(&NetworkChessApp::serverReadHandler, this, _1),
                bind(&NetworkChessApp::serverOnNewConnection, this));
        
        m_state.serverRunning.store(true);
        
        return m_server->getAddresses()[0];
    }
    else
        cout << "Server is already running." << endl;
    return "";
}

void NetworkChessApp::stopServer() {
    if (m_state.serverRunning.load()) {
        m_server.reset(nullptr);
        m_state.serverStop();
        cout << "Server successfully stopped." << endl;
    }
    else
        cout << "Server is not running." << endl;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Game - related
// private
// - - - - - -
void NetworkChessApp::initStartBoard() {
    for(int i=0; i<64; i++)
        m_state.board[i] = 0;
    
    for(int i=8; i<16; i++) {
        m_state.board[i] = ChessType::bPawn;
        m_state.board[i + 8*5] = ChessType::wPawn;
    }
    
    m_state.board[0] = m_state.board[7] = ChessType::bRock;
    m_state.board[56] = m_state.board[63] = ChessType::wRock;
    
    m_state.board[1] = m_state.board[6] = ChessType::bKnight;
    m_state.board[57] = m_state.board[62] = ChessType::wKnight;
    
    m_state.board[2] = m_state.board[5] = ChessType::bBishop;
    m_state.board[58] = m_state.board[61] = ChessType::wBishop;
    
    m_state.board[3] = ChessType::bQueen;
    m_state.board[4] = ChessType::bKing;
    
    m_state.board[59] = ChessType::wQueen;
    m_state.board[60] = ChessType::wKing;
}

void NetworkChessApp::serializeBoard() {
    stringstream serial;
    
    for (int i=0; i<64; i++)
        serial << m_state.board[i];
    m_state.serializedBoard = serial.str();
}

void NetworkChessApp::deserializeBoard(const std::string &data) {
    int n, j=0;
    for (int i=0; i<data.length(); i++) {
        n = data[i]-'0';
        if (n>0)
            n = n*10 + data[++i]-'0';
        m_state.board[j++] = n;
    }
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Game - related
// public
// - - - - - -

void NetworkChessApp::makeMove(int idxFrom, int idxTo) {
    if (idxFrom >= 0 && idxFrom <= 63 && idxTo >= 0 && idxTo <= 63) {
        m_state.board[idxTo] = m_state.board[idxFrom];
        m_state.board[idxFrom] = 0;
        
        serializeBoard();
        m_onBoardChange(m_state.board);
        
        string data(msgboardSenderHeader + m_state.serializedBoard);
        cout << "Sending " << data << endl;
        if (m_state.serverRunning.load())
            m_server->transmit(data);
        else if (m_state.clientConnected.load())
            m_client->transmit(m_state.gameServerId.load(), data);
        else
            cout << "Cannot send.";
    }
}

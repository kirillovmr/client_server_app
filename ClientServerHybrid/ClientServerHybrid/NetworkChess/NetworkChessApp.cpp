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


void requestHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec) {
    if (!ec) {
        std::cout << "Request #" << request_id << " has completed. Response: " << response << std::endl;
    } else if (ec == boost::asio::error::operation_aborted) {
        std::cout << "Request #" << request_id << " has been cancelled by the user." << std::endl;
    } else {
        std::cout << "Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << std::endl;
    }
    return;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Instantiation
// - - - - - -
NetworkChessApp::NetworkChessApp(OnBoardChange onBoardChange): m_serverPort(3030),
    m_connectingToGameServer(false), m_cancelConnectionSent(false),
    m_onBoardChange(onBoardChange)
{
    for(int i=0; i<64; i++)
        m_board[i] = 0;
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
        m_serializedBoard = cdata;
        deserializeBoard(cdata);
        if(m_onBoardChange)
            m_onBoardChange(m_board);
    }
}

void NetworkChessApp::connGameServerHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec) {
    if (ec == boost::asio::error::operation_aborted) {
        cout << "!> GS: Request #" << request_id << " has been cancelled by the user." << endl;
    } else if (ec == boost::asio::error::eof) {
        cout << "!> GS: connection broken. Reconnect..." << endl;
        connect(m_gameServerIP, m_gameServerPort);
    } else if (ec == boost::asio::error::connection_refused) {
        if (m_cancelConnectionSent.load()) {
            cout << "!> GS: connection cancelled." << endl;
            m_cancelConnectionSent.store(false);
        }
        else {
            cout << "!> GS: connection refused, Reconnect..." << endl;
            connect(m_gameServerIP, m_gameServerPort);
        }
    } else {
        cout << "!> GS: Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << endl;
    }
}

void NetworkChessApp::gameServerOnConnect() {
    m_connectingToGameServer.store(false);
    cout << "-> Connected to Game Server" << endl;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Client Side
// public
// - - - - - -
void NetworkChessApp::connect(string ip, unsigned short int port) {
    if (!m_server) {
        if (!m_client)
            m_client.reset(new AsyncTCPClient(m_clientPoolSize));
        
        m_gameServerIP = ip;
        m_gameServerPort = port;
        
        cout << "-> Joining gameServer at " << ip << ":" << port << endl;
        m_connectingToGameServer.store(true);
        m_gameServerId = m_client->connect(ip, port,
                bind(&NetworkChessApp::connGameServerHandler, this, _1, _2, _3),
                bind(&NetworkChessApp::readGameServerHandler, this, _1),
                bind(&NetworkChessApp::gameServerOnConnect, this));
    }
    else
        cout << "Server is running. Stop server before opening a connection." << endl;
}

void NetworkChessApp::cancel() {
    if (m_connectingToGameServer.load()) {
        m_client->disconnect(m_gameServerId);
        m_cancelConnectionSent.store(true);
    }
    else
        cout << "Cannot cancel connection to Game Server because we are not connecting." << endl;
}

void NetworkChessApp::disconnect() {
    if (m_gameServerId != -1) {
        m_client->disconnect(m_gameServerId);
        m_gameServerId = -1;
//        cout << "Disconnected from game server." << endl;
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
        m_serializedBoard = cdata;
        deserializeBoard(cdata);
        if(m_onBoardChange)
            m_onBoardChange(m_board);
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
    m_gameServerId = -1;
}

void NetworkChessApp::serverOnNewConnection() {
    cout << "-> New client connected." << endl;
    string data(msgboardSenderHeader + m_serializedBoard);
    m_server->transmit(data);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Server Side
// public
// - - - - - -
string NetworkChessApp::runServer() {
    if (!m_server) {
        
        // Destory client
        if (m_client) {
            m_client->disconnectAll();
            m_client.reset(nullptr);
        }
        
        // Init board
        initStartBoard();
        serializeBoard();
        if(m_onBoardChange)
            m_onBoardChange(m_board);
        
        // Starting server
        m_server.reset(new AsyncTCPServer(m_serverPoolSize));
        m_server->start(m_serverPort,
                bind(&NetworkChessApp::serverConnectionHandler, this, _1, _2, _3),
                bind(&NetworkChessApp::serverReadHandler, this, _1),
                bind(&NetworkChessApp::serverOnNewConnection, this));
        return m_server->getAddresses()[0];
    }
    else
        cout << "Server is already running." << endl;
    return "";
}

void NetworkChessApp::stopServer() {
    if (m_server) {
        m_server.reset(nullptr);
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
        m_board[i] = 0;
    
    for(int i=8; i<16; i++) {
        m_board[i] = ChessType::bPawn;
        m_board[i + 8*5] = ChessType::wPawn;
    }
    
    m_board[0] = m_board[7] = ChessType::bRock;
    m_board[56] = m_board[63] = ChessType::wRock;
    
    m_board[1] = m_board[6] = ChessType::bKnight;
    m_board[57] = m_board[62] = ChessType::wKnight;
    
    m_board[2] = m_board[5] = ChessType::bBishop;
    m_board[58] = m_board[61] = ChessType::wBishop;
    
    m_board[3] = ChessType::bQueen;
    m_board[4] = ChessType::bKing;
    
    m_board[59] = ChessType::wQueen;
    m_board[60] = ChessType::wKing;
}

void NetworkChessApp::serializeBoard() {
    stringstream serial;
    
    for (int i=0; i<64; i++)
        serial << m_board[i];
    m_serializedBoard = serial.str();
}

void NetworkChessApp::deserializeBoard(const std::string &data) {
    int n, j=0;
    for (int i=0; i<data.length(); i++) {
        n = data[i]-'0';
        if (n>0)
            n = n*10 + data[++i]-'0';
        m_board[j++] = n;
    }
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Game - related
// public
// - - - - - -

void NetworkChessApp::makeMove(int idxFrom, int idxTo) {
    if (idxFrom >= 0 && idxFrom <= 63 && idxTo >= 0 && idxTo <= 63) {
        m_board[idxTo] = m_board[idxFrom];
        m_board[idxFrom] = 0;
        
        serializeBoard();
        string data(msgboardSenderHeader + m_serializedBoard);
        cout << "Sending " << data << endl;
        if (m_server)
            m_server->transmit(data);
        else if (m_client && m_gameServerId >= 0)
            m_client->transmit(m_gameServerId, data);
        else
            cout << "Cannot send.";
    }
}

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

void NetworkChessApp::connGameServerHandler(unsigned int request_id, const std::string &response, const boost::system::error_code &ec) {
    if (ec == boost::asio::error::operation_aborted) {
        cout << "!> GS: Request #" << request_id << " has been cancelled by the user." << endl;
    } else if (ec == boost::asio::error::eof) {
        cout << "!> GS: connection broken. Reconnect..." << endl;
        connect(m_gameServerIP, m_gameServerPort);
    } else if (ec == boost::asio::error::connection_refused) {
        cout << "!> GS: connection refused, Reconnect..." << endl;
        connect(m_gameServerIP, m_gameServerPort);
    } else {
        cout << "!> GS: Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << endl;
    }
}

void NetworkChessApp::readGameServerHandler(std::string data) {
    cout << "Readed from GS: " << data << endl;
    string temp(data);
}

NetworkChessApp::NetworkChessApp():
        m_currentServerPort(3030), m_networkState(NetworkState::Connecting),
        connectingToGameServer(false)
{

}


void NetworkChessApp::gameServerOnConnect() {
    m_networkState = NetworkState::ConnToMatchingServer;
    cout << "-> Connected to Game Server" << endl;
}


void NetworkChessApp::runServer() {
    if (!m_server) {
        
        // Destory client
        if (m_client) {
            m_client.reset(nullptr);
        }
        
        // Starting server
        m_server.reset(new AsyncTCPServer(m_serverPoolSize));
        m_server->start(m_currentServerPort, requestHandler);
    }
}


void NetworkChessApp::stopServer() {
    if (m_server) {
        m_server.reset(nullptr);
    }
}

void NetworkChessApp::connect(string ip, unsigned short int port) {
    // Creating client
    if (!m_client)
        m_client.reset(new AsyncTCPClient(m_clientPoolSize));
    
    m_gameServerIP = ip;
    m_gameServerPort = port;
    
    cout << "-> Joining gameServer at " << ip << ":" << port << endl;
    m_gameServerId = m_client->connect(ip, port,
            bind(&NetworkChessApp::connGameServerHandler, this, _1, _2, _3),
            bind(&NetworkChessApp::readGameServerHandler, this, _1),
            bind(&NetworkChessApp::gameServerOnConnect, this));
}

void NetworkChessApp::disconnect() {
    if (m_gameServerId != -1) {
        m_client->disconnect(m_gameServerId);
        m_gameServerId = -1;
    }
}


void NetworkChessApp::serializeBoard(const vector<string> &boardValues) {
    stringstream serial;
    
    for (auto &pair : boardValues)
        serial << pair;
    
    m_serializedBoard = serial.str();
}


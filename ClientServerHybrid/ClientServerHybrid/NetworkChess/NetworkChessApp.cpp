//
//  NetworkChessApp.cpp
//  ClientServerHybrid
//
//  Created by Viktor Kirillov on 11/6/19.
//  Copyright © 2019 Viktor Kirillov. All rights reserved.
//

#include "NetworkChessApp.hpp"

using namespace std;

void requestHandler(unsigned int request_id, const std::string& response, const boost::system::error_code& ec) {
    if (!ec) {
        std::cout << "Request #" << request_id << " has completed. Response: " << response << std::endl;
    } else if (ec == boost::asio::error::operation_aborted) {
        std::cout << "Request #" << request_id << " has been cancelled by the user." << std::endl;
    } else {
        std::cout << "Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << std::endl;
    }
    return;
}


NetworkChessApp::NetworkChessApp(string matchingServerIP, unsigned short int matchingServerPort):
        m_matchingServerIP(matchingServerIP), m_matchingServerPort(matchingServerPort),
        m_currentServerPort(3030), m_networkState(NetworkState::Connecting)
{
    // Creating client
    m_client.reset(new AsyncTCPClient(m_clientPoolSize));
    
    // Connecting to matching server
    m_matchingServerId = m_client->connect(matchingServerIP, matchingServerPort, requestHandler, bind(&NetworkChessApp::readHandler, this, placeholders::_1));
    m_networkState = NetworkState::ConnToMatchingServer;
}


void NetworkChessApp::runServer() {
    // If connected to Matching server
    if (m_networkState == NetworkState::ConnToMatchingServer) {
        
        // If server is not yet running
        if (!m_server) {
            
            // Starting server
            m_server.reset(new AsyncTCPServer(m_serverPoolSize));
            m_server->start(m_currentServerPort, requestHandler);
            
            // Notifying matching server that we will work as a server
            m_client->write(m_matchingServerId, msgStartServer);
            
            
        }
    }
}


void NetworkChessApp::stopServer() {
    // If connected to Matching server
    if (m_networkState == NetworkState::ConnToMatchingServer) {
        
        // If server is running
        if (m_server) {
            
            // Notifying matching server
            m_client->write(m_matchingServerId, msgStopServer);
            
            // Stop server
            m_server.reset(nullptr);
        }
    }
}


void NetworkChessApp::join(int roomNum) {
    // If no server instance running
//    if (!m_server) {
        string msg(msgJoin + to_string(roomNum));
        m_client->write(m_matchingServerId, msg);
//    }
}


void NetworkChessApp::serializeBoard(const vector<string> &boardValues) {
    stringstream serial;
    
    for (auto &pair : boardValues)
        serial << pair;
    
    m_serializedBoard = serial.str();
    
    
}


void NetworkChessApp::readHandler(std::string data) {
    cout << "Readed: " << data << endl;
    string temp(data);
    
    // "room 13"
    if (temp.length() >= msgRoomNumber.length()) {
        if (temp.substr(0,5) == msgRoomNumber) {
            temp.erase(0,5);
            m_roomNumber = stoi(temp);
            cout << "-> Room number set to " << m_roomNumber << endl;
        }
    }
}

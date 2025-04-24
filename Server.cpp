#include "Server.h"

#include <stdexcept>
#include <iostream>

Server::Server(int virtualPort)
{
    listenHandle = SteamNetworkingSockets()->CreateListenSocketP2P(virtualPort, 0, nullptr);
    pollGroupHandle = SteamNetworkingSockets()->CreatePollGroup();
}

Server::~Server()
{
    SteamNetworkingSockets()->DestroyPollGroup(pollGroupHandle);
    SteamNetworkingSockets()->CloseListenSocket(listenHandle);
}

void Server::receiveMessages()
{
    SteamNetworkingMessage_t* messages = nullptr;
    int messageCount = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(
	    pollGroupHandle, &messages, 10);

    for (int i = 0; i < messageCount; ++i) {
	Client& client = clients[getClientIndex(messages[i].m_conn)];
	client.handleMessage(messages[i]);
    }
}

void Server::sendMessageToAll(uint32_t value)
{
    for (Client& client : clients) {
	client.sendMessage(value);
    }
}

void Server::onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback)
{
    // connection attempt
    if (callback->m_info.m_hListenSocket == listenHandle
	&& callback->m_eOldState == k_ESteamNetworkingConnectionState_None
	&& callback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {

	SteamNetworkingSockets()->AcceptConnection(callback->m_hConn);
	clients.emplace_back(callback->m_hConn);
	SteamNetworkingSockets()->SetConnectionPollGroup(callback->m_hConn, pollGroupHandle);

	std::cout << clients.back().getName() << " connected\n";

    // connection closed by local or remote host
    } else if (callback->m_eOldState == k_ESteamNetworkingConnectionState_Connected
	&& (callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer
	|| callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)) {

	try {
	    size_t clientIndex = getClientIndex(callback->m_hConn);
	    std::cout << clients[clientIndex].getName() << " disconnected\n";
	    clients.erase(clients.begin() + clientIndex);
	} catch(std::runtime_error& error) {}
    }
}

size_t Server::getClientIndex(HSteamNetConnection connectionHandle)
{
    for (size_t i = 0; i < clients.size(); ++i) {
	if (clients[i].getConnectionHandle() == connectionHandle) {
	    return i;
	}
    }

    throw std::runtime_error("Failed to find connected client with given handle");
    return 0;
}

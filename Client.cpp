#include "Client.h"

#include <iostream>

Client::Client(HSteamNetConnection connectionHandle) :
    connectionHandle(connectionHandle)
{
}

Client::Client(CSteamID hostId, int virtualPort)
{
    SteamNetworkingIdentity identity = {};
    identity.SetSteamID(hostId);
    connectionHandle = SteamNetworkingSockets()->ConnectP2P(identity, virtualPort, 0, nullptr);
}

Client::~Client()
{
    SteamNetworkingSockets()->CloseConnection(connectionHandle, 0, nullptr, false);
}

void Client::sendMessage(uint32_t value)
{
    EResult result = SteamNetworkingSockets()->SendMessageToConnection(
	    connectionHandle, &value, sizeof(value), 0, nullptr);
    if (result != k_EResultOK) {
	std::cerr << "STEAM failed to send message in Client.cpp\n";
    }
}

void Client::receiveMessages()
{
    SteamNetworkingMessage_t* messages = nullptr;
    int messageCount = SteamNetworkingSockets()->ReceiveMessagesOnConnection(
	    connectionHandle, &messages, 10);

    for (int i = 0; i < messageCount; ++i) {
	handleMessage(messages[i]);
    }
}

void Client::handleMessage(SteamNetworkingMessage_t& message)
{
    if (message.m_cbSize == sizeof(uint32_t)) {
	uint32_t value = *reinterpret_cast<uint32_t*>(message.m_pData);
	std::cout << getName() << " message: " << value << std::endl;
    } else {
	std::cout << "Unknown message size\n";
    }

    message.Release();
}

std::string Client::getName() const
{
    SteamNetConnectionInfo_t info = {};
    if (SteamNetworkingSockets()->GetConnectionInfo(connectionHandle, &info)) {
	CSteamID id = info.m_identityRemote.GetSteamID();
	if (id.IsValid()) {
	    return SteamFriends()->GetFriendPersonaName(id);
	}
    }

    return "unknownName";
}

HSteamNetConnection Client::getConnectionHandle()
{
    return connectionHandle;
}

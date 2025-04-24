#pragma once

#include <string>

#include "steam/steam_api.h"

class Client
{
public:
    // Constructor for when the connection is already made
    Client(HSteamNetConnection connectionHandle);
    
    // Constructor for creating a connection with the host
    Client(CSteamID hostId, int virtualPort = 0);

    // Closes the connection
    ~Client();

    void sendMessage(uint32_t value);

    void receiveMessages();

    void handleMessage(SteamNetworkingMessage_t& message);

    std::string getName() const;

    HSteamNetConnection getConnectionHandle();

private:
    HSteamNetConnection connectionHandle;
};

#pragma once

#include <vector>
#include <cstddef>

#include "Client.h"

class Server
{
public:
    Server(int virtualPort = 0);

    ~Server();

    void receiveMessages();

    void sendMessageToAll(uint32_t value);

private:
    HSteamListenSocket listenHandle;
    std::vector<Client> clients;
    HSteamNetPollGroup pollGroupHandle;

    size_t getClientIndex(HSteamNetConnection connectionHandle);

    STEAM_CALLBACK(Server, onConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
};

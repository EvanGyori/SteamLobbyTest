#pragma once

#include "Lobby.h"

#include "steam/steam_api.h"

class Multiplayer
{
public:
    void sendValueToAll(const Lobby& lobby, uint32_t value);

    void receiveMessages();

private:
    STEAM_CALLBACK(Multiplayer, onSteamNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t);
};

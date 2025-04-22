#pragma once

#include <string>

#include "steam/steam_api.h"

class Lobby
{
public:
    Lobby();

    void joinLobby(CSteamID lobbyId);

    void createLobby(const char* mapName);

    void leaveLobby();

    void sendMessage(const char* message);

    bool isInLobby() const;

    bool isHost() const;

    CSteamID getLobbyId() const;

private:
    CSteamID lobbyId;
    bool isHostFlag;
    std::string mapName;

    void onLobbyEnter(LobbyEnter_t* callback, bool bIOFailure);
    CCallResult<Lobby, LobbyEnter_t> lobbyEnterCallResult;

    void onLobbyCreated(LobbyCreated_t* callback, bool bIOFailure);
    CCallResult<Lobby, LobbyCreated_t> lobbyCreatedCallResult;
};

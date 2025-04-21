#pragma once

#include <string>

#include "Lobby.h"
#include "Multiplayer.h"

class Manager
{
public:
    Manager();

    ~Manager();
    
    // Returns true if init is successful,
    // false otherwise
    bool init();

    void run();

private:
    Lobby lobby;
    Multiplayer multiplayer;
    std::string mapName;
    bool wasSpacePressed, wasWPressed;

    STEAM_CALLBACK(Manager, onLobbyChatMsg, LobbyChatMsg_t);
    STEAM_CALLBACK(Manager, onLobbyChatUpdate, LobbyChatUpdate_t);

    void onLobbyMatchList(LobbyMatchList_t* callback, bool bIOFailure);
    CCallResult<Manager, LobbyMatchList_t> lobbyMatchListCallResult;
};

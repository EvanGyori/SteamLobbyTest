#pragma once

#include <string>

#include "Lobby.h"
//#include "Multiplayer.h"
#include "Server.h"
#include "Client.h"

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
    //Multiplayer multiplayer;
    Client* client;
    Server* server;
    std::string mapName;
    bool wasSpacePressed, wasWPressed;

    STEAM_CALLBACK(Manager, onLobbyChatMsg, LobbyChatMsg_t);
    STEAM_CALLBACK(Manager, onLobbyChatUpdate, LobbyChatUpdate_t);

    void onLobbyMatchList(LobbyMatchList_t* callback, bool bIOFailure);
    CCallResult<Manager, LobbyMatchList_t> lobbyMatchListCallResult;
};

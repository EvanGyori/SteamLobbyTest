#include "Lobby.h"

#include <cstring>
#include <iostream>

Lobby::Lobby() :
    lobbyId()
{
}

void Lobby::joinLobby(CSteamID lobbyId)
{
    SteamAPICall_t callResult = SteamMatchmaking()->JoinLobby(lobbyId);
    lobbyEnterCallResult.Set(callResult, this, &Lobby::onLobbyEnter);

    std::cout << "Joining lobby...\n";
}

void Lobby::createLobby(const char* mapName)
{
    SteamAPICall_t callResult = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 2);
    lobbyCreatedCallResult.Set(callResult, this, &Lobby::onLobbyCreated);
    this->mapName = mapName;

    std::cout << "Creating lobby...\n";
}

void Lobby::leaveLobby()
{
    SteamMatchmaking()->LeaveLobby(lobbyId);
    lobbyId = CSteamID();
}

void Lobby::sendMessage(const char* message)
{
    bool success = SteamMatchmaking()->SendLobbyChatMsg(lobbyId,
	    reinterpret_cast<const void*>(message), strlen(message) + 1);

    if (!success) {
	std::cerr << "STEAM failed to send lobby chat message\n";
    }
}

bool Lobby::isInLobby() const
{
    // Could also try IsValid()
    return lobbyId.ConvertToUint64() != 0;
}

CSteamID Lobby::getHostId() const
{
    return SteamMatchmaking()->GetLobbyOwner(lobbyId);
}

bool Lobby::isHost() const
{
    return getHostId() == SteamUser()->GetSteamID();
}

CSteamID Lobby::getLobbyId() const
{
    return lobbyId;
}

void Lobby::onLobbyEnter(LobbyEnter_t* callback, bool bIOFailure)
{
    if (!bIOFailure && callback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess) {
	lobbyId = callback->m_ulSteamIDLobby;
	std::cout << "Lobby successfully joined\n";
    } else {
	std::cerr << "STEAM failed to join lobby\n";
    }
}

void Lobby::onLobbyCreated(LobbyCreated_t* callback, bool bIOFailure)
{
    if (!bIOFailure && callback->m_eResult == k_EResultOK) {
	lobbyId = callback->m_ulSteamIDLobby;
	SteamMatchmaking()->SetLobbyData(lobbyId, "map", mapName.c_str());
	std::cout << "Lobby successfully created\n";
    } else {
	std::cerr << "STEAM failed to create lobby\n";
    }
}

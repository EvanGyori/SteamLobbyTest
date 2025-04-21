#include "Multiplayer.h"

#include <iostream>

void Multiplayer::sendValueToAll(const Lobby& lobby, uint32_t value)
{
    CSteamID lobbyId = lobby.getLobbyId();
    int playerCount = SteamMatchmaking()->GetNumLobbyMembers(lobbyId);

    for (int i = 0; i < playerCount; i++) {
	CSteamID playerId = SteamMatchmaking()->GetLobbyMemberByIndex(lobbyId, i);
	if (playerId != SteamUser()->GetSteamID()) {
	    SteamNetworkingIdentity identity = {};
	    identity.SetSteamID(playerId);
	    EResult result =
		SteamNetworkingMessages()->SendMessageToUser(identity, &value, sizeof(value),
			k_nSteamNetworkingSend_Reliable, 0);

	    if (result != k_EResultOK) {
		std::cerr << "STEAM failed to send message: " << result << std::endl;
	    }
	}
    }
}

void Multiplayer::receiveMessages()
{
    SteamNetworkingMessage_t* messages;
    int messageCount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, &messages, 10);
    for (int i = 0; i < messageCount; i++) {
	if (messages[i].m_cbSize == sizeof(uint32_t)) {
	    uint32_t value = *reinterpret_cast<uint32_t*>(messages[i].m_pData);
	    std::cout << "Message sent: " << value << std::endl;
	} else {
	    std::cout << "Unknown message\n";
	}
	messages[i].Release();
    }
}

void Multiplayer::onSteamNetworkingMessagesSessionRequest(
	SteamNetworkingMessagesSessionRequest_t* callback)
{
    SteamNetworkingMessages()->AcceptSessionWithUser(callback->m_identityRemote);
}

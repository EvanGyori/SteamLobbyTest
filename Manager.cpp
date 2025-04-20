#include "Manager.h"

#include <iostream>
#include <vector>
#include <cstdlib>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

const char* getPlayerName(CSteamID steamId)
{
    return SteamFriends()->GetFriendPersonaName(steamId);
}

#ifdef __linux__
void setNonBlockingMode(bool enable) {
    static struct termios oldt, newt;

    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt); // save old settings
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // disable buffering and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // Set non-blocking mode
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    } else {
        // Restore old settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, 0); // remove O_NONBLOCK
    }
}

bool keyPressed(char &c) {
    c = getchar();
    if (c != EOF) {
        return true;
    }
    return false;
}
#endif

void DisableConsoleInput() {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    // Disable all input processing flags
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE);
    SetConsoleMode(hStdin, mode);
#elif __linux__
    setNonBlockingMode(true);
#endif
}

void EnableConsoleInput() {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    // Disable all input processing flags
    mode |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE);
    SetConsoleMode(hStdin, mode);

    FlushConsoleInputBuffer(hStdin);
#elif __linux__
    setNonBlockingMode(false);
#endif
}

Manager::Manager() :
    wasSpacePressed(false)
{
}

Manager::~Manager()
{
    SteamAPI_Shutdown();
}

bool Manager::init()
{
    if (!SteamAPI_Init()) {
	std::cerr << "STEAM failed to init\n";
	return false;
    }

    return true;
}

void Manager::run()
{
    std::cout << "Map: ";
    std::cin >> mapName;
    DisableConsoleInput();

    SteamMatchmaking()->AddRequestLobbyListStringFilter("map", mapName.c_str(),
	    k_ELobbyComparisonEqual);
    SteamAPICall_t callResult = SteamMatchmaking()->RequestLobbyList();
    lobbyMatchListCallResult.Set(callResult, this, &Manager::onLobbyMatchList);

    std::cout << "Fetching lobbies...\n";

    while (true) {
	SteamAPI_RunCallbacks();

#ifdef _WIN32
	bool isSpacePressed = GetAsyncKeyState(VK_SPACE) & 0x8000;
#elif __linux__
	char key = 0;
	keyPressed(key);
	bool isSpacePressed = key == ' ';
#endif
	if (lobby.isInLobby() && wasSpacePressed && !isSpacePressed) {
	    std::cout << ":";
	    std::string message;

	    EnableConsoleInput();
	    std::cin >> message;
	    DisableConsoleInput();

	    lobby.sendMessage(message.c_str());
	}

	wasSpacePressed = isSpacePressed;
    }
}

void Manager::onLobbyChatMsg(LobbyChatMsg_t* callback)
{
    if (callback->m_eChatEntryType == k_EChatEntryTypeChatMsg) {
	std::vector<char> message(4000);
	int messageSize = SteamMatchmaking()->GetLobbyChatEntry(
		callback->m_ulSteamIDLobby, callback->m_iChatID,
		nullptr, &message.front(), message.size() - 1, nullptr);

	std::cout << getPlayerName(callback->m_ulSteamIDUser) << ": " << &message.front() << std::endl;
    } else {
	std::cerr << "STEAM unknown message type\n";
    }
}

void Manager::onLobbyChatUpdate(LobbyChatUpdate_t* callback)
{
    const char* name = getPlayerName(callback->m_ulSteamIDUserChanged);
    switch (callback->m_rgfChatMemberStateChange) {
	case k_EChatMemberStateChangeEntered:
	    std::cout << name << " joined\n";
	    break;
	case k_EChatMemberStateChangeLeft:
	    std::cout << name << " left\n";
	    break;
	case k_EChatMemberStateChangeDisconnected:
	    std::cout << name << " disconnected\n";
	    break;
    }
}

void Manager::onLobbyMatchList(LobbyMatchList_t* callback, bool bIOFailure)
{
    if (bIOFailure) {
	std::cout << "Failed to fetch lobbies\n";
    } else if (callback->m_nLobbiesMatching > 0) {
	CSteamID lobbyId = SteamMatchmaking()->GetLobbyByIndex(0);
	lobby.joinLobby(lobbyId);
    } else {
	std::cout << "No lobbies found\n";
	lobby.createLobby(mapName.c_str());
    }
}

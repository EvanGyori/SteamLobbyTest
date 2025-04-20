cmake_minimum_required(VERSION 3.20)
project(SteamLobbyTest LANGUAGES CXX)

if(WIN32)
    set(steamlib "steam_api64.lib")
else()
    set(steamlib "libsteam_api.so")
endif()

add_executable(lobbyTest main.cpp Lobby.cpp Manager.cpp)
target_link_libraries(lobbyTest PUBLIC "${CMAKE_SOURCE_DIR}/${steamlib}")

add_custom_command(TARGET lobbyTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/steam_appid.txt"
    "$<TARGET_FILE_DIR:lobbyTest>/steam_appid.txt"
)

cmake_minimum_required(VERSION 3.10)
project(BattleshipGame C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread -D_POSIX_C_SOURCE=200809L")

# Common library
add_library(common STATIC
    common/serialize.c
    common/game_logic.c
    common/ship_shapes.c
    common/utils.c
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

# Server executable
add_executable(server
    server/main.c
    server/server.c
)

target_link_libraries(server common pthread)
target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/server
)

# Client executable
add_executable(client
    client/main.c
    client/client.c
    client/ui.c
)

target_link_libraries(client common pthread)
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)


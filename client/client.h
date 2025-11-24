#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include "game_logic.h"
#include <stdbool.h>

// Client state
typedef enum {
    CLIENT_STATE_CONNECTING,
    CLIENT_STATE_WAITING,
    CLIENT_STATE_PLACEMENT,
    CLIENT_STATE_PLAYING,
    CLIENT_STATE_FINISHED
} ClientState;

// Game client
typedef struct {
    int server_fd;
    ClientState state;
    
    Board own_board;
    Board opponent_board;
    
    bool my_turn;
    bool running;
    
    char player_name[32];
} GameClient;

// Initialize client
bool client_init(GameClient* client, const char* host, int port, const char* player_name);

// Run client (blocks)
void client_run(GameClient* client);

// Cleanup client
void client_cleanup(GameClient* client);

#endif // CLIENT_H


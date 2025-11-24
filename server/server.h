#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"
#include "game_logic.h"
#include <pthread.h>
#include <stdbool.h>

// Game state
typedef enum {
    GAME_STATE_WAITING,      // Waiting for 2 players
    GAME_STATE_PLACEMENT,    // Players placing ships
    GAME_STATE_PLAYING,      // Game in progress
    GAME_STATE_FINISHED      // Game over
} GameState;

// Player info
typedef struct {
    int fd;
    char name[32];
    Board board;
    bool ready;
    bool connected;
    pthread_t thread;
} Player;

// Game server
typedef struct {
    int listen_fd;
    int port;
    
    Player players[2];
    int player_count;
    int current_turn; // 0 or 1
    
    GameState state;
    
    pthread_mutex_t mutex;
    bool running;
} GameServer;

// Initialize server
bool server_init(GameServer* server, int port);

// Start server (blocks)
void server_run(GameServer* server);

// Cleanup server
void server_cleanup(GameServer* server);

#endif // SERVER_H


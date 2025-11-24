#include "server.h"
#include "utils.h"
#include "serialize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>

#define TIMEOUT_SEC 30

static GameServer* g_server = NULL;

// Signal handler for graceful shutdown
static void signal_handler(int sig) {
    (void)sig;
    if (g_server) {
        g_server->running = false;
    }
}

// Handle client disconnection
static void handle_disconnect(GameServer* server, int player_idx) {
    pthread_mutex_lock(&server->mutex);
    
    Player* player = &server->players[player_idx];
    if (player->connected) {
        printf("Player %d (%s) disconnected\n", player_idx + 1, player->name);
        player->connected = false;
        
        if (player->fd >= 0) {
            close(player->fd);
            player->fd = -1;
        }
        
        // Notify other player
        int other_idx = 1 - player_idx;
        if (server->players[other_idx].connected) {
            send_opponent_left(server->players[other_idx].fd);
        }
        
        server->state = GAME_STATE_FINISHED;
    }
    
    pthread_mutex_unlock(&server->mutex);
}

// Handle ship placement from client
static void handle_place_ship(GameServer* server, int player_idx, const PlaceShipPayload* payload) {
    pthread_mutex_lock(&server->mutex);
    
    Player* player = &server->players[player_idx];
    char error_msg[128];
    
    if (place_ship(&player->board, &payload->placement, error_msg, sizeof(error_msg))) {
        send_place_ack(player->fd);
        
        // Check if all ships placed
        if (all_ships_placed(&player->board)) {
            player->ready = true;
            send_ready(player->fd);
            
            // Check if both players ready
            if (server->players[0].ready && server->players[1].ready) {
                server->state = GAME_STATE_PLAYING;
                server->current_turn = 0;
                
                send_your_turn(server->players[0].fd);
                send_opponent_turn(server->players[1].fd);
                
                printf("Game started! Player 1's turn.\n");
            }
        }
    } else {
        send_place_error(player->fd, error_msg);
    }
    
    pthread_mutex_unlock(&server->mutex);
}

// Handle shot from client
static void handle_shoot(GameServer* server, int player_idx, const ShootPayload* payload) {
    pthread_mutex_lock(&server->mutex);
    
    // Verify it's this player's turn
    if (server->current_turn != player_idx) {
        send_error(server->players[player_idx].fd, "Not your turn");
        pthread_mutex_unlock(&server->mutex);
        return;
    }
    
    int opponent_idx = 1 - player_idx;
    Player* opponent = &server->players[opponent_idx];
    
    // Check if already targeted
    if (is_cell_targeted(&opponent->board, payload->row, payload->col)) {
        send_error(server->players[player_idx].fd, "Already targeted");
        pthread_mutex_unlock(&server->mutex);
        return;
    }
    
    // Process shot
    ShipType sunk_ship = 0;
    ShotResult result = process_shot(&opponent->board, payload->row, payload->col, &sunk_ship);
    
    // Send result to both players
    send_shoot_result(server->players[player_idx].fd, payload->row, payload->col, result, sunk_ship);
    send_shoot_result(server->players[opponent_idx].fd, payload->row, payload->col, result, sunk_ship);
    
    printf("Player %d shot %c%d: %s\n", player_idx + 1, 'A' + payload->row, payload->col + 1,
           result == SHOT_HIT ? "HIT" : (result == SHOT_SUNK ? "SUNK" : "MISS"));
    
    // Check game over
    if (is_game_over(&opponent->board)) {
        send_game_over(server->players[player_idx].fd, true);
        send_game_over(server->players[opponent_idx].fd, false);
        server->state = GAME_STATE_FINISHED;
        printf("Game over! Player %d wins!\n", player_idx + 1);
    } else {
        // Switch turn
        server->current_turn = opponent_idx;
        send_your_turn(server->players[opponent_idx].fd);
        send_opponent_turn(server->players[player_idx].fd);
    }
    
    pthread_mutex_unlock(&server->mutex);
}

// Client handler thread
static void* client_handler(void* arg) {
    int player_idx = *((int*)arg);
    free(arg);
    
    GameServer* server = g_server;
    Player* player = &server->players[player_idx];
    
    printf("Player %d handler started\n", player_idx + 1);
    
    while (server->running && player->connected) {
        MessageHeader header;
        
        if (!recv_message_header(player->fd, &header)) {
            handle_disconnect(server, player_idx);
            break;
        }
        
        // Allocate payload buffer
        void* payload = NULL;
        if (header.length > 0) {
            payload = malloc(header.length);
            if (!payload) {
                fprintf(stderr, "Memory allocation failed\n");
                handle_disconnect(server, player_idx);
                break;
            }
            
            if (!recv_message_payload(player->fd, payload, header.length)) {
                free(payload);
                handle_disconnect(server, player_idx);
                break;
            }
        }
        
        // Handle message
        switch ((MessageType)header.type) {
            case MSG_PLACE_SHIP:
                if (header.length == sizeof(PlaceShipPayload)) {
                    handle_place_ship(server, player_idx, (PlaceShipPayload*)payload);
                }
                break;
                
            case MSG_SHOOT:
                if (header.length == sizeof(ShootPayload)) {
                    handle_shoot(server, player_idx, (ShootPayload*)payload);
                }
                break;
                
            default:
                printf("Unknown message type: %d\n", header.type);
                break;
        }
        
        if (payload) {
            free(payload);
        }
    }
    
    printf("Player %d handler finished\n", player_idx + 1);
    return NULL;
}

bool server_init(GameServer* server, int port) {
    memset(server, 0, sizeof(GameServer));
    
    server->port = port;
    server->player_count = 0;
    server->current_turn = 0;
    server->state = GAME_STATE_WAITING;
    server->running = true;
    
    for (int i = 0; i < 2; i++) {
        server->players[i].fd = -1;
        server->players[i].connected = false;
        board_init(&server->players[i].board);
    }
    
    if (pthread_mutex_init(&server->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        return false;
    }
    
    server->listen_fd = create_listening_socket(port);
    if (server->listen_fd < 0) {
        pthread_mutex_destroy(&server->mutex);
        return false;
    }
    
    return true;
}

void server_run(GameServer* server) {
    g_server = server;
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);
    
    // Get and print local IP
    char local_ip[64];
    if (get_local_ip(local_ip, sizeof(local_ip))) {
        printf("===========================================\n");
        printf("Battleship Server Started\n");
        printf("===========================================\n");
        printf("Listening on: %s:%d\n", local_ip, server->port);
        printf("Also accessible on: 0.0.0.0:%d\n", server->port);
        printf("Waiting for 2 players to connect...\n");
        printf("===========================================\n\n");
    }
    
    // Accept 2 players
    while (server->running && server->player_count < 2) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        int client_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }
        
        printf("New connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // Receive connect message
        MessageHeader header;
        if (!recv_message_header(client_fd, &header) || header.type != MSG_CONNECT) {
            close(client_fd);
            continue;
        }
        
        ConnectPayload payload;
        if (!recv_message_payload(client_fd, &payload, sizeof(payload))) {
            close(client_fd);
            continue;
        }
        
        // Add player
        pthread_mutex_lock(&server->mutex);
        int player_idx = server->player_count;
        Player* player = &server->players[player_idx];
        
        player->fd = client_fd;
        player->connected = true;
        safe_strncpy(player->name, payload.player_name, sizeof(player->name));
        
        server->player_count++;
        pthread_mutex_unlock(&server->mutex);
        
        printf("Player %d connected: %s\n", player_idx + 1, player->name);
        
        // Send acknowledgment
        send_connect_ack(client_fd);
        
        if (server->player_count < 2) {
            send_waiting(client_fd);
        }
        
        // Start client handler thread
        int* idx_ptr = malloc(sizeof(int));
        *idx_ptr = player_idx;
        if (pthread_create(&player->thread, NULL, client_handler, idx_ptr) != 0) {
            perror("pthread_create");
            free(idx_ptr);
            handle_disconnect(server, player_idx);
        }
    }
    
    // Both players connected, start placement phase
    if (server->player_count == 2) {
        printf("\nBoth players connected! Starting placement phase...\n\n");
        
        pthread_mutex_lock(&server->mutex);
        server->state = GAME_STATE_PLACEMENT;
        send_start_placement(server->players[0].fd);
        send_start_placement(server->players[1].fd);
        pthread_mutex_unlock(&server->mutex);
        
        // Wait for game to finish
        while (server->running && server->state != GAME_STATE_FINISHED) {
            sleep(1);
        }
        
        // Wait for client threads to finish
        for (int i = 0; i < 2; i++) {
            if (server->players[i].connected) {
                pthread_join(server->players[i].thread, NULL);
            }
        }
    }
    
    printf("\nServer shutting down...\n");
}

void server_cleanup(GameServer* server) {
    server->running = false;
    
    for (int i = 0; i < 2; i++) {
        if (server->players[i].fd >= 0) {
            close(server->players[i].fd);
        }
    }
    
    if (server->listen_fd >= 0) {
        close(server->listen_fd);
    }
    
    pthread_mutex_destroy(&server->mutex);
}


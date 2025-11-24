#include "client.h"
#include "ui.h"
#include "utils.h"
#include "serialize.h"
#include "ship_shapes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

// Handle placement phase
static bool handle_placement_phase(GameClient* client) {
    for (int ship_idx = 0; ship_idx < NUM_SHIPS; ship_idx++) {
        ShipType ship_type = (ShipType)ship_idx;
        
        bool placed = false;
        while (!placed) {
            // Display current board
            Board empty_board;
            board_init(&empty_board);
            ui_display_boards(&client->own_board, &empty_board, true);
            ui_display_placement_instructions(ship_type);
            
            // Get placement from user
            int row, col, rotation;
            if (!ui_prompt_placement(ship_type, &row, &col, &rotation)) {
                sleep(2); // Give user time to read the error message
                continue;
            }
            
            // Show preview of the ship placement
            ui_clear_screen();
            ui_display_board_with_preview(&client->own_board, "YOUR BOARD (Preview)", 
                                          ship_type, row, col, rotation);
            ui_display_placement_instructions(ship_type);
            
            // Validate locally first
            ShipPlacement placement;
            placement.type = ship_type;
            placement.row = row;
            placement.col = col;
            placement.rotation = rotation;
            
            char error_msg[128];
            if (!validate_ship_placement(&client->own_board, &placement, error_msg, sizeof(error_msg))) {
                ui_display_message(error_msg, true);
                sleep(2);
                continue;
            }
            
            // Send to server
            if (!send_place_ship(client->server_fd, &placement)) {
                ui_display_message("Failed to send placement to server", true);
                return false;
            }
            
            // Wait for server response
            MessageHeader header;
            if (!recv_message_header(client->server_fd, &header)) {
                ui_display_message("Connection lost", true);
                return false;
            }
            
            if (header.type == MSG_PLACE_ACK) {
                // Place locally
                place_ship(&client->own_board, &placement, error_msg, sizeof(error_msg));
                ui_display_message("Ship placed successfully!", false);
                placed = true;
                sleep(1);
            } else if (header.type == MSG_PLACE_ERROR) {
                PlaceErrorPayload payload;
                recv_message_payload(client->server_fd, &payload, sizeof(payload));
                ui_display_message(payload.error_msg, true);
                sleep(2);
            } else {
                ui_display_message("Unexpected server response", true);
                return false;
            }
        }
    }
    
    // Wait for ready message
    MessageHeader header;
    if (!recv_message_header(client->server_fd, &header) || header.type != MSG_READY) {
        ui_display_message("Protocol error", true);
        return false;
    }
    
    ui_display_message("All ships placed! Waiting for opponent...", false);
    return true;
}

// Handle playing phase
static bool handle_playing_phase(GameClient* client) {
    while (client->state == CLIENT_STATE_PLAYING) {
        // Display boards
        ui_display_boards(&client->own_board, &client->opponent_board, true);
        
        if (client->my_turn) {
            printf("\n%s=== YOUR TURN ===%s\n", COLOR_GREEN, COLOR_RESET);
            
            // Get shot from user
            int row, col;
            if (!ui_prompt_shot(&row, &col)) {
                sleep(2); // Give user time to read the error message
                continue;
            }
            
            // Check if already targeted
            if (is_cell_targeted(&client->opponent_board, row, col)) {
                ui_display_message("You already shot there!", true);
                sleep(1);
                continue;
            }
            
            // Send shot to server
            if (!send_shoot(client->server_fd, row, col)) {
                ui_display_message("Failed to send shot", true);
                return false;
            }
            
            // Wait for result
            MessageHeader header;
            if (!recv_message_header(client->server_fd, &header)) {
                ui_display_message("Connection lost", true);
                return false;
            }
            
            if (header.type == MSG_SHOOT_RESULT) {
                ShootResultPayload payload;
                recv_message_payload(client->server_fd, &payload, sizeof(payload));
                
                // Update opponent board
                ShotResult result = (ShotResult)payload.result;
                if (result == SHOT_HIT || result == SHOT_SUNK) {
                    client->opponent_board.grid[payload.row][payload.col] = CELL_HIT;
                } else {
                    client->opponent_board.grid[payload.row][payload.col] = CELL_MISS;
                }
                
                ui_display_shot_result(payload.row, payload.col, result);
                sleep(2);
                
                // Check for game over
                if (!recv_message_header(client->server_fd, &header)) {
                    return false;
                }
                
                if (header.type == MSG_GAME_OVER) {
                    GameOverPayload game_over;
                    recv_message_payload(client->server_fd, &game_over, sizeof(game_over));
                    
                    ui_display_boards(&client->own_board, &client->opponent_board, true);
                    ui_display_game_over(game_over.you_won);
                    client->state = CLIENT_STATE_FINISHED;
                    return true;
                } else if (header.type == MSG_OPPONENT_TURN) {
                    client->my_turn = false;
                } else {
                    ui_display_message("Protocol error", true);
                    return false;
                }
            } else if (header.type == MSG_ERROR) {
                ErrorPayload payload;
                recv_message_payload(client->server_fd, &payload, sizeof(payload));
                ui_display_message(payload.error_msg, true);
                sleep(2);
            } else {
                ui_display_message("Unexpected server response", true);
                return false;
            }
        } else {
            printf("\n%s=== OPPONENT'S TURN ===%s\n", COLOR_YELLOW, COLOR_RESET);
            printf("Waiting for opponent...\n");
            
            // Wait for opponent's shot result or turn notification
            MessageHeader header;
            if (!recv_message_header(client->server_fd, &header)) {
                ui_display_message("Connection lost", true);
                return false;
            }
            
            if (header.type == MSG_SHOOT_RESULT) {
                ShootResultPayload payload;
                recv_message_payload(client->server_fd, &payload, sizeof(payload));
                
                // Update own board (opponent shot at us)
                ShotResult result = (ShotResult)payload.result;
                if (result == SHOT_HIT || result == SHOT_SUNK) {
                    client->own_board.grid[payload.row][payload.col] = CELL_HIT;
                } else {
                    client->own_board.grid[payload.row][payload.col] = CELL_MISS;
                }
                
                printf("\nOpponent shot at %c%d: %s\n", 
                       'A' + payload.row, payload.col + 1,
                       result == SHOT_HIT ? "HIT!" : (result == SHOT_SUNK ? "SUNK!" : "MISS"));
                sleep(2);
                
                // Check for game over or next turn
                if (!recv_message_header(client->server_fd, &header)) {
                    return false;
                }
                
                if (header.type == MSG_GAME_OVER) {
                    GameOverPayload game_over;
                    recv_message_payload(client->server_fd, &game_over, sizeof(game_over));
                    
                    ui_display_boards(&client->own_board, &client->opponent_board, true);
                    ui_display_game_over(game_over.you_won);
                    client->state = CLIENT_STATE_FINISHED;
                    return true;
                } else if (header.type == MSG_YOUR_TURN) {
                    client->my_turn = true;
                } else {
                    ui_display_message("Protocol error", true);
                    return false;
                }
            } else if (header.type == MSG_OPPONENT_LEFT) {
                ui_display_message("Opponent disconnected. You win!", false);
                client->state = CLIENT_STATE_FINISHED;
                return true;
            } else {
                ui_display_message("Unexpected message", true);
                return false;
            }
        }
    }
    
    return true;
}

bool client_init(GameClient* client, const char* host, int port, const char* player_name) {
    memset(client, 0, sizeof(GameClient));
    
    client->state = CLIENT_STATE_CONNECTING;
    client->running = true;
    client->my_turn = false;
    
    safe_strncpy(client->player_name, player_name, sizeof(client->player_name));
    
    board_init(&client->own_board);
    board_init(&client->opponent_board);
    
    // Connect to server
    printf("Connecting to %s:%d...\n", host, port);
    client->server_fd = create_client_socket(host, port);
    if (client->server_fd < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return false;
    }
    
    printf("Connected! Sending player info...\n");
    
    // Send connect message
    if (!send_connect(client->server_fd, player_name)) {
        fprintf(stderr, "Failed to send connect message\n");
        close(client->server_fd);
        return false;
    }
    
    // Wait for acknowledgment
    MessageHeader header;
    if (!recv_message_header(client->server_fd, &header) || header.type != MSG_CONNECT_ACK) {
        fprintf(stderr, "Server rejected connection\n");
        close(client->server_fd);
        return false;
    }
    
    printf("Connection accepted!\n");
    return true;
}

void client_run(GameClient* client) {
    ui_welcome();
    sleep(1);
    
    // Wait for game to start
    MessageHeader header;
    if (!recv_message_header(client->server_fd, &header)) {
        ui_display_message("Connection lost", true);
        return;
    }
    
    if (header.type == MSG_WAITING) {
        ui_display_message("Waiting for another player to join...", false);
        
        // Wait for start placement
        if (!recv_message_header(client->server_fd, &header)) {
            ui_display_message("Connection lost", true);
            return;
        }
    }
    
    if (header.type != MSG_START_PLACEMENT) {
        ui_display_message("Protocol error", true);
        return;
    }
    
    // Placement phase
    client->state = CLIENT_STATE_PLACEMENT;
    ui_display_message("Starting placement phase...", false);
    sleep(1);
    
    if (!handle_placement_phase(client)) {
        return;
    }
    
    // Wait for game to start
    if (!recv_message_header(client->server_fd, &header)) {
        ui_display_message("Connection lost", true);
        return;
    }
    
    if (header.type == MSG_YOUR_TURN) {
        client->my_turn = true;
        ui_display_message("Game starting! You go first!", false);
    } else if (header.type == MSG_OPPONENT_TURN) {
        client->my_turn = false;
        ui_display_message("Game starting! Opponent goes first!", false);
    } else {
        ui_display_message("Protocol error", true);
        return;
    }
    
    sleep(2);
    
    // Playing phase
    client->state = CLIENT_STATE_PLAYING;
    handle_playing_phase(client);
    
    // Wait before exit
    printf("\nPress Enter to exit...");
    getchar();
}

void client_cleanup(GameClient* client) {
    if (client->server_fd >= 0) {
        close(client->server_fd);
    }
}


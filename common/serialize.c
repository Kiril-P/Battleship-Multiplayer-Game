#include "serialize.h"
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

// Send all bytes
static bool send_all(int fd, const void* data, size_t size) {
    const uint8_t* ptr = (const uint8_t*)data;
    size_t sent = 0;
    
    while (sent < size) {
        ssize_t n = write(fd, ptr + sent, size - sent);
        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                sleep(1000); // Brief wait for non-blocking sockets
                continue;
            }
            perror("send_all write");
            return false;
        }
        if (n == 0) {
            return false; // Connection closed
        }
        sent += n;
    }
    return true;
}

// Receive all bytes
static bool recv_all(int fd, void* data, size_t size) {
    uint8_t* ptr = (uint8_t*)data;
    size_t received = 0;
    
    while (received < size) {
        ssize_t n = read(fd, ptr + received, size - received);
        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                sleep(1000);
                continue;
            }
            perror("recv_all read");
            return false;
        }
        if (n == 0) {
            return false; // Connection closed
        }
        received += n;
    }
    return true;
}

bool send_message(int fd, MessageType type, const void* payload, size_t payload_size) {
    MessageHeader header;
    header.type = (uint8_t)type;
    header.length = htons((uint16_t)payload_size);
    
    if (!send_all(fd, &header, sizeof(header))) {
        return false;
    }
    
    if (payload_size > 0 && payload != NULL) {
        if (!send_all(fd, payload, payload_size)) {
            return false;
        }
    }
    
    return true;
}

bool recv_message_header(int fd, MessageHeader* header) {
    if (!recv_all(fd, header, sizeof(*header))) {
        return false;
    }
    header->length = ntohs(header->length);
    return true;
}

bool recv_message_payload(int fd, void* payload, size_t payload_size) {
    if (payload_size == 0) return true;
    return recv_all(fd, payload, payload_size);
}

bool send_connect(int fd, const char* player_name) {
    ConnectPayload payload;
    memset(&payload, 0, sizeof(payload));
    strncpy(payload.player_name, player_name, sizeof(payload.player_name) - 1);
    return send_message(fd, MSG_CONNECT, &payload, sizeof(payload));
}

bool send_connect_ack(int fd) {
    return send_message(fd, MSG_CONNECT_ACK, NULL, 0);
}

bool send_waiting(int fd) {
    return send_message(fd, MSG_WAITING, NULL, 0);
}

bool send_start_placement(int fd) {
    return send_message(fd, MSG_START_PLACEMENT, NULL, 0);
}

bool send_place_ship(int fd, const ShipPlacement* placement) {
    PlaceShipPayload payload;
    payload.placement = *placement;
    return send_message(fd, MSG_PLACE_SHIP, &payload, sizeof(payload));
}

bool send_place_ack(int fd) {
    return send_message(fd, MSG_PLACE_ACK, NULL, 0);
}

bool send_place_error(int fd, const char* error_msg) {
    PlaceErrorPayload payload;
    memset(&payload, 0, sizeof(payload));
    strncpy(payload.error_msg, error_msg, sizeof(payload.error_msg) - 1);
    return send_message(fd, MSG_PLACE_ERROR, &payload, sizeof(payload));
}

bool send_ready(int fd) {
    return send_message(fd, MSG_READY, NULL, 0);
}

bool send_your_turn(int fd) {
    return send_message(fd, MSG_YOUR_TURN, NULL, 0);
}

bool send_opponent_turn(int fd) {
    return send_message(fd, MSG_OPPONENT_TURN, NULL, 0);
}

bool send_shoot(int fd, uint8_t row, uint8_t col) {
    ShootPayload payload;
    payload.row = row;
    payload.col = col;
    return send_message(fd, MSG_SHOOT, &payload, sizeof(payload));
}

bool send_shoot_result(int fd, uint8_t row, uint8_t col, ShotResult result, ShipType ship_type) {
    ShootResultPayload payload;
    payload.row = row;
    payload.col = col;
    payload.result = (uint8_t)result;
    payload.ship_type = ship_type;
    return send_message(fd, MSG_SHOOT_RESULT, &payload, sizeof(payload));
}

bool send_game_over(int fd, bool you_won) {
    GameOverPayload payload;
    payload.you_won = you_won;
    return send_message(fd, MSG_GAME_OVER, &payload, sizeof(payload));
}

bool send_opponent_left(int fd) {
    return send_message(fd, MSG_OPPONENT_LEFT, NULL, 0);
}

bool send_error(int fd, const char* error_msg) {
    ErrorPayload payload;
    memset(&payload, 0, sizeof(payload));
    strncpy(payload.error_msg, error_msg, sizeof(payload.error_msg) - 1);
    return send_message(fd, MSG_ERROR, &payload, sizeof(payload));
}

bool send_timeout(int fd) {
    return send_message(fd, MSG_TIMEOUT, NULL, 0);
}


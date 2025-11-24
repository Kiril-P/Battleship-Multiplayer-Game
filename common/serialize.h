#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "protocol.h"
#include <stddef.h>
#include <stdbool.h>

// Send message with header and payload
bool send_message(int fd, MessageType type, const void* payload, size_t payload_size);

// Receive message header
bool recv_message_header(int fd, MessageHeader* header);

// Receive message payload
bool recv_message_payload(int fd, void* payload, size_t payload_size);

// Convenience functions for specific messages
bool send_connect(int fd, const char* player_name);
bool send_connect_ack(int fd);
bool send_waiting(int fd);
bool send_start_placement(int fd);
bool send_place_ship(int fd, const ShipPlacement* placement);
bool send_place_ack(int fd);
bool send_place_error(int fd, const char* error_msg);
bool send_ready(int fd);
bool send_your_turn(int fd);
bool send_opponent_turn(int fd);
bool send_shoot(int fd, uint8_t row, uint8_t col);
bool send_shoot_result(int fd, uint8_t row, uint8_t col, ShotResult result, ShipType ship_type);
bool send_game_over(int fd, bool you_won);
bool send_opponent_left(int fd);
bool send_error(int fd, const char* error_msg);
bool send_timeout(int fd);

#endif // SERIALIZE_H


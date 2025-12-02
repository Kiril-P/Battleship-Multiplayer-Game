#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

// Grid dimensions
#define GRID_SIZE 6
#define NUM_SHIPS 4

// Ship types
typedef enum {
    SHIP_DESTROYER_1 = 0,  // 2 cells horizontal
    SHIP_DESTROYER_2 = 1,  // 2 cells vertical
    SHIP_L_SHAPE = 2,      // 3 cells L-shaped
    SHIP_Z_SHAPE = 3       // 4 cells Z-shaped
} ShipType;

// Ship rotation (DEPRECATED: rotation removed, kept for protocol compatibility)
// Always send 0 - rotation is ignored
typedef uint8_t Rotation;

// Cell state
typedef enum {
    CELL_EMPTY = 0,
    CELL_SHIP = 1,
    CELL_MISS = 2,
    CELL_HIT = 3
} CellState;

// Message types
typedef enum {
    MSG_CONNECT = 1,
    MSG_CONNECT_ACK = 2,
    MSG_WAITING = 3,
    MSG_START_PLACEMENT = 4,
    MSG_PLACE_SHIP = 5,
    MSG_PLACE_ACK = 6,
    MSG_PLACE_ERROR = 7,
    MSG_READY = 8,
    MSG_YOUR_TURN = 9,
    MSG_OPPONENT_TURN = 10,
    MSG_SHOOT = 11,
    MSG_SHOOT_RESULT = 12,
    MSG_GAME_OVER = 13,
    MSG_OPPONENT_LEFT = 14,
    MSG_ERROR = 15,
    MSG_TIMEOUT = 16
} MessageType;

// Result of a shot
typedef enum {
    SHOT_MISS = 0,
    SHOT_HIT = 1,
    SHOT_SUNK = 2
} ShotResult;

// Ship placement info
typedef struct {
    ShipType type;
    uint8_t row;      // 0-5 (A-F)
    uint8_t col;      // 0-5 (1-6)
    Rotation rotation; // 0-3
} __attribute__((packed)) ShipPlacement;

// Message header
typedef struct {
    uint8_t type;     // MessageType
    uint16_t length;  // Payload length (network byte order)
} __attribute__((packed)) MessageHeader;

// Connect message payload
typedef struct {
    char player_name[32];
} __attribute__((packed)) ConnectPayload;

// Place ship message payload
typedef struct {
    ShipPlacement placement;
} __attribute__((packed)) PlaceShipPayload;

// Place error message payload
typedef struct {
    char error_msg[128];
} __attribute__((packed)) PlaceErrorPayload;

// Shoot message payload
typedef struct {
    uint8_t row;
    uint8_t col;
} __attribute__((packed)) ShootPayload;

// Shoot result message payload
typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t result;  // ShotResult
    ShipType ship_type; // If sunk
} __attribute__((packed)) ShootResultPayload;

// Game over message payload
typedef struct {
    bool you_won;
} __attribute__((packed)) GameOverPayload;

// Error message payload
typedef struct {
    char error_msg[128];
} __attribute__((packed)) ErrorPayload;

#endif // PROTOCOL_H




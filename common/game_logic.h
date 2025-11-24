#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "protocol.h"
#include "ship_shapes.h"
#include <stdbool.h>
#include <stddef.h>

// Player's board
typedef struct {
    CellState grid[GRID_SIZE][GRID_SIZE];
    ShipPlacement ships[NUM_SHIPS];
    bool ships_placed[NUM_SHIPS];
    int ships_remaining;
    int placement_count;
} Board;

// Initialize a board
void board_init(Board* board);

// Validate ship placement (check bounds, overlap, correct rotation)
bool validate_ship_placement(const Board* board, const ShipPlacement* placement, char* error_msg, size_t error_size);

// Place a ship on the board
bool place_ship(Board* board, const ShipPlacement* placement, char* error_msg, size_t error_size);

// Process a shot, return result
ShotResult process_shot(Board* board, int row, int col, ShipType* sunk_ship);

// Check if cell has already been targeted
bool is_cell_targeted(const Board* board, int row, int col);

// Check if all ships are placed
bool all_ships_placed(const Board* board);

// Check if game is over (all ships sunk)
bool is_game_over(const Board* board);

#endif // GAME_LOGIC_H


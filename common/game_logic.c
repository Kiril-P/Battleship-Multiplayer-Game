#include "game_logic.h"
#include <string.h>
#include <stdio.h>

void board_init(Board* board) {
    memset(board, 0, sizeof(Board));
    board->ships_remaining = NUM_SHIPS;
    board->placement_count = 0;
    
    // Initialize grid to empty
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            board->grid[i][j] = CELL_EMPTY;
        }
    }
}

bool validate_ship_placement(const Board* board, const ShipPlacement* placement, char* error_msg, size_t error_size) {
    // Check if this ship type has already been placed
    if (board->ships_placed[placement->type]) {
        snprintf(error_msg, error_size, "Ship type %d already placed", placement->type);
        return false;
    }
    
    // Check rotation is valid (0-3)
    if (placement->rotation > 3) {
        snprintf(error_msg, error_size, "Invalid rotation: %d (must be 0-3)", placement->rotation);
        return false;
    }
    
    // Get ship cells
    int cells[MAX_SHIP_CELLS][2];
    int num_cells = get_ship_cells(placement->type, placement->rotation, 
                                    placement->row, placement->col, cells, MAX_SHIP_CELLS);
    
    if (num_cells <= 0) {
        snprintf(error_msg, error_size, "Invalid ship configuration");
        return false;
    }
    
    // Check all cells are in bounds and not occupied
    for (int i = 0; i < num_cells; i++) {
        int r = cells[i][0];
        int c = cells[i][1];
        
        if (r < 0 || r >= GRID_SIZE || c < 0 || c >= GRID_SIZE) {
            snprintf(error_msg, error_size, "Ship out of bounds at (%d, %d)", r, c);
            return false;
        }
        
        if (board->grid[r][c] != CELL_EMPTY) {
            snprintf(error_msg, error_size, "Cell (%d, %d) already occupied", r, c);
            return false;
        }
    }
    
    return true;
}

bool place_ship(Board* board, const ShipPlacement* placement, char* error_msg, size_t error_size) {
    if (!validate_ship_placement(board, placement, error_msg, error_size)) {
        return false;
    }
    
    // Get ship cells
    int cells[MAX_SHIP_CELLS][2];
    int num_cells = get_ship_cells(placement->type, placement->rotation,
                                    placement->row, placement->col, cells, MAX_SHIP_CELLS);
    
    // Place ship on grid
    for (int i = 0; i < num_cells; i++) {
        int r = cells[i][0];
        int c = cells[i][1];
        board->grid[r][c] = CELL_SHIP;
    }
    
    // Record placement
    board->ships[placement->type] = *placement;
    board->ships_placed[placement->type] = true;
    board->placement_count++;
    
    return true;
}

ShotResult process_shot(Board* board, int row, int col, ShipType* sunk_ship) {
    *sunk_ship = 0; // Default
    
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        return SHOT_MISS;
    }
    
    CellState cell = board->grid[row][col];
    
    if (cell == CELL_SHIP) {
        board->grid[row][col] = CELL_HIT;
        
        // Check if any ship was sunk
        for (int ship_idx = 0; ship_idx < NUM_SHIPS; ship_idx++) {
            if (!board->ships_placed[ship_idx]) continue;
            
            ShipPlacement* placement = &board->ships[ship_idx];
            int cells[MAX_SHIP_CELLS][2];
            int num_cells = get_ship_cells(placement->type, placement->rotation,
                                          placement->row, placement->col, cells, MAX_SHIP_CELLS);
            
            bool all_hit = true;
            bool has_this_cell = false;
            
            for (int i = 0; i < num_cells; i++) {
                int r = cells[i][0];
                int c = cells[i][1];
                
                if (r == row && c == col) {
                    has_this_cell = true;
                }
                
                if (board->grid[r][c] != CELL_HIT) {
                    all_hit = false;
                }
            }
            
            if (has_this_cell && all_hit) {
                *sunk_ship = placement->type;
                board->ships_remaining--;
                return SHOT_SUNK;
            }
        }
        
        return SHOT_HIT;
    } else if (cell == CELL_EMPTY) {
        board->grid[row][col] = CELL_MISS;
        return SHOT_MISS;
    } else {
        // Already shot here
        return SHOT_MISS;
    }
}

bool is_cell_targeted(const Board* board, int row, int col) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        return false;
    }
    
    CellState cell = board->grid[row][col];
    return (cell == CELL_HIT || cell == CELL_MISS);
}

bool all_ships_placed(const Board* board) {
    return board->placement_count == NUM_SHIPS;
}

bool is_game_over(const Board* board) {
    return board->ships_remaining == 0;
}


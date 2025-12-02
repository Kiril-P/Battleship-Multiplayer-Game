#include "ship_shapes.h"
#include <string.h>

// Define ship shapes - SIMPLIFIED (NO ROTATIONS!)
// Each ship has only ONE fixed orientation
// Each cell is defined as [row_offset, col_offset] from the base position

// Destroyer 1: 2 cells horizontal
// DD
static const int destroyer_1_shape[2][2] = {
    {0,0}, {0,1}     // horizontal (DD)
};

// Destroyer 2: 2 cells vertical
// D
// D
static const int destroyer_2_shape[2][2] = {
    {0,0}, {1,0}     // vertical
};

// L-shape: 3 cells (corner shape)
// X
// XX
static const int l_shape[3][2] = {
    {0,0}, {1,0}, {1,1}     // L corner down-right
};

// Z-shape: 4 cells - ACTUAL Z SHAPE
// ZZ
//  ZZ
static const int z_shape[4][2] = {
    {0,0}, {0,1}, {1,1}, {1,2}   // Z horizontal (like steps)
};


int get_ship_cells(ShipType type, Rotation rotation, int base_row, int base_col, 
                   int cells_out[][2], int max_cells) {
    (void)rotation; // Rotation parameter ignored - kept for compatibility
    
    int num_cells = get_ship_size(type);
    if (num_cells > max_cells) return -1;
    
    const int (*shape)[2] = NULL;
    
    switch (type) {
        case SHIP_DESTROYER_1:
            shape = destroyer_1_shape;
            num_cells = 2;
            break;
            
        case SHIP_DESTROYER_2:
            shape = destroyer_2_shape;
            num_cells = 2;
            break;
            
        case SHIP_L_SHAPE:
            shape = l_shape;
            num_cells = 3;
            break;
            
        case SHIP_Z_SHAPE:
            shape = z_shape;
            num_cells = 4;
            break;
            
        default:
            return -1;
    }
    
    // Convert relative offsets to absolute positions
    for (int i = 0; i < num_cells; i++) {
        cells_out[i][0] = base_row + shape[i][0];
        cells_out[i][1] = base_col + shape[i][1];
    }
    
    return num_cells;
}

int get_ship_size(ShipType type) {
    switch (type) {
        case SHIP_DESTROYER_1:
            return 2;
        case SHIP_DESTROYER_2:
            return 2;
        case SHIP_L_SHAPE:
            return 3;
        case SHIP_Z_SHAPE:
            return 4;
        default:
            return 0;
    }
}

char get_ship_display_char(ShipType type) {
    switch (type) {
        case SHIP_DESTROYER_1:
            return 'D';
        case SHIP_DESTROYER_2:
            return 'd';
        case SHIP_L_SHAPE:
            return 'L';
        case SHIP_Z_SHAPE:
            return 'Z';
        default:
            return '?';
    }
}

const char* get_ship_name(ShipType type) {
    switch (type) {
        case SHIP_DESTROYER_1:
            return "Destroyer (Horizontal)";
        case SHIP_DESTROYER_2:
            return "Destroyer (Vertical)";
        case SHIP_L_SHAPE:
            return "L-Ship";
        case SHIP_Z_SHAPE:
            return "Z-Ship";
        default:
            return "Unknown";
    }
}




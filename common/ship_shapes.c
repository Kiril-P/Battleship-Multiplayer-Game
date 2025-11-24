#include "ship_shapes.h"
#include <string.h>

// Define ship shapes for each type and rotation
// Rotation 0-3 covers horizontal and vertical orientations only
// Each cell is defined as [row_offset, col_offset] from the base position

// Destroyer: 2 cells straight
// Rotations: 0=horizontal right, 1=vertical down, 2=horizontal left, 3=vertical up
static const int destroyer_shapes[4][2][2] = {
    {{0,0}, {0,1}},     // 0: horizontal ->
    {{0,0}, {1,0}},     // 1: vertical ↓
    {{0,0}, {0,-1}},    // 2: horizontal <-
    {{0,0}, {-1,0}}     // 3: vertical ↑
};

// L-shape: 3 cells (looks like └ or rotated versions)
// Base L-shape is: X
//                  XX
// Rotations: 0=└, 1=┌, 2=┘, 3=┐
static const int l_shape_shapes[4][3][2] = {
    {{0,0}, {1,0}, {1,1}},      // 0: └ (down-right)
    {{0,0}, {0,1}, {1,1}},      // 1: ┌ (right-down)
    {{0,0}, {-1,0}, {-1,-1}},   // 2: ┘ (up-left)
    {{0,0}, {0,-1}, {-1,-1}}    // 3: ┐ (left-up)
};

// Z-shape: 4 cells (looks like Z or S)
// Base Z-shape is:  XX
//                   XX
// Rotations: 0=square horizontal, 1=Z vertical, 2=square horizontal reversed, 3=Z vertical reversed
static const int z_shape_shapes[4][4][2] = {
    {{0,0}, {0,1}, {1,0}, {1,1}},       // 0: square (2x2 horizontal)
    {{0,0}, {1,0}, {1,1}, {2,1}},       // 1: Z vertical
    {{0,0}, {0,-1}, {-1,0}, {-1,-1}},   // 2: square rotated 180
    {{0,0}, {-1,0}, {-1,-1}, {-2,-1}}   // 3: Z vertical reversed
};


int get_ship_cells(ShipType type, Rotation rotation, int base_row, int base_col, 
                   int cells_out[][2], int max_cells) {
    int num_cells = get_ship_size(type);
    if (num_cells > max_cells) return -1;
    
    rotation = rotation % 4; // Ensure valid rotation (0-3)
    
    const int (*shape)[2] = NULL;
    
    switch (type) {
        case SHIP_DESTROYER_1:
            shape = destroyer_shapes[rotation];
            num_cells = 2;
            break;
            
        case SHIP_L_SHAPE:
            shape = l_shape_shapes[rotation];
            num_cells = 3;
            break;
            
        case SHIP_Z_SHAPE:
            shape = z_shape_shapes[rotation];
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
            return "Destroyer";
        case SHIP_L_SHAPE:
            return "L-Ship";
        case SHIP_Z_SHAPE:
            return "Z-Ship";
        default:
            return "Unknown";
    }
}


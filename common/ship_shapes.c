#include "ship_shapes.h"
#include <string.h>

// Define ship shapes for each type and rotation
// Rotation 0-7 covers all possible orientations
// Each cell is defined as [row_offset, col_offset] from the base position

// Destroyer: 2 cells straight
// Rotations: 0=horizontal right, 1=diagonal down-right, 2=vertical down, 3=diagonal down-left
//            4=horizontal left, 5=diagonal up-left, 6=vertical up, 7=diagonal up-right
static const int destroyer_shapes[8][2][2] = {
    {{0,0}, {0,1}},     // 0: horizontal ->
    {{0,0}, {1,1}},     // 1: diagonal ↘
    {{0,0}, {1,0}},     // 2: vertical ↓
    {{0,0}, {1,-1}},    // 3: diagonal ↙
    {{0,0}, {0,-1}},    // 4: horizontal <-
    {{0,0}, {-1,-1}},   // 5: diagonal ↖
    {{0,0}, {-1,0}},    // 6: vertical ↑
    {{0,0}, {-1,1}}     // 7: diagonal ↗
};

// L-shape: 3 cells (looks like └ or rotated versions)
// Base L-shape is: X
//                  XX
static const int l_shape_shapes[8][3][2] = {
    {{0,0}, {1,0}, {1,1}},      // 0: └
    {{0,0}, {0,1}, {1,0}},      // 1: ┐ rotated 45°
    {{0,0}, {0,1}, {1,1}},      // 2: ┌
    {{0,0}, {1,0}, {0,-1}},     // 3: ┘ rotated -45°
    {{0,0}, {-1,0}, {-1,-1}},   // 4: ┘
    {{0,0}, {0,-1}, {-1,0}},    // 5: └ rotated 180+45°
    {{0,0}, {0,-1}, {-1,-1}},   // 6: ┐
    {{0,0}, {-1,0}, {0,1}}      // 7: ┌ rotated -45°
};

// Z-shape: 4 cells (looks like Z or S)
// Base Z-shape is:  XX
//                   XX
static const int z_shape_shapes[8][4][2] = {
    {{0,0}, {0,1}, {1,0}, {1,1}},       // 0: square
    {{0,0}, {0,1}, {1,1}, {1,2}},       // 1: Z horizontal
    {{0,0}, {1,0}, {1,1}, {2,1}},       // 2: Z vertical
    {{0,0}, {0,1}, {-1,1}, {-1,2}},     // 3: S horizontal
    {{0,0}, {0,-1}, {-1,0}, {-1,-1}},   // 4: square rotated 180
    {{0,0}, {0,-1}, {-1,-1}, {-1,-2}},  // 5: Z horizontal reversed
    {{0,0}, {-1,0}, {-1,-1}, {-2,-1}},  // 6: Z vertical reversed
    {{0,0}, {0,-1}, {1,-1}, {1,-2}}     // 7: S horizontal reversed
};

// Large L-shape: 5 cells (3x3 L)
// Base shape is: X
//                X
//                XXX
static const int large_l_shapes[8][5][2] = {
    {{0,0}, {1,0}, {2,0}, {2,1}, {2,2}},        // 0: ┗━
    {{0,0}, {0,1}, {0,2}, {1,0}, {2,0}},        // 1: ┏ rotated 45°
    {{0,0}, {0,1}, {0,2}, {1,2}, {2,2}},        // 2: ┏━
    {{0,0}, {0,1}, {0,2}, {1,0}, {2,0}},        // 3: (variant)
    {{0,0}, {0,1}, {0,2}, {-1,2}, {-2,2}},      // 4: ┛━ 
    {{0,0}, {0,-1}, {0,-2}, {-1,0}, {-2,0}},    // 5: ┓ rotated
    {{0,0}, {0,-1}, {0,-2}, {-1,-2}, {-2,-2}},  // 6: ┓━
    {{0,0}, {0,-1}, {0,-2}, {1,0}, {2,0}}       // 7: ┗ rotated
};

int get_ship_cells(ShipType type, Rotation rotation, int base_row, int base_col, 
                   int cells_out[][2], int max_cells) {
    int num_cells = get_ship_size(type);
    if (num_cells > max_cells) return -1;
    
    rotation = rotation % 8; // Ensure valid rotation
    
    const int (*shape)[2] = NULL;
    
    switch (type) {
        case SHIP_DESTROYER_1:
        case SHIP_DESTROYER_2:
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
            
        case SHIP_LARGE_L:
            shape = large_l_shapes[rotation];
            num_cells = 5;
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
        case SHIP_DESTROYER_2:
            return 2;
        case SHIP_L_SHAPE:
            return 3;
        case SHIP_Z_SHAPE:
            return 4;
        case SHIP_LARGE_L:
            return 5;
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
        case SHIP_LARGE_L:
            return 'B'; // B for Big/Large
        default:
            return '?';
    }
}

const char* get_ship_name(ShipType type) {
    switch (type) {
        case SHIP_DESTROYER_1:
            return "Destroyer 1";
        case SHIP_DESTROYER_2:
            return "Destroyer 2";
        case SHIP_L_SHAPE:
            return "L-Ship";
        case SHIP_Z_SHAPE:
            return "Z-Ship";
        case SHIP_LARGE_L:
            return "Large L";
        default:
            return "Unknown";
    }
}


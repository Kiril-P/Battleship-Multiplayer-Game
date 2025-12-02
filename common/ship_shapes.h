#ifndef SHIP_SHAPES_H
#define SHIP_SHAPES_H

#include "protocol.h"

// Maximum cells any ship can occupy
#define MAX_SHIP_CELLS 5

// Ship definition structure
typedef struct {
    int cells[MAX_SHIP_CELLS][2]; // [cell_index][row_offset, col_offset]
    int num_cells;
    char display_char; // Character to display on grid
} ShipDefinition;

// Get ship definition for a given type and rotation
// Returns number of cells occupied
int get_ship_cells(ShipType type, Rotation rotation, int base_row, int base_col, 
                   int cells_out[][2], int max_cells);

// Get ship size
int get_ship_size(ShipType type);

// Get ship display character
char get_ship_display_char(ShipType type);

// Get ship name
const char* get_ship_name(ShipType type);

#endif // SHIP_SHAPES_H




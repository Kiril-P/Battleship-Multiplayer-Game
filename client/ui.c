#include "ui.h"
#include "ship_shapes.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void ui_clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void ui_welcome() {
    ui_clear_screen();
    printf("%s", COLOR_CYAN);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║                                            ║\n");
    printf("║           BATTLESHIP GAME                  ║\n");
    printf("║        Network Multiplayer Edition         ║\n");
    printf("║                                            ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("%s\n", COLOR_RESET);
}

static void print_cell(CellState cell, bool show_ship, char ship_char) {
    switch (cell) {
        case CELL_EMPTY:
            printf("%s ~ %s", COLOR_BLUE, COLOR_RESET);
            break;
        case CELL_SHIP:
            if (show_ship) {
                printf("%s%s %c %s", COLOR_GREEN, BG_GREEN, ship_char, COLOR_RESET);
            } else {
                printf("%s ~ %s", COLOR_BLUE, COLOR_RESET);
            }
            break;
        case CELL_MISS:
            printf("%s · %s", COLOR_GRAY, COLOR_RESET);
            break;
        case CELL_HIT:
            printf("%s%s X %s", COLOR_RED, BG_RED, COLOR_RESET);
            break;
    }
}

void ui_display_board(const Board* board, const char* title, bool show_ships) {
    printf("\n%s%s%s\n", COLOR_YELLOW, title, COLOR_RESET);
    printf("    ");
    for (int col = 0; col < GRID_SIZE; col++) {
        printf("  %d ", col + 1);
    }
    printf("\n");
    
    printf("   ┌");
    for (int col = 0; col < GRID_SIZE; col++) {
        printf("───");
        if (col < GRID_SIZE - 1) printf("┬");
    }
    printf("┐\n");
    
    for (int row = 0; row < GRID_SIZE; row++) {
        printf(" %c │", 'A' + row);
        
        for (int col = 0; col < GRID_SIZE; col++) {
            CellState cell = board->grid[row][col];
            
            // Find which ship is at this position
            char ship_char = '#';
            if (show_ships && cell == CELL_SHIP) {
                for (int s = 0; s < NUM_SHIPS; s++) {
                    if (board->ships_placed[s]) {
                        ShipPlacement* placement = (ShipPlacement*)&board->ships[s];
                        int cells[MAX_SHIP_CELLS][2];
                        int num_cells = get_ship_cells(placement->type, placement->rotation,
                                                       placement->row, placement->col, cells, MAX_SHIP_CELLS);
                        
                        for (int i = 0; i < num_cells; i++) {
                            if (cells[i][0] == row && cells[i][1] == col) {
                                ship_char = get_ship_display_char(placement->type);
                                break;
                            }
                        }
                    }
                }
            }
            
            print_cell(cell, show_ships, ship_char);
            
            if (col < GRID_SIZE - 1) printf("│");
        }
        printf("│\n");
        
        if (row < GRID_SIZE - 1) {
            printf("   ├");
            for (int col = 0; col < GRID_SIZE; col++) {
                printf("───");
                if (col < GRID_SIZE - 1) printf("┼");
            }
            printf("┤\n");
        }
    }
    
    printf("   └");
    for (int col = 0; col < GRID_SIZE; col++) {
        printf("───");
        if (col < GRID_SIZE - 1) printf("┴");
    }
    printf("┘\n");
}

void ui_display_boards(const Board* own_board, const Board* opponent_board, bool show_own_ships) {
    ui_clear_screen();
    
    printf("\n%s", COLOR_CYAN);
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("%s", COLOR_RESET);
    
    // Display legend
    printf("\n%sLegend:%s ", COLOR_YELLOW, COLOR_RESET);
    printf("%s ~ %s Water  ", COLOR_BLUE, COLOR_RESET);
    printf("%s · %s Miss  ", COLOR_GRAY, COLOR_RESET);
    printf("%s%s X %s Hit  ", COLOR_RED, BG_RED, COLOR_RESET);
    printf("%s%s # %s Ship\n", COLOR_GREEN, BG_GREEN, COLOR_RESET);
    
    // Display boards side by side (simplified - one after another for clarity)
    ui_display_board(own_board, "YOUR BOARD", show_own_ships);
    ui_display_board(opponent_board, "OPPONENT'S BOARD", false);
    
    printf("\n%s", COLOR_CYAN);
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("%s", COLOR_RESET);
}

void ui_display_placement_instructions(ShipType current_ship) {
    printf("\n%s", COLOR_YELLOW);
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ Place your ship: %-40s │\n", get_ship_name(current_ship));
    printf("│ Size: %d cells, Character: '%c'%*s│\n", 
           get_ship_size(current_ship), 
           get_ship_display_char(current_ship),
           35, "");
    printf("│                                                             │\n");
    printf("│ Enter position and rotation (0-7):                         │\n");
    printf("│ Format: <ROW><COL> R<ROTATION>                             │\n");
    printf("│ Example: A1 R0  or  D5 R3                                  │\n");
    printf("│                                                             │\n");
    printf("│ Rotations: 0=→ 1=↘ 2=↓ 3=↙ 4=← 5=↖ 6=↑ 7=↗              │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    printf("%s", COLOR_RESET);
}

void ui_display_message(const char* message, bool is_error) {
    if (is_error) {
        printf("\n%s[ERROR] %s%s\n", COLOR_RED, message, COLOR_RESET);
    } else {
        printf("\n%s[INFO] %s%s\n", COLOR_GREEN, message, COLOR_RESET);
    }
}

void ui_display_shot_result(int row, int col, ShotResult result) {
    char coord[4];
    format_coordinate(row, col, coord, sizeof(coord));
    
    printf("\n%s", COLOR_CYAN);
    printf("┌─────────────────────────────────────┐\n");
    printf("│ Shot at %s: ", coord);
    
    switch (result) {
        case SHOT_MISS:
            printf("%sMISS%s", COLOR_GRAY, COLOR_RESET);
            printf("%*s│\n", 23, "");
            break;
        case SHOT_HIT:
            printf("%sHIT!%s", COLOR_YELLOW, COLOR_RESET);
            printf("%*s│\n", 23, "");
            break;
        case SHOT_SUNK:
            printf("%sSUNK!%s", COLOR_RED, COLOR_RESET);
            printf("%*s│\n", 22, "");
            break;
    }
    
    printf("%s└─────────────────────────────────────┘%s\n", COLOR_CYAN, COLOR_RESET);
}

void ui_display_game_over(bool won) {
    printf("\n\n%s", won ? COLOR_GREEN : COLOR_RED);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║                                            ║\n");
    if (won) {
        printf("║          🎉 VICTORY! 🎉                    ║\n");
        printf("║     You sunk all enemy ships!             ║\n");
    } else {
        printf("║          💔 DEFEAT 💔                      ║\n");
        printf("║     All your ships were sunk...           ║\n");
    }
    printf("║                                            ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("%s\n", COLOR_RESET);
}

bool ui_prompt_placement(ShipType ship_type __attribute__((unused)), int* row, int* col, int* rotation) {
    char input[64];
    
    printf("\n%s> %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    // Remove newline
    input[strcspn(input, "\n")] = 0;
    
    // Parse input: "A1 R0"
    char coord_str[8] = "";
    int rot = 0;
    
    if (sscanf(input, "%7s R%d", coord_str, &rot) != 2) {
        ui_display_message("Invalid format. Use: A1 R0", true);
        return false;
    }
    
    if (!parse_coordinate(coord_str, row, col)) {
        ui_display_message("Invalid coordinate. Use A-H for row, 1-8 for column", true);
        return false;
    }
    
    if (rot < 0 || rot > 7) {
        ui_display_message("Invalid rotation. Use 0-7", true);
        return false;
    }
    
    *rotation = rot;
    return true;
}

bool ui_prompt_shot(int* row, int* col) {
    char input[64];
    
    printf("\n%sEnter target (e.g., A1): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    // Remove newline
    input[strcspn(input, "\n")] = 0;
    
    if (!parse_coordinate(input, row, col)) {
        ui_display_message("Invalid coordinate. Use A-H for row, 1-8 for column", true);
        return false;
    }
    
    return true;
}


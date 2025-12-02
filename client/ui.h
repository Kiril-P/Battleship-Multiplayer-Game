#ifndef UI_H
#define UI_H

#include "protocol.h"
#include "game_logic.h"

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_WHITE   "\033[1;37m"
#define COLOR_GRAY    "\033[0;37m"

#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_BLUE    "\033[44m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

// Clear screen
void ui_clear_screen();

// Display welcome screen
void ui_welcome();

// Display both boards side by side
void ui_display_boards(const Board* own_board, const Board* opponent_board, bool show_own_ships);

// Display single board
void ui_display_board(const Board* board, const char* title, bool show_ships);

// Display board with ship placement preview
void ui_display_board_with_preview(const Board* board, const char* title, 
                                    ShipType ship_type, int preview_row, 
                                    int preview_col, int preview_rotation);

// Display placement instructions
void ui_display_placement_instructions(ShipType current_ship);

// Display visual representation of ship shape
void ui_display_ship_shape(ShipType ship_type, int rotation);

// Display game status message
void ui_display_message(const char* message, bool is_error);

// Display shot result
void ui_display_shot_result(int row, int col, ShotResult result);

// Display game over screen
void ui_display_game_over(bool won);

// Prompt for input
bool ui_prompt_placement(ShipType ship_type, int* row, int* col, int* rotation);
bool ui_prompt_shot(int* row, int* col);

#endif // UI_H




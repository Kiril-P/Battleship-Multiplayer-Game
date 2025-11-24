# File Manifest - Battleship Game

Complete description of every file in the project.

## Documentation Files

### README.md
**Purpose:** Main project documentation  
**Contains:**
- Feature list and overview
- Build instructions
- Running instructions (local and network)
- How to play (placement and shooting)
- Troubleshooting guide
- Project structure
- Technical architecture

### QUICKSTART.md
**Purpose:** Rapid setup guide  
**Contains:**
- 30-second build instructions
- Ship placement cheat sheet
- Shooting reference
- Multiplayer setup
- Quick troubleshooting

### NETWORK_SETUP.md
**Purpose:** Cross-machine networking guide  
**Contains:**
- Step-by-step network setup
- IP address detection
- Firewall configuration
- Network scenarios (Wi-Fi, LAN, campus)
- Connectivity testing
- Port forwarding
- Security notes

### PROJECT_SUMMARY.md
**Purpose:** Academic/grading reference  
**Contains:**
- Requirements checklist
- Technical specifications
- Code statistics
- Features demonstrated
- Testing scenarios
- Grade criteria mapping

### FILE_MANIFEST.md
**Purpose:** This file - complete file descriptions

## Build System

### CMakeLists.txt
**Purpose:** CMake build configuration  
**Contains:**
- Project definition (C11 standard)
- Compiler flags (-Wall -Wextra -pthread)
- Common library target (static)
- Server executable target
- Client executable target
- Include directories

### .gitignore
**Purpose:** Git version control ignore rules  
**Contains:**
- Build directories
- Object files
- Executables
- IDE files
- System files

### demo.sh
**Purpose:** Automated demo script  
**Contains:**
- Tmux session setup
- Automatic build check
- Three-window layout (server + 2 clients)
- Launch commands

## Common Library (Shared Code)

### common/protocol.h
**Purpose:** Network protocol definitions  
**Contains:**
- Grid constants (GRID_SIZE=8, NUM_SHIPS=5)
- Enums: ShipType, CellState, MessageType, ShotResult
- Structs: ShipPlacement, MessageHeader
- Message payload structs (16 types)
- All structs are `__attribute__((packed))`

**Key Types:**
- `MessageType`: 16 message types for client-server communication
- `ShipType`: 5 ship types (2 destroyers, L, Z, Large L)
- `CellState`: Empty, Ship, Miss, Hit
- `ShotResult`: Miss, Hit, Sunk

### common/serialize.h / serialize.c
**Purpose:** Binary protocol serialization  
**Contains:**
- `send_message()`: Generic message sender
- `recv_message_header()`: Receive message header
- `recv_message_payload()`: Receive message payload
- Convenience functions for each message type (16 functions)
- Network byte order conversion (htons/ntohs)
- Blocking send/receive with retry logic

**Functions:** 20 total (3 generic + 17 message-specific)

### common/game_logic.h / game_logic.c
**Purpose:** Game rules and validation  
**Contains:**
- `Board` struct: Grid, ship placements, status
- `board_init()`: Initialize empty board
- `validate_ship_placement()`: Check bounds, overlaps, rotation
- `place_ship()`: Add ship to board
- `process_shot()`: Handle shot, detect sunk ships
- `is_cell_targeted()`: Check if already shot
- `all_ships_placed()`: Placement phase complete check
- `is_game_over()`: All ships sunk check

**Core Logic:**
- Bounds checking for 8×8 grid
- Overlap detection
- Hit detection with ship tracking
- Win condition evaluation

### common/ship_shapes.h / ship_shapes.c
**Purpose:** Ship shape definitions  
**Contains:**
- Shape arrays for all 5 ship types × 8 rotations
- `get_ship_cells()`: Get absolute positions for ship
- `get_ship_size()`: Return number of cells
- `get_ship_display_char()`: Get character for rendering
- `get_ship_name()`: Get human-readable name

**Ship Definitions:**
- Destroyer: 2 cells, 8 rotations
- L-Shape: 3 cells, 8 rotations  
- Z-Shape: 4 cells, 8 rotations
- Large L: 5 cells, 8 rotations
- Each rotation defined as array of [row_offset, col_offset]

### common/utils.h / utils.c
**Purpose:** Network and utility functions  
**Contains:**
- `create_listening_socket()`: Server socket creation
- `create_client_socket()`: Client connection
- `set_nonblocking()`: Configure non-blocking I/O
- `set_socket_timeout()`: Configure timeouts
- `get_local_ip()`: Auto-detect server IP
- `safe_strncpy()`: Safe string copy
- `parse_coordinate()`: "A1" → (0, 0)
- `format_coordinate()`: (0, 0) → "A1"

**Key Features:**
- SO_REUSEADDR socket option
- getifaddrs() for IP detection
- DNS lookup support
- Safe string handling

## Server Implementation

### server/main.c
**Purpose:** Server entry point  
**Contains:**
- Argument parsing (port number)
- Server initialization
- Main loop invocation
- Cleanup on exit

**Usage:** `./server [port]`  
**Default:** port 5000

### server/server.h
**Purpose:** Server interface  
**Contains:**
- `GameServer` struct: Listen socket, players, game state, mutex
- `Player` struct: FD, name, board, thread
- `GameState` enum: Waiting, Placement, Playing, Finished
- Function prototypes

**State Management:**
- 2 player slots
- Current turn tracking
- Thread synchronization

### server/server.c
**Purpose:** Server implementation  
**Contains:**
- `server_init()`: Setup server socket, mutex
- `server_run()`: Main accept loop
- `client_handler()`: Thread function for each client
- `handle_place_ship()`: Process ship placement
- `handle_shoot()`: Process shot, check win
- `handle_disconnect()`: Clean up on disconnect
- Signal handlers (SIGINT, SIGTERM, SIGPIPE)

**Flow:**
1. Accept 2 connections
2. Start thread per client
3. Manage placement phase
4. Manage shooting phase
5. Detect game over
6. Handle disconnects

**Threading:**
- Main thread: Accept connections
- Client threads: Handle messages
- Mutex: Protect game state

## Client Implementation

### client/main.c
**Purpose:** Client entry point  
**Contains:**
- Argument parsing (IP, port, name)
- Client initialization
- Main loop invocation
- Cleanup on exit

**Usage:** `./client <ip> <port> [name]`  
**Example:** `./client 192.168.1.50 5000 Alice`

### client/client.h
**Purpose:** Client interface  
**Contains:**
- `GameClient` struct: Server FD, state, boards, turn flag
- `ClientState` enum: Connecting, Waiting, Placement, Playing, Finished
- Function prototypes

**State Management:**
- Own board (with ships)
- Opponent board (fog of war)
- Turn tracking

### client/client.c
**Purpose:** Client implementation  
**Contains:**
- `client_init()`: Connect to server
- `client_run()`: Main game loop
- `handle_placement_phase()`: Interactive ship placement
- `handle_playing_phase()`: Interactive shooting
- Message receive and dispatch
- Turn management

**Flow:**
1. Connect to server
2. Wait for second player
3. Place all ships (interactive)
4. Wait for game start
5. Shoot on turns
6. Display results
7. Check win/loss

**User Interaction:**
- Prompt for placement: "A1 R0"
- Prompt for shot: "A1"
- Display results immediately
- Update boards after each action

### client/ui.h / ui.c
**Purpose:** Terminal user interface  
**Contains:**
- ANSI color code definitions
- `ui_clear_screen()`: Clear terminal
- `ui_welcome()`: Show banner
- `ui_display_board()`: Render single board
- `ui_display_boards()`: Render both boards
- `ui_display_placement_instructions()`: Help text
- `ui_display_message()`: Status/error messages
- `ui_display_shot_result()`: Show hit/miss/sunk
- `ui_display_game_over()`: Win/loss screen
- `ui_prompt_placement()`: Input for placement
- `ui_prompt_shot()`: Input for shooting

**Visual Elements:**
- Box drawing characters (┌─┬─┐)
- Color-coded cells:
  - Blue ~ (water)
  - Gray · (miss)
  - Red X (hit)
  - Green D/d/L/Z/B (ships)
- Professional formatting
- Clear legends
- Responsive layout

**ANSI Colors Used:**
- RED: Hits, errors
- GREEN: Ships, success
- YELLOW: Instructions, highlights
- BLUE: Water
- CYAN: Borders, titles
- GRAY: Misses

## Build Artifacts (Generated)

### build/ directory
**Created by:** CMake  
**Contains:**
- `server`: Server executable (~53KB)
- `client`: Client executable (~54KB)
- `libcommon.a`: Common library (~30KB)
- CMake cache and build files
- Object files (.o)

**Build Commands:**
```bash
mkdir build && cd build
cmake ..
make
```

## File Statistics

| Category | Files | Lines of Code |
|----------|-------|---------------|
| Headers | 8 | ~400 |
| Implementation | 10 | ~2,100 |
| Documentation | 5 | ~1,500 |
| Build System | 2 | ~50 |
| **Total** | **25** | **~4,050** |

## Dependency Graph

```
server executable
├── server/main.c
├── server/server.c
└── libcommon.a

client executable
├── client/main.c
├── client/client.c
├── client/ui.c
└── libcommon.a

libcommon.a (static library)
├── common/serialize.c
├── common/game_logic.c
├── common/ship_shapes.c
└── common/utils.c
```

## Header Dependencies

```
protocol.h
└── (no dependencies - base types)

ship_shapes.h
└── protocol.h

serialize.h
└── protocol.h

game_logic.h
├── protocol.h
└── ship_shapes.h

utils.h
└── (no dependencies)

server.h
├── protocol.h
└── game_logic.h

client.h
├── protocol.h
└── game_logic.h

ui.h
├── protocol.h
└── game_logic.h
```

## Code Organization Principles

1. **Separation of Concerns:**
   - Protocol: Data structures only
   - Serialize: Network I/O
   - Game Logic: Rules and validation
   - Ship Shapes: Geometry
   - Utils: Helper functions
   - Server: Game management
   - Client: User interaction
   - UI: Presentation

2. **Layering:**
   - Layer 1: Protocol definitions
   - Layer 2: Serialization, shapes, utils
   - Layer 3: Game logic
   - Layer 4: Client/Server
   - Layer 5: UI (client only)

3. **Modularity:**
   - Each .c file has corresponding .h
   - Headers are self-contained
   - No circular dependencies
   - Clear interfaces

4. **Reusability:**
   - Common code in library
   - Generic serialization functions
   - Portable utilities
   - Platform-independent protocol

## Testing Each Module

### Protocol (protocol.h):
- Struct sizes are correct
- Packing works properly
- Enums have valid ranges

### Serialization:
- Messages serialize/deserialize correctly
- Network byte order handled
- Error conditions detected

### Game Logic:
- Ship placement validation works
- Hit detection accurate
- Win condition correct

### Ship Shapes:
- All rotations defined
- Shapes match specification
- Bounds checking works

### Server:
- Accepts 2 clients
- Threads work properly
- State synchronized
- Disconnects handled

### Client:
- Connects successfully
- UI renders correctly
- Input parsing works
- Updates in real-time

## File Size Reference

| File | Size | Purpose |
|------|------|---------|
| protocol.h | ~3 KB | Definitions |
| serialize.c | ~6 KB | I/O operations |
| game_logic.c | ~4 KB | Game rules |
| ship_shapes.c | ~5 KB | Shape arrays |
| utils.c | ~4 KB | Utilities |
| server.c | ~10 KB | Server logic |
| client.c | ~9 KB | Client logic |
| ui.c | ~8 KB | UI rendering |

## Navigation Guide

**Want to understand the protocol?**
→ Read `common/protocol.h`

**Want to see game rules?**
→ Read `common/game_logic.c`

**Want to see ship definitions?**
→ Read `common/ship_shapes.c`

**Want to understand server?**
→ Read `server/server.c`

**Want to understand client?**
→ Read `client/client.c`

**Want to see UI?**
→ Read `client/ui.c`

**Want to build?**
→ Read `CMakeLists.txt`

**Want to play?**
→ Read `README.md`

**Want network setup?**
→ Read `NETWORK_SETUP.md`

**Want quick start?**
→ Read `QUICKSTART.md`

---

**Every file has a purpose. Every function has a reason. Every line is intentional.**


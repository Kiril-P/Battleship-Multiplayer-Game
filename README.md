# Battleship - Multiplayer Network Game

A production-quality, multiplayer turn-based Battleship game implemented in pure C using POSIX sockets and threading. Features a beautiful ANSI-colored terminal UI and custom ship shapes inspired by Sea Battle 2.

## Features

- **6×6 Grid** (rows A-F, columns 1-6)
- **4 Simple Ships**:
  - One 2-cell Destroyer (horizontal: DD)
  - One 2-cell Destroyer (vertical)
  - One 3-cell L-shaped ship
  - One 4-cell Z-shaped ship (actual Z, not square!)
- **Real TCP Networking** - works across different machines on the same network
- **Client-Server Architecture** with authoritative server
- **Multi-threaded** - one thread per client
- **Beautiful Terminal UI** with ANSI colors
- **Simple ship placement** - no rotation needed, fixed ship orientations
- **Fog of war** - can't see opponent's ships until you hit them
- **Graceful disconnect handling**

## Technical Highlights

- Pure C (C11 standard)
- POSIX sockets with non-blocking I/O
- Thread-safe game state with mutexes
- Binary network protocol with proper serialization
- Custom ship shape definitions with rotation support
- Comprehensive input validation and error handling
- Clean modular architecture with separated concerns
- CMake build system

## Prerequisites

- Linux or macOS
- GCC or Clang compiler
- CMake 3.10 or higher
- pthread library (included in POSIX systems)

## Building the Project

```bash
# Clone or navigate to the project directory
cd Computer-Programming-Project

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Executables will be in the build directory:
# - ./server
# - ./client
```

## Running the Game

### Option 1: Single Machine Testing (Local)

**Terminal 1 - Start Server:**
```bash
cd build
./server 8080
```

```
To find your IP for other players, use:
  macOS/Linux: ifconfig | grep "inet "
  Windows: ipconfig
```

**Terminal 2 - Player 1:**
```bash
cd build
./client 127.0.0.1 8080 Alice
```

**Terminal 3 - Player 2:**
```bash
cd build
./client 127.0.0.1 8080 Bob
```

### Option 2: Multiple Machines (Network Play)

This is the real multiplayer experience! All machines must be on the same network (Wi-Fi, LAN, or campus network).

**Machine 1 - Server:**
```bash
cd build
./server 8080    # Use 8080 for macOS (5000 conflicts with AirPlay)
```

**Find your server's IP address using:**

**On macOS:**
```bash
ifconfig | grep "inet " | grep -v 127.0.0.1
```

**On Linux:**
```bash
ip addr show | grep "inet " | grep -v 127.0.0.1
```

**On Windows (if using WSL):**
```bash
ipconfig
```

**Then share the IP with other players (e.g., `192.168.1.50`)**

**Machine 2 - Player 1:**
```bash
cd build
./client 192.168.1.50 8080 Player1    # Use your server's IP
```

**Machine 3 - Player 2:**
```bash
cd build
./client 192.168.1.50 8080 Player2    # Use your server's IP
```

## How to Play

### Phase 1: Ship Placement

Each player must place all 4 ships on their board:

1. **Destroyer (Horizontal)**: 2 cells - DD
2. **Destroyer (Vertical)**: 2 cells
3. **L-Ship**: 3 cells in L-shape
4. **Z-Ship**: 4 cells in Z-shape

**Placement Format:**
```
<ROW><COLUMN>
```

**Examples:**
```
A1    - Place Destroyer (Horizontal) at A1
A4    - Place Destroyer (Vertical) at A4
D2    - Place L-Ship at D2
F2    - Place Z-Ship at F2
```

**Ship Shapes (Fixed Orientations):**
- **Destroyer (Horizontal)**: `DD` (2 cells wide)
- **Destroyer (Vertical)**: `D` over `D` (2 cells tall)
- **L-Ship**: `X` then `XX` (corner shape)
- **Z-Ship**: `ZZ` then ` ZZ` (zigzag)

The game validates your placement and prevents:
- Ships going out of bounds
- Ships overlapping
- Placing the same ship twice

### Phase 2: Battle

Players take turns shooting at the opponent's board.

**Shooting Format:**
```
<ROW><COLUMN>
```

**Examples:**
```
A1    - Shoot at row A, column 1
D5    - Shoot at row D, column 5
F6    - Shoot at row F, column 6
```

**Results:**
- **MISS** (·) - Shot hit water
- **HIT** (X) - Shot hit a ship
- **SUNK** (X) - Shot destroyed the last cell of a ship

**Win Condition:** First player to sink all 4 opponent ships wins!

## Game Board Legend

```
~ (blue)   - Water / Unknown
· (gray)   - Miss
X (red)    - Hit
D,d,L,Z    - Your ships (only visible on your board)
           - D = Horizontal Destroyer
           - d = Vertical Destroyer
```

## Network Configuration

### Port Recommendation

**Important:** On macOS, port 5000 is used by AirPlay Receiver. Use port **8080** instead!

### Firewall Settings

If players can't connect, ensure the server's firewall allows incoming connections on your chosen port:

**macOS:**
```bash
# Check firewall status
sudo /usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate

# Add exception if needed (generally not required)
```

**Linux (ufw):**
```bash
sudo ufw allow 8080/tcp
```

**Linux (iptables):**
```bash
sudo iptables -A INPUT -p tcp --dport 8080 -j ACCEPT
```

### Port Configuration

To use a different port:

**Server:**
```bash
./server 8080
```

**Client:**
```bash
./client 192.168.1.50 8080 PlayerName
```

## Project Structure

```
Computer-Programming-Project/
├── CMakeLists.txt           # Build configuration
├── README.md                # This file
├── common/                  # Shared code
│   ├── protocol.h           # Network protocol definitions
│   ├── serialize.c/h        # Message serialization
│   ├── game_logic.c/h       # Game rules and validation
│   ├── ship_shapes.c/h      # Ship definitions
│   └── utils.c/h            # Network and utility functions
├── server/                  # Server implementation
│   ├── main.c               # Server entry point
│   ├── server.c             # Server logic
│   └── server.h             # Server interface
└── client/                  # Client implementation
    ├── main.c               # Client entry point
    ├── client.c             # Client logic
    ├── client.h             # Client interface
    ├── ui.c                 # Terminal UI rendering
    └── ui.h                 # UI interface
```

## Technical Architecture

### Network Protocol

Binary protocol with message types and network byte order:

```c
MessageHeader (3 bytes):
  - type: uint8_t (MessageType enum)
  - length: uint16_t (payload size, network byte order)

Followed by type-specific payload
```

### Threading Model

- **Main thread**: Accepts connections, manages game state
- **Client threads**: One per player, handles messages from that client
- **Mutex protection**: All shared game state access is synchronized

### Ship Shape System

Ships are defined as arrays of cell offsets from a base position. Each ship has a fixed orientation (no rotation). The validation system checks bounds and overlaps before placement.

### Game State Machine

**Server States:**
- WAITING → PLACEMENT → PLAYING → FINISHED

**Client States:**
- CONNECTING → WAITING → PLACEMENT → PLAYING → FINISHED

## Troubleshooting

### "Connection refused"
- Ensure server is running
- Verify IP address is correct
- Check firewall settings
- Ensure both machines are on same network

### "Bind failed: Address already in use"
- Port 5000 is already in use
- Wait a minute for OS to release the port
- Or use a different port: `./server 5001`

### Ships won't place
- Ensure ship fits on board (check if it goes out of bounds)
- Verify no overlap with existing ships
- Use simple format: `A1` (just the coordinate)
- Remember: Grid is A-F (rows) and 1-6 (columns)
- Ships have fixed shapes - make sure there's room!

### Can't see opponent's board updating
- This is normal! Opponent's ships are hidden (fog of war)
- You'll only see hits (X) and misses (·) on opponent's board
- Your own board shows all your ships

### "Protocol error" or unexpected disconnection
- Network interruption occurred
- Server crashed
- Try restarting both server and clients

## Demo Script (tmux)

For easy demonstration, here's a tmux script to run all three terminals:

```bash
#!/bin/bash
# Save as demo.sh and run: chmod +x demo.sh && ./demo.sh

cd build

# Start tmux session
tmux new-session -d -s battleship

# Window 0: Server
tmux send-keys -t battleship:0 './server 5000' C-m

# Window 1: Player 1
tmux new-window -t battleship:1
tmux send-keys -t battleship:1 'sleep 2 && ./client 127.0.0.1 5000 Alice' C-m

# Window 2: Player 2
tmux new-window -t battleship:2
tmux send-keys -t battleship:2 'sleep 3 && ./client 127.0.0.1 5000 Bob' C-m

# Attach to session
tmux attach-session -t battleship
```

Use `Ctrl+B` then number key (0, 1, 2) to switch between windows.
Use `Ctrl+B` then `D` to detach.
Use `tmux kill-session -t battleship` to close all windows.

## Performance Notes

- Server handles 2 concurrent players efficiently
- Message overhead: 3-byte header + payload
- Typical round-trip latency: <10ms on LAN
- Memory footprint: <5MB per process
- Thread overhead: ~8KB per client thread

## Future Enhancements

- [ ] Add unit tests (Unity framework)
- [ ] Support for replays
- [ ] Spectator mode
- [ ] Game statistics and leaderboards
- [ ] AI opponent
- [ ] Configurable grid sizes
- [ ] TLS encryption for network traffic
- [ ] Reconnection support

## License

This is a university project. Free to use for educational purposes.

## Credits

Ship shapes inspired by Sea Battle 2 / Warships Fleet.

Built with ❤️ using pure C, POSIX sockets, and pthreads.

---

**Enjoy the game! May your shots be true and your fleet victorious! ⚓️🎯**




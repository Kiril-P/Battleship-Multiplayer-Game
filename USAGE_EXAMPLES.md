# Usage Examples - Battleship Game

Concrete examples for every use case.

## Example 1: Local Testing (One Machine, 3 Terminals)

### Terminal 1 - Server
```bash
cd /Users/kirilpetrovski/Documents/IE/Programming\ C/Computer-Programming-Project/build
./server 5000

# Expected Output:
===========================================
Battleship Server Started
===========================================
Listening on: 192.168.1.50:5000
Also accessible on: 0.0.0.0:5000
Waiting for 2 players to connect...
===========================================
```

### Terminal 2 - Player 1 (Alice)
```bash
cd /Users/kirilpetrovski/Documents/IE/Programming\ C/Computer-Programming-Project/build
./client 127.0.0.1 5000 Alice

# Ship Placement Example:
A1 R0    # Destroyer 1: horizontal at top-left
C1 R0    # Destroyer 2: horizontal below first
E1 R0    # L-Ship: basic L shape
A4 R0    # Z-Ship: at top-right area
E5 R0    # Large L: middle-right area

# Shooting Example:
A1
B2
C3
D4
...
```

### Terminal 3 - Player 2 (Bob)
```bash
cd /Users/kirilpetrovski/Documents/IE/Programming\ C/Computer-Programming-Project/build
./client 127.0.0.1 5000 Bob

# Different placement strategy:
H8 R4    # Destroyer 1: horizontal at bottom-right (facing left)
F8 R4    # Destroyer 2: below first
A8 R6    # L-Ship: top-right corner (facing up)
D1 R2    # Z-Ship: left side (vertical)
H1 R6    # Large L: bottom-left (facing up)

# Shooting:
H8
G7
F6
...
```

## Example 2: Network Play (Different Machines - Same Wi-Fi)

### Scenario: 3 Laptops on "HomeWiFi" Network

**Machine 1** (MacBook - Server):
```bash
# IP: 192.168.1.50 (auto-detected)
cd Computer-Programming-Project/build
./server 5000

# Server displays:
Listening on: 192.168.1.50:5000
```

**Machine 2** (Windows/Linux Laptop - Player 1):
```bash
# IP: 192.168.1.51
cd Computer-Programming-Project/build
./client 192.168.1.50 5000 Emma

# When prompted, place ships:
B2 R0
D2 R0
B5 R0
E4 R2
F6 R0
```

**Machine 3** (Another Laptop - Player 2):
```bash
# IP: 192.168.1.52
cd Computer-Programming-Project/build
./client 192.168.1.50 5000 Liam

# When prompted, place ships:
G1 R2
G4 R2
A6 R0
C7 R0
E1 R0
```

## Example 3: Campus Network

### Scenario: Computer Lab Machines

**Lab Computer 12** (Server):
```bash
# Campus IP: 10.50.30.12
cd ~/battleship/build
./server 5000

# Share "10.50.30.12" with classmates
```

**Lab Computer 15** (Player):
```bash
# Campus IP: 10.50.30.15
cd ~/battleship/build
./client 10.50.30.12 5000 StudentA
```

**Personal Laptop on Campus WiFi** (Player):
```bash
# Campus WiFi IP: 10.50.30.45
cd ~/battleship/build
./client 10.50.30.12 5000 StudentB
```

## Example 4: Demo Mode (tmux)

### Single Command Launch
```bash
cd Computer-Programming-Project
./demo.sh

# Automatically opens:
# - Window 0: Server
# - Window 1: Player 1 (Alice)
# - Window 2: Player 2 (Bob)

# Switch between windows:
Ctrl+B then 0  # Server window
Ctrl+B then 1  # Player 1 window
Ctrl+B then 2  # Player 2 window

# Detach (leave running):
Ctrl+B then D

# Reattach later:
tmux attach -t battleship

# Kill all:
tmux kill-session -t battleship
```

## Example 5: Different Port

### When Port 5000 is Busy

**Server:**
```bash
./server 8080

# Output:
Listening on: 192.168.1.50:8080
```

**Clients:**
```bash
./client 192.168.1.50 8080 Player1
./client 192.168.1.50 8080 Player2
```

## Example 6: Complete Game Walkthrough

### Full Game from Start to Finish

**Server Terminal:**
```
$ ./server 5000
===========================================
Battleship Server Started
===========================================
Listening on: 192.168.1.100:5000
Waiting for 2 players to connect...
===========================================

New connection from 192.168.1.101:54321
Player 1 connected: Alice

New connection from 192.168.1.102:54322
Player 2 connected: Bob

Both players connected! Starting placement phase...

Game started! Player 1's turn.
Player 1 shot A1: MISS
Player 2 shot H8: HIT
Player 1 shot B2: HIT
Player 2 shot H7: MISS
...
Player 1 shot D5: SUNK
Game over! Player 1 wins!

Server shutting down...
```

**Player 1 Terminal (Alice):**
```
$ ./client 192.168.1.100 5000 Alice
Connecting to 192.168.1.100:5000...
Connected! Sending player info...
Connection accepted!
[INFO] Waiting for another player to join...
[INFO] Starting placement phase...

┌─────────────────────────────────────────┐
│ Place your ship: Destroyer 1            │
│ Size: 2 cells, Character: 'D'          │
│ Format: <ROW><COL> R<ROTATION>          │
│ Example: A1 R0  or  D5 R3               │
└─────────────────────────────────────────┘

> A1 R0
[INFO] Ship placed successfully!

> C1 R0
[INFO] Ship placed successfully!

> E1 R0
[INFO] Ship placed successfully!

> G1 R0
[INFO] Ship placed successfully!

> A5 R0
[INFO] Ship placed successfully!

[INFO] All ships placed! Waiting for opponent...
[INFO] Game starting! You go first!

=== YOUR TURN ===
Enter target (e.g., A1): A1

┌─────────────────────────────────┐
│ Shot at A1: MISS                │
└─────────────────────────────────┘

=== OPPONENT'S TURN ===
Waiting for opponent...
Opponent shot at H8: HIT!

=== YOUR TURN ===
Enter target (e.g., A1): B2

┌─────────────────────────────────┐
│ Shot at B2: HIT!                │
└─────────────────────────────────┘

...

╔════════════════════════════════════════════╗
║                                            ║
║          🎉 VICTORY! 🎉                    ║
║     You sunk all enemy ships!             ║
║                                            ║
╚════════════════════════════════════════════╝

Press Enter to exit...
```

**Player 2 Terminal (Bob):**
```
$ ./client 192.168.1.100 5000 Bob
Connecting to 192.168.1.100:5000...
Connected! Sending player info...
Connection accepted!
[INFO] Starting placement phase...

> H8 R4
[INFO] Ship placed successfully!

> F8 R4
[INFO] Ship placed successfully!

> D8 R4
[INFO] Ship placed successfully!

> B8 R4
[INFO] Ship placed successfully!

> H1 R6
[INFO] Ship placed successfully!

[INFO] All ships placed! Waiting for opponent...
[INFO] Game starting! Opponent goes first!

=== OPPONENT'S TURN ===
Waiting for opponent...
Opponent shot at A1: MISS

=== YOUR TURN ===
Enter target (e.g., A1): H8

┌─────────────────────────────────┐
│ Shot at H8: HIT!                │
└─────────────────────────────────┘

...

╔════════════════════════════════════════════╗
║                                            ║
║          💔 DEFEAT 💔                      ║
║     All your ships were sunk...           ║
║                                            ║
╚════════════════════════════════════════════╝

Press Enter to exit...
```

## Example 7: Ship Placement Patterns

### Conservative Strategy (Corners)
```
A1 R0    # Top-left
A7 R0    # Top-right
H1 R2    # Bottom-left
H6 R0    # Bottom-right
D4 R0    # Center
```

### Aggressive Strategy (Clustered)
```
B2 R0    # Group them
B4 R0
D2 R0
D4 R2
F3 R0
```

### Defensive Strategy (Scattered)
```
A1 R0    # Far corners
A8 R4
H1 R2
H8 R6
D4 R0    # One in center
```

### Edge Strategy
```
A3 R0    # Top edge
D1 R2    # Left edge
H4 R4    # Bottom edge
E8 R6    # Right edge
D4 R0    # Center
```

## Example 8: Shooting Patterns

### Checkerboard Pattern
```
A1, A3, A5, A7
B2, B4, B6, B8
C1, C3, C5, C7
D2, D4, D6, D8
...
```

### Spiral Pattern
```
A1, A2, A3, A4, A5, A6, A7, A8
B8, C8, D8, E8, F8, G8, H8
H7, H6, H5, H4, H3, H2, H1
G1, F1, E1, D1, C1, B1
B2, B3, ...
```

### Random Pattern
```
D4, A7, H2, C5, F1, B8, G6, E3, ...
```

### Hunt and Target
```
# Hunt phase: checkerboard
A1 -> miss
C3 -> miss
E5 -> HIT!

# Target phase: adjacent cells
E6 -> HIT!
E7 -> SUNK!

# Back to hunt
A3 -> miss
...
```

## Example 9: Error Handling

### Invalid Placement
```
> Z9 R0
[ERROR] Invalid coordinate. Use A-H for row, 1-8 for column

> A1 R9
[ERROR] Invalid rotation. Use 0-7

> A1 R0
[INFO] Ship placed successfully!

> A1 R0
[ERROR] Ship type 0 already placed

> A2 R0
[ERROR] Cell (0, 1) already occupied
```

### Invalid Shooting
```
> Z9
[ERROR] Invalid coordinate. Use A-H for row, 1-8 for column

> A1
[INFO] Shot result: MISS

> A1
[INFO] You already shot there!
```

### Connection Issues
```
$ ./client 192.168.1.999 5000 Player
Connecting to 192.168.1.999:5000...
Unknown host: 192.168.1.999
Failed to connect to server

$ ./client 192.168.1.50 9999 Player
Connecting to 192.168.1.50:9999...
connect: Connection refused
Failed to connect to server
```

## Example 10: Finding Your IP

### macOS
```bash
$ ifconfig | grep "inet " | grep -v 127.0.0.1
    inet 192.168.1.50 netmask 0xffffff00 broadcast 192.168.1.255
```
Use: `192.168.1.50`

### Linux
```bash
$ ip addr show | grep "inet " | grep -v 127.0.0.1
    inet 10.0.0.50/24 brd 10.0.0.255 scope global wlan0
```
Use: `10.0.0.50`

### Both
```bash
$ hostname -I
192.168.1.50 
```

## Example 11: Testing Connectivity

### Before Starting Game
```bash
# Step 1: Start server
Terminal1$ ./server 5000
Listening on: 192.168.1.50:5000

# Step 2: Test connectivity (from client machine)
Terminal2$ ping 192.168.1.50
PING 192.168.1.50: 56 data bytes
64 bytes from 192.168.1.50: icmp_seq=0 ttl=64 time=2.123 ms
^C

# Step 3: Test port (after server started)
Terminal2$ nc -zv 192.168.1.50 5000
Connection to 192.168.1.50 5000 port [tcp/*] succeeded!

# Step 4: Connect client
Terminal2$ ./client 192.168.1.50 5000 Player1
Connecting to 192.168.1.50:5000...
Connected!
```

## Example 12: Multiple Games

### Game 1 (Port 5000)
```bash
# Server
./server 5000

# Clients
./client 192.168.1.50 5000 GameA_P1
./client 192.168.1.50 5000 GameA_P2
```

### Game 2 (Port 5001) - Simultaneously!
```bash
# Different server instance
./server 5001

# Different clients
./client 192.168.1.50 5001 GameB_P1
./client 192.168.1.50 5001 GameB_P2
```

## Troubleshooting Examples

### Issue: Can't Connect
```bash
# Check if server is running
$ ps aux | grep server
user  12345  ...  ./server 5000

# Check if port is listening
$ lsof -i :5000
COMMAND   PID USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
server  12345 user    3u  IPv4 0x...      0t0  TCP *:5000 (LISTEN)

# Try localhost first
$ ./client 127.0.0.1 5000 Test
# If this works, it's a network issue
# If this fails, it's a server issue
```

### Issue: Port Already in Use
```bash
$ ./server 5000
bind: Address already in use

# Find process
$ lsof -i :5000
server  12345 user  3u  IPv4  ...  TCP *:5000 (LISTEN)

# Kill it
$ kill 12345

# Or wait 60 seconds

# Or use different port
$ ./server 5001
```

---

## Quick Command Reference

```bash
# Build
mkdir build && cd build && cmake .. && make

# Run server
./server [port]

# Run client
./client <ip> <port> <name>

# Demo
./demo.sh

# Find IP
ifconfig | grep "inet "

# Test connection
ping <ip>
nc -zv <ip> <port>

# Kill server
Ctrl+C  # or  kill <pid>

# Clean build
cd build && make clean && make
```

**Ready to play!** 🎮⚓️


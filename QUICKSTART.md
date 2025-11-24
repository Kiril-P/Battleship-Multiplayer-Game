# Battleship - Quick Start Guide

## 30-Second Setup

```bash
# Build
mkdir build && cd build
cmake ..
make

# Run server
./server 5000

# Run clients (in other terminals, replace IP if on different machines)
./client 127.0.0.1 5000 Player1
./client 127.0.0.1 5000 Player2
```

## Ship Placement Cheat Sheet

**Format:** `<ROW><COL> R<ROTATION>`

**Example placements that work:**
```
A1 R0    # Destroyer 1 at A1, horizontal
C1 R2    # Destroyer 2 at C1, vertical
E1 R0    # L-Ship at E1, basic L
G1 R0    # Z-Ship at G1, horizontal
A5 R0    # Large L at A5
```

**Rotations:**
- 0 = →  (right)
- 2 = ↓  (down)
- 4 = ←  (left)
- 6 = ↑  (up)
- 1,3,5,7 = diagonals

## Shooting Cheat Sheet

Just type the coordinate: `A1`, `B3`, `H8`, etc.

## Multiplayer Setup (Different Machines)

### Server Machine:
1. Run: `./server 5000`
2. Note the IP shown (e.g., `192.168.1.50`)
3. Share this IP with players

### Player Machines:
```bash
./client 192.168.1.50 5000 YourName
```

Replace `192.168.1.50` with actual server IP.

## Troubleshooting

**Can't connect?**
- Check IP address
- Verify same Wi-Fi network
- Try localhost first: `127.0.0.1`
- Check firewall settings

**Build errors?**
- Ensure CMake 3.10+
- Run: `cd build && make clean && cmake .. && make`

**Port already in use?**
- Change port: `./server 5001`
- Wait 60 seconds for OS to free port

## Demo with tmux

```bash
./demo.sh
```

Switch windows: `Ctrl+B` then `0`, `1`, or `2`

## Tips

1. Place ships on opposite edges to start
2. Take notes on hits/misses
3. Use diagonal rotations for tricky placements
4. Remember: You can't see opponent ships until you hit them!

Happy gaming! ⚓️


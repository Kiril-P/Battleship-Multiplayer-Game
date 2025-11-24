# Network Setup Guide - Playing Across Different Machines

This guide walks you through setting up the Battleship game for multiplayer across different computers on the same network (Wi-Fi, LAN, or campus network).

## Prerequisites

- All machines must be on the **same network** (same Wi-Fi network or LAN)
- Server machine must allow incoming connections on port 5000
- Game must be built on all machines (or copy executables)

## Step-by-Step Setup

### 1. Build on All Machines

On each machine that will run the game:

```bash
cd Computer-Programming-Project
mkdir build
cd build
cmake ..
make
```

### 2. Start the Server (Machine 1)

On the machine that will host the game:

```bash
cd build
./server 5000
```

You should see output like this:

```
===========================================
Battleship Server Started
===========================================
Listening on: 192.168.1.50:5000
Also accessible on: 0.0.0.0:5000
Waiting for 2 players to connect...
===========================================
```

**Important:** Note the IP address shown (e.g., `192.168.1.50`). This is what players will use to connect.

### 3. Connect Player 1 (Machine 2)

On a different machine:

```bash
cd build
./client 192.168.1.50 5000 Alice
```

Replace `192.168.1.50` with the actual IP from step 2.

### 4. Connect Player 2 (Machine 3)

On another different machine (or same as Player 1 if testing):

```bash
cd build
./client 192.168.1.50 5000 Bob
```

The game will automatically start once both players connect!

## Finding Your Server's IP Address

### macOS:
```bash
# Option 1: ifconfig
ifconfig | grep "inet " | grep -v 127.0.0.1

# Option 2: System Preferences
System Preferences → Network → Select your connection → IP Address shown
```

### Linux:
```bash
# Option 1: ip command
ip addr show | grep "inet " | grep -v 127.0.0.1

# Option 2: hostname
hostname -I
```

### Windows (WSL):
```bash
# In PowerShell
ipconfig

# Look for IPv4 Address under your active network adapter
```

## Network Scenarios

### Scenario 1: Same Wi-Fi Network (Easiest)

**Setup:**
- All devices connected to same Wi-Fi
- Most common for home/dorm play

**Example:**
- Server: MacBook on "MyWiFi" (192.168.1.50)
- Player 1: Desktop on "MyWiFi" (192.168.1.51)
- Player 2: Laptop on "MyWiFi" (192.168.1.52)

**Commands:**
```bash
# Server (MacBook)
./server 5000

# Player 1 (Desktop)
./client 192.168.1.50 5000 Player1

# Player 2 (Laptop)
./client 192.168.1.50 5000 Player2
```

### Scenario 2: Campus/Enterprise Network

**Setup:**
- All devices on same institutional network
- May need to check firewall policies

**Example:**
- Server: Lab computer (10.0.5.100)
- Player 1: Personal laptop (10.0.5.101)
- Player 2: Friend's laptop (10.0.5.102)

**Commands:**
```bash
# Server
./server 5000

# Players
./client 10.0.5.100 5000 YourName
```

### Scenario 3: Wired LAN

**Setup:**
- All devices connected via Ethernet
- Fastest and most reliable option

**Example:**
- Server: Desktop (192.168.0.100)
- Player 1: Laptop 1 (192.168.0.101)
- Player 2: Laptop 2 (192.168.0.102)

## Firewall Configuration

### macOS Firewall

**Check if firewall is blocking:**
```bash
/usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate
```

**If blocked, allow the server executable:**
1. System Preferences → Security & Privacy → Firewall
2. Click "Firewall Options"
3. Click "+" and add the `server` executable
4. Set to "Allow incoming connections"

Or temporarily disable (not recommended):
```bash
sudo /usr/libexec/ApplicationFirewall/socketfilterfw --setglobalstate off
```

### Linux Firewall (ufw)

**Allow port 5000:**
```bash
sudo ufw allow 5000/tcp
sudo ufw status
```

**Or temporarily disable:**
```bash
sudo ufw disable
```

### Linux Firewall (iptables)

**Allow port 5000:**
```bash
sudo iptables -A INPUT -p tcp --dport 5000 -j ACCEPT
sudo iptables -L
```

## Troubleshooting

### Problem: "Connection refused"

**Possible causes:**
1. Server not running
2. Wrong IP address
3. Firewall blocking
4. Not on same network

**Solutions:**
```bash
# Verify server is running
ps aux | grep server

# Test local connectivity first
./client 127.0.0.1 5000 TestPlayer

# Check if port is open
nc -zv 192.168.1.50 5000  # Linux/macOS

# Ping the server
ping 192.168.1.50
```

### Problem: "No route to host"

**Cause:** Devices on different networks or subnets

**Solution:**
- Verify both devices show IP in same range (e.g., both 192.168.1.x)
- Connect to same Wi-Fi network
- Check if VPN is active (may route traffic differently)

### Problem: Server doesn't show correct IP

**Cause:** Multiple network interfaces

**Solution:**
```bash
# List all network interfaces
ifconfig -a          # macOS
ip addr show         # Linux

# Look for:
# - WiFi: usually 192.168.x.x or 10.0.x.x
# - Ethernet: usually 192.168.x.x or 10.0.x.x
# - VPN: various ranges
# - Loopback: 127.0.0.1 (don't use this!)
```

### Problem: Works locally but not on network

**Cause:** Server binding to localhost only

**Fix:** Already handled! Server binds to `0.0.0.0` which accepts connections from any interface.

### Problem: "Bind failed: Address already in use"

**Cause:** Port 5000 already in use

**Solutions:**
```bash
# Option 1: Use different port
./server 5001
./client 192.168.1.50 5001 Player1

# Option 2: Find and kill process using port
lsof -i :5000           # macOS/Linux
kill -9 <PID>

# Option 3: Wait 60 seconds for OS to release port
```

## Testing Connectivity

### Before Playing:

1. **Ping test:**
```bash
ping 192.168.1.50
```
Should show replies. Press Ctrl+C to stop.

2. **Port test (after starting server):**
```bash
# macOS/Linux
nc -zv 192.168.1.50 5000

# Should show: Connection to 192.168.1.50 5000 port [tcp/*] succeeded!
```

3. **Telnet test:**
```bash
telnet 192.168.1.50 5000

# Should connect (may show garbage, just press Ctrl+] then type 'quit')
```

## Using Different Ports

If port 5000 is blocked or in use:

**Server:**
```bash
./server 8080
```

**Clients:**
```bash
./client 192.168.1.50 8080 PlayerName
```

Common alternative ports: 8080, 8000, 3000, 12345

## Network Performance Tips

1. **Use wired Ethernet** when possible for lowest latency
2. **Close bandwidth-heavy apps** (streaming, downloads)
3. **Sit closer to Wi-Fi router** for better signal
4. **Avoid VPNs** which add latency
5. **Use 5GHz Wi-Fi** instead of 2.4GHz if available

## Security Notes

- Game uses **unencrypted TCP** - don't use over untrusted networks
- Only play with people you trust
- Server accepts first 2 connections - no authentication
- Use on local/campus networks only, not over internet

## Advanced: Port Forwarding (For Internet Play)

**Not recommended but possible:**

1. Configure router port forwarding: External 5000 → Internal 192.168.1.50:5000
2. Find public IP: `curl ifconfig.me`
3. Share public IP with players
4. Players connect to public IP

**Security warning:** This exposes your computer to the internet. Only do this if you understand the risks!

## Demo Video Setup (tmux)

For presentations showing network play on one machine:

```bash
./demo.sh
```

This simulates 3 machines using different terminal windows.

---

## Quick Reference Card

```
┌─────────────────────────────────────────────┐
│           NETWORK PLAY CHECKLIST            │
├─────────────────────────────────────────────┤
│ ☐ All machines on same network              │
│ ☐ Server built and ready                    │
│ ☐ Server IP noted                           │
│ ☐ Firewall allows port 5000                 │
│ ☐ Clients built and ready                   │
│                                             │
│ Commands:                                   │
│   Server:  ./server 5000                    │
│   Client:  ./client <IP> 5000 <Name>        │
│                                             │
│ Test:     ping <server_ip>                  │
│           nc -zv <server_ip> 5000           │
└─────────────────────────────────────────────┘
```

## Need Help?

Common IP ranges to look for:
- Home Wi-Fi: `192.168.0.x` or `192.168.1.x`
- Campus: `10.x.x.x` or `172.16.x.x`
- Corporate: Varies widely

If still having issues, check with your network administrator about local firewall policies.

---

**Happy network gaming! ⚓️🌐**


#!/bin/bash
# Battleship Game Demo Script
# This script starts the server and two clients in separate tmux windows

echo "Starting Battleship Game Demo..."

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Build directory not found. Building project..."
    mkdir build
    cd build
    cmake ..
    make
    cd ..
fi

# Check if executables exist
if [ ! -f "build/server" ] || [ ! -f "build/client" ]; then
    echo "Executables not found. Building project..."
    cd build
    cmake ..
    make
    cd ..
fi

# Kill existing battleship session if it exists
tmux kill-session -t battleship 2>/dev/null

# Start tmux session
tmux new-session -d -s battleship -n "Server"

# Window 0: Server
tmux send-keys -t battleship:0 'cd build && ./server 5000' C-m

# Wait a moment for server to start
sleep 2

# Window 1: Player 1
tmux new-window -t battleship:1 -n "Player1"
tmux send-keys -t battleship:1 'cd build && ./client 127.0.0.1 5000 Alice' C-m

# Window 2: Player 2
tmux new-window -t battleship:2 -n "Player2"
tmux send-keys -t battleship:2 'cd build && ./client 127.0.0.1 5000 Bob' C-m

echo ""
echo "Demo started in tmux session 'battleship'"
echo ""
echo "Commands:"
echo "  - Switch windows: Ctrl+B then 0/1/2"
echo "  - Detach: Ctrl+B then D"
echo "  - Kill session: tmux kill-session -t battleship"
echo ""
echo "Attaching to session..."
sleep 1

# Attach to session
tmux attach-session -t battleship


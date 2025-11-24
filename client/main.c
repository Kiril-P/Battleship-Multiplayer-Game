#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port> [player_name]\n", argv[0]);
        fprintf(stderr, "Example: %s 192.168.1.50 5000 Player1\n", argv[0]);
        return 1;
    }
    
    const char* host = argv[1];
    int port = atoi(argv[2]);
    
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port number: %s\n", argv[2]);
        return 1;
    }
    
    const char* player_name = (argc > 3) ? argv[3] : "Player";
    
    GameClient client;
    
    if (!client_init(&client, host, port, player_name)) {
        fprintf(stderr, "Failed to initialize client\n");
        return 1;
    }
    
    client_run(&client);
    client_cleanup(&client);
    
    return 0;
}


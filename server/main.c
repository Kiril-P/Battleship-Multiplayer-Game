#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int port = 5000; // Default port
    
    // Parse command line arguments
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number: %s\n", argv[1]);
            fprintf(stderr, "Usage: %s [port]\n", argv[0]);
            return 1;
        }
    }
    
    GameServer server;
    
    if (!server_init(&server, port)) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    
    server_run(&server);
    server_cleanup(&server);
    
    return 0;
}




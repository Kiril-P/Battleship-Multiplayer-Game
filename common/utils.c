#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <ctype.h>
#include <sys/time.h>

int create_listening_socket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    
    // Set SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        close(sockfd);
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }
    
    if (listen(sockfd, 5) < 0) {
        perror("listen");
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

int create_client_socket(const char* host, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // Try to convert as IP address first
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        // Try DNS lookup
        struct hostent* he = gethostbyname(host);
        if (he == NULL) {
            fprintf(stderr, "Unknown host: %s\n", host);
            close(sockfd);
            return -1;
        }
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    }
    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

bool set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl F_GETFL");
        return false;
    }
    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl F_SETFL O_NONBLOCK");
        return false;
    }
    
    return true;
}

bool set_socket_timeout(int fd, int timeout_sec) {
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt SO_RCVTIMEO");
        return false;
    }
    
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt SO_SNDTIMEO");
        return false;
    }
    
    return true;
}

bool get_local_ip(char* buffer, size_t buffer_size) {
    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    bool found = false;
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return false;
    }
    
    // Look for first non-loopback IPv4 address
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            char* ip = inet_ntoa(addr->sin_addr);
            
            // Skip loopback
            if (strncmp(ip, "127.", 4) != 0) {
                snprintf(buffer, buffer_size, "%s", ip);
                found = true;
                break;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    
    if (!found) {
        snprintf(buffer, buffer_size, "127.0.0.1");
    }
    
    return true;
}

void safe_strncpy(char* dest, const char* src, size_t dest_size) {
    if (dest_size == 0) return;
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

bool parse_coordinate(const char* str, int* row, int* col) {
    if (str == NULL || strlen(str) < 2) return false;
    
    char row_char = toupper(str[0]);
    if (row_char < 'A' || row_char > 'F') return false;
    
    *row = row_char - 'A';
    
    char col_char = str[1];
    if (col_char < '1' || col_char > '6') return false;
    
    *col = col_char - '1';
    
    return true;
}

void format_coordinate(int row, int col, char* buffer, size_t buffer_size) {
    if (buffer_size < 3) return;
    buffer[0] = 'A' + row;
    buffer[1] = '1' + col;
    buffer[2] = '\0';
}


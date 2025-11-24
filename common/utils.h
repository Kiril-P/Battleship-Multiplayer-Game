#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

// Network utilities
int create_listening_socket(int port);
int create_client_socket(const char* host, int port);
bool set_nonblocking(int fd);
bool set_socket_timeout(int fd, int timeout_sec);

// Get local IP address
bool get_local_ip(char* buffer, size_t buffer_size);

// Safe string utilities
void safe_strncpy(char* dest, const char* src, size_t dest_size);

// Parse coordinate (e.g., "A1" -> row=0, col=0)
bool parse_coordinate(const char* str, int* row, int* col);

// Format coordinate (e.g., row=0, col=0 -> "A1")
void format_coordinate(int row, int col, char* buffer, size_t buffer_size);

#endif // UTILS_H


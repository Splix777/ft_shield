#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdbool.h>
#include <sys/types.h>

#include "threads.h"

#define PORT 4242
#define MAX_CLIENTS 3
#define MAX_QUEUE 10

#define CHAR_BUFFER 1024

#define STORED_HASH "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"

#define AUTH_PROMPT "Enter password: "
#define AUTH_SUCCESS "Authenticated\n"
#define AUTH_FAILURE "Authentication failed\n"
#define WELCOME_MSG "Welcome to the server\n"
#define SERVER_FULL "Server is full, try again later\n"

typedef struct s_client t_client;
typedef struct s_server t_server;

typedef struct s_client
{
    int socket_fd;           // Socket file descriptor
    int shell_fd;            // File descriptor for shell interaction
    pthread_t output_thread; // Thread for monitoring shell output

    bool is_authenticated; // Authentication state for the client
    bool shell_active;     // If the shell is currently active

    pid_t shell_pid; // Process ID for capturing shell output (Persistent)
} t_client;

typedef struct s_server
{
    t_client clients[MAX_CLIENTS]; // Array of clients
    struct sockaddr_in addr;       // Server address (IP and port)
    int socket_fd;                 // Socket file descriptor
    int max_fd;                    // Maximum file descriptor
    int client_count;              // Current number of clients
    int port;                      // Port number

    fd_set masterfds; // File descriptor set for master (all clients)
    fd_set readfds;   // File descriptor set for reading (active clients)
} t_server;

// Server Setup
void boot_server(t_server *server, shared_control_t *control);

// Middleware
int authenticate_client(t_client *client);

// Connection Handling
void send_message(int fd, const char *message);
void remove_client(t_server *server, t_client *client);
void new_client(t_server *server);
void client_handler(t_server *server, t_client *client);

// Shell Interaction
void spawn_shell(t_client *client);
void help(t_client *client);

#endif // SERVER_H
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "threads.h"
#include "server.h"
#include "logger.h"

static int configure_server(t_server *server)
{
    bzero(server, sizeof(t_server));
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        memset(&server->clients[i], -1, sizeof(t_client));
        server->clients[i].is_authenticated = false;
        server->clients[i].shell_active = false;
    }

    server->port = PORT;
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server->socket_fd < 0)
        return 0;

    int opt = 1;
    setsockopt(server->socket_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    fcntl(server->socket_fd, F_SETFL, O_NONBLOCK);

    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = INADDR_ANY;
    server->addr.sin_port = htons(server->port);

    if (bind(server->socket_fd,
             (struct sockaddr *)&server->addr,
             sizeof(server->addr)) < 0)
    {
        log_to_file("Failed to bind server.");
        close(server->socket_fd);
        return 0;
    }

    if (listen(server->socket_fd, MAX_QUEUE) < 0)
    {
        log_to_file("Failed to listen on server.");
        close(server->socket_fd);
        return 0;
    }

    return 1;
}

void boot_server(t_server *server, shared_control_t *control)
{
    if (!configure_server(server))
    {
        log_to_file("Failed to configure server.");
        atomic_store(&control->keep_running, 0);
        atomic_store(&control->exit_status, 1);
        return;
    }

    FD_ZERO(&server->readfds);
    FD_ZERO(&server->masterfds);

    FD_SET(server->socket_fd, &server->masterfds);

    server->max_fd = server->socket_fd;
}

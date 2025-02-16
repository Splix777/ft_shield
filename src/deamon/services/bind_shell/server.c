#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "server.h"
#include "logger.h"

static void cleanup_server(t_server *server)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (server->clients[i].socket_fd != -1)
            remove_client(server, &server->clients[i]);
    }
    close(server->socket_fd);
}

static t_client *middleware(t_server *server, int fd)
{
    t_client *client = NULL;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (server->clients[i].socket_fd == fd)
            client = &server->clients[i];

    if (!client->is_authenticated)
    {
        if (!authenticate_client(client))
        {
            send_message(client->socket_fd, AUTH_FAILURE);
            remove_client(server, client);
            return NULL;
        }
        send_message(client->socket_fd, AUTH_SUCCESS);
        return NULL;
    }

    return client;
}

void *bind_shell_server(void *arg)
{
    shared_control_t *control = (shared_control_t *)arg;
    t_server server;

    boot_server(&server, control);
    log_to_file("Server started on port %d.", server.port);
    while (atomic_load(&control->keep_running))
    {
        if (atomic_load(&control->pause_server))
        {
            sleep(1);
            continue;
        }

        server.readfds = server.masterfds;
        if (select(server.max_fd + 1, &server.readfds, NULL, NULL, NULL) < 0)
            break;

        for (int fd = server.socket_fd; fd <= server.max_fd; fd++)
        {
            if (!FD_ISSET(fd, &server.readfds))
                continue;

            if (fd == server.socket_fd)
                new_client(&server);
            else
            {
                t_client *client = middleware(&server, fd);
                if (client)
                    client_handler(&server, client);
            }
        }
    }

    cleanup_server(&server);
    return NULL;
}

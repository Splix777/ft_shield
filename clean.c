#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PID_FILE "/run/ft_shield.pid"
#define BIN_PATH "/sbin/ft_shield"
#define LOG_PATH "/tmp/ft_shield.log"
#define SERVICE_PATH "/etc/systemd/system/ft_shield.service"
#define SERVICE_SYMLINK "/etc/systemd/system/multi-user.target.wants/ft_shield.service"

int main()
{
    // Clean up ft_shield

    // Remove the /bin/ft_shield file
    if (unlink(BIN_PATH) < 0)
        perror("Cannot remove /sbin/ft_shield");

    // Rmove the logs at /tmp/ft_shield.log
    if (unlink(LOG_PATH) < 0)
        perror("Cannot remove /tmp/ft_shield.log");

    // Remove the systemd service file
    if (unlink(SERVICE_PATH) < 0)
        perror("Cannot remove /etc/systemd/system/ft_shield.service");

    // Remove the multi-user.target symlink
    if (unlink(SERVICE_SYMLINK) < 0)
        perror("Cannot remove /etc/systemd/system/multi-user.target.wants/ft_shield.service");

    // Remove the systemd service
    if (system("systemctl --no-pager stop ft_shield") != 0)
        fprintf(stderr, "Cannot stop ft_shield service\n");

    // Reload systemd to recognize new service file
    if (system("systemctl --no-pager daemon-reload") != 0)
        fprintf(stderr, "Failed to reload systemd\n");

    // Remove the PID file
    if (unlink(PID_FILE) < 0)
        perror("Cannot remove PID file");

    printf("ft_shield has been removed\n");
    return 0;
}
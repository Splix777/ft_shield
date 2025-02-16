#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>

#include "logger.h"

void log_to_file(const char *format, ...)
{
    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT | O_SYNC, 0644);
    if (fd < 0)
    {
        perror("Error opening log file");
        return;
    }

    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file)
    {
        perror("Unable to open log file");
        return;
    }

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    va_end(args);

    fclose(log_file);
}

void delete_logs()
{
    if (access(LOG_FILE, F_OK) == -1)
        return;

    remove(LOG_FILE);
}

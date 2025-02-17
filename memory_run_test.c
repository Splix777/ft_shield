#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

#define FT_SHIELD_PATH "ft_shield"

int main()
{
    int file_fd = open(FT_SHIELD_PATH, O_RDONLY);
    if (file_fd == -1)
    {
        perror("open");
        return 1;
    }

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("lseek");
        close(file_fd);
        return 1;
    }
    lseek(file_fd, 0, SEEK_SET);

    int mem_fd = syscall(SYS_memfd_create, "ft_shield_mem", 0);
    if (mem_fd == -1)
    {
        perror("memfd_create");
        close(file_fd);
        return 1;
    }

    char *buffer = malloc(file_size);
    if (!buffer)
    {
        perror("malloc");
        close(file_fd);
        close(mem_fd);
        return 1;
    }

    if (read(file_fd, buffer, file_size) != file_size)
    {
        perror("read");
        free(buffer);
        close(file_fd);
        close(mem_fd);
        return 1;
    }

    if (write(mem_fd, buffer, file_size) != file_size)
    {
        perror("write");
        free(buffer);
        close(file_fd);
        close(mem_fd);
        return 1;
    }

    free(buffer);
    close(file_fd);

    char *argv[] = {NULL};
    char *envp[] = {NULL};

    printf("Executing ft_shield from memory...\n");
    fexecve(mem_fd, argv, envp);

    perror("fexecve");
    close(mem_fd);
    return 1;
}

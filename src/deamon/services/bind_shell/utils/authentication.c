#include <fcntl.h>

#include "server.h"
#include "sha256.h"

static int verify_password(const char *password, const char *stored_hash)
{
    char modifiable_password[CHAR_BUFFER];

    strncpy(modifiable_password, password, CHAR_BUFFER);
    modifiable_password[CHAR_BUFFER - 1] = '\0';
    modifiable_password[strcspn(modifiable_password, "\n")] = 0;

    return (strcmp(SHA256(modifiable_password), stored_hash) == 0) ? 1 : 0;
}

int authenticate_client(t_client *client)
{
    char *buffer = malloc(CHAR_BUFFER);
    if (!buffer)
        return 0;

    if (recv(client->socket_fd, buffer, CHAR_BUFFER, O_NONBLOCK) <= 0)
    {
        free(buffer);
        return 0;
    }

    if (verify_password(buffer, STORED_HASH))
        client->is_authenticated = true;

    free(buffer);
    return (client->is_authenticated) ? 1 : 0;
}

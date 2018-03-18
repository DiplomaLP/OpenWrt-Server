#include "server.h"
#include <stdio.h>

int main(int argc , char *argv[])
{
    struct server *server;

    int ret = server_create(5000, &server);
    if (ret != 0) {
        fprintf(stderr, "server_create() failed! ret = %d \n", ret);
        return -1;
    }

    server_destroy(server);

    return 0;
}
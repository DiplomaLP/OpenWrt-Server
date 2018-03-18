#include "server.h"
#include <stdio.h>
#include <memory.h>

int test1_handler(const char *buffer, size_t buffer_len) {
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    return 0;
}

int test2_handler(const char *buffer, size_t buffer_len) {
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    return -1;
}

int main(int argc , char *argv[])
{
    struct server *server;

    struct server_subscriber subscriber_test1 = {.command = "test1", .handler = test1_handler};
    struct server_subscriber subscriber_test2 = {.command = "test2", .handler = test2_handler};

    int ret = server_create(12345, &server);
    if (ret != 0) {
        fprintf(stderr, "server_create() failed! ret = %d \n", ret);
        return -1;
    }

    server_add_subscriber(server, &subscriber_test1);
    server_add_subscriber(server, &subscriber_test2);

    server_start(server);

    server_destroy(server);

    return 0;
}
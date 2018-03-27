#include "server.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

int move_left_handler(const char *buffer, size_t buffer_len) {
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    system("echo \"1\" > /dev/ttyATH0");

    return 0;
}

int move_right_handler(const char *buffer, size_t buffer_len) {
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    system("echo \"0\" > /dev/ttyATH0");

    return 0;
}

int main(int argc , char *argv[])
{
    struct server *server;

    struct server_subscriber subscriber_test1 = {.command = "MOVE_LEFT", .handler = move_left_handler};
    struct server_subscriber subscriber_test2 = {.command = "MOVE_RIGHT", .handler = move_right_handler};

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
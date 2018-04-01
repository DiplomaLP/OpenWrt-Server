#pragma once

#include <sys/param.h>
#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

#define ALREADY_CONNECTED "ALREADY_CONNECTED"

#define CONNECTION_CLOSED "CONNECTION_CLOSED"

#define CONNECTED_SUCCESSFULLY "CONNECTED_SUCCESSFULLY"

#define COMMAND_CLOSE_CONNECTION "COMMAND_CLOSE_CONNECTION"

#define BUFFER_SIZE (256L)

#define COMMAND_SIZE (128)

#define SERVER_SUBSCRIBER_COUNT (10)

struct server_subscriber {
    const char *command;

    const void *user_data;

    int (*handler)(const char *buffer, size_t buffer_len, const void *user_data);
};

struct server {
    int socket;

    char buf[BUFFER_SIZE];

    uint32_t subscribers_count;

    struct server_subscriber subscribers[SERVER_SUBSCRIBER_COUNT];
};

int server_create(uint16_t port, struct server **server_out);

int server_add_subscriber(struct server *self, const struct server_subscriber *subscriber);

int server_start(struct server *self);

int server_destroy(struct server *self);

__END_DECLS

#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int server_create(uint16_t port, struct server **server_out)
{
    int ret = 0;

    struct server *server = calloc(sizeof(*server), 1);

    if (server == NULL) {
        fprintf(stderr, "calloc(server) failed! \n");
        return -ENOMEM;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "socket(AF_INET, SOCK_STREAM, 0) failed, return -1, errno = %d \n", errno);
        free(server);

        return -errno;
    }

    fprintf(stderr, " --- socket created, socket = %d\n", server_socket);

    server->socket = server_socket;

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    ret = bind(server->socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        fprintf(stderr, "bind(socket = %d, ...), return = %d, errno = %d\n", server->socket, ret, errno);
        close(server->socket);
        free(server);
        return -errno;
    }

    fprintf(stderr, " --- bind done\n");

    ret = listen(server->socket, 3);
    if (ret == -1 ) {
        fprintf(stderr, "listen(server->socket = %d, 3), return = %d, errno = %d\n", server->socket, ret, errno);
        close(server->socket);
        free(server);
        return -errno;
    }

    fprintf(stderr, " --- listen done\n");

    *server_out = server;

    return 0;
}

static int server_subscriber_parse(struct server *self, const char*message) {

    char message_copy[BUFFER_SIZE];
    memset(message_copy, 0, BUFFER_SIZE);

    strncpy(message_copy, message, BUFFER_SIZE);
    message_copy[BUFFER_SIZE-1] = '\0';

    char *find_term = strchr(message_copy, ':');

    if (find_term == NULL) {
        fprintf(stderr, "didn't find the \':\' in the message: %s \n", message_copy);
        return -1;
    }

    find_term[0] = '\0';

    for (int i = 0; i < self->subscribers_count; i++) {
        if (strncmp(message_copy, self->subscribers[i].command, COMMAND_SIZE) == 0) {
            int ret = self->subscribers[i].handler(message, strnlen(message, BUFFER_SIZE));
            if (ret != 0) {
                fprintf(stderr, "subscriber_handler failedd, ret = %d subscriber: %s, message = %s \n",
                        ret,
                        self->subscribers[i].command,
                        message);
            }

            return ret;
        }
    }

    fprintf(stderr, "the message handler didn't matched, message = %s \n", message);

    return -1;
}

int server_start(struct server *self)
{
    socklen_t addr_len;
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));

    int client_sock = accept(self->socket, (struct sockaddr *)&client, &addr_len);
    if (client_sock == -1) {
        fprintf(stderr, "accept(self->socket = %d, ...)failed, ret = %d, errno = %d \n", self->socket, client_sock, errno);
        return -errno;
    }

    self->client_fd = client_sock;

    fprintf(stderr, " --- connection accepted client socket = %d \n", client_sock);

    while(true) {
        int ret = recv(self->client_fd, self->buf, BUFFER_SIZE, 0);
        if (ret == -1) {
            break;
        }
        ret = server_subscriber_parse(self, self->buf);
        if (ret != 0)
            break;
    }

    const char* connection_closed = "connection closed";
    send(self->client_fd, connection_closed, strlen(connection_closed), 0);

    close(client_sock);
}

int server_add_subscriber(struct server *self, struct server_subscriber* subscriber)
{
    if (self->subscribers_count < SERVER_SUBSCRIBER_COUNT) {
        self->subscribers[self->subscribers_count++] = *subscriber;
        return 0;
    }

    return -ENOSPC;
}

int server_stop(struct server *self)
{
    return 0;
}

int server_destroy(struct server *self)
{
    if (self->client_fd) {
        close(self->client_fd);
    }

    close(self->socket);

    free(self);

    return 0;
}

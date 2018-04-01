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
        perror("bind");
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
        find_term = strchr(message_copy, ';');

        if (find_term == NULL) {
            fprintf(stderr, "didn't find the \';\' in the message: %s \n", message_copy);
            return -1;
        }
    }

    find_term[0] = '\0';

    for (int i = 0; i < self->subscribers_count; i++) {
        if (strncmp(message_copy, self->subscribers[i].command, COMMAND_SIZE) == 0) {
            struct server_subscriber* subscriber = &self->subscribers[i];
            int ret = subscriber->handler(message, strnlen(message, BUFFER_SIZE), subscriber->user_data);
            if (ret != 0) {
                fprintf(stderr, "subscriber_handler failedd, ret = %d subscriber: %s, message = %s \n",
                        ret,
                        self->subscribers[i].command,
                        message);
            }

            return ret;
        }
    }

    fprintf(stderr, "the message handler didn't match, message = %s \n", message);

    return -1;
}

static int get_data(struct server *self, int client_socket) {
    fd_set rd_set;
    FD_ZERO(&rd_set);
    FD_SET(self->socket, &rd_set);
    FD_SET(client_socket, &rd_set);

    int nfd = MAX(self->socket, client_socket) + 1;

    int ret = select(nfd, &rd_set, NULL, NULL, NULL);
    switch (ret) {
        case 0:
            fprintf(stderr, "timeout");
            break;
        case -1:
            perror("select");
            return -errno;
        default:
            break;
    }

    if (FD_ISSET(self->socket, &rd_set)) {
        struct sockaddr_in client;
        memset(&client, 0, sizeof(client));
        socklen_t addr_len;

        fprintf(stderr, "try to connect already connected! \n");
        int client_socket2 = accept(self->socket, (struct sockaddr *)&client, &addr_len);
        if (client_socket2 == -1) {
            fprintf(stderr, "new connect accept(self->socket = %d, ...)failed, ret = %d, errno = %d \n",
                    self->socket,
                    client_socket2,
                    errno);

            return -errno;
        }
        fprintf(stderr, "sending already connected \n");

        ssize_t send_ret = send(client_socket2, ALREADY_CONNECTED, strlen(ALREADY_CONNECTED),0);
        if (send_ret == -1) {
            fprintf(stderr, "send(ALREADY_CONNECTED failure. error = %d ) \n", errno);
        }else if (send_ret != strlen(CONNECTION_CLOSED)) {
            fprintf(stderr, "send(ALREADY_CONNECTED return = %d) \n", send_ret);
        }

        shutdown(client_socket2, SHUT_RDWR);

        close(client_socket2);
    }

    if (FD_ISSET(client_socket, &rd_set)) {
        ssize_t ret_recv = recv(client_socket, self->buf, BUFFER_SIZE, 0);
        if (ret_recv == -1) {
            fprintf(stderr, "recv(client_sock = %d, ) failed, ret = %d, errno = %d\n", client_socket, ret_recv, errno);
            return -errno;
        }
        self->buf[ret_recv] = '\0';
        return (int)ret_recv;
    }

    return 0;
}

static int command_close_connection(const char *buffer, size_t buffer_len, const void *user_data) {
    fprintf(stderr, "received connection close, buffer = %s\n", buffer);
    return -ECANCELED;
}

int server_start(struct server *self)
{
    struct server_subscriber subscriber = {
            .handler = command_close_connection,
            .command = COMMAND_CLOSE_CONNECTION};

    server_add_subscriber(self, &subscriber);

    ssize_t ret;
    socklen_t addr_len;
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));

    int client_sock = accept(self->socket, (struct sockaddr *)&client, &addr_len);
    if (client_sock == -1) {
        fprintf(stderr, "accept(self->socket = %d, ...)failed, ret = %d, errno = %d \n", self->socket, client_sock, errno);
        return -errno;
    }

    fprintf(stderr, " --- connection accepted client socket = %d \n", client_sock);

    ret = send(client_sock, CONNECTED_SUCCESSFULLY, strlen(CONNECTED_SUCCESSFULLY), 0);
    if (ret == -1) {
        fprintf(stderr, "send(CONNECTED_SUCCESSFULLY failure. error = %d \n)", errno);
        goto connection_close;
    }else if (ret != strlen(CONNECTED_SUCCESSFULLY)) {
        fprintf(stderr, "send(CONNECTED_SUCCESSFULLY return = %d\n)", ret);
        goto connection_close;
    }

    const uint32_t try_parse = 20;
    uint32_t count_of_parse_error = 0;

    while(true) {
        ret = get_data(self, client_sock);
        if (ret < 0) {
            break;
        }
        if (ret == 0) {
            continue;
        }

        ret = server_subscriber_parse(self, self->buf);
        if (ret == -ECANCELED) {
            fprintf(stderr, "received connection closed! \n");
            break;
        }
        if (ret != 0) {
            if (++count_of_parse_error > try_parse) {
                break;
            }
        } else {
            count_of_parse_error = 0;
        }
    }

connection_close:

    fprintf(stderr, "sending close connection\n");
    ret = send(client_sock, CONNECTION_CLOSED, strlen(CONNECTION_CLOSED), 0);
    if (ret == -1) {
        fprintf(stderr, "send(connection_closed failure. error = %d \n)", errno);
    }else if (ret != strlen(CONNECTION_CLOSED)) {
        fprintf(stderr, "send(connection_closed return = %d\n)", ret);
    }

    shutdown(client_sock, SHUT_RDWR);
    close(client_sock);

    return 0;
}

int server_add_subscriber(struct server *self, const struct server_subscriber *subscriber)
{
    if (self->subscribers_count < SERVER_SUBSCRIBER_COUNT) {
        self->subscribers[self->subscribers_count++] = *subscriber;
        return 0;
    }

    return -ENOSPC;
}


int server_destroy(struct server *self)
{
   close(self->socket);

    free(self);

    return 0;
}

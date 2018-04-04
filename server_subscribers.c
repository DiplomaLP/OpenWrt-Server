#include <fcntl.h>
#include <zconf.h>
#include <memory.h>
#include "server_subscribers.h"

#define SUBSCRIBERS_LEN (4)

static int move_left_handler(const char *buffer, size_t buffer_len, const void *user_data)
{
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    const struct server_subscriber_ctx *subscriber_ctx = (const struct server_subscriber_ctx*)user_data;

    ssize_t size = write(subscriber_ctx->tty_fd, buffer, buffer_len);
    if (size == -1) {
        perror("left_handler, write(tty_fd, \"1\") failed");
    } else if (size != buffer_len) {
        fprintf(stderr, "left_handler, write(tty_fd, \"1\") failed, ret = %d\n", size);
    }

    return 0;
}

static int move_right_handler(const char *buffer, size_t buffer_len, const void *user_data)
{
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    const struct server_subscriber_ctx *subscriber_ctx = (const struct server_subscriber_ctx*)user_data;

    ssize_t size = write(subscriber_ctx->tty_fd, buffer, buffer_len);
    if (size == -1) {
        perror("right_handler, write(tty_fd, \"0\") failed");
    } else if (size != buffer_len) {
        fprintf(stderr, "right_handler, write(tty_fd, \"0\") failed, ret = %d\n", size);
    }

    return 0;
}

static int move_camera_handler(const char *buffer, size_t buffer_len, const void *user_data)
{
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    const struct server_subscriber_ctx *subscriber_ctx = (const struct server_subscriber_ctx*)user_data;

    ssize_t size = write(subscriber_ctx->tty_fd, buffer, buffer_len);
    if (size == -1) {
        perror("right_handler, write(tty_fd, \"0\") failed");
    } else if (size != buffer_len) {
        fprintf(stderr, "right_handler, write(tty_fd, \"0\") failed, ret = %d\n", size);
    }

    return 0;
}

static int light_turn_on_off_handler(const char *buffer, size_t buffer_len, const void *user_data)
{
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    const struct server_subscriber_ctx *subscriber_ctx = (const struct server_subscriber_ctx*)user_data;

    char *parser = strstr(buffer, ":");
    char value = parser[1];

    const char* data_to_send;

    if (value == '0') {
        data_to_send = "0 light turn off";
    } else if (value == '1') {
        data_to_send = "1 light turn on";
    }

    ssize_t size = write(subscriber_ctx->tty_fd, data_to_send, strlen(data_to_send));
    if (size == -1) {
        perror("right_handler, write(tty_fd, \"0\") failed");
    } else if (size != strlen(data_to_send)) {
        fprintf(stderr, "right_handler, write(tty_fd, \"0\") failed, ret = %d\n", size);
    }

    return 0;
}


int server_subscribers_init_ctx(struct server_subscriber_ctx  *subscriber_ctx)
{

    int fd = open("/dev/ttyATH0", O_WRONLY);
    if (fd == -1) {
        perror("open(ttyATH0) failed");
        return -1;
    }

    subscriber_ctx->tty_fd = fd;
    return 0;
}

void server_subscribers_get(const struct server_subscriber_ctx *ctx,
                            struct server_subscriber **subscribers_out,
                            int *subscribers_len_out)
{
    static struct server_subscriber subscribers[SUBSCRIBERS_LEN];

    subscribers[0].user_data = ctx;
    subscribers[0].command = "MOVE_LEFT";
    subscribers[0].handler = move_left_handler;

    subscribers[1].user_data = ctx;
    subscribers[1].command = "MOVE_RIGHT";
    subscribers[1].handler = move_right_handler;

    subscribers[2].user_data = ctx;
    subscribers[2].command = "MOVE_CAMERA";
    subscribers[2].handler = move_camera_handler;

    subscribers[3].user_data = ctx;
    subscribers[3].command = "LIGHT_TURN";
    subscribers[3].handler = light_turn_on_off_handler;

    *subscribers_len_out = SUBSCRIBERS_LEN;
    *subscribers_out = subscribers;
}

int server_subscribers_deinit_ctx(struct server_subscriber_ctx  *subscriber_ctx)
{
    int ret = close(subscriber_ctx->tty_fd);
    if (ret == -1) {
        perror("close(tty_fd) failed");
    }
    return ret;
}

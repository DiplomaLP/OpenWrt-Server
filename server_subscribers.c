#include <fcntl.h>
#include <zconf.h>
#include <memory.h>
#include "server_subscribers.h"

static int move_left_handler(const char *buffer, size_t buffer_len, const void *user_data)
{
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    const struct server_subscriber_ctx *subscriber_ctx = (const struct server_subscriber_ctx*)user_data;

    const char* data_to_send = "1";
    const size_t data_len = strlen(data_to_send);

    ssize_t size = write(subscriber_ctx->tty_fd, data_to_send, data_len);
    if (size == -1) {
        perror("left_handler, write(tty_fd, \"1\") failed");
    } else if (size != data_len) {
        fprintf(stderr, "left_handler, write(tty_fd, \"1\") failed, ret = %d\n", size);
    }

    return 0;
}

static int move_right_handler(const char *buffer, size_t buffer_len, const void *user_data)
{
    fprintf(stderr, " --- in handler, buffer = %s, buffer_len = %d \n", buffer, buffer_len);

    const struct server_subscriber_ctx *subscriber_ctx = (const struct server_subscriber_ctx*)user_data;

    const char* data_to_send = "0";
    const size_t data_len = strlen(data_to_send);

    ssize_t size = write(subscriber_ctx->tty_fd, data_to_send, data_len);
    if (size == -1) {
        perror("right_handler, write(tty_fd, \"0\") failed");
    } else if (size != data_len) {
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
    static struct server_subscriber subscribers[2];

    subscribers[0].user_data = ctx;
    subscribers[0].command = "MOVE_LEFT";
    subscribers[0].handler = move_left_handler;

    subscribers[1].user_data = ctx;
    subscribers[1].command = "MOVE_RIGHT";
    subscribers[1].handler = move_right_handler;

    *subscribers_len_out = 2;
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

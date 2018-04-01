#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "server.h"

__BEGIN_DECLS

struct server_subscriber_ctx {
    int tty_fd;
};

int server_subscribers_init_ctx(struct server_subscriber_ctx  *subscriber_ctx);

void server_subscribers_get(const struct server_subscriber_ctx *ctx,
                            struct server_subscriber **subscribers_out,
                            int *subscribers_len_out);

int server_subscribers_deinit_ctx(struct server_subscriber_ctx  *subscriber_ctx);

__END_DECLS

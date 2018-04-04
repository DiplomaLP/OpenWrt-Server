#include <zconf.h>
#include "server.h"
#include "server_subscribers.h"

int server_job()
{
    struct server *server;

    int ret = server_create(12345, &server);
    if (ret != 0) {
        fprintf(stderr, "server_create() failed! ret = %d \n", ret);
        return -1;
    }

    struct server_subscriber_ctx subscriber_ctx;
    ret = server_subscribers_init_ctx(&subscriber_ctx);
    if (ret != 0) {
        server_destroy(server);
    }

    struct server_subscriber *subscribers;
    int subscribers_len;
    server_subscribers_get(&subscriber_ctx, &subscribers, &subscribers_len);

    for (int i = 0; i < subscribers_len; i++) {
        server_add_subscriber(server, &subscribers[i]);
    }

    server_start(server);

    server_destroy(server);

    return 0;
}


int main(int argc , char *argv[])
{
    while(1) {
        fprintf(stderr, "--- Start server!!! --- \n\n");
        server_job();
        fprintf(stderr, "--- Server restart, sleep(1) \n\n");
        sleep(1);

    }
}
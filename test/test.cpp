#include <thread>

#include "client.h"
#include "../server.h"

using namespace std;

class test_suite {
protected:
    const uint32_t port = 12345;
    struct server *server;
    thread serverThread;

public:
    test_suite() {
        int ret = server_create(port, &server);
        if (ret != 0) {
            abort();
        }

        serverThread = std::thread([&]() {

            if (server_start(server) != 0)
                abort();
        });
    }

    virtual ~test_suite() {
        if (server) {
            if (server_destroy(server)) {
                abort();
            }
        }

        serverThread.join();
    }
};

class check_connect : public test_suite
{
public:
    Client client;
    check_connect() {
            client.connect("127.0.0.1", port);
    }

    virtual ~check_connect() {
        client.destroy();
    }
};

int main() {
    check_connect connect;

    return 0;
}
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

    int addSubscriber(const struct server_subscriber &subscriber) {
        return server_add_subscriber(server, &subscriber);
    }

    virtual ~test_suite() {
        serverThread.join();

        if (server_destroy(server)) {
            abort();
        }

    }
};

class check_connect : public test_suite
{
    Client client;
public:
    int connect() {
        int ret = client.connect("127.0.0.1", port);
        if (ret != 0) {
            fprintf(stderr, "client connect failed! \n");
        }
        return ret;
    }

    void sendCommand(string command) {
        client.sendCommand(command);
    }

    void disconnect() {
        client.destroy();
    }
};

int main() {
    check_connect test;

    test.connect();

    Client client;
    client.connect("127.0.0.1", 12345);

    test.sendCommand(COMMAND_CLOSE_CONNECTION);
    test.disconnect();

    return 0;
}
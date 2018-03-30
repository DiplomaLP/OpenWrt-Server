#pragma once

#include <stdint.h>
#include <string>

using namespace std;

class Client
{
    int fdSocket;

public:
    Client();

    int connect(const char *ip, uint32_t port);

    void sendCommand(string command);

    void destroy();

    ~Client();
};
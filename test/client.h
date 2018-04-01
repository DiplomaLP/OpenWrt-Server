#pragma once

#include <stdint.h>
#include <string>

using namespace std;

class Client
{
    int fdSocket;
    char recvBuff[1024];

public:
    Client();

    int connect(const char *ip, uint32_t port);

    void sendCommand(string command);

    void destroy();

    ~Client();
};
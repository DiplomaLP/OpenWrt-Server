
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <zconf.h>
#include "client.h"
#include "../server.h"

Client::Client() {

}

int Client::connect(const char *ip, uint32_t port) {
    ssize_t n = 0;
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));
    if((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if( ::connect(fdSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    n = recv(fdSocket, recvBuff, sizeof(recvBuff)-1, 0);
    recvBuff[n] = '\0';

    if (strcmp(recvBuff, CONNECTED_SUCCESSFULLY) != 0) {
        fprintf(stderr, "client: after try connect receive: %s \n", recvBuff);
        return -1;
    }
    return 0;
}

void Client::sendCommand(string command) {
    ssize_t n = send(fdSocket, command.c_str(), command.length(), 0);
    if (n != command.length()) {
        perror("send client!\n");
        fprintf(stderr, "send failed, return = %d\n", n);
    }
}

void Client::destroy() {
    close(fdSocket);
}

Client::~Client() {

}

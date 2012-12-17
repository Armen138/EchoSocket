#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <sstream>
#include "../include/Connection.h"

class WebSocket
{
    public:
        WebSocket(int port);
        ~WebSocket();
        void run();
        void closeSocket();
    protected:
    private:
        int socketFD;
        std::vector<Connection*> conn;
        void readMessages();
        void acceptConnections();
        void handShake();
};

#endif // WEBSOCKET_H

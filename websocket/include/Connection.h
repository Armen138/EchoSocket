#ifndef CONNECTION_H
#define CONNECTION_H
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
#include <string>
#include <vector>
#include <regex>
#include <map>
#include "../include/ConnectionListener.h"

enum E_STATUS {
    CONNECTING,
    READY,
    DISCONNECTED
};

class Connection
{
    public:
        Connection(int sock);
        ~Connection();
        E_STATUS status;
        void update();
        void sendMessage(std::string msg = "", bool fin = true,  bool continuation = false);
        void addEventListener(ConnectionListener* connectionListener);
    protected:
    private:
        std::vector<ConnectionListener*> connectionListeners;
        bool active;
        int socket;
        std::string fetch();
        void connectionError() { std::cout << "Connection error!\n"; }
        std::string lastMessage;
        unsigned long lastLen;
};

#endif // CONNECTION_H
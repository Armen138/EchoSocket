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

class Connection
{
    public:
        Connection(int sock);
        ~Connection();
        void update();
        void sendMessage(std::string msg);
    protected:
    private:
        bool active;
        int socket;
        std::string fetch();
        void connectionError() { std::cout << "Connection error!\n"; }
        std::string lastMessage;
        unsigned long lastLen;
};

#endif // CONNECTION_H

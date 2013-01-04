#include <iostream>
#include <signal.h>
//#include "include/WebSocket.h"
#include "include/Server.h"

using namespace std;

//WebSocket* websocket;
Server* server;

void kill_server_handler(int signum) {
    cout << "\nKilling server... \n";
    //delete websocket;
    delete server;
    exit(signum);
}

int main()
{
    signal(SIGINT, kill_server_handler);
    //websocket = new WebSocket(10138);
    server = new Server(10138);
	server->run();
    return 0;
}

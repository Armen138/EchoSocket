#include <iostream>
#include <signal.h>

#include "include/Server.h"

using namespace std;

Server* server;

void kill_server_handler(int signum) {
    cout << "\nKilling server... \n";
    delete server;
    exit(signum);
}

int main()
{
    signal(SIGINT, kill_server_handler);
    server = new Server(10138);
	server->run();
    return 0;
}

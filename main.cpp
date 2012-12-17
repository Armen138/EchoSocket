#include <iostream>
#include <signal.h>
#include "include/WebSocket.h"

using namespace std;

WebSocket* websocket;

void kill_server_handler(int signum) {
    cout << "\nKilling server... \n";
    delete websocket;
    exit(signum);
}

int main()
{
    signal(SIGINT, kill_server_handler);
    websocket = new WebSocket(10138);
    while(true) {
        websocket->run();
    }
    return 0;
}

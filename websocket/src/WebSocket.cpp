#include <zlib.h>
#include "../include/WebSocket.h"
#include "../include/SocketListener.h"

WebSocket::WebSocket(int port)
{
    //ctor
    struct sockaddr_in stSockAddr;
    socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(-1 == socketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(stSockAddr));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(port);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    const int optionValue = 1;
    const socklen_t optionLength = sizeof(optionValue);
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (void*)&optionValue, optionLength);

    if(-1 == bind(socketFD,(struct sockaddr *)&stSockAddr, sizeof(stSockAddr)))
    {
      perror("error bind failed");
      close(socketFD);
    }

    if(-1 == listen(socketFD, 10))
    {
      perror("error listen failed");
      close(socketFD);
      exit(EXIT_FAILURE);
    }
    std::cout << "Listening on " << port << "\n";
    int flags = fcntl(socketFD, F_GETFL, 0);
    fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);
}

void WebSocket::closeSocket() {
    std::cout << "Closing all sockets" << std::endl;
    unsigned int i;
    for(i = 0; i < conn.size(); i++) {
        delete conn[i];
    }
    int result = close(socketFD);
    if(result == -1) {
        std::cout << "Socket close error " << errno << std::endl;
    }
}

void WebSocket::addEventListener(SocketListener* socketListener) {
    socketListeners.push_back(socketListener);
}

void WebSocket::acceptConnections() {
    int ConnectFD = accept4(socketFD, NULL, NULL, SOCK_NONBLOCK);

    if(0 > ConnectFD && EAGAIN != errno && EWOULDBLOCK != errno) {
        perror("error accept failed");
    }
    if(ConnectFD > 0) {
        Connection* connection = new Connection(ConnectFD);
        //call socketlistener to pass connection
        for(unsigned int i = 0; i < socketListeners.size(); i++) {
            socketListeners[i]->onConnect(connection);
        }
        conn.push_back(connection);
    }
}

void WebSocket::readMessages() {
    if(conn.size() == 0 ) return;
    for(unsigned int i = 0; i < conn.size(); i++) {
        conn[i]->update();
    }
}

void WebSocket::run() {
    acceptConnections();
    //handShake();
    readMessages();
}

WebSocket::~WebSocket()
{
    //dtor
    closeSocket();
}

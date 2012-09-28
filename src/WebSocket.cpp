#include <zlib.h>
#include "../include/WebSocket.h"
#include "../include/Message.h"

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

    if(-1 == bind(socketFD,(struct sockaddr *)&stSockAddr, sizeof(stSockAddr)))
    {
      perror("error bind failed");
      close(socketFD);
      //exit(EXIT_FAILURE);
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
    unsigned int i;
    for(i = 0; i < incoming.size(); i++) {
        if (-1 == shutdown(incoming[i], SHUT_RDWR)) {
                perror("can not shutdown socket");
        }
    }
    for(i = 0; i < connections.size(); i++) {
        if (-1 == shutdown(connections[i], SHUT_RDWR)) {
                perror("can not shutdown socket");
        }
    }

    /*if (-1 == shutdown(ConnectFD, SHUT_RDWR)) {
        perror("can not shutdown socket");
    }*/
    close(socketFD);
}

void WebSocket::acceptConnections() {
    int ConnectFD = accept4(socketFD, NULL, NULL, SOCK_NONBLOCK);

    if(0 > ConnectFD && EAGAIN != errno && EWOULDBLOCK != errno) {
        perror("error accept failed");
    }
    if(ConnectFD > 0) {
        std::cout << "connection: " << ConnectFD << "\n";
        incoming.push_back(ConnectFD);
    }

      /* perform read write operations ...
      read(ConnectFD,buff,size)*/

    //close(ConnectFD);
}

void WebSocket::handShake() {
    if(incoming.size() == 0) return;
    int i;
    int s = 0;
    char buff[256];
    std::stringstream message;
    for(i = incoming.size(); i != -1; --i) {
        do {
            s = recv(incoming[i], (void*) buff, sizeof(buff), 0);
            if(s > 0) {
                message << strndup(buff, s);
            }
        } while (s > 0);
        if(message.str().size() > 0) {
            std::cout << message.str() << "\n";
            Message *header = new Message(message.str());
            message.clear();
            std::string response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " + header->accept() + "\r\nSec-WebSocket-Protocol: " + header->protocol() + "\r\n\r\n";
            s = send(incoming[i], response.c_str(), response.size(), 0);
            connections.push_back(incoming[i]);
            conn.push_back(new Connection(incoming[i]));
            incoming.erase(incoming.begin() + i);
            std::cout << "bytes sent: " << s << "\n";
        }
    }

}

void WebSocket::readMessages() {
    if(conn.size() == 0 ) return;
    for(unsigned int i = 0; i < conn.size(); i++) {
        conn[i]->update();
    }
    /*
    if(connections.size() == 0 ) return;
    int i, s;
    char buff[256];
    std::stringstream message("");
    bytes.clear();
    for(i = 0; i < connections.size(); i++) {
        do {
            s = recv(incoming[i], (void*) buff, sizeof(buff), 0);
            if(s > 0) {
                message << strndup(buff, s);
                bytes << strndup(buff, s);

            }
        } while (s > 0);
        if(message.str().size() > 0) {
            Message *frame = new Message(message.str());
            //std::cout << "client " << i << " : " << message.str() << "\n";
            message.clear();
        }
        //read from connections[i]
    }
    */
}

void WebSocket::run() {
    acceptConnections();
    handShake();
    readMessages();
}

WebSocket::~WebSocket()
{
    //dtor
    closeSocket();
}

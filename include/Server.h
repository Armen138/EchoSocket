#ifndef SERVER_H
#define SERVER_H

#include "../websocket/include/WebSocket.h"
#include "../include/Client.h"

class Server: public SocketListener {
public:
	Server(int port);
	~Server();
	void run();
	void onConnect(Connection* connection);
private:
	WebSocket* websocket;
	std::vector<Client*> clients;
};

#endif
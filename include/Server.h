#ifndef SERVER_H
#define SERVER_H

#include "../websocket/include/WebSocket.h"
#include "../include/Client.h"

class Server: public SocketListener {
public:
	Server(int port);
	virtual ~Server();
	void run();
	void onConnect(Connection* connection);
    void kill();
private:
	WebSocket* websocket;
	std::vector<Client*> clients;
};

#endif

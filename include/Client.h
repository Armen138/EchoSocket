#ifndef _CLIENT_H
#define _CLIENT_H

#include "../websocket/include/WebSocket.h"

class Client: public ConnectionListener {
public:
	Client(Connection* connection);
	void onMessage(std::string message);
private:
	Connection* connection;
};

#endif

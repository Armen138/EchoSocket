#ifndef __SocketListener_h__
#define __SocketListener_h__

class Connection;

class SocketListener {
public:
	virtual void onConnect(Connection* connection) {};
	virtual void onClose(Connection* connection, std::string reason) {};
};

#endif
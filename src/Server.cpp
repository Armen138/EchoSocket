#include "../include/Server.h"

Server::Server(int port) {
	websocket = new WebSocket(port);
	websocket->addEventListener(this);
}

Server::~Server() {
	delete websocket;
}

void Server::run() {
	while(true) {
		websocket->run();
	}
}

void Server::onConnect(Connection* connection) {
	std::cout << "New Connection\n";
	clients.push_back(new Client(connection));
}
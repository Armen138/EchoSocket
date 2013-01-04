#include "../include/Client.h"

Client::Client(Connection* connection) {
	connection->addEventListener(this);
	this->connection = connection;
}

void Client::onMessage(std::string message) {
	std::cout << "Message " << message << "\n";
	this->connection->sendMessage(message);
}
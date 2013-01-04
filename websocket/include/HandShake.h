#ifndef HANDSHAKE_H
#define HANDSHAKE_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

namespace HandShake {
	bool HandShake(int socket);
	std::string fetch(int socket);
	std::map<std::string, std::string> parse(std::string raw);
};

#endif
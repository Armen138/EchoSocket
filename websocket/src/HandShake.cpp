#include "../include/HandShake.h"
#include "../include/sha1.h"
#include "../include/base64.h"

namespace HandShake {

	std::string fetch(int socket) {
		ssize_t s = 0;
	    char buff[256];
	    std::stringstream message;
	    do {
	        s = recv(socket, (void*) buff, sizeof(buff), MSG_WAITALL);
	        if(s > 0) {
	            message << strndup(buff, s);
	        }
            if(s == -1) {
                //std::cout << "Socket error " << errno << std::endl;
            }
	    } while (s > 0);
	    return message.str();
	}


	std::map<std::string, std::string> parse(std::string raw) {
	    std::vector<std::string> headerLines;
	    std::map<std::string, std::string> headers;
	    size_t last= 0, endofline = 0;
	    do {
	        endofline = raw.find("\n", last);
	        if(endofline != std::string::npos) {
	            headerLines.push_back(raw.substr(last, endofline - last - 1));
	            last = endofline + 1;
	        } else {
	            headerLines.push_back(raw.substr(last));
	        }
	        size_t colon = headerLines.back().find(":");
	        if(colon != std::string::npos) {
	            std::string key = headerLines.back().substr(0, colon);
	            std::string value = headerLines.back().substr(colon + 2);
	            headers[key] = value; //+2 to account for extra whitespace
	        }
	    } while (endofline != std::string::npos);
	    return headers;
	}
	bool HandShake(int socket, Connection* connection) {
		std::string magick = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		std::string raw = "";
        while(raw == "") {
            raw = fetch(socket);
        }
		std::map<std::string, std::string> headers = parse(raw);

        connection->userAgent = headers["User-Agent"];
        connection->extensions = headers["Sec-WebSocket-Extensions"];
        connection->protocol = headers["Sec-WebSocket-Protocol"];
        connection->version = atoi(headers["Sec-WebSocket-Version"].c_str());

	    std::string handshake =  headers["Sec-WebSocket-Key"] + magick;
	    unsigned char hash[20];
	    sha1::calc(handshake.c_str(), handshake.size(), hash);

	    std::string response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " + base64_encode(hash, 20) + "\r\nSec-WebSocket-Protocol: " + headers["Sec-WebSocket-Protocol"] + "\r\n\r\n";
		return  (send(socket, response.c_str(), response.size(), 0) > 0);
	}

}

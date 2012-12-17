#include "../include/HandShake.h"
#include "../include/sha1.h"
#include "../include/base64.h"

namespace HandShake {

	std::string fetch(int socket) {
		int s = 0;
	    char buff[256];
	    std::stringstream message;
	    do {
	        s = recv(socket, (void*) buff, sizeof(buff), 0);
	        if(s > 0) {
	            message << strndup(buff, s);
	        }
	    } while (s > 0);
	    return message.str();
	}


	std::map<std::string, std::string> parse(std::string raw) {
	    std::vector<std::string> headerLines;
	    std::map<std::string, std::string> headers;
	    unsigned int last = 0, endl = 0;
	    do {
	        endl = raw.find("\n", last);
	        if(endl != std::string::npos) {
	            std::cout << "last: " << last << " endl: " << endl << " substr: " << raw.substr(last, endl - last - 1) << "\n";
	            headerLines.push_back(raw.substr(last, endl - last - 1));
	            last = endl + 1;
	        } else {
	            headerLines.push_back(raw.substr(last));
	        }
	        unsigned int colon = headerLines.back().find(":");
	        if(colon != std::string::npos) {
	            std::string key = headerLines.back().substr(0, colon);
	            std::string value = headerLines.back().substr(colon + 2);
	            std::cout << "key: |" << key << "| value: " << value << "\n";
	            headers[key] = value; //+2 to account for extra whitespace
	            std::cout << (*headers.find(key)).second << " <-- val\n";
	        }
	    } while (endl != std::string::npos);
	    for( std::map<std::string, std::string>::iterator ii=headers.begin(); ii!=headers.end(); ++ii) {
	        std::cout << (*ii).first << "::: " << (*ii).second << std::endl;
	    }
	    return headers;
	}
	bool HandShake(int socket) {
		std::string magick = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		std::string raw = fetch(socket);
		std::map<std::string, std::string> headers = parse(raw);

	    std::string handshake =  headers["Sec-WebSocket-Key"] + magick;
	    unsigned char hash[20];
	    sha1::calc(handshake.c_str(), handshake.size(), hash);
	    std::string response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " + base64_encode(hash, 20) + "\r\nSec-WebSocket-Protocol: " + headers["Sec-WebSocket-Protocol"] + "\r\n\r\n";
		return  (send(socket, response.c_str(), response.size(), 0) > 0);
	}

}
/*
HandShake::HandShake(int socket) {
	this->socket = socket;
	std::string magick = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::string raw = fetch();
	std::map<std::string, std::string> headers = parse(raw);

    std::string handshake =  headers["Sec-WebSocket-Key"] + magick;
    unsigned char hash[20];
    sha1::calc(handshake.c_str(), handshake.size(), hash);
    std::string response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " + base64_encode(hash, 20) + "\r\nSec-WebSocket-Protocol: " + headers["Sec-WebSocket-Protocol"] + "\r\n\r\n";
	int s = send(socket, response.c_str(), response.size(), 0);
}*/

/*
std::string HandShake::fetch() {
	int s = 0;
    char buff[256];
    std::stringstream message;
    do {
        s = recv(socket, (void*) buff, sizeof(buff), 0);
        if(s > 0) {
            message << strndup(buff, s);
        }
    } while (s > 0);
    return message.str();
}
*/
/*
std::map<std::string, std::string> HandShake::parse(std::string raw) {
    std::vector<std::string> headerLines;
    std::map<std::string, std::string> headers;
    unsigned int last = 0, endl = 0;
    do {
        endl = raw.find("\n", last);
        if(endl != std::string::npos) {
            std::cout << "last: " << last << " endl: " << endl << " substr: " << raw.substr(last, endl - last - 1) << "\n";
            headerLines.push_back(raw.substr(last, endl - last - 1));
            last = endl + 1;
        } else {
            headerLines.push_back(raw.substr(last));
        }
        unsigned int colon = headerLines.back().find(":");
        if(colon != std::string::npos) {
            std::string key = headerLines.back().substr(0, colon);
            std::string value = headerLines.back().substr(colon + 2);
            std::cout << "key: |" << key << "| value: " << value << "\n";
            headers[key] = value; //+2 to account for extra whitespace
            std::cout << (*headers.find(key)).second << " <-- val\n";
        }
    } while (endl != std::string::npos);
    for( std::map<std::string, std::string>::iterator ii=headers.begin(); ii!=headers.end(); ++ii) {
        std::cout << (*ii).first << "::: " << (*ii).second << std::endl;
    }
    return headers;
}*/
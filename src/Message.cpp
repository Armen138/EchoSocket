#include "../include/Message.h"
#include "../include/sha1.h"
#include "../include/base64.h"

std::string Message::magick = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

Message::Message(std::string raw)
{
    if(raw.substr(0, 14) == "GET / HTTP/1.1") {
        //handshake header;
        openingHandshake(raw);
    }
}

void Message::openingHandshake(std::string raw) {

    std::vector<std::string> headerLines;
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
}

std::string Message::accept() {
    std::string handshake = key() + magick;
    unsigned char hash[20];
    sha1::calc(handshake.c_str(), handshake.size(), hash);
    return base64_encode(hash, 20);
}

Message::~Message()
{
    //dtor
}

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
/*
std::string Message::unravel(std::string raw) {
    std::istringstream input(raw);
    std::stringstream msg("");
    unsigned char byte;
    unsigned char opcode;
    unsigned char mask[4];
    input >> byte;
    bool fin = ((byte & (1 << 7)) == 0x80);
    opcode = byte & 0x0F;
    input >> byte;
    bool masked = ((byte & (1 << 7)) == 0x80);
    unsigned long len = byte & 0x7F;
    std::cout << "len? " << len << "\n";
    if(len == 126) {
        input >> byte;
        len = ((unsigned int)byte) * 256;
        input >> byte;
        len += (unsigned int) byte;
        std::cout << "len? " << len << "\n";
    } else {
        if(len == 127) {
            long long eightbytelen;
            input >> eightbytelen;
            std::cout << "len? " << eightbytelen << "\n";
            len = (unsigned long) eightbytelen;
        }
    }
    input >> mask[0] >> mask[1] >> mask[2] >> mask[3];
    for(unsigned int i = 0; i < len; i++) {
        unsigned char byte, j;
        char* o = new char(2);
        input >> byte;
        j = i % 4;
        o[0] = ((char)byte ^ mask[j]);
        o[1] = '\0';
        msg << o;
    }
    std::cout << "fin " << fin << "\n";
    std::cout << "masked " << masked << "\n";
    std::cout << "len " << len << "\n";
    std::cout << "opcode " << (int)opcode << "\n";
    std::cout << "msg " << msg.str() << "\n";
    return "";
}
*/
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

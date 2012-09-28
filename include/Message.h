#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <map>

class Message
{
    public:
        Message(std::string raw);
        ~Message();
        bool hasHeader(std::string key) { return headers.count(key) == 1; };
        std::string protocol() { return (*headers.find("Sec-WebSocket-Protocol")).second; };
        std::string origin() { return headers["Origin"]; };
        std::string key() { return headers["Sec-WebSocket-Key"]; };
        std::string version() { return headers["Sec-WebSocket-Version"]; };
        std::string extensions() { return headers["Sec-WebSocket-Extensions"]; };
        std::string accept();
        std::map<std::string, std::string> headers;
        static std::string magick;
    protected:
    private:
        void openingHandshake(std::string raw);
        std::string unravel(std::string raw);
        unsigned long seek;
};

#endif // MESSAGE_H

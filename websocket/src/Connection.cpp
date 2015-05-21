#include "../include/Connection.h"
#include "../include/HandShake.h"
#include "../include/ConnectionListener.h"

Connection::Connection(int sock)
{
    //ctor
    socket = sock;
    active = false;
    status = CONNECTING;
}

Connection::~Connection()
{
    if (-1 == shutdown(socket, SHUT_RDWR)) {
        perror("can not shutdown socket");
    }
    //dtor
}

void Connection::update() {
    if(active) {
        std::string msg = fetch();
        if(msg != "") {
            for(unsigned int i = 0; i < connectionListeners.size(); i++) {
                connectionListeners[i]->onMessage(msg);
            }
        }
    } else {
        bool connected = HandShake::HandShake(socket, this);
        if(connected) {
            active = true;
            status = READY;
            std::cout << "Requested protocol: " << this->protocol << std::endl;
        }
    }
}


void Connection::addEventListener(ConnectionListener* connectionListener) {
    connectionListeners.push_back(connectionListener);
}

void Connection::sendMessage(std::string msg, bool fin, bool continuation) {
    uint64_t length = msg.size();
    unsigned char dataStart = 2;
    unsigned char frame[length + 11];
    frame[0] = (0 | ( 1 << 7)); //FIN = 1
    if(!fin) {
        frame[0] = 0;
    }
    if(!continuation) {
        frame[0] |= 1;
    }
    if(length < 126) {
        frame[1] = length;
    } else {
        if (length < 65533) {
            frame[1] = 126;
            frame[2] = length / 256;
            frame[3] = length % 256;
            dataStart = 4;
        } else {
            std::vector<std::string> pieces;
            unsigned int    step = 65530,
                            seek = 0;
            while(seek < length) {
                pieces.push_back(msg.substr(seek, step));
                seek += step;
            }

            for(unsigned int i = 0; i < pieces.size(); i++) {
                sendMessage(pieces[i], (i == pieces.size() -1) , (i != 0));
            }
            return;
        }
    }
    frame[dataStart] = '\0';
    strncat((char*)frame, msg.c_str(), msg.size());

    send(socket, frame, dataStart + length, 0);
}

std::string Connection::fetch() {
    int s;
    unsigned char   byte[1],
                    maskingKey[4];

    s = recv(socket, (void*) byte, 1, 0);
    if(s == 0) return "";
    if(s != 1) {
        if(s != -1 ) {
            std::cout << "error " << s << "\n";
            status = DISCONNECTED;
            for(unsigned int i = 0; i < connectionListeners.size(); i++) {
                connectionListeners[i]->onClose("recv error" );
            }
        } else {
            if(errno != EAGAIN)
                perror("error recv failed");
        }
        return "";
    }
    int byteVal = (int)byte[0];
    unsigned char opcode = byteVal & 0x0F;
    s = recv(socket, (void*) byte, 1, 0);
    if(s != 1) {
        connectionError();
        return "";
    }
    byteVal = (int)byte[0];
    bool masked = ((byteVal & (1 << 7)) == 0x80);
    if(!(opcode == 1 || opcode == 0)) return "";
    if(!masked) {
        std::cout << "warning: unmasked frame ignored\n";
        return "";
    }
    unsigned long len = 0;
    if(opcode == 1) {
        len = byteVal & 0x7F;
        if(len == 126) {
            unsigned char tb[2];
            s = recv(socket, (void*) tb, 2, 0);
            len = (unsigned long)(tb[0] * 256 + tb[1]);
        } else {
            if(len == 127) {
                unsigned char eb[8];
                uint64_t eightbyte = 0;
                s = recv(socket, (void*) eb, 8, 0);
                for(int i = 0; i < 8; i++) {
                    eightbyte = (eightbyte * 256) + eb[i];
                }
                len = (unsigned long) eightbyte;
            }
        }
        lastLen = len;
        //std::cout << "len: " << len << "\n";
    } else {
        len = lastLen;
    }
    unsigned char maskedData[512];
    unsigned char unMaskedData[len + 1];
    s = recv(socket, (void*) maskingKey, 4, 0);
    unsigned long received = 0;
    bool moreData = true;
    while(moreData) {
        s = recv(socket, (void*) maskedData, 512, 0);
        if(s > 0) {
            for(int i = 0; i < s; i++) {
                unMaskedData[received + i] = (unsigned char)((int)maskedData[i] ^ (int)maskingKey[(received + i) % 4]);
            }
            received += s;
        } else {
            moreData = false;
        }
    }
    unMaskedData[len] = '\0';
    return std::string((char*)unMaskedData);
}

#include "../include/Connection.h"

Connection::Connection(int sock)
{
    //ctor
    this->socket = sock;
    active = true;
}

Connection::~Connection()
{
    //dtor
}

void Connection::update() {
    std::string msg = fetch();
    if(msg != "") {
        //std::cout << msg << "\n";
        std::cout << "message length: " << msg.size() << "\n";
        sendMessage(msg);
    }

}

void Connection::sendMessage(std::string msg) {
    unsigned long length = msg.size();
    unsigned char dataStart = 2;
    unsigned char frame[length + 3];
    frame[0] = 0 | ( 1 << 7); //FIN = 1
    frame[0] |= 1; // opcode = 1 (text)
    if(length < 126) {
        frame[1] = length;
    } else {
        if (length < 65535) {
            frame[1] = 126;

            frame[2] = length / 256;
            frame[3] = length - (length / 256);
            std::cout << "[out] " << (int)frame[2] << ":" << (int)frame[3] <<"\n";
            dataStart = 4;

        } else {
            unsigned char big[8];
            frame[1] = 127;
            memcpy(big, &length, 8);
            std::cout << "[out] len: " << length <<"\n";
            for(int i = 0; i < 8; i++) {
                frame[2 + i]  = big[i];
                std::cout << "[out] byte: " << (int)big[i] <<"\n";
            }
            dataStart = 10;
        }
    }
    frame[dataStart] = '\0';
    strncat((char*)frame, msg.c_str(), msg.size());
    send(socket, frame, dataStart + length + 1, 0);
}

std::string Connection::fetch() {
    int s;
    unsigned char byte[1];// = new unsigned char(1);
    unsigned char maskingKey[4];// = new unsigned char(4);
    s = recv(socket, (void*) byte, 1, 0);
    if(s == 0) return "";
    if(s != 1) {
        //connectionError();
        if(s != -1 ) {
            std::cout << "error " << s << "\n";
        } else {
            if(errno != EAGAIN)
                perror("error recv failed");
        }
        return "";
    }
    int byteVal = (int)byte[0];
    bool fin = ((byteVal & (1 << 7)) == 0x80);
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
    std::cout << "fin: " << fin << "\n";
    std::cout << "opcode " << (int)opcode << "\n";
    std::cout << "masked: " << masked << "\n";
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
        std::cout << "len: " << len << "\n";
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

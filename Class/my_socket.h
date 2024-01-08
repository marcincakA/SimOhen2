#ifndef SOCKETS_CLIENT_MY_SOCKET_H
#define SOCKETS_CLIENT_MY_SOCKET_H

#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <string>

class MySocket {
public:
    static MySocket* createConnection(std::string hostName, short port);
    ~MySocket();
    void sendData(const std::string& data);
    bool sendFile(const char* fileName);
    bool receiveFile(const char *fileName);
    void sendEndMessage();
protected:
    MySocket(SOCKET socket);
private:
    static const char * endMessage;
    SOCKET connectSocket;
};

#endif //SOCKETS_CLIENT_MY_SOCKET_H

#include <string>
#include <netinet/in.h>

#pragma once

// оставить дефайн или поместить в класс?
#define MSG_MAX_LEN 1024

class ClientCls {
protected:
    const char* SERVER_IP;

    int socket;
    struct sockaddr_in address;
    
    // int sockClientCreate();
    int createConnection();   
    void doGetRequest();
    void doSetRequest();
    void showMenu();
    
public:
    ClientCls(std::string ipAddr);
    int sockClientConnect();
    int getSocket();  // нужна или?
    ~ClientCls();

};

class ClientClsVer2 : public ClientCls {

public:
    ClientClsVer2(std::string ipAddr) : ClientCls(ipAddr) {createConnection();}
    int sockClientConnect();
    ~ClientClsVer2();
};
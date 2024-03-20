#include <string>
#include <map>
#include <rocksdb/db.h>
#include <sys/socket.h>

#pragma once

// оставить дефайн или поместить в класс?
#define MSG_MAX_LEN 1024

class ServerCls {
private:
    const std::string GET_REQUEST = "GET";
    const std::string SET_REQUEST = "SET";

    int listener;
    struct sockaddr_in address;
    rocksdb::DB* db;
    //vector<int> clients; - нужно ли хранить всех подключившихся клиентов?

    std::string getClientId(int socket, sockaddr_in addr);
    int sockServerCreate(std::string idAddr);
    int sockSend(int socket, sockaddr_in addr, std::string msg);
    int sockRecv(int socket, sockaddr_in addr, std::string &msg);
    int dbOpen(std::string dbName);
    int dbGet(std::string key, std::string &value);
    int dbSet(std::string key, std::string value);
    void *thread_func(void *arg);
    
public:
    ServerCls(std::string idAddr, std::string dbName);
    int sockSeverAccepst();
    ~ServerCls();

};
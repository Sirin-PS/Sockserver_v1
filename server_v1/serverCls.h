#include <string>
#include <rocksdb/db.h>
#include <netinet/in.h>

#pragma once

// оставить дефайн или поместить в класс?
#define MSG_MAX_LEN 1024

class ServerCls {
protected:
    const int QUEUE_LEN = 10;
    const char* SERVER_IP;

    int listener;
    struct sockaddr_in address;
    rocksdb::DB* db;
    //vector<int> clients; - нужно ли хранить всех подключившихся клиентов?
 
    int sockServerCreate();
    int dbOpen(std::string dbName);
    void thread_proc(int socket);
    void doGetRequest(int socket);
    void doSetRequest(int socket);
    std::string getClientIp(int socket);
    std::string dbGet(std::string key);
    std::string dbSet(std::string key, std::string value);
        
public:
    ServerCls(std::string ipAddr, std::string dbName);
    int sockSeverAccepst();
    int getListener();  // нужны ли эти две функции?
    rocksdb::DB* getDb();
    ~ServerCls();

};

class ServerClsVer2 : public ServerCls {
protected:
    void thread_proc(int socket);
public:
    ServerClsVer2(std::string ipAddr, std::string dbName) : ServerCls(ipAddr, dbName) {}
    int sockSeverAccepst();
};
#include <string>
#include <map>
#include <rocksdb/db.h>
#include <sys/socket.h>

#pragma once

// оставить дефайн или поместить в класс?
#define MSG_MAX_LEN 1024

class ServerCls {
private:
    const int QUEUE_LEN = 10;

    int listener;
    struct sockaddr_in address;
    rocksdb::DB* db;
    //vector<int> clients; - нужно ли хранить всех подключившихся клиентов?
    
    int sockServerCreate(std::string idAddr);
    int dbOpen(std::string dbName);
    void *thread_func(void *arg);
    void doGetRequest(int socket);
    void doSetRequest(int socket);
    std::string getClientIp(int socket);
    std::string dbGet(std::string key);
    std::string dbSet(std::string key, std::string value);
        
public:
    ServerCls(std::string idAddr, std::string dbName);
    int sockSeverAccepst();
    int getListener();  // нужны ли эти две функции?
    rocksdb::DB* getDb();
    ~ServerCls();

};
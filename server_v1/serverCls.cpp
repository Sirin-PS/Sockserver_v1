#include "serverCls.h"
#include <iostream>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <vector>
#include <rocksdb/db.h>
#include <rocksdb/options.h>

// constructor
ServerCls::ServerCls(std::string ipAddr, std::string dbName) {
    SERVER_IP = ipAddr.c_str();
    if (sockServerCreate() == -1) {
        std::cout << "error with server socket creating!\n";
    }
    if (dbOpen(dbName) == -1) {
        std::cout << "error with db opening!\n";
    }
}

int ServerCls::sockServerCreate() {
    this->listener = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listener < 0) {
        perror("socket");
        return -1;
        // exit(EXIT_FAILURE);
    }
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(8080);
    //this->address.sin_addr.s_addr = htonl(idAddr); 
    inet_aton(SERVER_IP, &address.sin_addr);
    if (bind(this->listener, (struct sockaddr*) &(this->address), sizeof(this->address)) < 0) {
        perror("bind");
        return -1;
        // exit(EXIT_FAILURE);
    }
    listen(this->listener, ServerCls::QUEUE_LEN);
    return 0;
}

int ServerCls::dbOpen(std::string dbName) {
    rocksdb::Options options;
    rocksdb::Status status;
    options.create_if_missing = true;
    status = rocksdb::DB::Open(options, dbName, &(this->db));
    if (status.ok()) {
        return 0;
    }
    else {
        return -1; 
    }
}

int ServerCls::getListener() {
    return this->listener;
}

rocksdb::DB* ServerCls::getDb() {
    return this->db;
}

int ServerCls::sockSeverAccepst() {
    int client_sock = accept(this->listener, NULL, NULL);
    if (client_sock < 0) {
        perror("accept");
        std::cout << "accept error";
        return -1;
        // exit(EXIT_FAILURE);
    }
    else {
        std::thread thread1(&ServerCls::thread_proc, this, client_sock);
        //std::thread::id tid = (int)thread1.get_id();
        thread1.detach();
        return 0;
    }
}

void ServerCls::thread_proc(int socket) {
    char request_buf[MSG_MAX_LEN] = "";

    // прием команды от клиента
    std::map<std::string, int> commands
    {
        {"GET", 1}, {"SET", 2}, {"OUT", 3}
    };
    if (recv(socket, &request_buf, MSG_MAX_LEN, 0) < 0) {
        std::cout << "request receive error";
        close(socket);
        return;
    }
    // определение команды
    std::string request = request_buf;    
    int request_num = commands[request];
    switch(request_num) {
        case 1:
            doGetRequest(socket);
            break;        
        case 2:
            doSetRequest(socket);
            break;           
        default:
            std::cout << "command undefined";
            break;
    }
    close(socket);
    return;
}

void ServerCls::doGetRequest(int socket) {
    char msg[MSG_MAX_LEN] = "";
    std::string usr_ip = getClientIp(socket);
    std::string result = dbGet(usr_ip);
    strcpy(msg, result.c_str());
    send(socket, msg, sizeof(msg), 0); 
    return;
}

void ServerCls::doSetRequest(int socket) {
    char rcv_buf[MSG_MAX_LEN] = "";
    char msg[MSG_MAX_LEN] = "";
    std::string usr_ip = getClientIp(socket);
    recv(socket, rcv_buf, MSG_MAX_LEN, 0);
    std::string usr_info = rcv_buf;
    std::string result = dbSet(usr_ip, usr_info);
    strcpy(msg, result.c_str());
    send(socket, msg, sizeof(msg), 0); 
    return;
}

std::string ServerCls::getClientIp(int socket) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    // определение ip-адреса
    if (getpeername(socket, (struct sockaddr*) &client_addr, &addrlen) < 0) {
        //perror("getpeername");
        std::cout << "getpeername error";
    }
    std::string usr_ip = inet_ntoa(client_addr.sin_addr);
    return usr_ip;
}

std::string ServerCls::dbGet(std::string key) {
    rocksdb::Status status;
    std::string result;
    status = this->db->Get(rocksdb::ReadOptions(), key, &result);
    if (status.ok()) {
        return result;
    }
    else if (status.IsNotFound()) {
        result = "Info is not found!";
        return result;
    }
    else {
        return status.ToString();
    }
}

std::string ServerCls::dbSet(std::string key, std::string value) {
    rocksdb::Status status;
    std::string result;
    status = this->db->Put(rocksdb::WriteOptions(), key, value);
    if (status.ok()) {
        result = "Info successfully updated!";
        return result;
    }
    else {
        return status.ToString();
    }
}

ServerCls::~ServerCls() {
    close(listener);
    rocksdb::Status status = db->Close();
    delete db;
}

//------------ServerClsVer2---------------------
int ServerClsVer2::sockSeverAccepst() {
    int client_sock = accept(this->listener, NULL, NULL);
    if (client_sock < 0) {
        perror("accept");
        std::cout << "accept error";
        return -1;
        // exit(EXIT_FAILURE);
    }
    else {
        std::cout << "thread created!\n";
        std::thread thread1(&ServerClsVer2::thread_proc, this, client_sock);
        //std::thread::id tid = (int)thread1.get_id();
        thread1.detach();
        return 0;
    }
}

void ServerClsVer2::thread_proc(int socket) {
    char request_buf[MSG_MAX_LEN] = "";

    // прием команды от клиента
    std::map<std::string, int> commands
    {
        {"GET", 1}, {"SET", 2}, {"OUT", 3}
    };
    int request_num = 0;
    while(request_num != 3) {
        if (recv(socket, &request_buf, MSG_MAX_LEN, 0) < 0) {
            std::cout << "request receive error";
            close(socket);
            return;
        }    
        // определение команды
        std::string request = request_buf;    
        request_num = commands[request];
        switch(request_num) {
            case 1:
                doGetRequest(socket);
                break;        
            case 2:
                doSetRequest(socket);
                break; 
            case 3:
                std::cout << "Seance finished!\n";             
                break;          
            default:
                std::cout << "command undefined\n";
                break;
        }
    }    
    close(socket);
    return;
}
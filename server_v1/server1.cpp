#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <vector>
#include <rocksdb/db.h>
#include <rocksdb/options.h>

#define MSG_MAX_LEN 1024
#define GET_REQUEST "GET"
#define SET_REQUEST "SET"

using namespace std;

rocksdb::DB* db;
int listener;
//vector<int> clients;

// 
void sig_handler(int sig) {
    //close(client_sock);
    cout << "\nsignal trapped!\n";
    close(listener);
    rocksdb::Status status = db->Close();
    delete db;
    exit(0);
}

void *service_func(void *arg) {
    int client_sock = *(int*)arg;    
    rocksdb::Status status;    
    char request_buf[MSG_MAX_LEN] = "";
    char msg[MSG_MAX_LEN] = "";

    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    // определение ip-адреса
    if (getpeername(client_sock, (struct sockaddr*) &client_addr, &addrlen) == -1) {
        //perror("getpeername");
        close(client_sock);
        cout << "getpeername error";
        pthread_exit(0);
    }
    string usr_ip = inet_ntoa(client_addr.sin_addr);

    // прием команды от пользователя (set или get)
    int msg_len = recv(client_sock, request_buf, MSG_MAX_LEN, 0);
    if (msg_len <= 0) {
        //perror("recv");
        close(client_sock);
        cout << "recv error";
        pthread_exit(0);
    }
    cout << "\nrequest: " << request_buf << endl;

    // определение команды
    string check_request = request_buf;
    if (check_request == GET_REQUEST) {     
        cout << "\ngetting value..." << endl;   
        string usr_info;
        status = db->Get(rocksdb::ReadOptions(), usr_ip, &usr_info);
        if (status.ok()) {
            strcpy(msg, usr_info.c_str());
            send(client_sock, msg, sizeof(msg), 0); 
        }
        else if (status.IsNotFound()) {
            strcpy(msg, "Info is not found!");
            send(client_sock, msg, sizeof(msg), 0); 
        }
        else {
            strcpy(msg, status.ToString().c_str());
            send(client_sock, msg, sizeof(msg), 0); 
        }
        cout << "Done!" << endl;
    }
    else if (check_request == SET_REQUEST) {
        cout << "\nputting value..." << endl; 
        char rcv_buf[MSG_MAX_LEN] = "";
        int msg_len = recv(client_sock, rcv_buf, MSG_MAX_LEN, 0);
        if (msg_len <= 0) {
            //perror("recv");
            close(client_sock);
            cout << "recv error";
            pthread_exit(0);
        }
        string usr_info = rcv_buf;
        status = db->Put(rocksdb::WriteOptions(), usr_ip, usr_info);
        if (status.ok()) {
            strcpy(msg, "Info successfully updated!");
            send(client_sock, msg, sizeof(msg), 0); 
        }
        else {
            strcpy(msg, status.ToString().c_str());
            send(client_sock, msg, sizeof(msg), 0); 
        }
        cout << "Done!" << endl;   
    }
    else {
        strcpy(msg, "Command undefined!");
        send(client_sock, msg, sizeof(msg), 0); 
    }

    close(client_sock);
    pthread_exit(0);
}


int main(int argc, char *argv[]) {    
    // rocksdb::DB* db;
    rocksdb::Options options;
    rocksdb::Status status;
    options.create_if_missing = true;
    status = rocksdb::DB::Open(options, "/tmp/testdb2", &db);
    assert(status.ok());

    // int listener;
    int client_sock;
    struct sockaddr_in addr;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 

    if (bind(listener, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    // sigaction: закрытие сокета и выход по ctrl+c
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sig_handler;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    sigaction(SIGINT, &act, 0);
    // signal(SIGINT, sig_handler);

    listen(listener, 10);
    while(1) {
        client_sock = accept(listener, NULL, NULL);
        if (client_sock < 0) {
            perror("accept");
            cout << "accept error";
            // exit(EXIT_FAILURE);
        }
        else {
            pthread_t tid;
            pthread_create(&tid, NULL, service_func, (void*) &client_sock);
            pthread_detach(tid);
        }        
    }
    close(listener);
    status = db->Close();
    delete db;

    return 0;
}
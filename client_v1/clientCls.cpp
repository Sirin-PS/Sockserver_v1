#include "clientCls.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <math.h>

ClientCls::ClientCls(std::string ipAddr) {
    ClientCls::SERVER_IP = ipAddr.c_str();
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(8080);
    inet_aton(ClientCls::SERVER_IP, &address.sin_addr);
}

int ClientCls::getSocket() {
    return this->socket;
}

int ClientCls::sockClientConnect() {
    // прием команды от клиента
    int choice = 0; 
    showMenu();
    do {
        std::cin >> choice;
        switch(choice) {
            case 1:     // get
                createConnection();
                doGetRequest();
                close(this->socket);
                break;        
            case 2:     // set
                createConnection();
                doSetRequest();
                close(this->socket);
                break;
            case 3:     // out
                std::cout << "\nBye!\n";
                break;
            default:
                std::cout << "\ncommand undefined\n";
                break;
        }
    } while (choice != 3);
    
    return 0;
}

void ClientCls::showMenu() {
    std::cout << "\n\tGet value: 1" << std::endl;
    std::cout << "\tSet value: 2" << std::endl;
    std::cout << "\tExit: 3" << std::endl;
}

int ClientCls::createConnection() {
    //int sock;
    this->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket < 0) {
        perror("socket");
        //exit(EXIT_FAILURE);
        return -1;
    }
    if (connect(this->socket, (struct sockaddr*) &(this->address), sizeof(this->address)) < 0) {
        perror("connect");
        //exit(EXIT_FAILURE);
        return -1;
    }
    return 0;
}

void ClientCls::doGetRequest() {
    char msg_recv_buf[MSG_MAX_LEN];
    char request_buf[] = "GET";
    send(this->socket, request_buf, sizeof(request_buf), 0);
    recv(this->socket, msg_recv_buf, MSG_MAX_LEN, 0);
    std::cout << "Msg from server:   " << msg_recv_buf << std::endl;
}

void ClientCls::doSetRequest() {
    char msg_recv_buf[MSG_MAX_LEN];
    char msg_send_buf[MSG_MAX_LEN];
    char request_buf[] = "SET";

    send(this->socket, request_buf, sizeof(request_buf), 0);
    std::cout << "\nrequest: " << request_buf << std::endl;
    std::cout << "\nEnter message for server:   ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.getline(msg_send_buf, MSG_MAX_LEN);

    send(this->socket, msg_send_buf, sizeof(msg_send_buf), 0);
    recv(this->socket, msg_recv_buf, MSG_MAX_LEN, 0);
    std::cout << "Msg from server:   " << msg_recv_buf << std::endl;
}

//------------ClientClsVer2---------------------
int ClientClsVer2::sockClientConnect() {
    // прием команды от клиента
    int choice = 0;
    showMenu();
    do {
        std::cin >> choice;
        switch(choice) {
            case 1:     // get
                doGetRequest();
                break;        
            case 2:     // set
                doSetRequest();
                break;
            case 3: {   // out
                char request_buf[] = "OUT";
                send(this->socket, request_buf, sizeof(request_buf), 0);
                std::cout << "\nBye!\n";
                break;
            }                
            default:
                std::cout << "\ncommand undefined\n";
                break;
        }
    } while(choice != 3);
    return 0;
}

ClientClsVer2::~ClientClsVer2() {
    close(socket);
}
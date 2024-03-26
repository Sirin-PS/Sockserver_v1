#include "clientCls.h"
#include <iostream>
#include <sys/socket.h>

using namespace std;

int main(int argc, char **argv)
{    
    // ClientCls client1("127.0.0.1");
    // client1.sockClientConnect();

    ClientClsVer2 client2("127.0.0.1");
    client2.sockClientConnect();
    return 0;
}
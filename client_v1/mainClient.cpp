#include "clientCls.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{    
    ClientCls client1("127.0.0.1");
    client1.sockClientConnect();
    return 0;
}
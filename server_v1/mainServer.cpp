#include "serverCls.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{    
    ServerCls server1("127.0.0.1", "/tmp/testdb1");

    while(1) {
        server1.sockSeverAccepst();
    }
    return 0;
}
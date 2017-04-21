#include "logging.h"
#include "threads.h"
#include "epoller.h"
#include "channel.h"
#include "eventloop.h"
#include "acceptor.h"
#include "server.h"

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

using namespace star;
using namespace std;

int main() {
    setlogfile("1.log");
    setloglevel("TRACE");
    
    Server server(12345);
    server.setConnectionCallback(
            [](const TcpConnectionPtr& conn) {
                if (conn->connected()) {
                    printf("onConnection(): new connection\n");
                } else {
                    printf("failed\n");
                }
            }); 

    server.setMessageCallback(
            [](const TcpConnectionPtr& conn, const char* data, ssize_t len){
                printf("onMessage():received %ld bytes from connection\n", len);
                printf("conntion id %d\n", conn->connId());
                ::write(2, data, len);
            });

    server.start();

    return 0;
}

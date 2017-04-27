#include <star/star.h>
#include <signal.h>

#include <unistd.h>
#include <signal.h>

using namespace star;
using namespace std;


int main() {
    setlogfile("1.log");
    setloglevel("TRACE");
        
    Server server(12345);
    server.signal(SIGINT, [&server]{ server.exit();});
    server.setThreadNum(4);
    server.setConnectionCallback(
            [](const TcpConnectionPtr& conn) {
                if (conn->connected()) {
                    info("%s connectionCallback:new connection %d.\n", util::moment().data(), conn->connId());
                } else {
                    info("failed\n");
                }
            }); 

    server.setMessageCallback(
            [](const TcpConnectionPtr& conn, ReadBuffer* buffer){
                info("%s connection %d messageCallback:received %ld bytes from connection\n", util::moment().data(), conn->connId(), buffer->len());
                const string str = buffer->retrieveAsString();
                conn->send(str);
            });
    
    server.start();

    return 0;
}

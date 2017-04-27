#include <star/star.h>

using namespace star;

int main() {
    setloglevel("TRACE");
    Server server(12222);

    server.setConnectionCallback(
            [](const TcpConnectionPtr& conn) {
                if (conn->connected()) {
                    conn->send(util::moment()+"\n"); 
                    conn->shutdown();
                }
            });    

    server.start();
    return 0;
}

#include <star/star.h>

using namespace star;

int main() {
    Server server(12000);
    server.setMessageCallback(
            [](const TcpConnectionPtr& conn, ReadBuffer* buffer) {
                const std::string message(buffer->retrieveAsString());
            });
    server.start();

    return 0;
}

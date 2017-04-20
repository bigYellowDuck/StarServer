#include "server.h" 
#include "logging.h"

#include <unistd.h>

namespace star {

Server::Server(EventLoop *loop, int port)
    : loop_(loop),
      acceptor_(loop, port) {
    acceptor_.setNewConnectionCallback([this](int sockfd, struct sockaddr_in* addr) {
        newConnection(sockfd, addr);
    });      
}

void Server::start() {
    trace("start running server");
    acceptor_.listen();
    loop_->loop();
}

void Server::newConnection(int sockfd, struct sockaddr_in* addr) {
    trace("server accept a new connection");
    (void)addr;
    char buf[32] = "Hello, I am server!";
    int n = ::write(sockfd, buf, sizeof(buf));
}

}  // end of namespace stat

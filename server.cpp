#include "server.h" 
#include "logging.h"

#include <unistd.h>

namespace star {

Server::Server(int port)
    : loop_(),
      acceptor_(&loop_, port),
      nextConnId_(0) {
    acceptor_.setNewConnectionCallback(
        [this](int sockfd, struct sockaddr_in* addr) {
        newConnection(sockfd, addr);
    });      
}

void Server::start() {
    trace("start running server");
    acceptor_.listen();
    loop_.loop();
}

void Server::newConnection(int sockfd, struct sockaddr_in* addr) {
    ++nextConnId_;
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    trace("server accept a new connectioni %s", buf);
    (void)addr;
    char buf2[32] = "Hello, I am server!";
    int n = ::write(sockfd, buf2, sizeof(buf));
}

}  // end of namespace stat

#include "server.h" 
#include "logging.h"

#include <unistd.h>

namespace star {

Server::Server(int port)
    : loop_(new EventLoop),
      acceptor_(new Acceptor(loop_.get(), port)),
      nextConnId_(1) {
    acceptor_->setNewConnectionCallback(
        [this](int sockfd, struct sockaddr_in* addr) {
        newConnection(sockfd, addr);
    });      
}

void Server::start() {
    trace("start running server");
    acceptor_->listen();
    loop_->loop();
}

void Server::removeConnection(const TcpConnectionPtr& conn) {
    connections_.erase(conn->connId());
}

void Server::newConnection(int sockfd, struct sockaddr_in* addr) {
    char buf[16];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    trace("server accept a new connection %s", buf);

    TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop_.get(), sockfd, nextConnId_);
    connections_[nextConnId_] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(
            [this](const TcpConnectionPtr& conn) {
                this->removeConnection(conn);
            });
    conn->connectionEstablished();
    
    ++nextConnId_;
    (void)addr;
}

}  // end of namespace stat

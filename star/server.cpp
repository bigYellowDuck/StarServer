#include "server.h" 
#include "logging.h"
#include "eventloop.h"
#include "acceptor.h"
#include "signal.h"

#include <string.h>

namespace star {

Server::Server(int port)
    : running_(false),
      loop_(new EventLoop),
      acceptor_(new Acceptor(loop_.get(), port)),
      signal_(new Signal(loop_.get())),
      nextConnId_(1) {
    acceptor_->setNewConnectionCallback(
        [this](int sockfd, struct sockaddr_in* addr) {
        newConnection(sockfd, addr);
    });      
}

void Server::start() {
    assert(!running_);
    running_ = true;
    trace("start running server");
    acceptor_->listen();
    loop_->loop();
}

void Server::exit() {
    assert(running_);
    running_ = false;
    trace("Server::exit");
    loop_->exit();
    multiLoop_->exit();
}

void Server::signal(int signo, SignalCallback&& callback) {
    signal_->signal(signo, std::move(callback));
}

void Server::cancelSignal(int signo) {
    signal_->cancel(signo);
}

void Server::removeConnection(const TcpConnectionPtr& conn) {
    connections_.erase(conn->connId());
}

void Server::setThreadNum(int numThreads) {
    multiLoop_ = std::move(std::unique_ptr<MultiEventLoop>(new MultiEventLoop(loop_.get())));
    multiLoop_->setThreadNum(numThreads);
    multiLoop_->start();
}

void Server::newConnection(int sockfd, struct sockaddr_in* addr) {
    char buf[16];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    trace("server accept a new connection %s", buf);
    
    EventLoop* ioLoop = multiLoop_ ? multiLoop_->getNextLoop() : loop_.get();
    
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop, sockfd, nextConnId_);
    connections_[nextConnId_] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(
            [this](const TcpConnectionPtr& conn) {
                this->removeConnection(conn);
            });
    loop_->runInLoopThread([conn] {
            conn->connectionEstablished();
        });

    ++nextConnId_;
    (void)addr;
}

}  // end of namespace stat

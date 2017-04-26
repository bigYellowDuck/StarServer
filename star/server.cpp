#include "server.h" 
#include "logging.h"

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/signalfd.h>

namespace star {

Server::Server(int port)
    : running_(false),
      loop_(new EventLoop),
      acceptor_(new Acceptor(loop_.get(), port)),
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

void Server::signal(int signo, const SignalCallback& callback) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signo);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    int sfd = ::signalfd(-1, &mask, SFD_NONBLOCK|SFD_CLOEXEC);
    fatalif(sfd<0, "Server::signal failed %d %s", errno, strerror(errno));

    std::unique_ptr<Channel> signalChannel(new Channel(loop_.get(), sfd));
    
    signalChannel->enableRead(true);
    signalChannel->setReadCallBack(
            [sfd, callback] {
                struct signalfd_siginfo si;
                ::read(sfd, &si, sizeof(si));
                callback();
        });
    signalChannel->addToPoller();
    signalChannels_.push_back(std::move(signalChannel));
    trace("Server::signal finish");
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

    EventLoop* ioLoop = multiLoop_->getNextLoop();
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

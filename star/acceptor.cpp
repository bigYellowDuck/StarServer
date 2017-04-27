#include "acceptor.h"
#include "logging.h"

#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>

namespace star {

Socket::Socket(int sockfd)
    : fd_(sockfd) {
    fatalif(fd_<0, "socket fd error");    
}

Socket::~Socket() {
    if (fd_ > 0) {
        close(fd_);
    }
}

void Socket::bindOrDie(const struct sockaddr_in* addr) {
    int r = ::bind(fd_, (const struct sockaddr*)addr, sizeof(sockaddr_in));
    fatalif(r==-1, "bind error %d %s", errno, strerror(errno));
}

void Socket::listenOrDie() {
    int r = ::listen(fd_, 128);
    fatalif(r==-1, "listen error %d %s", errno, strerror(errno));
}

int Socket::connect(const struct sockaddr_in* addr) {
    return ::connect(fd_, (const struct sockaddr*)addr, sizeof(sockaddr_in));
}

void Socket::shutdownWrite() {
    int r = ::shutdown(fd_, SHUT_WR);
    fatalif(r<0, "Socket::shutdownWrite failed %d %s", errno, strerror(errno));
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    int r = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fatalif(r<0, "setsockopt reuseaddr error %d %s", errno, strerror(errno));
}

void Socket::setNoDelay(bool on) {
    int optval = on ? 1 : 0;
    int r = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    fatalif(r<0, "setsockopt no delay error %d %s", errno, strerror(errno));
}

Socket Socket::createTCP() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    fatalif(fd<0, "socket error %d %s", errno, strerror(errno));
    return Socket(fd);
}


Acceptor::Acceptor(EventLoop* loop, int port) 
    : loop_(loop),
      socket_(Socket::createTCP()),
      port_(port),
      channel_(new Channel(loop, socket_.fd())) { 
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port_);
   
    socket_.setReuseAddr(true);
    socket_.bindOrDie(&addr_);
    channel_->enableReadWrite(false,false);
    channel_->addToPoller();
}


void Acceptor::listen() {
    socket_.listenOrDie();
    
    socket_.setNoDelay(true);

    channel_->setReadCallBack([this]{handleRead();});    
    channel_->enableRead(true);
    channel_->updateToPoller();

    trace("acceptor listening");
}

void Acceptor::handleRead() {
    trace("acceptor handleRead");
    
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int connfd = ::accept4(channel_->fd(), (struct sockaddr*)&addr, &addrlen, SOCK_NONBLOCK|SOCK_CLOEXEC);
    
    if (connfd >= 0) {
        if (newConnectCallback_)
            newConnectCallback_(connfd, &addr_);
    } else {
        trace("accept error %d %s", errno, strerror(errno));
        close(connfd);
    }

}

}  // end of namespace star 


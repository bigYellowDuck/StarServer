#include "tcpconn.h"
#include "logging.h"
#include "acceptor.h"

#include <unistd.h>
#include <string.h>

namespace star {

TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             int connId) 
    : loop_(loop),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop_, sockfd)),
      connId_(connId),
      state_(kConnecting) {
    
}

TcpConnection::~TcpConnection() {
    if (state_ == kDisconnected)
        trace("TcpConnection %d destoryed", connId_);
}

void TcpConnection::connectionEstablished() {
    trace("connection %d established", connId_);
    channel_->setReadCallBack([this]{handleRead();});
    channel_->enableRead(true);
    channel_->addToPoller();
    setState(KConnected);

    trace("call connectionCallback");
    if (connectionCallback_)
        connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead() {
    trace("connection %d handleRead", connId_);
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
    
    if (n>0) {
        if (messageCallback_) { 
            trace("call messageCallback");
            messageCallback_(shared_from_this(), buf, n);
        }
    } else if (n==0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleClose() {
    channel_->enableReadWrite(false, false);
    channel_->updateToPoller();
    state_ = kDisconnected;

    trace("connection %d disconnected", connId_);
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
    info("TcpConnectionPtr::handleError %d %s", errno, strerror(errno));
}



}  // end of namespace star

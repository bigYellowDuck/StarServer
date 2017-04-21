#include "tcpconn.h"
#include "logging.h"
#include "acceptor.h"

#include <unistd.h>

namespace star {

TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             int connId) 
    : loop_(loop),
      socket_(new Socket(sockfd)),
      channel_(loop_, sockfd),
      connId_(connId),
      state_(kConnecting) {
    
}

void TcpConnection::connectionEstablished() {
    trace("connection %d established", connId_);
    channel_.setReadCallBack([this]{handleRead();});
    channel_.enableRead(true);
    channel_.addToPoller();
    setState(KConnected);

    trace("call connectionCallback");
    if (connectionCallback_)
        connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead() {
    trace("connection %d handleRead", connId_);
    char buf[65536];
    ssize_t n = ::read(channel_.fd(), buf, sizeof(buf));

    trace("call messageCallback_");
    if (messageCallback_) 
        messageCallback_(shared_from_this(), buf, n);
}


}  // end of namespace star

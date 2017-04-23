#include "tcpconn.h"
#include "logging.h"
#include "acceptor.h"
#include "channel.h"

#include <unistd.h>
#include <string.h>
#include <sys/uio.h>

namespace star {

int ReadBuffer::readFd(int fd) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = avail();
    vec[0].iov_base = const_cast<char*>(buf_.data()+endOfData_);
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    ssize_t n = readv(fd, vec, 2);
    if (n<0) {
        error("readBuffer readv error %d %s", errno, strerror(errno));
    } else if (static_cast<size_t>(n) <= writable) {
        addIndex(n);
    } else {
        setIndex(bufSize());
        append(extrabuf, n - writable);
    }

    return n;
}

TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             int connId) 
    : loop_(loop),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop_, sockfd)),
      connId_(connId),
      state_(kConnecting) {
    socket_->setNoDelay(true);    
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

   ssize_t n = readBuffer_.readFd(channel_->fd());

    if (n>0) {
        if (messageCallback_) { 
            trace("call messageCallback");
            messageCallback_(shared_from_this(), &readBuffer_);
        }
    } else if (n==0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleWrite() {
    trace("connection %d handleWrite", connId_);
    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(),
                            writeBuffer_.dataForSend(),
                            writeBuffer_.readyBytes());
        if (n>0) {
            writeBuffer_.retrieve(n);
            if (writeBuffer_.empty()) {
                channel_->enableWrite(false);
                channel_->updateToPoller();
            } else {
                trace("connection %d is going to send more data", connId_);
            }
        } else {
            error("connection %d handleWrite error %d", connId_, errno);
        }
    } else {
        trace("connection %d is down, can't write", connId_);
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

void TcpConnection::send(const std::string& message) {
    ssize_t nwrote = 0;
    if (!channel_->isWriting() && writeBuffer_.empty()) {
        nwrote = ::write(channel_->fd(), message.data(), message.size());
        if (nwrote >= 0) {
            if (static_cast<size_t>(nwrote) < message.size())
                trace("connect %d is going to send more data", connId_);
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
                error("connection %d send error %d", connId_, errno);
        }
    }

    if (static_cast<size_t>(nwrote) < message.size()) {
        writeBuffer_.append(message.data(), message.size());
        if (!channel_->isWriting()) {
            channel_->setWriteCallBack([this]{handleWrite();});
            channel_->enableWrite(true);
            channel_->updateToPoller();
        }
    }
}

}  // end of namespace star

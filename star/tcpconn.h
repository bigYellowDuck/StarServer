#ifndef STARSERVER_TCPCONN_H
#define STARSERVER_TCPCONN_H


#include <memory>
#include <functional>
#include <string>
#include <algorithm>

#include "util.h"
#include "eventloop.h"

namespace star {

class ReadBuffer;
class Socket;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, ReadBuffer*)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

class Buffer : public Noncopyable {
  public:
    Buffer():endOfData_(0) { buf_.resize(65536); }
    void append(const char* data, size_t len) {
        if (avail() > len) {
           copy(data, data+len, buf_.begin()+endOfData_); 
           endOfData_ += len;
        } else {      // FIXME:find a better way to deal with memory increase problem
            std::string exbuf;
            exbuf.resize(buf_.size() * 2);
            buf_.swap(exbuf);
            copy(exbuf.begin(), exbuf.begin()+endOfData_, buf_.begin());
        }
    }
    
    size_t len() const noexcept { return endOfData_; }
    size_t bufSize() const noexcept { return buf_.size(); }
    size_t avail() const noexcept { return buf_.size() - endOfData_; }
  protected:
    void addIndex(size_t n) { endOfData_ += n; }
    void setIndex(size_t n) { endOfData_ = n; }

    std::string buf_;
    size_t endOfData_;
};

class ReadBuffer : public Buffer {
  public:
    ReadBuffer()
        : Buffer() {
    }
    
    int readFd(int fd);
    const std::string retrieveAsString() noexcept {
        std::string str(buf_.begin(), buf_.begin()+endOfData_);
        endOfData_ = 0;
        return str;
    }
};

class WriteBuffer : public Buffer {
  public:
    WriteBuffer()
        : Buffer(),
          indexForSend_(0) {
    }

    void retrieve(size_t n) { 
        indexForSend_ += n; 
        if (indexForSend_ == endOfData_) {
            indexForSend_ = 0;
            setIndex(0);
        }
    } 
    const char* dataForSend() const noexcept { return buf_.data()+indexForSend_; }
    
    size_t readyBytes() const noexcept { return endOfData_ - indexForSend_; }
    bool empty() const noexcept { return endOfData_ == indexForSend_; }

  private:
    size_t indexForSend_;
};

class TcpConnection : public Noncopyable, 
                      public std::enable_shared_from_this<TcpConnection> {
  public:
    TcpConnection(EventLoop* loop, int sockfd, int connId); 
    ~TcpConnection();

    void setConnectionCallback(const ConnectionCallback& callback) {
        connectionCallback_ = callback;
    }

    void setConnectionCallback(ConnectionCallback&& callback) {
        connectionCallback_ = std::move(callback);
    }

    void setMessageCallback(const MessageCallback& callback) {
        messageCallback_ = callback;
    }

    void setMessageCallback(MessageCallback&& callback) {
        messageCallback_ = std::move(callback);
    }

    void setCloseCallback(const CloseCallback& callback) {
        closeCallback_ = callback;
    }
    
    void setCloseCallback(CloseCallback&& callback) {
        closeCallback_ = std::move(callback);
    }

    void connectionEstablished();
    
    int connId() const noexcept { return connId_; }
    bool connected() const noexcept { return state_ == KConnected; }

    void send(const std::string& message);
  private:
    enum State {kConnecting, KConnected, kDisconnected};
    void setState(State s) { state_ = s;}

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop* loop_;  
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    int connId_;
    State state_;
    ReadBuffer readBuffer_;
    WriteBuffer writeBuffer_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};


}  // end of namespace star

#endif // STARSERVER_TCPCONN_H

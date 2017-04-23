#ifndef STARSERVER_TCPCONN_H
#define STARSERVER_TCPCONN_H

#include <memory>
#include <functional>

#include "util.h"
#include "eventloop.h"
#include "channel.h"

namespace star {

class Socket;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, const char*, size_t)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

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

  private:
    enum State {kConnecting, KConnected, kDisconnected};
    void setState(State s) { state_ = s;}

    void handleRead();
    void handleClose();
    void handleError();

    EventLoop* loop_;  
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    int connId_;
    State state_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};



}  // end of namespace star

#endif // STARSERVER_TCPCONN_H

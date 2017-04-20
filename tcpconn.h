#ifndef STARSERVER_TCPCONN_H
#define STARSERVER_TCPCONN_H

#include <memory>

#include "util.h"
#include "eventloop.h"

namespace star {

class Socket;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, const char*, size_t)>;

class TcpConnection : public Noncopyable, 
                      public std::enable_shared_from_this<TcpConnectionPtr> {
  public:
    TcpConnection(EventLoop* loop, int sockfd, int connId); 

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

  private:
    EventLoop* loop_;
    std::unique_ptr<Socket> socket_;
    int connId_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};



}  // end of namespace star

#endif // STARSERVER_TCPCONN_H

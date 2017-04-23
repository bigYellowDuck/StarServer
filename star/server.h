#ifndef STARSERVER_SERVER_H
#define STARSERVER_SERVER_H

#include "util.h"
#include "eventloop.h"
#include "acceptor.h"
#include "tcpconn.h"

#include <memory>
#include <map>

namespace star {

class Server : public Noncopyable {
    using ConnectionMap = std::map<int, TcpConnectionPtr>;
  public:
    Server() = delete;
    explicit Server(int port);
    
    void start();

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

    void removeConnection(const TcpConnectionPtr& conn);
  private:
    void newConnection(int sockfd, struct sockaddr_in* addr);
    
    std::unique_ptr<EventLoop> loop_;
    std::unique_ptr<Acceptor> acceptor_;
    int nextConnId_;
    ConnectionMap connections_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};

}  // end of namespace star

#endif // STARSERVER_SERVER_H

#ifndef STARSERVER_SERVER_H
#define STARSERVER_SERVER_H

#include "util.h"
#include "eventloop.h"
#include "acceptor.h"
#include "tcpconn.h"

namespace star {

class Server : public Noncopyable {
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

  private:
    void newConnection(int sockfd, struct sockaddr_in* addr);
    
    EventLoop loop_;
    Acceptor acceptor_;

    int nextConnId_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};

}  // end of namespace star

#endif // STARSERVER_SERVER_H

#ifndef STARSERVER_SERVER_H
#define STARSERVER_SERVER_H

#include "util.h"
#include "tcpconn.h"

#include <netinet/in.h>

#include <memory>
#include <map>

namespace star {

class EventLoop;
class MultiEventLoop;
class Acceptor;
class Signal; 

class Server : public Noncopyable {
    using ConnectionMap = std::map<int, TcpConnectionPtr>;
    using SignalCallback = std::function<void()>;
  public:
    Server() = delete;
    explicit Server(int port, const std::string& driver=" ");
    
    void start();

    void exit();

    void signal(int signo, const SignalCallback& callback) {
        signal(signo, SignalCallback(callback));
    }

    void signal(int signo, SignalCallback&& callback);
    void cancelSignal(int signo);

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

    void setThreadNum(int numThreads);
  private:
    void newConnection(int sockfd, struct sockaddr_in* addr);
    
    bool running_;
    std::unique_ptr<EventLoop> loop_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<MultiEventLoop> multiLoop_;
    std::unique_ptr<Signal> signal_;
    int nextConnId_;
    ConnectionMap connections_;
    
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};

}  // end of namespace star

#endif // STARSERVER_SERVER_H

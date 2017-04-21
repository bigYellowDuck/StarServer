#ifndef STARSERVER_ACCEPTOR_H
#define STARSERVER_ACCEPTOR_H

#include "logging.h"
#include "eventloop.h"
#include "channel.h"

#include <netinet/in.h>

namespace star {

class Socket : public Noncopyable {
  public:
    explicit Socket(int sockfd);
    ~Socket();

    Socket(Socket&& sock) : fd_(sock.fd_) {
        sock.fd_ = -1;
    }

    Socket& operator=(Socket&& sock) {
        std::swap(fd_, sock.fd_);
        return *this;
    }

    int fd() const noexcept { return fd_; }

    void bindOrDie(const struct sockaddr_in* addr);
    void listenOrDie();

    int connect(const struct sockaddr_in* addr);

    void setReuseAddr(bool on);
    void setNoDelay(bool on);

    static Socket createTCP();

  private:
    int fd_;
};


class Acceptor : public Noncopyable {
    using NewConnectionCallback = std::function<void(int, struct sockaddr_in*)>;
  public:
    Acceptor(EventLoop* loop, int port);
    ~Acceptor();

    void listen();

    void handleRead();
    
    void setNewConnectionCallback(const NewConnectionCallback& callback) {
        newConnectCallback_ = callback;
    }

    void setNewConnectionCallback(NewConnectionCallback&& callback) {
        newConnectCallback_ = std::move(callback);
    }

    EventLoop* eventLoop() const noexcept { return loop_; }
    int port() const noexcept { return port_; }
  private:
    EventLoop* loop_;
    Socket socket_;
    int port_;
    Channel channel_;
    struct sockaddr_in addr_;
    NewConnectionCallback newConnectCallback_;
};

}  // end of namespace star

#endif // STARSERVER_ACCEPTOR_H


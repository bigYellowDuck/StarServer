#ifndef STARSERVER_SERVER_H
#define STARSERVER_SERVER_H

#include "util.h"
#include "eventloop.h"
#include "acceptor.h"

namespace star {

class Server : public Noncopyable {
  public:
    Server(EventLoop* loop, int port);
    void start();
  private:
    void newConnection(int sockfd, struct sockaddr_in* addr);
    
    EventLoop* loop_;    
    Acceptor acceptor_;
};

}  // end of namespace star

#endif // STARSERVER_SERVER_H

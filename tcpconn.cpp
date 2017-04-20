#include "tcpconn.h"
#include "logging.h"
#include "acceptor.h"

namespace star {

TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             int connId) 
    : loop_(loop),
      socket_(new Socket(sockfd)),
      connId_(connId) {
    
}

}  // end of namespace star

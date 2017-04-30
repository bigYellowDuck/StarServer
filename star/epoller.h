#ifndef STARSERVER_EPOLLER_H
#define STARSERVER_EPOLLER_H

#include "abstractPoller.h"

#include <sys/epoll.h>

#include <set>

namespace star {

const int kMaxEvents = 2000;
//const int kReadEvent = EPOLLIN | EPOLLPRI;
//const int kWriteEvent = EPOLLOUT;

class Epoller : public AbstractPoller {
  public:
    Epoller();
    ~Epoller();
    void addChannel(Channel* ch) override;
    void updateChannel(Channel* ch) override;
    void removeChannel(Channel* ch) override;
    void loop_once(int waitMs) override;

    int fd() const noexcept { return fd_; }
  private: 
    int fd_;
    int activeFds_;
    std::set<Channel*> liveChannels_;
    struct epoll_event activeEvents_[kMaxEvents];

    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kErrorEvent;
};

}  // end of namespace star

#endif // STARSERVER_EPOLLER_H


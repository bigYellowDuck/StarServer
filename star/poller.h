#ifndef STARSERVER_POLLER_H
#define STARSERVER_POLLER_H

#include "abstractPoller.h"

#include <poll.h>

#include <map>
#include <vector>

namespace star {

// const int kReadEvent = POLLIN|POLLPRI;
// const int kWriteEvent = POLLOUT;
// const int kErrorEvent = POLLERR;


class Poller : public AbstractPoller {
  public:
    Poller();
    ~Poller();

    void addChannel(Channel* ch) override;
    void updateChannel(Channel* ch) override;
    void removeChannel(Channel* ch) override;
    void loop_once(int waitMs) override;
    
  private:
    std::map<int, Channel*> liveChannels_;
    std::vector<struct pollfd> pfdList_;

    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kErrorEvent;
};

}  // end of namespace star

#endif // STARSERVER_POLLER_H


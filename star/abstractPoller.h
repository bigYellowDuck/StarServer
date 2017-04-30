#ifndef STARSERVER_ABSTRACTPOLLER_H
#define STARSERVER_ABSTRACTPOLLER_H

#include "util.h"

namespace star {

class Channel;

class AbstractPoller : public Noncopyable {
  public:
    AbstractPoller() {}
    virtual ~AbstractPoller() {}
    virtual void addChannel(Channel* ch) = 0;
    virtual void removeChannel(Channel* ch) = 0;
    virtual void updateChannel(Channel* ch) = 0;
    virtual void loop_once(int waitMs) = 0;
}; 

}  // end of namespace star

#endif // STARSERVER_ABSTRACTPOLLER_H


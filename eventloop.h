#ifndef STARSERVER_EVENTLOOP_H
#define STARSERVER_EVENTLOOP_H

#include "util.h"
#include "epoller.h"

#include <memory>
#include <atomic>

namespace star {

class EventLoop : public Noncopyable {
  public: 
    EventLoop()
        : quit_(false),
          poller_(new Epoller) {
    
    }

    ~EventLoop() {}

    void loop() {

        quit_ = false;

        while (!quit_) {
            poller_->loop_once(-1);
        }    
    }
   
    void addChannel(Channel* ch) { poller_->addChannel(ch); }
    void updateChannel(Channel* ch) { poller_->updateChannel(ch); }
    void removeChannel(Channel* ch) { poller_->removeChannel(ch); }

  private:
    std::atomic<bool> quit_;
    std::unique_ptr<AbstractPoller> poller_;
};

}  // end of namespace star

#endif // STARSERVER_EVENT_H


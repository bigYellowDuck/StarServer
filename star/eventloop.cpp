#include "eventloop.h"
#include "logging.h"

#include <stdio.h>
#include <signal.h>

namespace star {

void EventLoop::loop() {
    quit_ = false;

    while (!quit_) {
        poller_->loop_once(-1);
    }
}

class IgnoreSigPipe {
  public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe initObj;

}  // end of namespace star

#include "eventloop.h"

namespace star {

void EventLoop::loop() {
    quit_ = false;

    while (!quit_) {
        poller_->loop_once(-1);
    }
}


}  // end of namespace star

#include "eventloop.h"
#include "logging.h"
#include "channel.h"
#include "epoller.h"
#include "poller.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

namespace star {

class IgnoreSigPipe {
  public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe initObj;

EventLoop::EventLoop(Driver driver)
    : driver_(driver),
      tid_(std::this_thread::get_id()),
      quit_(false),
      poller_(),
      callingPendingTasks_(false),
      wakeupFd_(util::createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)) {
    if (driver == dEpoller) {
        poller_.reset(new Epoller);
    } else {
        poller_.reset(new Poller);
    }

    wakeupChannel_->enableRead(true);
    wakeupChannel_->setReadCallBack(
            [this]{
                uint64_t n;    
                int r = ::read(wakeupFd_, &n, sizeof(uint64_t));  // clear counter;
            });
    wakeupChannel_->addToPoller();
}

EventLoop::~EventLoop() {

}

void EventLoop::loop() {
    quit_ = false;
    trace("EventLoop::loop");
    while (!quit_) {
        poller_->loop_once(-1);
        doPendingTasks();
    }
}

void EventLoop::exit() {
   assert(!quit_);
   quit_ = true;
   wakeup();
   trace("EventLoop::exit");
}

bool EventLoop::isInLoopThread() const noexcept {
    return tid_ == std::this_thread::get_id();
}

void EventLoop::runInLoopThread(Task&& task) {
    if (isInLoopThread())
        task();
    else
        queueInLoop(std::move(task));
}

void EventLoop::queueInLoop(Task&& task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingTasks_.push_back(std::move(task));
    }
    if (!isInLoopThread() || callingPendingTasks_) 
        wakeup();
}

void EventLoop::wakeup() {
    uint64_t n = 1;
    int r = ::write(wakeupFd_, &n, sizeof(uint64_t));  // add counter;
    fatalif(r<0, "wakeup write failed %d %s", errno, strerror(errno));
}

void EventLoop::doPendingTasks() {
    std::vector<Task> tasks;
    callingPendingTasks_ = true;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks.swap(pendingTasks_);
    }

    for (size_t i=0; i<tasks.size(); ++i) {
        tasks[i](); 
    }

    callingPendingTasks_ = false;
}

MultiEventLoop::MultiEventLoop(EventLoop* baseLoop) 
    : baseLoop_(baseLoop),
      started_(false),
      numThreads_(0),
      next_(0) {

}

MultiEventLoop::~MultiEventLoop() {
    
}


void MultiEventLoop::start() {
    assert(!started_);
    started_ = true;
    trace("MultiEventLoop::start -  %d threads run", numThreads_);
    
    std::unique_ptr<ThreadPool> threadPool(new ThreadPool(numThreads_, false));
    threadPool_.swap(threadPool);
    threadPool_->start();
    
    loops_.resize(numThreads_);
    for (int i=0; i<numThreads_; ++i) {
        threadPool_->addTask(
            [this, i] {
                // loops_[i] = std::move(std::unique_ptr<EventLoop>(new EventLoop));
                loops_[i].reset(new EventLoop(baseLoop_->driver()));
                loops_[i]->loop();
        });
    }
}

void MultiEventLoop::exit() {
    assert(started_);
    started_ = false;
    trace("MultiEventLoop::exiting");
    for (int i=0; i<numThreads_; ++i) {
        loops_[i]->exit();
    }
    threadPool_->exit();
    threadPool_->join();
    trace("MultiEventLoop::exited");
}

// Only can be called in baseLoop
EventLoop* MultiEventLoop::getNextLoop() {
    EventLoop* loop = baseLoop_;

    if (!loops_.empty()) {
        loop = loops_[next_].get();
        ++next_;

        if (static_cast<size_t>(next_) >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

}  // end of namespace star

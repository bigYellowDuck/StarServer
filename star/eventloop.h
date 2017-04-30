#ifndef STARSERVER_EVENTLOOP_H
#define STARSERVER_EVENTLOOP_H

#include "util.h"
#include "abstractPoller.h"
#include "threads.h"

#include <memory>
#include <atomic>
#include <vector>

namespace star {

class EventLoop : public Noncopyable {
    using Task = std::function<void()>;
  public:
    enum Driver {dEpoller, dPoller};
    explicit EventLoop(Driver driver=dEpoller);
    ~EventLoop();

    void loop();
    void exit();

    bool isInLoopThread() const noexcept;
    void runInLoopThread(const Task& task) { runInLoopThread(Task(task)); }
    void runInLoopThread(Task&& task);
    
    void addChannel(Channel* ch) { poller_->addChannel(ch); }
    void updateChannel(Channel* ch) { poller_->updateChannel(ch); }
    void removeChannel(Channel* ch) { poller_->removeChannel(ch); }

    Driver driver() const noexcept { return driver_; } 
  private:
    void queueInLoop(Task&& task);
    void wakeup();
    void doPendingTasks();
    
    Driver driver_;
    std::thread::id tid_;
    std::atomic<bool> quit_;
    std::unique_ptr<AbstractPoller> poller_;
    bool callingPendingTasks_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::mutex mutex_;
    std::vector<Task> pendingTasks_;
};

class MultiEventLoop : public Noncopyable {
  public:
    MultiEventLoop(EventLoop* baseLoop);
    ~MultiEventLoop();
    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    int threads() const noexcept { return numThreads_; }
    void start();
    void exit();
    EventLoop* getNextLoop();
  private:
    EventLoop* baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoop>> loops_;
    std::unique_ptr<ThreadPool> threadPool_;
};

}  // end of namespace star

#endif // STARSERVER_EVENT_H


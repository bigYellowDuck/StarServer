#ifndef STARSERVER_SIGNAL_H
#define STARSERVER_SIGNAL_H

#include "util.h"

#include <memory>
#include <map>

namespace star {

class Channel;
class EventLoop;

class Signal : public Noncopyable {
    using SignalCallback = std::function<void()>;
    using SignalMap = std::map<int, SignalCallback>;
  public:
    explicit Signal(EventLoop* loop);
    ~Signal();

    void signal(int signo, const SignalCallback& callback);
    void signal(int signo, SignalCallback&& callback);

    void cancel(int signo);

  private:
    void handleRead();
    
    EventLoop* loop_;
    sigset_t mask_;
    int sfd_;
    std::unique_ptr<Channel> channel_;
    SignalMap signalMap_;
};

}  // end of namespace star

#endif // STARSERVER_SIGNAL_H

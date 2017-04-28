#include "signal.h"
#include "logging.h"
#include "channel.h"

#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <sys/signalfd.h>

namespace star {

Signal::Signal(EventLoop *loop)
    : loop_(loop),
      sfd_(-1) {
    sigemptyset(&mask_);
    sfd_ = ::signalfd(-1, &mask_, SFD_NONBLOCK|SFD_CLOEXEC); 
    fatalif(sfd_<0, "Signal constructor error %d %s", errno, strerror(errno));
    channel_ = std::move(std::unique_ptr<Channel>(new Channel(loop_, sfd_)));
    channel_->enableRead(true);
    channel_->setReadCallBack([this]{ handleRead(); });
    channel_->addToPoller();
}

Signal::~Signal() {

}

void Signal::signal(int signo, const SignalCallback& callback) {
    signal(signo, SignalCallback(callback));
}

void Signal::signal(int signo, SignalCallback&& callback) {
    sigaddset(&mask_, signo);
    sigprocmask(SIG_BLOCK, &mask_, NULL);
    signalMap_[signo] = std::move(callback);
    int r = ::signalfd(sfd_, &mask_, SFD_NONBLOCK|SFD_CLOEXEC);
    fatalif(r<0, "Signal::signal signalfd error %d %s", errno, strerror(errno));
}

void Signal::cancel(int signo) {
    if (sigismember(&mask_, signo)) {
        sigdelset(&mask_, signo);
        sigprocmask(SIG_SETMASK, &mask_, NULL);
        signalMap_.erase(signo);
    } 
}

void Signal::handleRead() {
    struct signalfd_siginfo sigInfo;
    int r = ::read(sfd_, &sigInfo, sizeof(sigInfo));
    assert(r==sizeof(sigInfo));
    
    auto iter = signalMap_.find(sigInfo.ssi_signo);
    if (iter != signalMap_.end()) {
        (iter->second)();
    }
}

}  // end of namespace star

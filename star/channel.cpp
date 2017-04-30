#include "channel.h"
#include "logging.h"

#include <atomic>

#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>

namespace star {

static_assert(POLLIN==EPOLLIN, "POLLIN != EPOLLIN");
static_assert(POLLOUT==EPOLLOUT, "POLLOUT != EPOLLOUT");
static_assert(POLLPRI==EPOLLPRI, "POLLPRI != EPOLLPRI");
static_assert(POLLERR==EPOLLERR, "POLLERR != EPOLLERR");



const int kReadEvent = POLLIN|POLLPRI;
const int kWriteEvent = POLLOUT;
const int kErrorEvent = POLLERR;

Channel::Channel(EventLoop* loop, int fd)
    : fd_(fd),
      events_(0),
      loop_(loop),
      index_(-1) {
    static std::atomic<int64_t> id(0);
    id_ = ++id;
}

Channel::~Channel() {
    close();
}

void Channel::enableRead(bool enable) {
    if (enable) {
        events_ |= kReadEvent;
    } else {
        events_ &= ~kReadEvent;
    }
}

void Channel::enableWrite(bool enable) {
    if (enable) {
        events_ |= kWriteEvent;
    } else {
        events_ &= ~kWriteEvent;
    }
}

void Channel::enableReadWrite(bool readable, bool writable) {
    if (readable) {
        events_ |= kReadEvent;
    } else {
        events_ &= ~kReadEvent;
    }

    if (writable) {
        events_ |= kWriteEvent;
    } else {
        events_ &= ~kWriteEvent;
    }
}

bool Channel::readEnabled() {
    return events_ & kReadEvent;
}

bool Channel::writeEnabled() {
    return events_ & kWriteEvent;
}

bool Channel::isWriting() const noexcept { 
    return events_ & kWriteEvent; 
}

void Channel::handleRead() {
    if (readCallBack_)
        readCallBack_();
}

void Channel::handleWrite() {
    if (writeCallBack_)
        writeCallBack_();
}

void Channel::handleError() {
    if (errorCallBack_)
        errorCallBack_();
}

void Channel::close() {
    if (fd_ >= 0) {
        trace("close channel %lld fd %d", id_, fd_);
        loop_->removeChannel(this);
        ::close(fd_);
        fd_ = -1;
    }
}

}  // end of namespace star


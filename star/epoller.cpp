#include "epoller.h"
#include "channel.h"
#include "logging.h"

#include <unistd.h>
#include <string.h>

namespace star {

const int Epoller::kReadEvent = EPOLLIN|EPOLLPRI;
const int Epoller::kWriteEvent = EPOLLOUT;
const int Epoller::kErrorEvent = EPOLLERR;


Epoller::Epoller() 
    : activeFds_(-1) {
    fd_ = epoll_create1(EPOLL_CLOEXEC);
    fatalif(fd_<0, "epoll create error %d %s", errno, strerror(errno));
    trace("epoller fd %d created", fd_);
}

Epoller::~Epoller() {
    while (liveChannels_.size()) {
        (*liveChannels_.begin())->close();
    } 
    ::close(fd_);
    trace("poller %d destoryed", fd_);
}

void Epoller::addChannel(Channel* ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events();
    ev.data.ptr = ch;
    trace("adding channel %lld fd %d events %d epoll %d",
            (long long )ch->id(), ch->fd(), ch->events(), fd_);
    int r = epoll_ctl(fd_, EPOLL_CTL_ADD, ch->fd(), &ev);
    fatalif(r==-1, "epoll_ctl add failed %d %s", errno, strerror(errno));
    liveChannels_.insert(ch);
}

void Epoller::updateChannel(Channel* ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events();
    ev.data.ptr = ch;
    trace("updating channel %lld fd %d events %d epoll %d", 
            (long long)ch->id(), ch->fd(), ch->events(), fd_);
    int r = epoll_ctl(fd_,EPOLL_CTL_MOD, ch->fd(), &ev);
    fatalif(r==-1, "epoll_ctl modify failed %d %s", errno, strerror(errno));
}

void Epoller::removeChannel(Channel *ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events();
    ev.data.ptr = ch;
    trace("removing channel %lld fd %d events %d epoll %d",
            (long long)ch->id(), ch->fd(), ch->events(), fd_);
    int r = epoll_ctl(fd_, EPOLL_CTL_DEL, ch->fd(), &ev);
    fatalif(r==-1, "epoll_ctl remove failed %d %s", errno, strerror(errno));
    
    liveChannels_.erase(ch);

    for (int i = activeFds_-1; i>=0; --i) {
        if (ch == activeEvents_[i].data.ptr) { 
            activeEvents_[i].data.ptr = NULL;
            break;
        }
    }

}

void Epoller::loop_once(int waitMs) {
    activeFds_ = ::epoll_wait(fd_, activeEvents_, kMaxEvents, waitMs);
    fatalif(activeFds_==-1 && errno!=EINTR, 
            "epoll_wait faile %d %s", errno, strerror(errno));
    while (--activeFds_ >= 0) {
        int i = activeFds_;
        Channel *ch = static_cast<Channel*>(activeEvents_[i].data.ptr);
        int events = activeEvents_[i].events;
        if (ch) {
            if (events & kReadEvent) {
                trace("channel %lld fd %d handle read", (long long)ch->id(), ch->fd());
                ch->handleRead();
            }
            if (events & kWriteEvent) {
                trace("channel %lld fd %d handle write", (long long)ch->id(),ch->fd());
                ch->handleWrite();
            }
            if (events & kErrorEvent) {
                error("channel %lld fd %d handle error", (long long)ch->id(), ch->fd());
                ch->handleError();
            }
        }
    }
}

}  // end of namespace star


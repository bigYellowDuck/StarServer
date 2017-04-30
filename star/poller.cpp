#include "poller.h"
#include "channel.h"
#include "logging.h"

#include <string.h>

#include <algorithm>

namespace star {

const int Poller::kReadEvent = POLLIN|POLLPRI;
const int Poller::kWriteEvent = POLLOUT;
const int Poller::kErrorEvent = POLLERR;

Poller::Poller() {
    trace("poller creating");
}

Poller::~Poller() {
    trace("poller destroying");
}

void Poller::addChannel(Channel *ch) {
    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = ch->fd();
    pfd.events = static_cast<short>(ch->events());
    pfd.revents = 0;
    pfdList_.push_back(std::move(pfd));
    ch->set_index(static_cast<int>(pfdList_.size())-1);
    liveChannels_[pfd.fd] = ch;
    trace("add channel %lld fd %d events %d to pfdlist index %d",
          (long long)ch->id(), ch->fd(), ch->events(), ch->index());
}

void Poller::updateChannel(Channel* ch) {
    int idx = ch->index();
    assert(0<=idx && idx<static_cast<int>(pfdList_.size()));
    struct pollfd& pfd = pfdList_[idx];
    assert(pfd.fd == ch->fd());
    pfd.events = static_cast<short>(ch->events());
    pfd.revents = 0;
    trace("update channel %lld fd %d events %d to pfdlist index %d",
          (long long)ch->id(), ch->fd(), ch->events(), idx);
}

void Poller::removeChannel(Channel* ch) {
    int idx = ch->index();
    assert(0<=idx && idx<static_cast<int>(pfdList_.size()));
    liveChannels_.erase(ch->fd());
    if (static_cast<size_t>(idx) == pfdList_.size()-1) {
        pfdList_.pop_back();
    } else {
        int channelAtEnd = pfdList_.back().fd;
        std::iter_swap(pfdList_.begin()+idx, pfdList_.end()-1);
        liveChannels_[channelAtEnd]->set_index(idx);
        pfdList_.pop_back();
    }
    trace("remove channel %lld fd %d to pfdlist index %d",
          (long long)ch->id(), ch->fd(), idx);
}

void Poller::loop_once(int waitMs) {
    int activeFds = ::poll(pfdList_.data(), pfdList_.size(), waitMs);
    fatalif(activeFds<0, "Poller::loop_once failed %d %s", errno, strerror(errno));
    for (auto pfdIter=pfdList_.cbegin(); 
        pfdIter!=pfdList_.cend() && activeFds>0; ++pfdIter) {
        if (pfdIter->revents > 0) {
            --activeFds;
            auto channelIter = liveChannels_.find(pfdIter->fd);
            assert(channelIter != liveChannels_.end());
            Channel* ch = channelIter->second;
            assert(ch->fd()==pfdIter->fd);
            
            if (pfdIter->revents & kReadEvent) {
                trace("channel %lld fd %d handle read", (long long)ch->id(), ch->fd());
                ch->handleRead();
            }
            if (pfdIter->revents & kWriteEvent) {
                trace("channel %lld fd %d handle write", (long long)ch->id(), ch->fd());
                ch->handleWrite();
            }
            if (pfdIter->revents & kErrorEvent) {
                trace("channel %lld fd %d handle error", (long long)ch->id(), ch->fd());
                ch->handleError();
            }
        } 
    }

}

}  // end of namespace star

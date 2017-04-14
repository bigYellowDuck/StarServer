#ifndef STARSERVER_CHANNEL_H
#define STARSERVER_CHANNEL_H

#include "util.h"
#include "epoller.h"

namespace star{

class Channel : public Noncopyable {
    using EventCallBack = std::function<void()>;
  public:
    Channel(AbstractPoller* poller, int fd);
    ~Channel();

    void setReadCallBack(const EventCallBack& cb)
    { readCallBack_ = cb; }
    
    void setReadCallBack(EventCallBack&& cb)
    { readCallBack_ = std::move(cb); }

    void setWriteCallBack(const EventCallBack& cb)
    { writeCallBack_ = cb; }

    void setWriteCallBack(EventCallBack&& cb)
    { writeCallBack_ = std::move(cb); }

    void setErrorCallBack(const EventCallBack& cb)
    { errorCallBack_ = cb; }

    void setErrorCallBack(EventCallBack&& cb)
    { errorCallBack_ = std::move(cb); }

    int64_t id() const noexcept { return id_; }
    int fd() const noexcept { return fd_; }
    uint32_t events() const noexcept { return events_; }

    void addToPoller() { poller_->addChannel(this); }
    void updateToPoller() { poller_->updateChannel(this); }
    void removeToPoller() { poller_->removeChannel(this); }

    void enableRead(bool enable);
    void enableWrite(bool enable);
    void enableReadWrite(bool readable, bool writable);
    bool readEnabled();
    bool writeEnabled();

    void handleRead();
    void handleWrite();
    void handleError();

    void close();
  private:
    int fd_;
    uint32_t events_;
    AbstractPoller* poller_;
    int64_t id_;

    EventCallBack readCallBack_;
    EventCallBack writeCallBack_;
    EventCallBack errorCallBack_;
};

}  // end of namespace star

#endif // STARSERVER_CHANNEL_H


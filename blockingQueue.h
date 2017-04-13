#ifndef STARSERVER_BLOCKINGQUEUE_H
#define STARSERVER_BLOCKINGQUEUE_H

#include "util.h"

#include <mutex>
#include <condition_variable>
#include <deque>
#include <atomic>

namespace star {

template <typename T>
class BlockingQueue : public Noncopyable {
    using LockGuard = std::lock_guard<std::mutex>;
  public:
    BlockingQueue()
      : mutex_(),
        notEmpty_(),
        queue_(),
        exit_(false) {
    
    }

    ~BlockingQueue() = default;

    bool push(T&& t) {
        if (exit_)
            return false;

        {
            LockGuard lock(mutex_);
            queue_.push_back(std::move(t));
        }
        notEmpty_.notify_one();

        return true;
    }

    bool push(const T& t) {
        if (exit_)
            return false;

        {
            LockGuard lock(mutex_);
            queue_.push_back(t);
        }
        notEmpty_.notify_one();

        return true;
    }

    bool pop(T* v) {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this](){ return exit_ || !this->queue_.empty(); });

        if (queue_.empty())
            return false;
        
        *v = std::move(queue_.front());
        queue_.pop_front();

        return true;
    }

    size_t size() const noexcept {
        LockGuard lock(mutex_);
        return queue_.size();
    }

    void exit() {
        exit_ = true;
        LockGuard locl(mutex_);
        notEmpty_.notify_all();
    }

    bool exited() const noexcept { 
        return exit_;
    }

  private:
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::deque<T> queue_;
    std::atomic<bool> exit_;
};

}  // end of namespace star

#endif // STARSERVER_BLOCINGQUEUE_H


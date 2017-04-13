#ifndef STARSERVER_BLOCKINGQUEUE_H
#define STARSERVER_BLOCKINGQUEUE_H

#include "util.h"

#include <mutex>
#include <condition_variable>
#include <deque>

namespace star {

template <typename T>
class BlockingQueue : public Noncopyable {
    using LockGuard = std::lock_guard<std::mutex>;
  public:
    BlockingQueue()
      : mutex_(),
        notEmpty_(),
        queue_() {
    
    }

    ~BlockingQueue() = default;

    void push(T&& t) {
        {
            LockGuard lock(mutex_);
            queue_.push_back(std::move(t));
        }
        notEmpty_.notify_one();
    }


    void push(const T& t) {
        {
            LockGuard lock(mutex_);
            queue_.push_back(t);
        }
        notEmpty_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this](){ return !this->queue_.empty(); });

        assert(!queue_.empty());
        
        T front(std::move(queue_.front()));
        queue_.pop_front();

        return front;
    }

    size_t size() const noexcept {
        LockGuard lock(mutex_);
        return queue_.size();
    }

  private:
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::deque<T> queue_;
};

}  // end of namespace star

#endif // STARSERVER_BLOCINGQUEUE_H


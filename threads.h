#ifndef STARSERVER_THREADS_H
#define STARSERVER_THREADS_H

#include "blockingQueue.h"

#include <thread>
#include <vector>

namespace star {
    
class ThreadPool : public Noncopyable {
    using Task = std::function<void()>;
  public:
    ThreadPool(size_t threads, bool start=true);
    ~ThreadPool();

    void start();
    void join();
    ThreadPool& exit() { tasks_.exit(); running_ = false; return *this; }

    void addTask(Task&& task);
    void addTask(const Task& task) { return addTask(Task(task)); }
    size_t taskSize() const noexcept { return tasks_.size(); }

  private:
    BlockingQueue<Task> tasks_;
    std::vector<std::thread> threads_;
    std::atomic<bool> running_;
};

}  // end of namespace star

#endif // STARSERVER_THREADS_H


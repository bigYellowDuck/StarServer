#include "threads.h"
#include "logging.h"
namespace star {

ThreadPool::ThreadPool(size_t threads, bool start)
    : tasks_(),
      threads_(threads),
      running_(false) {
    if (start) {
        this->start();
    }
}

ThreadPool::~ThreadPool() {
    assert(tasks_.exited() && !running_);
    if (tasks_.size()) {
        info("%lu tasks not processed when thread pool exited\n", 
            tasks_.size());
    }
}

void ThreadPool::start() {
    if (running_)
        return;

    for (auto& th : threads_) {
        std::thread t(
            [this]() {
                while (!tasks_.exited()) {
                    Task task;
                    if (tasks_.pop(&task))
                        task();
                }
            }
        );   
        th.swap(t);
    }

    running_ = true;
}

void ThreadPool::join() {
    for (auto& t : threads_)
        t.join();
}

void ThreadPool::addTask(Task&& task) {
    tasks_.push(std::move(task));
}

}  // end of namespace star


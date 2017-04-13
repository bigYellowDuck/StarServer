#include "threads.h"

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
    // TODO:safely exit thread pool
}

void ThreadPool::start() {
    if (running_)
        return;

    for (auto& th : threads_) {
        std::thread t(
            [this]() {
                Task task;
                while (1) {
                    task = tasks_.pop();
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


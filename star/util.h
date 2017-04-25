#ifndef STARSERVER_UTIL_H
#define STARSERVER_UTIL_H

#include <assert.h>

#include <functional>

namespace star {

class Noncopyable {
  public:
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable& operator=(const Noncopyable &) = delete;
 
  protected:
    Noncopyable() {}
    ~Noncopyable() {}
};

namespace util {

int createEventfd();

pid_t gettid();

int64_t microSecondSinceEpoch();

std::string moment(bool showMicroSeconds=false);    

} // end of namespace util 

} // end of namespace star

#endif // STARSERVER_UTIL_H


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

} // end of namespace star

#endif // STARSERVER_UTIL_H


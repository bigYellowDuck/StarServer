#include "util.h"
#include "logging.h"

#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>


namespace star {

namespace util {

int createEventfd() {
    int fd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    fatalif(fd<0, "eventfd failed %d %s", errno, strerror(errno));
    return fd;
}

pid_t gettid() {
   return static_cast<pid_t>(::syscall(SYS_gettid)); 
}

}  // end of namespace util

}  // end of namespace star

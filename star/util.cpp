#include "util.h"
#include "logging.h"

#include <sys/eventfd.h>
#include <string.h>

namespace star {

int Util::createEventfd() {
    int fd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    fatalif(fd<0, "eventfd failed %d %s", errno, strerror(errno));
    return fd;

}

}  // end of namespace star

#include "util.h"
#include "logging.h"

#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

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

int64_t microSecondSinceEpoch() {
    timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv.tv_sec*1000*1000 + tv.tv_usec;
}

std::string moment(bool showMicroSeconds) {
    int64_t microSeconds = microSecondSinceEpoch();
    time_t seconds = static_cast<time_t>(microSeconds / 1000 / 1000);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    char buf[32];

    if (showMicroSeconds) {
        int microSeconds_ = static_cast<int>(microSeconds%1000000);
        snprintf(buf, sizeof(buf), "%04d/%02d/%02d-%02d:%02d:%02d.%06d",
                 tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 microSeconds_);
    } else {
        snprintf(buf, sizeof(buf), "%04d/%02d/%02d-%02d:%02d:%02d",
                 tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }

    return buf;
}

}  // end of namespace util

}  // end of namespace star

#include "logging.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

namespace star {


const char* Logger::levelStrs_[LogLevelNum] = {
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
};

Logger::Logger()
    : fd_(-1),
      level_(INFO),
      filename_() {
}


Logger::~Logger() {
    if (fd_ != -1)
        close(fd_);
}

static thread_local pid_t t_tid;
void Logger::logv(int level, const char *file, int line, const char *fmt ...) {
    if (__builtin_expect(t_tid==0, 0))
        t_tid = util::gettid();
    
    if (level < level_) {
        return;
    }

    char buffer[4*1024];
    char *p = buffer;
    char *limit = buffer+sizeof(buffer);
    
    const std::string moment(util::moment(true));
    p += snprintf(p, limit-p,
            "%s %d %s %s:%d -  ",
            moment.data(), static_cast<int>(t_tid), levelStrs_[level], file, line);

    va_list args;
    va_start(args, fmt);
    p += vsnprintf(p, limit-p, fmt, args);
    va_end(args);

    p = p<limit-2 ? p : limit-2;

    while (*--p =='\n');
    *++p = '\n';
    *++p = '\0';

    int fd = fd_ == -1 ? 1 : fd_;
    int err = ::write(fd, buffer, p - buffer);
    if (err != p-buffer) {
        fprintf(stderr, "write log file %s failed. written %d errmsg: %s\n",
                filename_.data(), err, strerror(errno));
    }

    if (level == FATAL) {
        fprintf(stderr, "%s", buffer);
        assert(0);
    }
}

void Logger::setLogLevel(const std::string &level) {
    LogLevel ilevel = INFO;
    for (size_t i=0; i<LogLevelNum; ++i) {
        if (strcasecmp(levelStrs_[i], level.data()) == 0) {
            ilevel = (LogLevel)i;
            break;
        }
    }
    setLogLevel(ilevel);
}

void Logger::setFileName(const std::string &filename) {
    int fd = open(filename.data(), O_APPEND|O_CREAT|O_WRONLY|O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) {
        fprintf(stderr, "open log file %s failed. msg: %s ignored\n",
                filename.data(), strerror(errno));
        return;
    }
    filename_ = filename;
    if (fd_ == -1)
        fd_ = fd;
    else {
        int r = dup2(fd, fd_);
        fatalif(r<0, "dup2 failed");
        close(fd);
    }    
}

Logger& Logger::getLogger() {
    static Logger logger;
    return logger;
}



} // end of namespace star

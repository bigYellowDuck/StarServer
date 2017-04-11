#ifndef STARSERVER_LOGGING_H
#define STARSERVER_LOGGING_H

#include <string>
#include <stdio.h>

#include "util.h"

#define hlog(level, ...) \
    do { \
        if (level >= Logger::getLogger().getLogLevel()) { \
            Logger::getLogger().logv(level, __FILE__, __LINE__, __VA_ARGS__); \
        } \
    } while(0)

#define trace(...) hlog(Logger::TRACE, __VA_ARGS__)
#define debug(...) hlog(Logger::DEBUG, __VA_ARGS__)
#define info(...) hlog(Logger::INFO, __VA_ARGS__)
#define warn(...) hlog(Logger::WARN, __VA_ARGS__)
#define error(...) hlog(Logger::ERROR, __VA_ARGS__)
#define fatal(...) hlog(Logger::FATAL, __VA_ARGS__)
#define fatalif(b, ...) do { if((b)) { hlog(Logger::FATAL, __VA_ARGS__); } } while(0)
#define check(b, ...) do { if((b)) { hlog(Logger::FATAL, __VA_ARGS__); } } while(0)
#define exitif(b, ...) do { if((b)) { hlog(Logger::ERROR, __VA_ARGS__); _exit(1); } } while(0)

#define setloglevel(l) Logger::getLogger().setLogLevel(l)
#define setlogfile(n) Logger::getLogger().setFileName(n)

namespace star {

class Logger : public Noncopyable {
  public:
    Logger();
    ~Logger();

    enum LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, LogLevelNum };

    void logv(int level, const char *file, int line, const char *fmt ...);
    
    void setFileName(const std::string &filename);
    void setLogLevel(const std::string &level);
    void setLogLevel(LogLevel level) { level_ = level; }
    
    LogLevel getLogLevel() const noexcept { return level_; }
    const char* getLogLevelStr() const noexcept { return levelStrs_[level_]; }
    int getFd() const noexcept { return fd_; }

    static Logger& getLogger();

  private:
    static const char *levelStrs_[LogLevelNum];
    int fd_;
    LogLevel level_;
    std::string filename_;
};


}  // end of namespace star

#endif // STARSERVER_LOGGING_H


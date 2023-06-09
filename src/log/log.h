#ifndef  ELEVEN_LOG_H
#define  ELEVEN_LOG_H

#include <cstdint>
#include <string>
#include <memory>
#include <list>
#include <fstream>
#include <sstream>
#include <vector>
namespace eleven{
//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(){}
private:
    //文件名
    const char *m_file = nullptr;
    //行号
    uint32_t m_lineno = 0;
    //线程id
    uint32_t m_threadid = 0;
    //协程id
    uint32_t m_fiberid = 0;
    //日志内容
    std::string m_content;
    //时间戳
    uint64_t m_time = 0;
    //程序启动到现在的毫秒数
    uint32_t m_elapse = 0;
};

//日志级别
class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };
};

//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string &name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void fetal(LogEvent::ptr event);
    
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level level){m_level = level;}

private:
    std::string m_name;                         //日志名称
    LogLevel::Level m_level;                    //日志级别
    std::list<LogAppender::ptr> m_appenders;    //LogAppender集合

};

//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string &pattern);
    //%t    %threadId %m%n
    std::string format(LogEvent::ptr event);
private:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){}
        virtual void format(std::ostream &os, LogEvent::ptr event) = 0;
    }
    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;

};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){};

    virtual void log(LogLevel::Level level, LogEvent::ptr event);

    void setFormatter(LogFormatter::ptr farmatter){m_farmatter = farmatter;}
    LogFormatter::ptr getFormatter() const {return m_farmatter;}

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_farmatter;
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
};

//输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    bool reopen(); //重新打开文件
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}

#endif // ! ELEVEN_LOG_H#define  ELEVEN_LOG_H


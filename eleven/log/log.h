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
    LogEvent(const char *file, int32_t line, uint32_t elapse, uint32_t threadid, uint32_t fiberid,uint64_t time);
    const char *getFile() const {return m_file;}
    int32_t getLineNo() const {return m_lineno;}
    uint32_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const {return m_threadid;}
    uint32_t getFiberId() const {return m_fiberid;}
    int64_t getTime() const {return m_time;}
    std::string getContent() const {return m_ss.str();}
    std::stringstream &getSS() {return m_ss;}
private:
    //文件名
    const char *m_file = nullptr;
    //行号
    int32_t m_lineno = 0;
     //程序启动到现在的毫秒数
    uint32_t m_elapse = 0;
    //线程id
    uint32_t m_threadid = 0;
    //协程id
    uint32_t m_fiberid = 0;
      //时间戳
    int64_t m_time = 0;
    //日志流
    std::stringstream m_ss;
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
    static const char *ToString(LogLevel::Level level);
};
class Logger;
class LogFormatter;
//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    LogAppender() : m_level(LogLevel::DEBUG){}
    virtual ~LogAppender(){};
    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event){};

    void setFormatter(std::shared_ptr<LogFormatter> formatter){m_formatter = formatter;}
    std::shared_ptr<LogFormatter> getFormatter() const {return m_formatter;}

protected:
    LogLevel::Level m_level;    //构造函数默认初始化为最低等级 LogLevel::DEBUG
    std::shared_ptr<LogFormatter> m_formatter; //如果没有显示使用setFormatter函数定义格式，使用Logger默认自带的formatter
};

//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string &pattern);
    //%t    %threadId %m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){}
        virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;

};
//日志器
// 继承std::enable_shared_from_this，在成员函数中返回指向自己的智能指针
class Logger : public std::enable_shared_from_this<Logger>{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string &name = "root") : m_name(name),m_level(LogLevel::DEBUG){
        m_formatter.reset(new LogFormatter(std::string("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T%f:%l%T%m%n")));
    };
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

    const std::string &getName(){return m_name;}

private:
    std::string m_name;                         //日志名称
    LogLevel::Level m_level;                    //日志级别
    std::list<LogAppender::ptr> m_appenders;    //LogAppender集合
    std::shared_ptr<LogFormatter> m_formatter;  //日志格式
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    ~StdoutLogAppender(){}
};

//输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
    ~FileLogAppender(){}
    bool reopen(); //重新打开文件
private:
    std::string m_filename;
    std::ofstream m_filestream;
};



}

#endif // ! ELEVEN_LOG_H#define  ELEVEN_LOG_H


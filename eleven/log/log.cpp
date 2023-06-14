#include "log.h"
#include <map>
#include <functional>
#include <iostream>
#include <tuple>
namespace eleven{

void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        auto self = shared_from_this();
        for(auto &i : m_appenders){
            i->log(self,level, event);
        }
    }
}
LogEvent::LogEvent(const char *file, int32_t line, uint32_t elapse, 
            uint32_t threadid, uint32_t fiberid,uint64_t time)
            :m_file(file), m_lineno(line), m_elapse(elapse), m_threadid(threadid),
            m_fiberid(fiberid), m_time(time)
{

}

void Logger::debug(LogEvent::ptr event){
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event){
    log(LogLevel::INFO, event);

}
void Logger::error(LogEvent::ptr event){
    log(LogLevel::ERROR, event);

}
void Logger::warn(LogEvent::ptr event){
    log(LogLevel::WARN, event);

}
void Logger::fetal(LogEvent::ptr event){
    log(LogLevel::FATAL, event);

}

const char *LogLevel::ToString(LogLevel::Level level){
    switch(level){
#define RETURN_LEVEL(name) \
    case LogLevel::name:     \
        return #name;        \
        break;
    
    RETURN_LEVEL(DEBUG);
    RETURN_LEVEL(INFO);
    RETURN_LEVEL(WARN);
    RETURN_LEVEL(ERROR);
    RETURN_LEVEL(FATAL);
#undef RETURN_LEVEL
    default:
        return "UNKNOW LEVEL";
        break;
    }
    return "UNKNOW LEVEL";
}
void Logger::addAppender(LogAppender::ptr appender){
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = m_appenders.begin();it != m_appenders.end();++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

FileLogAppender::FileLogAppender(const std::string &filename){

}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_formatter->format(logger, level, event);
    }
}
bool FileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return true;
}
void StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
}

LogFormatter::LogFormatter(const std::string &pattern)
    :m_pattern(pattern)
{
    init();
}

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string &str = ""){} 
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    ElapseFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string &str=""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << logger->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem{
public:
    FiberIdFormatItem(const std::string &str=""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFiberId();
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string &str) : m_str(str){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << m_str;
    }
private:
    std::string m_str;
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S"):
        m_format(format)
    {
        if(m_format.empty()){
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        struct tm time;
        time_t  t = static_cast<time_t>(event->getTime());
        localtime_r(&t, &time);
        char buf[64] = {0};
        strftime(buf, sizeof(buf), m_format.c_str(), &time);
        os << buf;
    }
private:
    std::string m_format;
};


class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    FileNameFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFile();
    }
};

class LineNoFormatItem : public LogFormatter::FormatItem{
public:
    LineNoFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getLineNo();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << std::endl;
    }
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string &str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << "\t";
    }
};

std::string LogFormatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto & i : m_items){
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

void LogFormatter::init(){
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i){
        if(m_pattern[i] != '%'){
            nstr.append(1,m_pattern[i]);
            continue;
        }
        if((i+1) < m_pattern.size()){
            if(m_pattern[i+1] == '%'){
                nstr.append(1, '%');
                continue;
            }
        }
        size_t n = i + 1;
        int format_status = 0;
        size_t format_begin = 0;

        std::string str;
        std::string fmt;
        
        while( n < m_pattern.size()){
            if(!format_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' &&
                m_pattern[n] != '}'))
            {
                str = m_pattern.substr(i+1, n - i - 1);
                break;
            }
            if(format_status == 0){
                if(m_pattern[n] == '{'){
                    str = m_pattern.substr(i+1, n - i -1);
                    std::cout << "*" << str << std::endl;
                    format_status = 1;
                    format_begin = n;
                    ++n;
                    continue;
                }
            }else if(format_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(format_begin + 1, n - format_begin -1);
                    std::cout << "#" << fmt << std::endl;
                    format_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()){
                if(str.empty()){
                    str = m_pattern.substr(i+1);
                }
            }
        }

        if(format_status == 0){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }else if(format_status == 1){
            std::cout << "pattern parse error: "<<m_pattern << "-" << m_pattern.substr(i) <<std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));

        }
    }
    if(!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    /*
        %m : 消息内容
        %p : level
        %r : 启动日志的时间
        %c : 日志名称
        %t : 线程id
        %n : 回车换行
        %d : 时间
        %f : 文件名
        %l : 行号
    */
   static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_format_item = {
#define item(str, C) \
    {#str, [](const std::string &fmt){return FormatItem::ptr(new C(fmt));}}

    item(m, MessageFormatItem),
    item(p, LevelFormatItem),
    item(r, ElapseFormatItem),
    item(c, NameFormatItem),
    item(t, ThreadIdFormatItem),
    item(n, NewLineFormatItem),
    item(d, DateTimeFormatItem),
    item(f, FileNameFormatItem),
    item(l, LineNoFormatItem),
    item(T, TabFormatItem),
    item(F, FiberIdFormatItem)
#undef item
   };
   for(auto &i : vec){
    if(std::get<2>(i) == 0){
        m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
    }else{
        auto it = s_format_item.find(std::get<0>(i));
        if(it == s_format_item.end()){
            m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
        }else{
            m_items.push_back(it->second(std::get<1>(i)));
        }
    }
    std::cout << "{"<<std::get<0>(i) << "} - {"<< std::get<1>(i) << "} - {" <<std::get<2>(i)<<"}" << std::endl;
   }
   std::cout << m_items.size() << std::endl;
}

}
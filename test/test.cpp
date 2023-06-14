#include <iostream>
#include <string>
#include "../eleven/log/log.h"
#include <time.h>
#include "../eleven/util/util.h"
int main(){
    eleven::Logger::ptr logger(new eleven::Logger);
    logger->addAppender(eleven::LogAppender::ptr(new eleven::StdoutLogAppender));
    eleven::LogEvent::ptr event(new eleven::LogEvent(__FILE__, __LINE__, 0, eleven::getThreadId(), 2, time(0)));
    event->getSS() << "hello, eleven";
    logger->log(eleven::LogLevel::DEBUG, event);
}
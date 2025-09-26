#pragma once

#include "noncopyable.h"

#include <iostream>
#include <string>

//      为用户配置的宏
//      LogINFO("%s %d",arg1,arg2)
#define LogINFO(logmegformat,...)                           \
        do                                                  \
        {                                                   \
            Logger& logger = Logger::instance();            \
            logger.setLoggerLevel(INFO);                    \
            char buf[1024] = {0};                           \
            snprintf(buf,1024,logmegformat,##__VA_ARGS__);  \
            logger.log(buf);                                \
        }while(0)

#define LogERROR(logmegformat,...)                           \
        do                                                  \
        {                                                   \
            Logger& logger = Logger::instance();            \
            logger.setLoggerLevel(ERROR);                    \
            char buf[1024] = {0};                           \
            snprintf(buf,1024,logmegformat,##__VA_ARGS__);  \
            logger.log(buf);                                \
        }while(0)

#define LogFATAL(logmegformat,...)                           \
        do                                                  \
        {                                                   \
            Logger& logger = Logger::instance();            \
            logger.setLoggerLevel(FATAL);                    \
            char buf[1024] = {0};                           \
            snprintf(buf,1024,logmegformat,##__VA_ARGS__);  \
            logger.log(buf);                                \
        }while(0)

#ifdef MUDEBUG
#define LogDEBUG(logmegformat,...)                           \
        do                                                  \
        {                                                   \
            Logger& logger = Logger::instance();            \
            logger.setLoggerLevel(DEBUG);                    \
            char buf[1024] = {0};                           \
            snprintf(buf,1024,logmegformat,##__VA_ARGS__);  \
            logger.log(buf);                                \
        }while(0)
#else
#define LogDEBUG(logmegformat,...)   
#endif

// 定义日志等级的枚举
enum
{
    INFO,   // 普通信息
    ERROR,  // 错误信息
    FATAL,  // core信息
    DEBUG   // 调试信息
};

// 定义一个日志类
class Logger : noncopyable
{
public:
    // 用于返回一个日志对象
    static Logger& instance();
    // 设置日志等级
    void setLoggerLevel(int level);
    // 输出日志信息
    void log(std::string msg);
private:
    int logLevel_;
    Logger(){}
};


#include "logger.h" 
#include "TimeStamp.h" 

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::setLoggerLevel(int level)
{
    logLevel_ = level;
}

void Logger::log(std::string msg)
{
    switch (logLevel_)
    {
    case INFO:
        std::cout << "[INFO] ";
        break;
    case ERROR:
        std::cout << "[ERROR] ";
        break;
    case FATAL:
        std::cout << "[FATAL] ";
        break;
    case DEBUG:
        std::cout << "[DEBUG] ";
        break;
    }

    std::cout << TimeStamp::now().toString() << msg;
}
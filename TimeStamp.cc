#include "TimeStamp.h"

#include <ctime>
#include <cstdio>

TimeStamp::TimeStamp() : microSecondsSinceEpoch_(0)
{
}

TimeStamp::TimeStamp(std::int64_t microSecondsSinceEpoch) : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
    std::cout<<microSecondsSinceEpoch_<<std::endl;
}

TimeStamp TimeStamp::now()
{
    return TimeStamp(time(NULL));
}

std::string TimeStamp::toString()   const
{
    char buffer[128] = {0};
    std::cout<<microSecondsSinceEpoch_<<std::endl;
    tm *time = localtime(&microSecondsSinceEpoch_);
    snprintf(buffer, 128, "%4d/%02d/%02d %02d:%02d:%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
            time->tm_hour, time->tm_min, time->tm_sec);
    
    return buffer;
}

int main()
{
    std::cout << TimeStamp::now().toString() << std::endl;
}
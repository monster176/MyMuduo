#pragma once

#include <iostream>
#include <string>
#include <cstdint>

#include "noncopyable.h"

class TimeStamp : noncopyable
{
public:
    TimeStamp();
    TimeStamp(std::int64_t microSecondsSinceEpoch);
    static TimeStamp now();
    std::string toString() const;
private:
    std::int64_t microSecondsSinceEpoch_;
};

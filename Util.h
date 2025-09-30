#include "EventLoop.h"
#include "logger.h"

static EventLoop * CheckLoopNotNull(EventLoop * loop)
{
    
    if(loop == nullptr)
    {
        LogFATAL("%s:%s:%d Loop is null \n",__FILE__,__FUNCTION__,__LINE__);
    }
    return loop;
}
#include "Poller.h"
#include "EPollPoller.h"

#include <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if(getenv("MUDUO_USE_POLL"))
    {
        // 返回POLL的接口
        return nullptr;
    }
    else
    {
        // 返回EPOLL的接口
        return new EPollPoller(loop);
    }
}
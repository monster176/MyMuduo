#include "Channel.h"
#include "TimeStamp.h"
#include "EventLoop.h"
#include "logger.h"

#include <sys/epoll.h>

const int Channel::knoneEvent = 0;
const int Channel::kreadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kwriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd) : Loop_(loop), fd_(fd), events_(0), revents_(0), tied_(false), index_(-1)
{
}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::Update()
{
    // ADD code
    // loop_->updateChannel=();
}

void Channel::remove()
{
    // ADD code
    // loop_ -> removeChannel();
}

void Channel::handleEvent(TimeStamp receiveTime)
{
    if(tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(TimeStamp time)
{
    LogINFO("channel revents handle:%d",revents_);
    if((revents_&EPOLLHUP)&&!(revents_&EPOLLIN))
    {
        if(closeCallBack_)
        {
            LogDEBUG("handle close callback");
            closeCallBack_();
        }
    }

    if(revents_&EPOLLERR)
    {
        if(errorCallBack_)
        {
            LogDEBUG("handle error callback");
            errorCallBack_();
        }
    }

    if(revents_&EPOLLIN)
    {
        if(readCallBack_)
        {
            LogDEBUG("handle read callback");
            readCallBack_(time);
        }
    }

    if(revents_&EPOLLOUT)
    {
        if(writeCallBack_)
        {
            LogDEBUG("handle write callback");
            writeCallBack_();
        }
    }
}

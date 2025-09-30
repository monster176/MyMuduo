#include "EPollPoller.h"
#include "logger.h"
#include "Channel.h"

#include <cerrno>
#include <unistd.h>
#include <cstring>
// status key
// channel未加入poller中
const int kNew = -1;
// channel加入poller中
const int kAdded = 1;
// channel从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop) : Poller(loop), epoll_fd_(epoll_create1(EPOLL_CLOEXEC)), events_(kinitChannelSize)
{
    if (epoll_fd_ < 0)
    {
        LogFATAL("epoll_creat1 error:%d \n", errno);
    }
}

EPollPoller::~EPollPoller()
{
    close(epoll_fd_);
}

void EPollPoller::update(int operation, Channel *channel)
{
    const int fd = channel->fd();
    epoll_event event;
    bzero(&event,sizeof event);
    event.data.fd = channel->fd();
    event.data.ptr = channel;
    event.events = channel->events();

    if(epoll_ctl(epoll_fd_,operation,fd,&event)<0)
    {
        if(operation==EPOLL_CTL_DEL)
        {
            LogERROR("epoll_ctl del error %d\n",errno);
        }
        else
        {
            LogFATAL("epoll_ctl mod/add %d\n",errno);
        }
    }
}

void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    const int fd = channel->fd();
    LogINFO("func = %s fd = %d events = %d index = %d \n",__FUNCTION__,fd,channel->events(),index);
    
    if(index==kNew||index==kDeleted)
    {
        LogDEBUG("channel 未注册 需注册\n");
        if(index==kNew)
        {
            channels_[fd] = channel;
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD,channel);
    }
    else
    {
        if(channel->isNone())
        {
            update(EPOLL_CTL_DEL,channel);
            channel->setIndex(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD,channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel)
{
    const int fd = channel->fd();
    LogDEBUG("删除Channel fd  :  ");
    channels_.erase(fd);
    LogINFO("func = %s fd = %d \n", __FUNCTION__, fd);
    int idx = channel->index();
    if (idx == kAdded) // 现在是 已经添加的状态
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}


TimeStamp EPollPoller::Poll(int timeMs,ChannelList* activeChannels)
{
    LogDEBUG("func = %s => fd total count = %d \n",__FUNCTION__,static_cast<int>(channels->size()));
    
    int numEvents = epoll_wait(epoll_fd_,&*events_.begin(),static_cast<int>(events_.size()),timeMs);
    int saveErrno = errno;

    TimeStamp now(TimeStamp::now());

    if(numEvents>0)
    {
        LogINFO("%d event happen\n",numEvents);
        fullActiveChannels(numEvents,activeChannels);
        if(numEvents==events_.size())
        {
            events_.resize(numEvents*2);
        }
    }
    else if(numEvents==0)
    {
        LogDEBUG("%s timeout",__FUNCTION__);
    }
    else
    {
        if(saveErrno==EINTR)
        {
            errno = saveErrno;
            LogERROR("EPOLL_POLLER::poll() error");
        }
    }

    return now;
}

void EPollPoller::fullActiveChannels(int numEvents,ChannelList* activeChannels) const
{
    for(int i=0;i<numEvents;i++)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}
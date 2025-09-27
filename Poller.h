#pragma once

#include"noncopyable.h"
#include"TimeStamp.h"
#include"EventLoop.h"

#include<vector>
#include<unordered_map>

// 方便后续使用Channel类且不用写头文件
class Channel;

class Poller:noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    virtual TimeStamp Poll(TimeStamp time,ChannelList* channels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    bool hasChannel(Channel* channel)   const;
    Poller* newDefaultPoller(EventLoop* loop);

protected:
    using ChannelMap = std::unordered_map<int,Channel*>;
    ChannelMap channels_;

private:
    EventLoop ownerLoop_;

};
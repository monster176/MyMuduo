#pragma once

#include"noncopyable.h"
#include"TimeStamp.h"


#include<vector>
#include<unordered_map>

// 方便后续使用Channel类且不用写头文件
class Channel;
class EventLoop;

// muduo 库 中的 多路 事件 分发器
class Poller:noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;            // 使用 虚析构 函数 
									    	// 允许你通过基类指针或引用删除派生类对象
										  	// 从而支持多态删除
		   								    // 可以确保在删除派生类对象时，正确调用派生类的析构函数

    virtual TimeStamp Poll(int timeMs,ChannelList* channels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    bool hasChannel(Channel* channel)   const;
    static Poller* newDefaultPoller(EventLoop* loop);

protected:
    using ChannelMap = std::unordered_map<int,Channel*>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_;

};
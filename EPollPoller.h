#pragma once

#include "Poller.h"
#include "TimeStamp.h"

#include <sys/epoll.h>

// epoll 的 使用 流程 
// epoll_create
// epoll_ctl  add/mod/del
// epoll_wait
//

class EPollPoller:public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    // 重写 基类抽象方法 override
    TimeStamp Poll(int timeMs,ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    // 填写活跃链接
    void fullActiveChannels(int numEvents,ChannelList* activeChannels) const;

    // 更新Channel
    void update(int operation,Channel* channel);

    static const int kinitChannelSize = 16;

    using EventList = std::vector<epoll_event>;

    int epoll_fd_;
    EventList events_;
};

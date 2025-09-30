#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>

#include "noncopyable.h"
#include "TimeStamp.h"
#include "CurrentThread.h"


class Channel;
class Poller;

/**
 * 事件循环类，包含channel和poller
 */
class EventLoop:noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    using Functor = std::function<void()>;

    // 开启事件循环
    void loop();
    // 结束事件循环
    void quit();

    TimeStamp pollReturnTime() const {return pollerReturnTime_;}

    // 在当前loop中执行cb
    void runInLoop(Functor cb);
    // 把cb放到队列中唤醒loop所在的线程 执行cb
    void queenInLoop(Functor cb);

    // 唤醒loop所在的线程
    void wakeup();

    // EventLoop => Poller的方法
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // 如果他们相等,则表示当前线程是事件循环所属的线程，函数返回true，否则返回false
    // 判断EventLoop 对象是否在自己线程里面
    bool isInLoopThread() const {return thread_id_ == CurrentThread::tid();}

private:
    void handleRead(); // 唤醒weakup
    void doPendingFunctors(); // 执行回调函数

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_;      // 原子操作
    std::atomic_bool quit_;         // 退出loop循环
    std::atomic_bool callingPendingFunctors_;       // 表示当前loop是否有需要执行的回调操作

    const pid_t thread_id_;          // 记录当前loop所在线程的id
    TimeStamp pollerReturnTime_;

    std::unique_ptr<Poller> poller_;        // EventLoop 管理的Poller

    int wakeupFd_;                  // 当mainloop获取一个新用户的channel通过轮询算法选择subloop处理channel

    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    std::vector<Functor> pendingFunctions_; // 存储loop需要执行的所有回调操作
    std::mutex mutex_;                      // 互斥锁 保护vector的成员是线程安全操作

};
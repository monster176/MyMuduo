#include <sys/eventfd.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <memory>

#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "logger.h"

// 用于防止一个线程创建多个循环
__thread EventLoop *t_loopInThisThread = nullptr;

// 定义了默认的poller的IO复用的超时事件
const int kPollTimeMs = 10000;

// 创建wakeupfd 用来notify唤醒subReactor 处理新来的channel
int createEventfd()
{
    int evtfd = ::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);

    if(evtfd<0)
    {
        LogFATAL("eventfd error %d \n",errno);
    }

    return evtfd;
}

EventLoop::EventLoop() 
        : looping_(false)
        , quit_(false)
        , callingPendingFunctors_(false)
        , thread_id_(CurrentThread::tid())
        , poller_(Poller::newDefaultPoller(this))
        , wakeupFd_(createEventfd())
        , wakeupChannel_(new Channel(this,wakeupFd_)) 
                         // , currentActiveChannel_(nullptr)
{

    LogDEBUG("EventLoop create %p in thread %d \n",this,thread_id_);

    if(t_loopInThisThread)
    {
        LogFATAL("Another EventLoop %p exists in this thread %d \n",t_loopInThisThread,thread_id_);
    }
    else
    {
        t_loopInThisThread = this;
    }

    // 设置wakeupfd的事件类型 和 发生后的回调操作
    wakeupChannel_->setreadCallBack(std::bind(&EventLoop::handleRead,this));
    LogDEBUG("start wakeupChannel_->enableReading() \n");
    wakeupChannel_->enableReadEvent();

}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    LogINFO("EventLoop %p start looping \n",this);

    while(!quit_)
    {
        activeChannels_.clear();
        // 监听两个fd 一种client 的fd 一种 wakeupfd
        pollerReturnTime_ = poller_ -> Poll(kPollTimeMs,&activeChannels_);

        for(Channel *channel:activeChannels_)
        {
            // Poller 监听那些channel事件发生了，报告给EventLoop 通知channel相应事件
            channel->handleEvent(pollerReturnTime_);
        }
        // 执行当前EventLoop事件循环需要处理的回调操作
        doPendingFunctors();
    }

    LogINFO("EventLoop %p stop looping \n",this);
    looping_ = false;

}

// 退出事件循环
// loop 在 自己的线程中调用quit
// loop 在 其他的线程中调用quit
void EventLoop::quit()
{
    quit_ = true;

    if(!isInLoopThread())
    {
        wakeup();
    }
    
    // quit_ = false;
}

void EventLoop::runInLoop(Functor cb)
{
    LogDEBUG("now in EventLoop::runInLoop(Functor cb) \n");
    if(isInLoopThread())
    {
        LogINFO("在当前loop所在的线程中 执行callback \n");
        cb();
    }
    else
    {
        LogINFO("在非当前线程中执行callback | 需要唤醒loop所在的线程执行callback \n");
        queenInLoop(cb);
    }   
}

void EventLoop::queenInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctions_.emplace_back(cb);
    }

    // 唤醒相应的，需要执行上面回调操作的 loop的线程
    if(!isInLoopThread()||callingPendingFunctors_)  // 代表当前的loop正在执行回调但是又有新的回调
    {
        wakeup();       // 唤醒loop所在的线程
    }
}

void EventLoop::handleRead()
{
    LogDEBUG("执行EventLoop::handleRead的回调函数 \n");

    uint16_t one = 1;
    ssize_t n = read(wakeupFd_,&one,sizeof one);
    if(n!=sizeof one)
    {
        LogERROR("EventLoop::handleRead() reads %ld bytes instead of 8",n);
    }
}

// 唤醒loop所在的线程
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_,&one,sizeof one);
    if(n!=sizeof one)
    {
        LogERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n",n);
    }
}

// EventLoop => Poller方法
void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors() // 执行回调函数
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true; // 需要执行回调

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctions_);
    }

    for(const Functor& functor:functors)
    {
        LogDEBUG("执行当前loop需要执行的回调操作===========================> OK");
        functor();          // 执行当前loop 的回调操作
    }
    callingPendingFunctors_ = false;
}

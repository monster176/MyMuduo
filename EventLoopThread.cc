#include "EventLoopThread.h"
#include "logger.h"


EventLoopThread::EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                const std::string name = std::string())
                : loop_(nullptr)
                , exiting_(false)
                , thread_(std::bind(&EventLoopThread::threadFunc,this),name)
                , mutex_()
                , cond_()
                , callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_!=nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    thread_.start();    // 启动底层的线程
    LogDEBUG("EventLoopThread::startLoop() 底层线程池启动成功\n");
    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_==nullptr)
        {
            LogDEBUG("cond_.wait(lock)ing \n");
            cond_.wait(lock);
            LogDEBUG("cond_.wait(lock)ing OK\n");
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;      // 创建 一个独立的线程 和上面是一一对应的 one loop per thread
    if(callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_  = &loop;
        cond_.notify_one();
    }
    loop.loop();            // EventLoop loop => Poller.poll
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}
 
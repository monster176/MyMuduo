#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include "logger.h"


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop,const std::string &name)
                    : baseLoop_(baseLoop)
                    , name_(name)
                    , started_(false)
                    , numThreads_(0)
                    , next_(0)      // 轮询下标
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // 源项目say ：     Don't delete loop it's  stack  variable 
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb)
{
    started_ = true;
    for(int i=0;i<numThreads_;i++)
    {
        LogDEBUG("debug out => EventLoopThreadPool::start \n");
        char buf[name_.size()+32] = {0};
        snprintf(buf,sizeof buf,"%s%d",name_.c_str(),i);        // 线程池的名称 + id 为某个线程的name
        // 创建Event loop thread
        EventLoopThread *t = new EventLoopThread(cb,buf);
        thread_.push_back(std::unique_ptr<EventLoopThread>(t));
        LogDEBUG("底层创建线程, 绑定一个新的EventLoop 返回loop的地址 ###### OK \n");
    }

    LogDEBUG("*** numThreads = %d | cb != null %s ? ** \n",numThreads_,(cb==nullptr?"Yes":"No"));

    // 整个服务 只有一个线程 运行着 baseloop
    if(numThreads_==0&&cb)
    {
        LogDEBUG("整个服务 只有一个线程 运行着 baseloop");
        cb(baseLoop_);
    }

}

// 使用轮询的方式 将baseloop 分配给channel 和 subloop
EventLoop* EventLoopThreadPool::getNextloop()
{
    EventLoop *loop = baseLoop_;

    if(!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if(next_ >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if(loops_.empty())
    {
        return std::vector<EventLoop*>(1,baseLoop_);
    }
    else 
    {
        return loops_;
    }
}
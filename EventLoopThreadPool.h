#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "noncopyable.h"

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop *baseLoop,const std::string &name);
    ~EventLoopThreadPool();

    void setThreadNum(int numberThreads){numThreads_ = numberThreads;}

    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // 如果 工作在多线程中 baseLoop 可以通过轮询的方式 分配subloop
    EventLoop* getNextloop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const {return started_;}
    const std::string name() const {return name_;}
private:
    EventLoop* baseLoop_;       // EventLoop loop; 一个线程
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> thread_;
    std::vector<EventLoop*> loops_;
};
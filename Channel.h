#pragma once

#include <functional>
#include <memory>

#include "TimeStamp.h" 
#include "EventLoop.h"
#include "noncopyable.h"

// 声明事件循环的类，以便后续的调用
// class EventLoop;

/**
 * 明确Channel和Poller和EventLoop的关系
 * Channel理解为通道，封装了sockfd和其感兴趣的event，如EPOLLIN，EPOLLOUT事件
 * 还绑定了poller返回的具体事件
 */
class Channel: noncopyable
{
public:
    using EventCallBack = std::function<void()>;
    using ReadEventCallBack = std::function<void(TimeStamp)>;

    Channel(EventLoop *loop,int fd);
    ~Channel();

    // fd得到poller通知以后，处理事件的
    void handleEvent(TimeStamp receiveTime);

    // 设置回调函数
    void setreadCallBack(ReadEventCallBack cb) {readCallBack_ = std::move(cb);}
    void setwriteCallBack(EventCallBack cb) {writeCallBack_ = std::move(cb);}
    void setcloseCallBack(EventCallBack cb) {closeCallBack_ = std::move(cb);}
    void seterrorCallBack(EventCallBack cb) {errorCallBack_ = std::move(cb);}

    // 弱 智能指针   
	// 防止 当 channel 被 手动 remove 掉 channel 还在进行 回调操作
    void tie(const std::shared_ptr<void>&);

    void enableReadEvent() {events_ |= kreadEvent; Update();}
    void disableReadEvent() {events_ &= ~kreadEvent; Update();}
    void enableWriteEvent() {events_ |= kwriteEvent; Update();}
    void disableWriteEvent() {events_ &= ~kwriteEvent; Update();}
    void disableAll() {events_ = knoneEvent; Update();}
     
    int fd() const  {return fd_;}
    int events() const  {return events_;}
    void setRevents(int rev)   {revents_ = events_;}
    int index() const {return index_;}
    void setIndex(int idx) {index_ = idx;}

    bool isReading() const {return events_ == kreadEvent;}
    bool isWriting() const {return events_ & kwriteEvent;}
    bool isNone() const    {return events_ & knoneEvent;}

    EventLoop ownerLoop() {return Loop_;} 
    void remove();

private:
    void Update();
    void handleEventWithGuard(TimeStamp time);

    static const int knoneEvent;
    static const int kreadEvent;
    static const int kwriteEvent;

    EventLoop Loop_;   // 事件循环
    const int fd_;      // Poller监听的是哪个文件
    int events_;        // 发生的具体的事件
    int revents_;       // 真正发生的事件
    int index_;         // 是哪个Poller

    std::weak_ptr<void> tie_;
    bool tied_;

    // 因为channel通道里面是能够获知fd最终发生的事件revents，所以它负责调用具体时间的回调操作
    ReadEventCallBack readCallBack_;    
    EventCallBack writeCallBack_;
    EventCallBack closeCallBack_;
    EventCallBack errorCallBack_;
};


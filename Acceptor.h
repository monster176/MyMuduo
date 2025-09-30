#pragma once

#include "noncopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>

class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int sockfd,const InetAddress&)>;

    Acceptor(EventLoop *loop,const InetAddress &listenAddr,bool reuseport);
    ~Acceptor();

    void setNewConnectonCallback(const NewConnectionCallback &cb) {newConnectionCallback_  = std::move(cb);}

    bool listening() const {return listenning_;}
    void listen();

private:
    void handleRead();

    //  Acceptor用的是用户定义的baseloop 也是mainloop
    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;

};
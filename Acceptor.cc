#include "Acceptor.h"
#include "logger.h"
#include "InetAddress.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

static int createSocketfd()
{
    int sockFd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,0);
    // int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockFd<0)
    {
        LogFATAL("%s:%s:%d socket error errno = %d \n",__FILE__,__FUNCTION__,__LINE__,errno);
    }
    LogDEBUG("Acceptor::createSocketfd() ============> OK\n");

    return sockFd;
}

Acceptor::Acceptor(EventLoop *loop,const InetAddress &listenAddr,bool reuseport)
                    : loop_(loop)
                    , acceptSocket_(createSocketfd())
                    , acceptChannel_(loop,acceptSocket_.fd())
                    , listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);

    acceptChannel_.setreadCallBack(std::bind(&Acceptor::handleRead,this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen()
{
    listenning_ = true;
    LogDEBUG("Listen Start ============> OK\n");
    acceptSocket_.listen();             // listen
    acceptChannel_.enableReadEvent();   // 注册acceptChannel => Poller
}

// listen 有时间发生了  就是有新用户链接了
void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd>=0)
    {
        if(newConnectionCallback_)
        {
            newConnectionCallback_(connfd,peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LogERROR("%s:%s:%d Accept error errno = %d \n",__FILE__,__FUNCTION__,__LINE__,errno);
        if(errno==EMFILE)
        {
            LogERROR("%s:%s:%d Accept error :|||{errno = EMFILE} \n",__FILE__,__FUNCTION__,__LINE__);
        }
    }
}
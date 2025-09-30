#include "Socket.h"
#include "logger.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/tcp.h>
#include <fcntl.h>


Socket::~Socket()
{
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localAddr)
{
    if(::bind(sockfd_,(sockaddr*)localAddr.getSockAddr(),sizeof(sockaddr_in))!=0)
    {
        LogFATAL("bind Sockfd %d fail\n",sockfd_);
    }
    LogDEBUG("Socket::bindAddress=============> OK\n");
}

void Socket::listen()
{
    if((::listen(sockfd_,1024))!=0)
    {
        LogFATAL("listen Sockfd %d fail\n",sockfd_);
    }
    LogDEBUG("Socket::listen=============> OK\n");
}

int Socket::accept(InetAddress *peeraddr)
{
    LogINFO("DEBUG OUT => int Socket::accept(InetAddress *peeraddr) \n");

    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    bzero(&addr,sizeof addr);
    int connfd = ::accept4(sockfd_,(sockaddr*)&addr,&len,SOCK_NONBLOCK|SOCK_CLOEXEC);

    // // 检查非阻塞模式和 CLOEXEC 标志
    // int flag = ::fcntl(sockfd_, F_GETFL, 0);
    // if (flag < 0) return -1;
    // if (!(flag & O_NONBLOCK)) {
    //     flag |= O_NONBLOCK;
    //     if (::fcntl(sockfd_, F_SETFL, flag) < 0) return -1;
    // }

    // flag = ::fcntl(sockfd_, F_GETFD, 0);
    // if (flag < 0) return -1;
    // if (!(flag & O_CLOEXEC)) {
    //     flag |= O_CLOEXEC;
    //     if (::fcntl(sockfd_, F_SETFD, flag) < 0) return -1;
    // }

    // int connfd = ::accept(sockfd_, (sockaddr *)&addr, &len);
    if(connfd>=0)
    {
        peeraddr->setSockAddr(addr);
    }
    LogDEBUG("Socket::accept connfd = %d ==========> OK",connfd);
    
    return connfd;
}

void Socket::shutdownWrite()
{
    if(::shutdown(sockfd_,SHUT_WR)<0)
    {
        LogERROR("shutdown Write Error\n");
    }
    LogDEBUG("!!!!!!!!!!!!!!!!!shutdownWrite sockfd:%d ==============> OK", sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
    int op = on?1:0;
    ::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&op,sizeof op);
}
void Socket::setReuseAddr(bool on)
{
    int op = on?1:0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&op,sizeof op);
}

void Socket::setReusePort(bool on)
{
    int op = on?1:0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&op,sizeof op);
}

void Socket::setKeepAlive(bool on)
{
    int op = on?1:0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&op,sizeof op);
}
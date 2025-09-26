#include "InetAddress.h"
#include <string.h>

InetAddress::InetAddress(uint16_t port,std::string ip)
{
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

uint16_t InetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}

std::string InetAddress::toIP() const
{
    char buf[64] = {0};
    inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
    return buf;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
    size_t len = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf+len,":%u",port);
    return buf;
}

int main()
{
    InetAddress addr(8888);
    std::cout<<addr.toIpPort()<<std::endl;
    return 0;
}
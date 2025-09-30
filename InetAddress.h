#pragma once

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

class InetAddress
{
public:
    explicit InetAddress(uint16_t port,std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr):addr_(addr){};

    const sockaddr_in* getSockAddr() const {return &addr_;}
    void setSockAddr(const sockaddr_in &addr){addr_ = addr;}

    uint16_t toPort() const;
    std::string toIP() const;
    std::string toIpPort() const;
private:
    sockaddr_in addr_;
};
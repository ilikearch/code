#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Common.hpp"

class InetAddr
{
private:
    void PortNet2Host()
    {
        _port = ::ntohs(_net_addr.sin_port);
    }
    void IpNet2Host()
    {
        char ipbuffer[64];
        const char *ip = ::inet_ntop(AF_INET, &_net_addr.sin_addr, ipbuffer, sizeof(ipbuffer));
        //(void)ip;
        _ip=ipbuffer;
    }

public:
    InetAddr()
    {
    }
    InetAddr(const struct sockaddr_in &addr) : _net_addr(addr)
    {
        PortNet2Host();
        IpNet2Host();
    }
    InetAddr(uint16_t port) : _port(port), _ip("")
    {
        _net_addr.sin_family = AF_INET;
        _net_addr.sin_port = htons(_port);
        _net_addr.sin_addr.s_addr = INADDR_ANY;
    }
    struct sockaddr *NetAddr() { return CONV(&_net_addr); }
    socklen_t NetAddrLen() { return sizeof(_net_addr); }
    std::string Ip() { return _ip; }
    uint16_t Port() { return _port; }
    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _net_addr;
    std::string _ip;
    uint16_t _port;
};
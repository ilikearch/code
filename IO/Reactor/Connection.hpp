#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include "InetAddr.hpp"

class Reactor;

// 普通的fd, Listensockfd;
// 让对fd的处理方式采用同一种方式
// 描述一个连接
class Connection
{
public:
    Connection() : _sockfd(-1), _events(0)
    {
        // 自动获得当前系统的时间戳
    }
    void UpdateTime()
    {
        // 更新时间,重新获取时间
    }
    void SetPeerInfo(const InetAddr &peer_addr)
    {
        _peer_addr = peer_addr;
    }
    void SetSockfd(int sockfd)
    {
        _sockfd = sockfd;
    }
    int Sockfd() { return _sockfd; }
    void SetEvents(uint32_t events) { _events = events; }
    uint32_t GetEvents() { return _events; }
    void SetOwner(Reactor *owner)
    {
        _owner = owner;
    }
    Reactor *GetOwner()
    {
        return _owner;
    }
    void Append(const std::string &in)
    {
        _inbuffer += in; // 把收到的数据，添加到自己的接受缓冲区
    }
    void AppendToOut(const std::string &out)
    {
        _outbuffer += out;
    }
    void DisCardOutString(int n)
    {
        _outbuffer.erase(0, n);
    }
    bool IsOutBufferEmpty() { return _outbuffer.empty(); } 
    std::string &OutString() // for test
    {
        return _outbuffer;
    }
    std::string &InBuffer() // 故意这样写
    {
        return _inbuffer;
    }
    void Close()
    {
        if(_sockfd>=0)
            close(_sockfd);
    }
    // 回调方法
    virtual void Sender() = 0;
    virtual void Recver() = 0;
    virtual void Excepter() = 0;
    ~Connection()
    {
    }

protected:
    int _sockfd;
    std::string _inbuffer;
    std::string _outbuffer;
    InetAddr _peer_addr; // 对应哪一个客户端

    // 添加一个指针
    Reactor *_owner;

    // 我关心的事件
    uint32_t _events; // 我这个connection关心的事件

    // lastmodtime
    uint64_t _timestamp;
};

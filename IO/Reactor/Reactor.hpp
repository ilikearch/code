#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include "Epoller.hpp"
#include "Connection.hpp"

using namespace EpollModule;

using connection_t = std::shared_ptr<Connection>;

class Reactor
{
    const static int event_num = 64;

private:
    bool IsConnectionExists(int sockfd)
    {
        return _connections.find(sockfd) != _connections.end();
    }

public:
    Reactor() : _isrunning(false), _epoller(std::make_unique<Epoller>())
    {
        _epoller->Init();
    }
    void InsertConnection(connection_t conn)
    {
        auto iter = _connections.find(conn->Sockfd());
        if (iter == _connections.end())
        {
            // 1. 把连接，放到unordered_map中进行管理
            _connections.insert(std::make_pair(conn->Sockfd(), conn));
            // 2. 把新插入进来的连接，写透到内核的epoll中
            _epoller->Add(conn->Sockfd(), conn->GetEvents());
            // 3. 设置关联关系，让connection回指当前对象
            conn->SetOwner(this);
            LOG(LogLevel::DEBUG) << "add connection success: " << conn->Sockfd();
        }
    }
    void EnableReadWrite(int sockfd, bool readable, bool writeable)
    {
        if (IsConnectionExists(sockfd))
        {
            // 修改用户层connection的事件
            uint32_t events = ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
            _connections[sockfd]->SetEvents(events);
            // 写透到内核中
            _epoller->Update(sockfd, _connections[sockfd]->GetEvents());
        }
    }
    void DelConnection(int sockfd)
    {
        if (IsConnectionExists(sockfd))
        {
            // 1. 从内核中异常对sockfd的关心
            _epoller->Delete(sockfd);
            // 2. 关闭特定的文件描述
            _connections[sockfd]->Close();
            // 3. 从_connections中移除对应的connection
            _connections.erase(sockfd);
        }
    }

    void Dispatcher(int n)
    {
        for (int i = 0; i < n; i++)
        {
            // 开始进行派发, 派发给指定的模块
            int sockfd = _revs[i].data.fd;
            uint32_t revents = _revs[i].events;
            if ((revents & EPOLLERR) || (revents & EPOLLHUP))
                revents = (EPOLLIN | EPOLLOUT); // 异常事件，转换成为读写事件
            if ((revents & EPOLLIN) && IsConnectionExists(sockfd))
            {
                _connections[sockfd]->Recver();
            }
            if ((revents & EPOLLOUT) && IsConnectionExists(sockfd))
            {
                _connections[sockfd]->Sender();
            }
        }
    }

    void LoopOnce(int timeout)
    {
        int n = _epoller->Wait(_revs, event_num, timeout);
        Dispatcher(n);
    }
    void DebugPrint()
    {
        std::cout << "Epoller 管理的fd: ";
        for (auto &iter : _connections)
        {
            std::cout << iter.first << " ";
        }
        std::cout << std::endl;
    }
    void Loop()
    {
        _isrunning = true;
        // int timeout = -1;
        int timeout = 1000;
        while (_isrunning)
        {
            LoopOnce(timeout);
            DebugPrint();
            // 超时管理
            // 简单的,遍历_connections, 判断当前时间 - connection的最近访问时间>XXX
            // 超时了
        }
        _isrunning = false;
    }
    void Stop()
    {
        _isrunning = false;
    }
    ~Reactor()
    {
    }

private:
    std::unique_ptr<Epoller> _epoller;                  // 不要忘记初始化
    std::unordered_map<int, connection_t> _connections; // fd: Connection， 服务器内部所有的连接
    bool _isrunning;
    struct epoll_event _revs[event_num];
};
#pragma once

#include <iostream>
#include <memory>
#include "Epoller.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Connection.hpp"
#include "IOService.hpp"
#include "Reactor.hpp"
#include "Protocol.hpp"
#include "Calculator.hpp"

using namespace SocketModule;
using namespace LogModule;
// 专门负责获取链接的模块
// 连接管理器
class Listener : public Connection
{
public:
    Listener(int port)
        : _listensock(std::make_unique<TcpSocket>()),
          _port(port)
    {
        _listensock->BuildTcpSocketMethod(_port);
        SetSockfd(_listensock->Fd());
        SetEvents(EPOLLIN|EPOLLET);
    }
    virtual void Sender() override
    {
    }
    // 我们回调到这里，天然就有父类connection
    virtual void Recver() override
    {
        // 读就绪，而是是listensock就绪
        // IO处理 --- 获取新连接
        // 你怎么知道，一次来的，就是一个连接呢？你怎么保证你一次读完了么？
        while (true)
        {
            InetAddr peer;
            int aerrno = 0;
            // accept 非阻塞的时候，就是IO，我们就向处理read一样，处理accept
            int sockfd = _listensock->Accepter(&peer, &aerrno);
            if (sockfd > 0)
            {
                // success
                // 我们毫不意外的是不能直接读取的！
                // sockfd 添加到epoll！
                // epollserver只认connection
                LOG(LogLevel::DEBUG) << "Accepter success: " << sockfd;
                // 普通的文件描述符，处理IO的，也是connection！
                // 2. sockfd包装成为Connection！
                auto conn = std::make_shared<IOService>(sockfd);
                conn->RegisterOnMessage(HandlerRequest); //???
                // 3. 插入到EpollServer
                GetOwner()->InsertConnection(conn);
            }
            else
            {
                if (aerrno == EAGAIN || aerrno == EWOULDBLOCK)
                {
                    LOG(LogLevel::DEBUG) << "accetper all connection ... done";
                    break;
                }
                else if (aerrno == EINTR)
                {
                    LOG(LogLevel::DEBUG) << "accetper intr by signal, continue";
                    continue;
                }
                else
                {
                    LOG(LogLevel::WARNING) << "accetper error ... Ignore";
                    break;
                }
            }
        }
    }
    virtual void Excepter() override
    {
    }
    int Sockfd() { return _listensock->Fd(); }
    ~Listener()
    {
        _listensock->Close();
    }

private:
    std::unique_ptr<Socket> _listensock;
    int _port;
};
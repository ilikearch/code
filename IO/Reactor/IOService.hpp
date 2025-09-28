#pragma once

#include <iostream>
#include <memory>
#include <functional>
#include "Epoller.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Connection.hpp"
#include "Protocol.hpp"
#include "Reactor.hpp"

using func_t = std::function<std::string(std::string &)>;

// 定位：只负责IO
class IOService : public Connection
{
    static const int size = 1024;

public:
    IOService(int sockfd)
    {
        // 1. 设置文件描述符非阻塞
        SetNonBlock(sockfd);
        SetSockfd(sockfd);
        SetEvents(EPOLLIN | EPOLLET);
    }
    virtual void Sender() override
    {
        // UpdateTime();
        // 直接写
        while (true)
        {
            ssize_t n = send(Sockfd(), OutString().c_str(), OutString().size(), 0);
            if (n > 0)
            {
                // 成功
                DisCardOutString(n); // 移除N个
            }
            else if (n == 0)
            {
                break;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // 缓冲区写满了，下次再来
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    Excepter();
                    return;
                }
            }
        }

        // 一种：outbuffer empty
        // 一种：发送缓冲区被写满了 && outbuffer没有empty，写条件不满足，使能sockfd在epoll中的事件
        if(!IsOutBufferEmpty())
        {
            // 修改对sockfd的事件关心！-- 开启对写事件关心
            // 按需设置！
            GetOwner()->EnableReadWrite(Sockfd(), true, true);
        }
        else{
            GetOwner()->EnableReadWrite(Sockfd(), true, false);
        }
    }
    // 我们回调到这里，天然就有父类connection
    virtual void Recver() override
    {
        // UpdateTime();
        // 1. 读取所有数据
        while (true) // ET模式
        {
            char buffer[size];
            ssize_t s = recv(Sockfd(), buffer, sizeof(buffer) - 1, 0); // 非阻塞
            if (s > 0)
            {
                buffer[s] = 0;
                // 读取成功了
                Append(buffer);
            }
            if (s == 0)
            {
                // 对端关闭连接
                Excepter();
                return;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    // 发生错误了
                    Excepter();
                    return;
                }
            }
        }

        // 走到下面，我一定把本轮数据读取完毕了
        std::cout << "outbuffer: \n"
                  << InBuffer() << std::endl;
        // 你能确保你读到的消息，就是一个完整的报文吗？？不能！！！
        // 我们怎么知道，读到了完整的请求呢？？协议！！！
        std::string result;
        if (_on_message)
            result = _on_message(InBuffer()); // 
        // 添加应答信息
        AppendToOut(result);

        // 如何处理写的问题, outbuffer 发送给对方的问题！
        if (!IsOutBufferEmpty())
        {
            //方案一: Sender(); // 直接发送, 推荐做法
            //方案二: 使能Writeable即可.
            GetOwner()->EnableReadWrite(Sockfd(), true, true);
        }

    }
    virtual void Excepter() override
    {
        // IO读取的时候，所有的异常处理，全部都会转化成为这个一个函数的调用
        // 出现异常，我们怎么做？？？
        // 打印日志，差错处理，关闭连接，Reactor异常connection, 从内核中，移除对fd的关心
        LOG(LogLevel::INFO) << "客户端连接可能结束，进行异常处理: " << Sockfd();
        GetOwner()->DelConnection(Sockfd());
    }
    void RegisterOnMessage(func_t on_message)
    {
        _on_message = on_message;
    }
    ~IOService()
    {
    }

private:
    func_t _on_message;
};
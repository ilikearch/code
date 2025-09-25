#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <sys/epoll.h>
#include "Log.hpp"
#include "Socket.hpp"

using namespace SocketModule;
using namespace LogModule;

const int gdefaultfd = -1;

#define MAX 4096

// 最开始的时候，tcpserver只有一个listensockfd
class EpollServer
{
    static const int revs_num = 64;

public:
    EpollServer(int port)
        : _port(port),
          _epfd(gdefaultfd),
          _listen_socket(std::make_unique<TcpSocket>()),
          _isrunning(false)
    {
    }
    void Init()
    {
        _listen_socket->BuildTcpSocketMethod(_port);
        // 1. 创建epoll模型
        _epfd = epoll_create(256);
        if (_epfd < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_create error";
            exit(EPOLL_CREATE_ERR);
        }
        LOG(LogLevel::DEBUG) << "epoll_create success: " << _epfd;
        // 2. 至少要将listensock添加到epoll模型中
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listen_socket->Fd();
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, _listen_socket->Fd(), &ev);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_ctl error";
            exit(EPOLL_CTL_ERR);
        }
    }
    void Loop()
    {
        int timeout = -1;
        _isrunning = true;
        while (_isrunning)
        {
            // 我们不能让accept来阻塞检测新连接到来，而应该让epoll来负责进行就绪事件的检测
            // 用户告诉内核，你要帮我关心&rfds，读事件啊！！
            int n = epoll_wait(_epfd, _revs, revs_num, timeout); // 通知上层的任务!
            switch (n)
            {
            case 0:
                std::cout << "time out..." << std::endl;
                break;
            case -1:
                perror("epoll_wait");
                break;
            default:
                // 有事件就绪了
                // rfds: 内核告诉用户，你关心的rfds中的fd，有哪些已经就绪了！！
                std::cout << "有事件就绪啦..., timeout: " << std::endl;
                Dispatcher(n);
                break;
            }
        }
        _isrunning = false;
    }
    void Accepter() // 回调函数呢？
    {
        InetAddr client;
        // listensockfd就绪了！获取新连接不就好了吗？
        auto sockptr = _listen_socket->Accepter(&client); // 会不会被阻塞呢？不会！select已经告诉我，listensockfd已经就绪了！只执行"拷贝"
        int newfd = sockptr->Fd();
        if (newfd < 0)
            return;
        else
        {
            std::cout << "获得了一个新的链接: " << newfd << " client info: " << client.Addr() << std::endl;
            // recv()?? 读事件是否就绪，我们并不清楚！newfd也托管给select，让select帮我进行关心新的sockfd上面的读事件就绪
            // 怎么把新的newfd托管给epoll?让epoll帮我去关心newfd上面的读事件
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = newfd;
            int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, newfd, &ev);
            if (n < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_ctl error";
                close(newfd);
            }
            LOG(LogLevel::DEBUG) << "epoll_ctl success: " << newfd;
        }
    }
    void Recver(int fd) // 回调函数？
    {
        // 合法的，就绪的，普通的fd
        // 这里的recv，对不对啊！不完善！必须得有协议！
        char buffer[1024];
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0); // 会不会被阻塞？就绪了
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client# " << buffer << std::endl;
            // 把读到的信息，在回显会去
            std::string message = "echo# ";
            message += buffer;
            send(fd, message.c_str(), message.size(), 0); // bug
        }
        else if (n == 0)
        {
            LOG(LogLevel::DEBUG) << "客户端退出, sockfd: " << fd;
            // 把fd从epoll中移除, 必须保证fd是一个合法的fd.
            int m = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            if (m < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_ctl error";
                return;
            }
            LOG(LogLevel::DEBUG) << "epoll_ctl success: " << fd;
            close(fd);
        }
        else
        {
            LOG(LogLevel::DEBUG) << "客户端读取出错, sockfd: " << fd;
            // 把fd从epoll中移除
            int m = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            if (m < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_ctl error";
                return;
            }
            LOG(LogLevel::DEBUG) << "epoll_ctl success: " << fd;
            close(fd);
        }
    }
    void Dispatcher(int rnum) // rfds就可能会有更多的fd就绪了，就不仅仅 是listenfd就绪了
    {
        for (int i = 0; i < rnum; i++)
        {
            int events = _revs[i].events;
            int fd = _revs[i].data.fd;
            if (fd == _listen_socket->Fd())
            {
                if (events & EPOLLIN)
                {
                    // listen sock fd 就绪
                    Accepter();
                }
            }
            else
            {
                // 普通文件描述符就绪
                if (events & EPOLLIN)
                {
                    // 读事件就绪
                    Recver(fd);
                }
                // else if(events & EPOLLOUT)
                // {
                //     //写事件就绪
                // }
            }
        }
    }
    ~EpollServer()
    {
        _listen_socket->Close();
        if (_epfd >= 0)
            close(_epfd);
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listen_socket;
    bool _isrunning;
    int _epfd;
    struct epoll_event _revs[revs_num];
};
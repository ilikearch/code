#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <poll.h>
#include "Log.hpp"
#include "Socket.hpp"

using namespace SocketModule;
using namespace LogModule;

const int gdefaultfd = -1;

#define MAX 4096

// 最开始的时候，tcpserver只有一个listensockfd
class PollServer
{
public:
    PollServer(int port)
        : _port(port),
          _listen_socket(std::make_unique<TcpSocket>()),
          _isrunning(false)
    {
    }
    void Init()
    {
        _listen_socket->BuildTcpSocketMethod(_port);
        for(int i = 0;i < MAX; i++)
        {
            _fds[i].fd = gdefaultfd;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }
        // 先把唯一的一个fd添加到poll中
        _fds[0].fd = _listen_socket->Fd();
        _fds[0].events |= POLLIN;
        // _fds[0].revents;
    }
    void Loop()
    {
        int timeout = -1;
        _isrunning = true;
        while (_isrunning)
        {
            // 我们不能让accept来阻塞检测新连接到来，而应该让poll来负责进行就绪事件的检测
            // 用户告诉内核，你要帮我关心&rfds，读事件啊！！
            int n = poll(_fds, MAX, timeout); // 通知上层的任务!
            switch (n)
            {
            case 0:
                std::cout << "time out..." << std::endl;
                break;
            case -1:
                perror("poll");
                break;
            default:
                // 有事件就绪了
                // rfds: 内核告诉用户，你关心的rfds中的fd，有哪些已经就绪了！！
                std::cout << "有事件就绪啦..., timeout: " << std::endl;
                Dispatcher(); // 把已经就绪的sockfd，派发给指定的模块
                TestFd();
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
        int newfd=sockptr->Fd();
        if (newfd < 0)
            return;
        else
        {
            std::cout << "获得了一个新的链接: " << newfd << " client info: " << client.Addr() << std::endl;
            // recv()?? 读事件是否就绪，我们并不清楚！newfd也托管给select，让select帮我进行关心新的sockfd上面的读事件就绪
            // 怎么把新的newfd托管给poll?让poll帮我去关心newfd上面的读事件呢？把newfd，添加到辅助数组即可！
            int pos = -1;
            for (int j = 0; j < MAX; j++)
            {
                if (_fds[j].fd == gdefaultfd)
                {
                    pos = j;
                    break;
                }
            }
            if (pos == -1)
            {
                // _fds进行自动扩容
                LOG(LogLevel::ERROR) << "服务器已经满了...";
                close(newfd);
            }
            else
            {
                _fds[pos].fd = newfd;
                _fds[pos].events = POLLIN;
            }
        }
    }
    void Recver(int who) // 回调函数？
    {
        // 合法的，就绪的，普通的fd
        // 这里的recv，对不对啊！不完善！必须得有协议！
        char buffer[1024];
        ssize_t n = recv(_fds[who].fd, buffer, sizeof(buffer) - 1, 0); // 会不会被阻塞？就绪了
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client# " << buffer << std::endl;
            // 把读到的信息，在回显会去
            std::string message = "echo# ";
            message += buffer;
            send(_fds[who].fd, message.c_str(), message.size(), 0); // bug
        }
        else if (n == 0)
        {
            LOG(LogLevel::DEBUG) << "客户端退出, sockfd: " << _fds[who].fd;
            close(_fds[who].fd);
            _fds[who].fd = gdefaultfd;
            _fds[who].events = _fds[who].revents = 0;
        }
        else
        {
            LOG(LogLevel::DEBUG) << "客户端读取出错, sockfd: " <<_fds[who].fd;
            close(_fds[who].fd);
            _fds[who].fd = gdefaultfd;
            _fds[who].events = _fds[who].revents = 0;
        }
    }
    void Dispatcher() // rfds就可能会有更多的fd就绪了，就不仅仅 是listenfd就绪了
    {
        for (int i = 0; i < MAX; i++)
        {
            if (_fds[i].fd == gdefaultfd)
                continue;
            // 文件描述符，先得是合法的
            if (_fds[i].fd == _listen_socket->Fd()) // 该fd是listenfd
            {
                if (_fds[i].revents & POLLIN)
                {
                    Accepter(); // 连接的获取
                }
            }
            else
            {
                if (_fds[i].revents & POLLIN)
                {
                    Recver(i); // IO的处理
                }
                // else if(_fds[i].revents & POLLOUT)
                // {
                //     // wirte
                // }
            }
        }
    }
    void TestFd()
    {
        std::cout << "pollfd: ";
        for(int i = 0; i < MAX; i++)
        {
            if(_fds[i].fd == gdefaultfd)
                continue;
            std::cout << _fds[i].fd << "[" << Events2Str(_fds[i].events)  << "] ";
        }
        std::cout << "\n";
    }
    std::string Events2Str(short events)
    {
        std::string s = ((events & POLLIN) ? "EPOLLIN" : "");
        s += ((events & POLLOUT) ? "POLLOUT" : "");
        return s;
    }
    ~PollServer()
    {
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listen_socket;
    bool _isrunning;
    struct pollfd _fds[MAX];
    // struct pollfd *_fds; // malloc
};
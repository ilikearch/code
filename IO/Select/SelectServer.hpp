#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <sys/select.h>
#include "Log.hpp"
#include "Socket.hpp"

using namespace SocketModule;
using namespace LogModule;

#define NUM sizeof(fd_set) * 8

const int gdefaultfd = -1;

// 最开始的时候，tcpserver只有一个listensockfd
class SelectServer
{
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listen_socket;
    bool _isrunning;
    int _fd_array[NUM]; // 辅助数组

public:
    SelectServer(int port)
        : _port(port),
          _listen_socket(std::make_unique<TcpSocket>()),
          _isrunning(false)
    {
    }
    void Init()
    {
        _listen_socket->BuildTcpSocketMethod(_port);
        for (int i = 0; i < NUM; i++)
        {
            _fd_array[i] = gdefaultfd;
        }
        // 添加listensockfd到数组中
        _fd_array[0] = _listen_socket->Fd();
    }
    void Loop()
    {
        fd_set rfds; // 读文件描述符集
        _isrunning = true;
        while (_isrunning)
        {
            // 清空rfds
            FD_ZERO(&rfds);
            struct timeval timeout = {10, 0};
            int maxfd = gdefaultfd;
            for (int i = 0; i < NUM; i++)
            {
                if (_fd_array[i] == gdefaultfd)
                    continue;
                // 合法fd
                FD_SET(_fd_array[i], &rfds); // 包含listensockfd
                // 更新最大值
                if (maxfd < _fd_array[i])
                {
                    maxfd = _fd_array[i];
                }
            }
            // 我们不能让accept来阻塞检测新连接到来，而应该让select来负责进行就绪事件的检测
            // 用户告诉内核，你要帮我关心&rfds，读事件啊！！
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, &timeout);
            switch (n)
            {
            case 0:
                std::cout << "time out..." << std::endl;
                break;
            case -1:
                perror("select");
                break;
            default:
                // 有事件就绪了
                // rfds: 内核告诉用户，你关心的rfds中的fd，有哪些已经就绪了！！
                std::cout << "有事件就绪啦..., timeout: " << timeout.tv_sec << ":" << timeout.tv_usec << std::endl;
                Dispatcher(rfds); // 把已经就绪的sockfd，派发给指定的模块
                break;
            }
        }
        _isrunning = false;
    }
    void Dispatcher(fd_set &rfds)
    {
        for (int i = 0; i < NUM; i++)
        {
            if (_fd_array[i] == gdefaultfd)
                continue;
            // 文件描述符，先得是合法的
            if (_fd_array[i] == _listen_socket->Fd())
            {
                if (FD_ISSET(_fd_array[i], &rfds))
                {
                    Accepter(); // 连接的获取
                }
            }
            else
            {
                if (FD_ISSET(_fd_array[i], &rfds))
                {
                    Recver(i); // IO的处理
                }
            }
        }
    }
    void Accepter()
    {
        InetAddr client;
        // listensockfd就绪了！获取新连接不就好了吗？
        auto sockptr = _listen_socket->Accepter(&client);
        int newfd = sockptr->Fd();
        if (newfd < 0)
            return;
        else
        {
            std::cout << "获得了一个新的链接: " << newfd << " client info: " << client.Addr() << std::endl;
            // recv()?? 读事件是否就绪，我们并不清楚！newfd也托管给select，让select帮我进行关心新的sockfd上面的读事件就绪
            // 怎么把新的newfd托管给select?让select帮我去关心newfd上面的读事件呢？把newfd，添加到辅助数组即可！
            int pos = -1;
            for (int j = 0; j < NUM; j++)
            {
                if (_fd_array[j] == gdefaultfd)
                {
                    pos = j;
                    break;
                }
            }
            if (pos == -1)
            {
                LOG(LogLevel::ERROR) << "服务器已经满了...";
                close(newfd);
            }
            else
            {
                _fd_array[pos] = newfd;
            }
        }
    }
    void Recver(int who) // 回调函数？
    {
        // 合法的，就绪的，普通的fd
        // 这里的recv，对不对啊！不完善！必须得有协议！
        char buffer[1024];
        ssize_t n = recv(_fd_array[who], buffer, sizeof(buffer) - 1, 0); // 会不会被阻塞？就绪了
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client# " << buffer << std::endl;
            // 把读到的信息，在回显回去
            std::string message = "echo# ";
            message += buffer;
            send(_fd_array[who], message.c_str(), message.size(), 0); // bug
        }
        else if (n == 0)
        {
            LOG(LogLevel::DEBUG) << "客户端退出, sockfd: " << _fd_array[who];
            close(_fd_array[who]);
            _fd_array[who] = gdefaultfd;
        }
        else
        {
            LOG(LogLevel::DEBUG) << "客户端读取出错, sockfd: " << _fd_array[who];
            close(_fd_array[who]);
            _fd_array[who] = gdefaultfd;
        }
    }
};
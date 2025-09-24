#ifndef __UDP_SERVER_HPP__
#define __UDP_SERVER_HPP__

#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <cerrno>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>

#include "InetAddr.hpp"
#include "Log.hpp"
#include "Common.hpp"
#include "ThreadPool.hpp"

using namespace LogModule;
using namespace ThreadPoolModule;

const static int gsockfd = -1;
// const static std::string gdefaultip = "127.0.0.1"; // 表示本地主机
const static uint16_t gdefaultport = 8080;

using adduser_t = std::function<void(InetAddr &id)>;
using remove_t = std::function<void(InetAddr &id)>;

using task_t = std::function<void()>;
using route_t = std::function<void(int sockfd, const std::string &message)>;

class nocopy
{
public:
    nocopy(){}
    nocopy(const nocopy &) = delete;
    const nocopy& operator = (const nocopy &) = delete;
    ~nocopy(){}
};

class UdpServer : public nocopy
{
public:
    UdpServer(uint16_t port = gdefaultport)
        : _sockfd(gsockfd),
          _addr(port),
          _isrunning(false)
    {
    }
    // 都是套路
    void InitServer()
    {
        // 1. 创建socket
        _sockfd = ::socket(AF_INET, SOCK_DGRAM, 0); // IP?PORT?网络？本地？
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket: " << strerror(errno);
            Die(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket success, sockfd is : " << _sockfd;

        // 2. 填充网络信息，并bind绑定
        // 2.1 有没有把socket信息，设置进入内核中？？没有，只是填充了结构体！
        // struct sockaddr_in local;
        // bzero(&local, sizeof(local));
        // local.sin_family = AF_INET;
        // local.sin_port = ::htons(_port);                  // 要被发送给对方的，即要发到网络中！
        // //local.sin_addr.s_addr = ::inet_addr(_ip.c_str()); // 1. string ip->4bytes 2. network order //TODO
        // local.sin_addr.s_addr = INADDR_ANY;

        // 2.2 bind ： 设置进入内核中
        int n = ::bind(_sockfd, _addr.NetAddr(), _addr.NetAddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind: " << strerror(errno);
            Die(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success";
    }
    void RegisterService(adduser_t adduser, route_t route, remove_t removeuser)
    {
        _adduser = adduser;
        _route = route;
        _removeuser = removeuser;
    }
    void Start()
    {
        _isrunning = true;
        while (true)
        {
            char inbuffer[1024]; // string
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer); // 必须设定

            ssize_t n = ::recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, CONV(&peer), &len);
            if (n > 0)
            {
                // 1. 消息内容 && 2. 谁发给我的
                // uint16_t clientport = ::ntohs(peer.sin_port);
                // std::string clientip = ::inet_ntoa(peer.sin_addr);

                InetAddr cli(peer);
                inbuffer[n] = 0;
                std::string messages;
                if (strcmp(inbuffer, "QUIT") == 0)
                {
                    // 移除观察者
                    _removeuser(cli);
                    messages = cli.Addr() + "# " + "我走了，你们聊!";
                }
                else
                {
                    // 2. 新增用户
                    _adduser(cli);
                    messages = cli.Addr() + "# " + inbuffer;
                }

                // 3. 构建转发任务，推送给线程池，让线程池进行转发
                task_t task = std::bind(UdpServer::_route, _sockfd, messages);
                ThreadPool<task_t>::getInstance()->Equeue(task);

                // std::string clientinfo = cli.Ip() + ":" + std::to_string(cli.Port()) + " # " + inbuffer;

                // LOG(LogLevel::DEBUG) << clientinfo;

                // std::string echo_string = "echo# ";
                // echo_string += inbuffer;

                // ::sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, CONV(&peer), sizeof(peer));
            }
        }
        _isrunning = false;
    }
    ~UdpServer()
    {
        if (_sockfd > gsockfd)
            ::close(_sockfd);
    }

private:
    int _sockfd;
    InetAddr _addr;
    bool _isrunning; // 服务器运行状态

    // 新增用户
    adduser_t _adduser;
    // 移除用户
    remove_t _removeuser;
    // 数据转发
    route_t _route;
};

#endif
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

#include "InetAddr.hpp"
#include "Log.hpp"
#include "Common.hpp"

using namespace LogModule;

const static int gsockfd = -1;
// const static std::string gdefaultip = "127.0.0.1"; // 表示本地主机
const static uint16_t gdefaultport = 8080;

class UdpServer
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

                std::string clientinfo = cli.Ip() + ":" + std::to_string(cli.Port()) + " # " + inbuffer;

                LOG(LogLevel::DEBUG) << clientinfo;

                std::string echo_string = "echo# ";
                echo_string += inbuffer;

                ::sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, CONV(&peer), sizeof(peer));
            }
        }
        _isrunning = false;
    }
    ~UdpServer()
    {
        if(_sockfd > gsockfd)
            ::close(_sockfd);
    }

private:
    int _sockfd;
    InetAddr _addr;
    // uint16_t _port;  // 服务器未来的端口号
    // // std::string _ip; // 服务器所对应的IP
    bool _isrunning; // 服务器运行状态
};

#endif
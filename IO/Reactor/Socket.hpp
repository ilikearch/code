#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"

namespace SocketModule
{
    using namespace LogModule;
    class Socket;

    using SockPtr = std::shared_ptr<Socket>;
    // 基类，规定创建socket的方法
    // 提供一个/若干个/固定模式的socket的方法
    // 模版方法模式
    class Socket
    {
    public:
        virtual ~Socket() = default;
        virtual void SocketOrDie() = 0;
        virtual void SetSocketOpt() = 0;
        virtual bool BindOrDie(int port) = 0;
        virtual bool ListenOrDie() = 0;
        virtual int Accepter(InetAddr *client, int *out_errno) = 0;
        virtual void Close() = 0;
        virtual int Recv(std::string *out) = 0;
        virtual int Send(const std::string &in) = 0;
        virtual int Fd() = 0;
        // 提供一个创建listensockfd的固定讨论
        void BuildTcpSocketMethod(int port)
        {
            SocketOrDie();
            SetSocketOpt();
            BindOrDie(port);
            ListenOrDie();
        }
        // 其他方法，需要的时候，在加
        // #ifdef WIN
        //     // 提供一个创建listensockfd的固定讨论
        //     void BuildTcpSocket()
        //     {
        //         SocketOrDie();
        //         SetSocketOpt();
        //         BindOrDie();
        //         ListenOrDie();
        //     }

        //     // 提供一个创建udpsockfd的固定讨论
        //     void BuildUdpSocket()
        //     {
        //     }
        // #else // Linux
        //     // 提供一个创建listensockfd的固定讨论
        //     void BuildTcpSocket()
        //     {
        //         SocketOrDie();
        //         SetSocketOpt();
        //         BindOrDie();
        //         ListenOrDie();
        //     }

        //     // 提供一个创建udpsockfd的固定讨论
        //     void BuildUdpSocket()
        //     {
        //     }
        // #endif
    };

    // class UdpSocket : public Socket
    // {
    // public:
    //     virtual ~TcpSocket()
    //     {}
    //     virtual int SocketOrDie() = 0;
    //     virtual void SetSocketOpt() = 0;
    //     virtual bool BindOrDie(int listensockfd) = 0;
    //     virtual bool ListenOrDie(int listensockfd) = 0;
    //     virtual int Accepter(int listensockfd) = 0;
    //     virtual void Close(int fd) = 0;

    // private:
    // };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket() : _sockfd(gdefaultsockfd)
        {
        }
        TcpSocket(int sockfd) : _sockfd(sockfd)
        {
        }
        virtual ~TcpSocket()
        {
        }
        virtual void SocketOrDie() override
        {
            _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::ERROR) << "socket error";
                exit(SOCKET_ERR);
            }
            SetNonBlock(_sockfd);
            LOG(LogLevel::DEBUG) << "socket create success: " << _sockfd;

        }
        virtual void SetSocketOpt() override
        {
            // 保证我们的服务器，异常断开之后，可以立即重启，不会有bind问题
            int opt = 1;
            int n = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,  &opt, sizeof(opt));
            (void)n;
        }
        virtual bool BindOrDie(int port) override
        {
            if (_sockfd == gdefaultsockfd)
                return false;
            InetAddr addr(port);
            int n = ::bind(_sockfd, addr.NetAddr(), addr.NetAddrLen());
            if (n < 0)
            {
                LOG(LogLevel::ERROR) << "bind error";
                exit(SOCKET_ERR);
            }
            LOG(LogLevel::DEBUG) << "bind create success: " << _sockfd;
            return true;
        }
        virtual bool ListenOrDie() override
        {
            if (_sockfd == gdefaultsockfd)
                return false;
            int n = ::listen(_sockfd, gbacklog);
            if (n < 0)
            {
                LOG(LogLevel::ERROR) << "listen error";
                exit(LISTEN_ERR);
            }
            LOG(LogLevel::DEBUG) << "listen create success: " << _sockfd;
            return true;
        }
        // 1. 文件描述符 2. client info
        int Accepter(InetAddr *client, int *out_errno) override
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int newsockfd = ::accept(_sockfd, CONV(&peer), &len);
            *out_errno = errno;
            if (newsockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept done";
                return -1;
            }
            client->SetAddr(peer, len);
            return newsockfd;
            // return std::make_shared<TcpSocket>(newsockfd);
        }
        virtual void Close() override
        {
            if (_sockfd == gdefaultsockfd)
                return;
            ::close(_sockfd);
        }

        virtual int Recv(std::string *out) override
        {
            char buffer[1024*8];
            auto size = ::recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            if (size > 0)
            {
                buffer[size] = 0;
                *out = buffer;
            }
            return size;
        }
        virtual int Send(const std::string &in) override
        {
            auto size = ::send(_sockfd, in.c_str(), in.size(), 0);
            return size;
        }

        virtual int Fd() override
        {
            return _sockfd;
        }

    private:
        int _sockfd;
    };

    // for test
    // int main()
    // {
    //     Socket *sk = new TcpSocket();
    //     sk->BuildTcpSocket(8080);
    // }
}
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
        virtual SockPtr Accepter(InetAddr *client) = 0;
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

    private:
    };

    class TcpSocket : public Socket
    {
    private:
        int _sockfd;

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
        virtual void SocketOrDie()override
        {
            _sockfd=::socket(AF_INET,SOCK_STREAM,0);
            if(_sockfd<0)
            {
                LOG(LogLevel::ERROR) << "socket error";
                exit(SOCKET_ERR);
            }
             LOG(LogLevel::DEBUG) << "socket create success: " << _sockfd;
        }
        virtual void SetSocketOpt() override
        {
            // 保证我们的服务器，异常断开之后，可以立即重启，不会有bind问题
            int opt = 1;
            int n = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,  &opt, sizeof(opt));
            (void)n;
        }
        virtual bool BindOrDie(int port)override
        {
            if (_sockfd == gdefaultsockfd)
                return false;
            InetAddr addr(port);
            int n=::bind(_sockfd,addr.NetAddr(),addr.NetAddrLen());
            if (n < 0)
            {
                LOG(LogLevel::ERROR) << "bind error";
                exit(SOCKET_ERR);
            }
            LOG(LogLevel::DEBUG) << "bind create success: " << _sockfd;
            return true;
        }
        virtual bool ListenOrDie()override
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
        virtual SockPtr Accepter(InetAddr *client) override
        {
            if (!client)
                return nullptr;
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int newsockfd = ::accept(_sockfd, CONV(&peer), &len);
            if (newsockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                return nullptr;
            }
            client->SetAddr(peer, len);
            return std::make_shared<TcpSocket>(newsockfd);
        }
        virtual void Close() override
        {
            if (_sockfd == gdefaultsockfd)
                return;
            ::close(_sockfd);
        }
        virtual int Fd() override
        {
            return _sockfd;
        }
        virtual int Recv(std::string *out)override
        {
            char buffer[1024*8];
            auto size=::recv(_sockfd,buffer,sizeof(buffer)-1,0);
            if(size>0)
            {
                buffer[size]=0;
                *out=buffer;
            }
            return size;
        }
        virtual int Send(const std::string &in) override
        {
            auto size=::send(_sockfd,in.c_str(),in.size(),0);
            return size;
        }
    };
}
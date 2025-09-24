#pragma once

#include <iostream>
#include <memory>
#include <functional>
#include <sys/wait.h>
#include "Socket.hpp"

namespace TcpServerModule
{
    using namespace SocketModule;
    using namespace LogModule;
    using tcphandler_t = std::function<bool(SockPtr, InetAddr)>;

    class TcpServer
    {
    private:
        // 一定要有一个Listensock
        std::unique_ptr<Socket> _listensockp;
        bool _running;
        tcphandler_t _handler;
        int _port;

    public:
        TcpServer(int port)
            : _listensockp(std::make_unique<TcpSocket>()),
              _running(false),
              _port(port)
        {
        }
        void InitServer(tcphandler_t handler)
        {
            _handler = handler;
            _listensockp->BuildTcpSocketMethod(_port);
        }
        void Loop()
        {
            _running = true;
            while (_running)
            {
                // 1.accept
                InetAddr clientaddr;
                auto sockfd = _listensockp->Accepter(&clientaddr);
                if (sockfd == nullptr)
                    continue;
                // io处理
                LOG(LogLevel::DEBUG) << "get a new client, info is: " << clientaddr.Addr();
                // sockfd->Recv();
                // sockfd->Send();
                pid_t id = fork();
                if (id == 0)
                {
                    _listensockp->Close();
                    if (fork() > 0)
                        exit(0);
                    _handler(sockfd, clientaddr);
                    exit(0);
                }
                sockfd->Close();
                waitpid(id, nullptr, 0);
            }
            _running = false;
        }
        ~TcpServer() { _listensockp->Close(); }
    };
}
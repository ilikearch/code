#pragma once

#include <iostream>
#include <sys/epoll.h>
#include "Log.hpp"
#include "Common.hpp"

using namespace LogModule;

namespace EpollModule
{
    class Epoller
    {
    public:
        Epoller() : _epfd(-1)
        {
        }
        void Init()
        {
            _epfd = epoll_create(256);
            if (_epfd < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_create error";
                exit(EPOLL_CREATE_ERR);
            }
            LOG(LogLevel::INFO) << "epoll_create success, epfd: " << _epfd;
        }
        int Wait(struct epoll_event revs[], int num, int timeout)
        {
            int n = epoll_wait(_epfd, revs, num, timeout);
            if (n < 0)
            {
                LOG(LogLevel::WARNING) << "epoll_wait error";
            }
            return n;
        }
        void Ctrl(int sockfd, uint32_t events, int flag)
        {
            struct epoll_event ev;
            ev.events = events;
            ev.data.fd = sockfd;
            int n = epoll_ctl(_epfd, flag, sockfd, &ev);
            if (n < 0)
            {
                LOG(LogLevel::WARNING) << "epoll_ctl error";
            }
        }
        void Add(int sockfd, uint32_t events)
        {
            Ctrl(sockfd, events, EPOLL_CTL_ADD);
        }
        void Update(int sockfd, uint32_t events)
        {
            Ctrl(sockfd, events, EPOLL_CTL_MOD);
        }
        void Delete(int sockfd)
        {
            int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
            if (n < 0)
            {
                LOG(LogLevel::WARNING) << "epoll_ctl error";
            }
        }
        ~Epoller()
        {
        }

    private:
        int _epfd;
    };
}
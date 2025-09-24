#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include <iostream>
#include <string>
#include <unistd.h>

class Channel{
    public:
    Channel(int wfd, pid_t who) : _wfd(wfd), _who(who)
    {
        // Channel-3-1234
        _name = "Channel-" + std::to_string(wfd) + "-" + std::to_string(who);
    }
    std::string Name()
    {
        return _name;
    }
    void Send(int cmd)
    {
        ::write(_wfd, &cmd, sizeof(cmd));
    }
    void Close()
    {
        ::close(_wfd);
    }
    pid_t Id()
    {
        return _who;
    }
    int wFd()
    {
        return _wfd;
    }
    ~Channel()
    {
    }

private:
    int _wfd;
    std::string _name;
    pid_t _who;
};
#endif
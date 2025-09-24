#pragma once
#include <iostream>
#include "Common.hpp"

class Server
{
public:
    Server() : _fd(gdefultfd)
    {
    }
    bool OpenPipeForRead()
    {
        _fd = OpenPipe(gForRead);
        if (_fd < 0)
            return false;
        return true;
    }
    // std::string *: 输出型参数
    // const std::string &: 输入型参数
    // std::string &: 输入输出型参数
    int RecvPipe(std::string *out)
    {
        char buffer[gsize];
        ssize_t n = ::read(_fd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            *out = buffer;
        }
        return n;
    }

    void ClosePipe()
    {
        ClosePipeHelper(_fd);
    }
    ~Server() {}

private:
    int _fd;
};
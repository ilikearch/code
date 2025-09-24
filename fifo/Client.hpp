#pragma once
#include <iostream>
#include "Common.hpp"

class Init
{
public:
    Init()
    {
        umask(0);
        int n = ::mkfifo(gpipeFile.c_str(), gmode);
        if (n < 0)
        {
            std::cerr << "mkfifo error" << std::endl;
            return;
        }
        std::cout << "mkfifo success" << std::endl;

        // sleep(10);
    }
    ~Init()
    {
        int n = ::unlink(gpipeFile.c_str());
        if (n < 0)
        {
            std::cerr << "unlink error" << std::endl;
            return;
        }
        std::cout << "unlink success" << std::endl;
    }
};

Init init;

class Client
{
public:
    Client() : _fd(gdefultfd)
    {
    }
    bool OpenPipeForWrite()
    {
        _fd = OpenPipe(gForWrite);
        if (_fd < 0)
            return false;
        return true;
    }
    // std::string *: 输出型参数
    // const std::string &: 输入型参数
    // std::string &: 输入输出型参数
    int SendPipe(const std::string &in)
    {
        return ::write(_fd, in.c_str(), in.size());
    }

    void ClosePipe()
    {
        ClosePipeHelper(_fd);
    }
    ~Client() {}

private:
    int _fd;
};
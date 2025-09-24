#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// 模拟进程间同步的过程！

const std::string gpipeFile = "./fifo";
const mode_t gfifomode = 0600;
const int gdefultfd = -1;
const int gsize = 1024;
const int gForRead = O_RDONLY;
const int gForWrite = O_WRONLY;

class Fifo
{
private:
    void OpenFifo(int flag)
    {
        // 如果读端打开文件时，写端还没打开，读端对用的open就会阻塞
        _fd = ::open(gpipeFile.c_str(), flag);
        if (_fd < 0)
        {
            std::cerr << "open error" << std::endl;
        }
    }

public:
    Fifo() : _fd(-1)
    {
        umask(0);
        int n = ::mkfifo(gpipeFile.c_str(), gfifomode);
        if (n < 0)
        {
            // std::cerr << "mkfifo error" << std::endl;
            return;
        }
        std::cout << "mkfifo success" << std::endl;
    }
    bool OpenPipeForWrite()
    {
        OpenFifo(gForWrite);
        if (_fd < 0)
            return false;
        return true;
    }
    bool OpenPipeForRead()
    {
        OpenFifo(gForRead);
        if (_fd < 0)
            return false;
        return true;
    }

    int Wait()
    {
        int code = 0;
        ssize_t n = ::read(_fd, &code, sizeof(code));
        if (n == sizeof(code))
        {
            return 0;
        }
        else if (n == 0)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    void Signal()
    {
        int code = 1;
        ::write(_fd, &code, sizeof(code));
    }

    ~Fifo()
    {
        if (_fd >= 0)
            ::close(_fd);
        int n = ::unlink(gpipeFile.c_str());
        if (n < 0)
        {
            std::cerr << "unlink error" << std::endl;
            return;
        }
        std::cout << "unlink success" << std::endl;
    }

private:
    int _fd;
};

Fifo gpipe;
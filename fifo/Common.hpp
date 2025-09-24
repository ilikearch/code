#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const std::string gpipeFile = "./fifo";
const mode_t gmode = 0600;
const int gdefultfd = -1;
const int gsize = 1024;
const int gForRead = O_RDONLY;
const int gForWrite = O_WRONLY;

int OpenPipe(int flag)
{
    // 如果读端打开文件时，写端还没打开，读端对应的open就会阻塞
    int fd = ::open(gpipeFile.c_str(), flag);
    if (fd < 0)
    {
        std::cerr << "open error" << std::endl;
    }
    return fd;
}

void ClosePipeHelper(int fd)
{
    if (fd >= 0)
        ::close(fd);
}
#pragma once

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ROOT "/"
#define devnull "/dev/null"

void Daemon(bool ischdir, bool isclose)
{
    // 1. 守护进程一般要屏蔽到特定的异常信号
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    // 2. 成为非组长
    if (fork() > 0)
        exit(0);

    // 3. 建立新会话
    setsid();

    // 4. 每一个进程都有自己的CWD，是否将当前进程的CWD更改成为 / 根目录
    if (ischdir)
        chdir(ROOT);

    // 5. 已经变成守护进程啦，不需要和用户的输入输出，错误进行关联了
    if (isclose)
    {
        ::close(0);
        ::close(1);
        ::close(2);
    }
    else
    {
        int fd = ::open(devnull, O_WRONLY);
        if (fd > 0)
        {
            // 各种重定向
            dup2(fd, 0);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }
    }
}
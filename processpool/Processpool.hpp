#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <functional>
#include "Task.hpp"
#include "Channel.hpp"

using work_t = std::function<void()>;

enum
{
    OK = 0,
    UsageError,
    PipeError,
    ForkError
};

class ProcessPool
{
public:
    ProcessPool(int n, work_t w)
        : processnum(n), work(w)
    {
    }
    int InitProcessPool()
    {
        // 创建指定个数进程
        for (int i = 0; i < processnum; ++i)
        {
            // 1.先有管道
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0)
                return PipeError;
            // 2.创建进程
            pid_t id = fork();
            if (id < 0)
                return ForkError;

            // 3.建立通信管道
            if (id == 0)
            {
                // 关闭历史wfd
                std::cout << getpid() << ", child close history fd: ";
                for (auto &c : channels)
                {
                    std::cout << c.wFd() << " ";
                    c.Close();
                }
                std::cout << " over" << std::endl;

                ::close(pipefd[1]); // read
                // child
                std::cout << "debug: " << pipefd[0] << std::endl;
                dup2(pipefd[0], 0);
                work();
                ::exit(0);
            }

            // 父进程执行
            ::close(pipefd[0]); // write
            channels.emplace_back(pipefd[1], id);
            // Channel ch(pipefd[1], id);
            // channels.push_back(ch);
        }
        return OK;
    }
    void DispathTask()
    {
        int who = 0;
        int num = 20;
        while (num--)
        {
            // 选择任务
            int task = tm.SelectTask();
            // 选择一个子线程channel
            Channel &cur = channels[who++];
            who %= channels.size();

            std::cout << "######################" << std::endl;
            std::cout << "send " << task << " to " << cur.Name() << ", 任务还剩: " << num << std::endl;
            std::cout << "######################" << std::endl;
            // 派发任务

            cur.Send(task);

            sleep(1);
        }
    }

    void CleanProcessPool()
    {
        // version 3
        for (auto &c : channels)
        {
            c.Close();
            pid_t rid = ::waitpid(c.Id(), nullptr, 0);
            if (rid > 0)
            {
                std::cout << "child " << rid << " wait ... success" << std::endl;
            }
        }
    }

private:
    std::vector<Channel> channels;
    int processnum;
    work_t work;
};
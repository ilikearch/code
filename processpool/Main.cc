#include "Processpool.hpp"
#include "Task.hpp"
void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " process-num" << std::endl;
}

// 我们自己就是master
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        return UsageError;
    }
    int num = std::stoi(argv[1]);
    ProcessPool *pp = new ProcessPool(num, Worker);
    // 1. 初始化进程池
    pp->InitProcessPool();
    // 2. 派发任务
    pp->DispathTask();
    // 3. 退出进程池
    pp->CleanProcessPool();

    // std::vector<Channel> channels;
    // // 1. 初始化进程池
    // InitProcessPool(num, channels, Worker);

    // // 2. 派发任务
    // DispatchTask(channels);

    // // 3. 退出进程池
    // CleanProcessPool(channels);

    delete pp;
    return 0;
}
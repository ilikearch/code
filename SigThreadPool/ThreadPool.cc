#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>

using namespace ThreadPoolModule;

int main()
{
    ENABLE_CONSOLE_LOG();
    ThreadPool<task_t>::getInstance()->Start();
    char c;
    int cnt = 5;
    while (cnt)
    {
        // std::cin >> c;
        ThreadPool<task_t>::getInstance()->Equeue(Push);
        cnt--;
        sleep(1);
    }

    ThreadPool<task_t>::getInstance()->Stop();
    ThreadPool<task_t>::getInstance()->Wait();





    // ENABLE_FILE_LOG();

    // std::unique_ptr<ThreadPool<task_t>> tp = std::make_unique<ThreadPool<task_t>>();
    // tp->Start();

    // int cnt = 10;
    // char c;
    // while (true)
    // {
    //     std::cin >> c;
    //     tp->Equeue(Push);
    //     // cnt--;
    //     // sleep(1);
    // }

    // tp->Stop();

    // sleep(3);

    // tp->Wait();

    return 0;
}

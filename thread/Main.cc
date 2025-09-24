#include "Thread.hpp"
#include <unordered_map>
#include <memory>

#define NUM 10

// using thread_ptr_t = std::shared_ptr<ThreadModule::Thread>;

class threadData
{
public:
    int max;
    int start;
};

void Count(threadData td)
{
    for (int i = td.start; i < td.max; i++)
    {
        std::cout << "i == " << i << std::endl;
        sleep(1);
    }
}

int main()
{
    threadData td;
    td.max = 60;
    td.start = 50;
    ThreadModule::Thread<threadData> t(Count, td);
    // ThreadModule::Thread<int> t(Count, 10);

    t.Start();

    t.Join();

    // 先描述，在组织！
    // std::unordered_map<std::string, thread_ptr_t> threads;
    // // 如果我要创建多线程呢？？？
    // for (int i = 0; i < NUM; i++)
    // {
    //     thread_ptr_t t = std::make_shared<ThreadModule::Thread>([](){
    //         while(true)
    //         {
    //             std::cout << "hello world" << std::endl;
    //             sleep(1);
    //         }
    //     });
    //     threads[t->Name()] = t;
    // }

    // for(auto &thread:threads)
    // {
    //     thread.second->Start();
    // }

    // for(auto &thread:threads)
    // {
    //     thread.second->Join();
    // }

    // ThreadModule::Thread t([](){
    //     while(true)
    //     {
    //         std::cout << "hello world" << std::endl;
    //         sleep(1);
    //     }
    // });

    // t.Start();
    // std::cout << t.Name() << "is running" << std::endl;
    // sleep(5);

    // t.Stop();
    // std::cout << "Stop thread : " << t.Name()<< std::endl;
    // sleep(1);
    // t.Join();
    // std::cout << "Join thread : " << t.Name()<< std::endl;

    return 0;
}
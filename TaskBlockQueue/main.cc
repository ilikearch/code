#include "BlockQueue.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <functional>
#include "Task.hpp"

void test()
{
    std::cout << "haha test..." << std::endl;
}
void hello()
{
    std::cout << "hehe hello" << std::endl;
} // 长传，下载，刷新，入库，同步等各种

using task_t = std::function<void()>;
using namespace BlockQueueModule;
using namespace TaskModule;

void *Consumer(void *args)
{
    // BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    // BlockQueue<task_t> *bq = static_cast<BlockQueue<task_t> *>(args);
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    while (true)
    {
        // task_t t;
        Task t;
        // 1. 从bq拿到数据
        bq->Pop(&t);

        // 2.做处理
        // t();
        t.Excute();
        printf("Consumer, 处理完成一个任务: %d+%d=%d\n", t.X(), t.Y(), t.Result());
        // printf("Consumer, 消费了一个数据: %d\n", data);
    }
}

void *Productor(void *args)
{
    // BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    // BlockQueue<task_t> *bq = static_cast<BlockQueue<task_t> *>(args);
    // int data = 10;
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);

    while (true)
    {
        sleep(1);
        // 1. 从外部获取数据

        int x = rand() % 10 + 1; // [1, 10]
        int y = rand() % 20 + 1; // [1, 20]
        Task t(x, y);            // 构建任务
        // data = 10; // 有数据？？？

        // 2. 生产到bq中
        // bq->Equeue(test);
        bq->Equeue(t);
        printf("producter 生产了一个任务\n");
        // printf("producter 生产了一个数据: %d\n", data);
        // data++;
    }
}

int main()
{
    srand(time(nullptr) ^ getpid());
    // 交易场所，不仅仅可以用来进行传递数据
    // 传递任务！！！v1: 对象 v2
    // BlockQueue<task_t> *bq = new BlockQueue<task_t>(5); // 共享资源 -> 临界资源
    BlockQueue<Task> *bq = new BlockQueue<Task>(5);
    // 单生产，单消费
    pthread_t c1, p1; //,c2, , p2, p3;
    pthread_create(&c1, nullptr, Consumer, bq);
    // pthread_create(&c2, nullptr, Consumer, bq);
    pthread_create(&p1, nullptr, Productor, bq);
    // pthread_create(&p2, nullptr, Productor, bq);
    // pthread_create(&p3, nullptr, Productor, bq);

    pthread_join(c1, nullptr);
    // pthread_join(c2, nullptr);
    pthread_join(p1, nullptr);
    // pthread_join(p2, nullptr);
    // pthread_join(p3, nullptr);

    delete bq;

    return 0;
}
#include "BlockQueue.hpp"
#include <pthread.h>
#include <unistd.h>

using namespace BlockQueueModule;

void *Consumer(void *args)
{
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    while(true)
    {
        int data;
        // 1. 从bq拿到数据
        bq->Pop(&data);
        
        // 2.做处理
        printf("Consumer, 消费了一个数据: %d\n", data);
    }
}

void *Productor(void *args)
{
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    int data = 10;
    while (true)
    {
        sleep(2);
        // 1. 从外部获取数据
        // data = 10; // 有数据？？？

        // 2. 生产到bq中
        bq->Equeue(data);

        printf("producter 生产了一个数据: %d\n", data);
        data++;
    }
}

int main()
{
    // 交易场所，不仅仅可以用来进行传递数据
    // 传递任务！！！v1: 对象 v2
    BlockQueue<int> *bq = new BlockQueue<int>(5); // 共享资源 -> 临界资源
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
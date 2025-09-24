#include "RingBuffer.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime>

using namespace RingBufferModule;

void *Consumer(void *args)
{
    RingBuffer<int> *ring_buffer = static_cast<RingBuffer<int> *>(args);
    while(true)
    {
        sleep(1);
        // sleep(1);
        // 1. 消费数据
        int data;
        ring_buffer->Pop(&data);

        // 2. 处理：花时间
        std::cout << "消费了一个数据: " << data << std::endl;
    }
}

void *Productor(void *args)
{
    RingBuffer<int> *ring_buffer = static_cast<RingBuffer<int> *>(args);
    int data = 0;
    while (true)
    {
        // 1. 获取数据：花时间
        // sleep(1);

        // 2. 生产数据
        ring_buffer->Equeue(data);
        std::cout << "生产了一个数据: " << data << std::endl;
        data++;
    }
}

int main()
{
    RingBuffer<int> *ring_buffer = new RingBuffer<int>(5); // 共享资源 -> 临界资源
    // 单生产，单消费
    pthread_t c1, p1, c2,c3,p2;
    pthread_create(&c1, nullptr, Consumer, ring_buffer);
    pthread_create(&c2, nullptr, Consumer, ring_buffer);
    pthread_create(&c3, nullptr, Consumer, ring_buffer);
    pthread_create(&p1, nullptr, Productor, ring_buffer);
    pthread_create(&p2, nullptr, Productor, ring_buffer);


    pthread_join(c1, nullptr);
    pthread_join(c2, nullptr);
    pthread_join(c3, nullptr);
    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);


    delete ring_buffer;

    return 0;
}
#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>
#include "Mutex.hpp"
#include "Cond.hpp"

namespace BlockQueueModule
{
    using namespace LockModle;
    using namespace CondModule;
    // version 2
    static const int gcap = 10;

    template <typename T>
    class BlockQueue
    {
    private:
        bool IsFull() { return _q.size() == _cap; }
        bool IsEmpty() { return _q.empty(); }

    public:
        BlockQueue(int cap = gcap) : _cap(cap), _cwait_num(0), _pwait_num(0)
        {
        }
        void Equeue(const T &in) // 生产者
        {
            LockGuard lockguard(_mutex);
            // 你想放数据，就能放数据吗？？生产数据是有条件的！
            // 结论1: 在临界区中等待是必然的(目前)
            while (IsFull()) // 5. 对条件进行判断，为了防止伪唤醒，我们通常使用while进行判断！
            {
                std::cout << "生产者进入等待..." << std::endl;

                // 2. 等是，释放_mutex
                _pwait_num++;
                _productor_cond.Wait(_mutex); // wait的时候，必定是持有锁的！！是有问题的！
                _pwait_num--;
                // 3. 返回，线程被唤醒&&重新申请并持有锁(它会在临界区内醒来！)

                std::cout << "生产者被唤醒..." << std::endl;
            }
            // 4. if(IsFull())不满足 || 线程被唤醒
            _q.push(in); // 生产

            // 肯定有数据！
            if (_cwait_num)
            {
                std::cout << "叫醒消费者" << std::endl;
                _consumer_cond.Notify();
            }
        }
        void Pop(T *out) // 消费者
        {
            LockGuard lockguard(_mutex);
            while (IsEmpty())
            {
                std::cout << "消费者进入等待..." << std::endl;
                _cwait_num++;
                _consumer_cond.Wait(_mutex); // 伪唤醒
                _cwait_num--;
                std::cout << "消费者被唤醒..." << std::endl;
            }
            // 4. if(IsEmpty())不满足 || 线程被唤醒
            *out = _q.front();
            _q.pop();

            // 肯定有空间
            if (_pwait_num)
            {
                std::cout << "叫醒生产者" << std::endl;
                _productor_cond.Notify();
            }
        }
        ~BlockQueue()
        {
        }

    private:
        std::queue<T> _q;     // 保存数据的容器，临界资源
        int _cap;             // bq最大容量
        Mutex _mutex;         // 互斥
        Cond _productor_cond; // 生产者条件变量
        Cond _consumer_cond;  // 消费者条件变量

        int _cwait_num;
        int _pwait_num;
    };
    // v1
    // static const int gcap = 10;

    // template <typename>
    // class BlockQueue
    // {
    // private:
    //     bool Isfull() { return _q.size() == _cap; }
    //     bool Isempty() { return _q.empty(); }

    // public:
    //     BlockQueue(int cap = gcap)
    //         : _cap(cap),
    //           _cwait_num(0),
    //           _pwait_num(0)
    //     {
    //         pthread_mutex_init(&_mutex, nullptr);
    //         pthread_cond_init(&_productor_cond, nullptr);
    //         pthread_cond_init(&_consumer_cond, nullptr);
    //     }

    //     void Equeue(const T &in) // 生产者
    //     {
    //         pthread_mutex_lock(&_mutex);
    //         // 你想放数据，就能放数据吗？？生产数据是有条件的！
    //         // 结论1: 在临界区中等待是必然的(目前)
    //         while (Isfull())
    //         {
    //             std::cout << "生产者进入等待..." << endl;
    //             // 2. 等是，释放_mutex
    //             _pwait_num++;
    //             pthread_cond_wait(&_productor_cond, &_mutex); // wait的时候，必定是持有锁的！！是有问题的！
    //             // 3. 返回，线程被唤醒&&重新申请并持有锁(它会在临界区内醒来！)
    //             _pwait_num--;
    //             std::cout << "生产者被唤醒..." << std::endl;
    //         }
    //         _q.push(in); // 生产
    //         // 肯定有数据

    //         if (_cwait_num)
    //         {
    //             std::cout << "唤醒消费者" << std::endl;
    //             pthread_cond_signal(&_consumer_cond);
    //         }

    //         pthread_mutex_unlock(&_mutex);
    //     }
    //     void Pop(T *out)//消费者
    //     {
    //         pthread_mutex_lock(&_mutex);
    //         while (Isempty())
    //         {
    //             std::cout << "消费者进入等待..." << std::endl;
    //             _cwait_num++;
    //             pthread_cond_wait(&_consumer_cond, nullptr);
    //             _cwait_num--;
    //             std::cout << "消费者被唤醒..." << std::endl;
    //         }
    //         // 4. if(IsEmpty())不满足 || 线程被唤醒
    //         *out = _q.front();
    //         _q.pop();

    //         // 肯定有空间
    //         if (_pwait_num)
    //         {
    //             std::cout << "叫醒生产者" << std::endl;
    //             pthread_cond_signal(&_productor_cond);
    //         }

    //         pthread_mutex_unlock(&_mutex);
    //     }
    //     ~BlockQueue()
    //     {
    //         pthread_mutex_destroy(&_mutex);
    //         pthread_cond_destroy(&_productor_cond);
    //         pthread_cond_destroy(&_consumer_cond);
    //     }

    // private:
    //     std::queue<T> _q;               // 保存数据的容器 临界资源
    //     int _cap;                       // bq最大容量
    //     pthread_mutex_t _mutex;         // 互斥
    //     pthread_cond_t _productor_cond; // 生产者条件变量
    //     pthread_cond_t _consumer_cond;  // 消费者条件变量

    //     int _cwait_num;
    //     int _pwait_num;
    // };
}
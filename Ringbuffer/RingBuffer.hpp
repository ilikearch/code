#pragma once

#include <iostream>
#include <vector>
#include <pthread.h>

#include "Sem.hpp"
#include "Mutex.hpp"

namespace RingBufferModule
{
    using namespace SemModule;
    using namespace LockModule;

    // RingQueue
    template <typename T>
    class RingBuffer
    {
    public:
        RingBuffer(int cap)
            : _ring(cap),
              _cap(cap),
              _p_step(0),
              _c_step(0),
              _datasem(0),
              _spacesem(cap)
        {
        }
        // 为什么没有判断？？信号量，本身就是表示资源数目的，只要成功，就一定有，不需要判断！
        void Equeue(const T &in)
        {
            // 生产者
            _spacesem.P();

            {
                LockGuard lockguard(_p_lock);
                _ring[_p_step] = in; // 生产完毕！
                _p_step++;
                _p_step %= _cap; // 维持唤醒特性
            }

            _datasem.V();
        }
        void Pop(T *out)
        {
            // 消费者

            _datasem.P();

            {
                LockGuard lockguard(_c_lock);
                *out = _ring[_c_step];
                _c_step++;
                _c_step %= _cap;
            }

            _spacesem.V();
        }
        ~RingBuffer()
        {
        }

    private:
        std::vector<T> _ring; // 环， 临界资源
        int _cap;             // 总容量
        int _p_step;          // 生产者位置
        int _c_step;          // 消费位置

        Sem _datasem;  // 数据信号量
        Sem _spacesem; // 空间信号量

        Mutex _p_lock;
        Mutex _c_lock;
    };
} // namespace RingBufferModule
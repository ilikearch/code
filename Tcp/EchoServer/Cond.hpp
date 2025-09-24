#pragma once

#include <iostream>
#include <pthread.h>
#include "Mutex.hpp"

namespace CondModule
{
    using namespace LockModule;

    class Cond
    {
    public:
        Cond()
        {
            int n = ::pthread_cond_init(&_cond, nullptr);
            (void)n;
        }
        void Wait(Mutex &lock) // 让我们的线程释放曾经持有的锁！
        {
            int n = ::pthread_cond_wait(&_cond, lock.LockPtr());
        }
        void Notify()
        {
            int n = ::pthread_cond_signal(&_cond);
            (void)n;
        }
        void NotifyAll()
        {
            int n = ::pthread_cond_broadcast(&_cond);
            (void)n;
        }
        ~Cond()
        {
            int n = ::pthread_cond_destroy(&_cond);
        }
    private:
        pthread_cond_t _cond;
    };
}
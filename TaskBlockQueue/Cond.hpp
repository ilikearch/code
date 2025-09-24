#pragma once

#include <iostream>
#include <pthread.h>
#include "Mutex.hpp"

namespace CondModule
{
    using namespace LockModle;
    class Cond
    {
    public:
        Cond()
        {
            int n = ::pthread_cond_init(&_cond, nullptr);
            (void)n;
        }
        void Wait(Mutex &mutex)
        { // 让线程释放曾经持有的锁！
            int n = ::pthread_cond_wait(&_cond, mutex.LockPtr());
            (void)n;
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
            (void)n;
        }

    private:
        pthread_cond_t _cond;
    };

}
#pragma once
#include <iostream>
#include <pthread.h>

namespace LockModule
{
    class Mutex
    {
    public:
        Mutex(const Mutex&) = delete;
        const Mutex& operator = (const Mutex&) = delete;
        Mutex()
        {
            int n = ::pthread_mutex_init(&_lock, nullptr);
            (void)n;
        }
        ~Mutex()
        {
            int n = ::pthread_mutex_destroy(&_lock);
            (void)n;
        }
        void Lock()
        {
            int n = ::pthread_mutex_lock(&_lock);
            (void)n;
        }
        pthread_mutex_t *LockPtr()
        {
            return &_lock;
        }
        void Unlock()
        {
            int n = ::pthread_mutex_unlock(&_lock);
            (void)n;
        }

    private:
        pthread_mutex_t _lock;
    };

    class LockGuard
    {
    public:
        LockGuard(Mutex &mtx):_mtx(mtx)
        {
            _mtx.Lock();
        }
        ~LockGuard()
        {
            _mtx.Unlock();
        }
    private:
        Mutex &_mtx;
    };
}
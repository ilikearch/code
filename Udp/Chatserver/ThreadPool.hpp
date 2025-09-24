#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <memory>
#include "Log.hpp"
#include "Mutex.hpp"
#include "Cond.hpp"
#include "Thread.hpp"

namespace ThreadPoolModule
{
    using namespace LogModule;
    using namespace ThreadModule;
    using namespace LockModule;
    using namespace CondModule;

    // 用来做测试的线程方法
    void DefaultTest()
    {
        while (true)
        {
            LOG(LogLevel::DEBUG) << "我是一个测试方法";
            sleep(1);
        }
    }

    using thread_t = std::shared_ptr<Thread>;

    const static int defaultnum = 5;

    template <typename T>
    class ThreadPool
    {
    private:
        bool IsEmpty() { return _taskq.empty(); }

        void HandlerTask(std::string name)
        {
            LOG(LogLevel::INFO) << "线程: " << name << ", 进入HandlerTask的逻辑";
            while (true)
            {
                // 1. 拿任务
                T t;
                {
                    LockGuard lockguard(_lock);
                    while (IsEmpty() && _isrunning)
                    {
                        _wait_num++;
                        _cond.Wait(_lock);
                        _wait_num--;
                    }
                    // 2. 任务队列为空 && 线程池退出了
                    if (IsEmpty() && !_isrunning)
                        break;

                    t = _taskq.front();
                    _taskq.pop();
                }

                // 2. 处理任务
                t(); // 规定，未来所有的任务处理，全部都是必须提供()方法！
            }
            LOG(LogLevel::INFO) << "线程: " << name << " 退出";
        }
        ThreadPool(const ThreadPool<T> &) = delete;
        ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

        ThreadPool(int num = defaultnum) : _num(num), _wait_num(0), _isrunning(false)
        {
            for (int i = 0; i < _num; i++)
            {
                _threads.push_back(std::make_shared<Thread>(std::bind(&ThreadPool::HandlerTask, this, std::placeholders::_1)));
                LOG(LogLevel::INFO) << "构建线程" << _threads.back()->Name() << "对象 ... 成功";
            }
        }

    public:
        static ThreadPool<T> *getInstance()
        {
            if (instance == NULL)
            {
                LockGuard lockguard(mutex);
                if (instance == NULL)
                {
                    LOG(LogLevel::INFO) << "单例首次被执行，需要加载对象...";
                    instance = new ThreadPool<T>();
                    instance->Start();
                }
            }

            return instance;
        }

        void Equeue(T &in)
        {
            LockGuard lockguard(_lock);
            if (!_isrunning)
                return;
            // _taskq.push(std::move(in));
            _taskq.push(in);
            if (_wait_num > 0)
                _cond.Notify();
        }
        void Start()
        {
            if (_isrunning)
                return;
            _isrunning = true; // bug fix??
            for (auto &thread_ptr : _threads)
            {
                LOG(LogLevel::INFO) << "启动线程" << thread_ptr->Name() << " ... 成功";
                thread_ptr->Start();
            }
        }
        void Wait()
        {
            for (auto &thread_ptr : _threads)
            {
                thread_ptr->Join();
                LOG(LogLevel::INFO) << "回收线程" << thread_ptr->Name() << " ... 成功";
            }
        }
        void Stop()
        {
            LockGuard lockguard(_lock);
            if (_isrunning)
            {
                // 3. 不能在入任务了
                _isrunning = false; // 不工作
                // 1. 让线程自己退出(要唤醒) && // 2. 历史的任务被处理完了
                if (_wait_num > 0)
                    _cond.NotifyAll();
            }
        }
        ~ThreadPool()
        {
        }

    private:
        std::vector<thread_t> _threads;
        int _num;
        int _wait_num;
        std::queue<T> _taskq; // 临界资源

        Mutex _lock;
        Cond _cond;

        bool _isrunning;

        static ThreadPool<T> *instance;
        static Mutex mutex; // 只用来保护单例
    };

    template <typename T>
    ThreadPool<T> *ThreadPool<T>::instance = NULL;
    template <typename T>
    Mutex ThreadPool<T>::mutex; // 只用来保护单例
}
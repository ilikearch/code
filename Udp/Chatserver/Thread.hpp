#ifndef _THREAD_HPP__
#define _THREAD_HPP__

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <sys/types.h>
#include <unistd.h>

// v1
namespace ThreadModule
{
    using func_t = std::function<void(std::string name)>;
    static int number = 1;
    enum class TSTATUS
    {
        NEW,
        RUNNING,
        STOP
    };

    class Thread
    {
    private:
        // 成员方法！
        static void *Routine(void *args)
        {
            Thread *t = static_cast<Thread *>(args);
            t->_status = TSTATUS::RUNNING;
            t->_func(t->Name());
            return nullptr;
        }
        void EnableDetach() { _joinable = false; }

    public:
        Thread(func_t func) : _func(func), _status(TSTATUS::NEW), _joinable(true)
        {
            _name = "Thread-" + std::to_string(number++);
            _pid = getpid();
        }
        bool Start()
        {
            if (_status != TSTATUS::RUNNING)
            {
                int n = ::pthread_create(&_tid, nullptr, Routine, this); // TODO
                if (n != 0)
                    return false;
                return true;
            }
            return false;
        }
        bool Stop()
        {
            if (_status == TSTATUS::RUNNING)
            {
                int n = ::pthread_cancel(_tid);
                if (n != 0)
                    return false;
                _status = TSTATUS::STOP;
                return true;
            }
            return false;
        }
        bool Join()
        {
            if (_joinable)
            {
                int n = ::pthread_join(_tid, nullptr);
                if (n != 0)
                    return false;
                _status = TSTATUS::STOP;
                return true;
            }
            return false;
        }
        void Detach()
        {
            EnableDetach();
            pthread_detach(_tid);
        }
        bool IsJoinable() { return _joinable; }
        std::string Name() {return _name;}
        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        pid_t _pid;
        bool _joinable; // 是否是分离的，默认不是
        func_t _func;
        TSTATUS _status;
    };
}

// v2
// namespace ThreadModule
// {
//     static int number = 1;
//     enum class TSTATUS
//     {
//         NEW,
//         RUNNING,
//         STOP
//     };

//     template <typename T>
//     class Thread
//     {
//         using func_t = std::function<void(T)>;
//     private:
//         // 成员方法！
//         static void *Routine(void *args)
//         {
//             Thread<T> *t = static_cast<Thread<T> *>(args);
//             t->_status = TSTATUS::RUNNING;
//             t->_func(t->_data);
//             return nullptr;
//         }
//         void EnableDetach() { _joinable = false; }

//     public:
//         Thread(func_t func, T data) : _func(func), _data(data), _status(TSTATUS::NEW), _joinable(true)
//         {
//             _name = "Thread-" + std::to_string(number++);
//             _pid = getpid();
//         }
//         bool Start()
//         {
//             if (_status != TSTATUS::RUNNING)
//             {
//                 int n = ::pthread_create(&_tid, nullptr, Routine, this); // TODO
//                 if (n != 0)
//                     return false;
//                 return true;
//             }
//             return false;
//         }
//         bool Stop()
//         {
//             if (_status == TSTATUS::RUNNING)
//             {
//                 int n = ::pthread_cancel(_tid);
//                 if (n != 0)
//                     return false;
//                 _status = TSTATUS::STOP;
//                 return true;
//             }
//             return false;
//         }
//         bool Join()
//         {
//             if (_joinable)
//             {
//                 int n = ::pthread_join(_tid, nullptr);
//                 if (n != 0)
//                     return false;
//                 _status = TSTATUS::STOP;
//                 return true;
//             }
//             return false;
//         }
//         void Detach()
//         {
//             EnableDetach();
//             pthread_detach(_tid);
//         }
//         bool IsJoinable() { return _joinable; }
//         std::string Name() { return _name; }
//         ~Thread()
//         {
//         }

//     private:
//         std::string _name;
//         pthread_t _tid;
//         pid_t _pid;
//         bool _joinable; // 是否是分离的，默认不是
//         func_t _func;
//         TSTATUS _status;
//         T _data;
//     };
// }

#endif
#pragma once

#include <semaphore.h>

namespace SemModule
{
    int defalutsemval = 1;
    class Sem
    {
    public:
        Sem(int value=defalutsemval)
        :_init_value(value)
        {   
            int n= ::sem_init(&_sem,0,_init_value);
            (void)n;
        }
        void P(){
            int n=::sem_wait(&_sem);
            (void)n;
        }
        void V(){
            int n=::sem_post(&_sem);
            (void)n;
        }
        ~Sem(){
            int n=::sem_destroy(&_sem);
            (void)n;
        }
    private:
        sem_t _sem;
        int _init_value;
    };
}
#pragma once

#include <iostream>
#include <unistd.h>

namespace TaskModule
{
    class Task
    {
        public:
        Task(){}
        Task(int a,int b):x(a),y(b)
        {
        }
        void Excute()
        {
            sleep(1); //用1S来进行模拟任务处理的时长
            result = x + y; //入库，访问缓存，访问网络，打印日志等等
        }
        int X() {return x;}
        int Y() {return y;}
        int Result() {return result;}
        ~Task()
        {}
    private:
        int x;
        int y;
        int result;
    };
}
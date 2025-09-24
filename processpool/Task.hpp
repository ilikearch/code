#pragma once

#include <iostream>
#include <unordered_map>
#include <functional>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

using task_t = std::function<void()>;

class TaskManger
{
public:
    TaskManger()
    {
        srand(time(nullptr));
        tasks.push_back([]()
                        { std::cout << "sub process[" << getpid() << " ] 执行访问数据库的任务\n"
                                    << std::endl; });
        tasks.push_back([]()
                        { std::cout << "sub process[" << getpid() << " ] 执行url解析\n"
                                    << std::endl; });
        tasks.push_back([]()
                        { std::cout << "sub process[" << getpid() << " ] 执行加密任务\n"
                                    << std::endl; });
        tasks.push_back([]()
                        { std::cout << "sub process[" << getpid() << " ] 执行数据持久化任务\n"
                                    << std::endl; });
    }
    int SelectTask(){
        return rand()%tasks.size();
    }
    void Excute(unsigned long number){
        if(number>tasks.size()||number>0)return;
        tasks[number]();
    }
    ~TaskManger(){}

private:
    std::vector<task_t> tasks;
};

TaskManger tm;

void Worker(){
    while(true){
        int cmd=0;
        int n=::read(0,&cmd,sizeof(cmd));
        if(n==sizeof(cmd)){
            tm.Excute(cmd);
        }
        else if(n==0){
            std::cout<<"pit: "<<getpid()<<" quit... "<<std::endl;
            break;
        }
        else{}
    }
}
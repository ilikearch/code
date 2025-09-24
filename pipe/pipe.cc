#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Fater process -> read
// Child process -> write
int main()
{    //1.创建管道
    int fds[2]={0};
    int n=pipe(fds);
    if(n!=0){
        std::cerr<<"pipe error"<<std::endl;
        return 1;
    }

    //2.创建子进程
    pid_t id=fork();
    if(id<0){
        std::cerr<<"fork error"<<std::endl;
        return 2;
    }
    else if(id==0){
        //子进程
        //3.关闭不需要的fd 关闭read
        close(fds[0]);
        int cnt=0;
        int total=0;
        while(true){
            std::string message="love you";
            total+=::write(fds[1],message.c_str(),message.size());
            cnt++;
            std::cout<< "total: "<<total<<std::endl;
            sleep(2);

        }
        exit(0);
    }

    else{
        //父进程
        //关闭不需要的fd 关闭write
        close(fds[1]);

        char buffer[1024];
        while(true){
            sleep(1);
            ssize_t n=::read(fds[0],buffer,1024);
            if(n>0){
                buffer[n]=0;
                std::cout << "child->father,message "<<buffer<<std::endl;
            }
            else if(n==0){
                // 如果写端关闭
                // 读端读完管道内部的数据，在读取的时候，
                // 就会读取到返回值0，表示对端关闭，也表示读到文件结尾
                std::cout << "n: " << n << std::endl;
                std::cout << "child quit ??? me too" << std::endl;
                break;
            }
            close(fds[0]);
            break;
            std::cout<<std::endl;
        }

        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        std::cout << "father wait child success: " << rid << " exit code: " <<
             ((status<<8)&0xFF) << ", exit sig: " << (status & 0x7F) << std::endl;
    }

    return 0;
}
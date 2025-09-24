#include <iostream>
#include <unistd.h>
#include <string.h>
#include "ShareMemory.hpp"
#include "Time.hpp"
#include "Fifo.hpp"

int main()
{
    std::cout << "time: " << GetCurrTime() << std::endl;
    shm.CreateShm();
    shm.AttachShm();

    gpipe.OpenPipeForRead();
    // sleep(10);
    // 在这里进行IPC
    struct data *image = (struct data *)shm.GetAddr();
    // std::cout << "server 虚拟地址: " << strinfo << std::endl;
    // printf("server 虚拟地址: %p\n", strinfo);

    while (true)
    {
        gpipe.Wait();

        printf("status: %s\n", image->status);
        printf("lasttime: %s\n", image->lasttime);
        printf("image: %s\n", image->image);
        strcpy(image->status, "过期");
    }

    // sleep(10);
    shm.DetachShm();
    shm.DeleteShm();
    return 0;
}

// int main()
// {
//     // 1. 创建Key

//     std::cout << "k : " << ToHex(k) << std::endl;
//     // 2. 创建共享内存 && 获取
//     // 注意: 共享内存也有权限！

//     sleep(5);

//     // 3. 共享内存挂接到自己的地址空间中
//     void *ret = shmat(shmid, nullptr, 0); // 为什么会失败？？？
//     std::cout << "attach done: " << (long long)ret << std::endl;

//     sleep(5);

//     // 在这里进行通信！

//     sleep(5);

//     // n. 删除共享内存
//     shmctl(shmid, IPC_RMID, nullptr);
//     std::cout << "delete shm done" << std::endl;
//     sleep(5);
//     return 0;
// }

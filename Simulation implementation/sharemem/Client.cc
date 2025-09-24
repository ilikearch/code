#include <iostream>
#include <string.h>
#include <string>
#include "ShareMemory.hpp"
#include "Time.hpp" 
#include "Fifo.hpp" 
int main()
{
    shm.GetShm();
    shm.AttachShm();
    gpipe.OpenPipeForWrite();
    struct data *image = (struct data *)shm.GetAddr();
    char ch = 'A';
    while (ch <= 'Z')
    {
        strcpy(image->status, "最新");
        strcpy(image->lasttime, GetCurrTime().c_str());
        strcpy(image->image, &ch);
        gpipe.Signal();
        ch++;
        sleep(5);
    }
    shm.DetachShm(); // 释放共享内存
    shm.DeleteShm();
    return 0;
}
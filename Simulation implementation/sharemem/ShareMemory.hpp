#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdalign.h>
#include <unistd.h>

const std::string gpath = "/root/code";
int gprojId = 0x6666;
// 操作系统，申请空间，是按照块为单位的：4KB，1KB， 2KB， 4MB
int gshmsize = 4096;
mode_t gmode = 0600;

std::string ToHex(key_t k)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "0x%x", k);
    return buffer;
}

class ShareMemory
{
private:
    void CreateShmHelper(int shmflg)
    {
        _key = ::ftok(gpath.c_str(), gprojId);
        if (_key < 0)
        {
            std::cerr << "ftok error" << std::endl;
            return;
        }
        _shmid = ::shmget(_key, gshmsize, shmflg);
        if (_shmid < 0)
        {
            std::cerr << "shmget erroor" << std::endl;
            return;
        }
        std::cout << "shmid: " << _shmid << std::endl;
    }

public:
    ShareMemory() : _shmid(-1), _key(0), _addr(nullptr)
    {
    }
    ~ShareMemory() {}
    void CreateShm()
    {
        if (_shmid == -1)
            CreateShmHelper(IPC_CREAT | IPC_EXCL | gmode);

        std::cout << "key: " << ToHex(_key) << std::endl;
    }
    void GetShm()
    {
        CreateShmHelper(IPC_CREAT);
    }
    void AttachShm()
    {
        _addr = ::shmat(_shmid, nullptr, 0);
        if ((long long)_addr == -1)
        {
            std::cout << "attach error" << std::endl;
        }
    }
    void DetachShm()
    {
        if (_addr != nullptr)
            ::shmdt(_addr);
        std::cout << "detach done" << std::endl;
    }

    void *GetAddr()
    {
        return _addr;
    }
    void DeleteShm()
    {
        shmctl(_shmid, IPC_RMID, nullptr);
    }
    void ShmMeta()
    {
        struct shmid_ds buffer; // 系统提供的数据类型
        int n = ::shmctl(_shmid, IPC_STAT, &buffer);
        if (n < 0)
            return;
        std::cout << "################" << std::endl;
        std::cout << buffer.shm_atime << std::endl;
        std::cout << buffer.shm_cpid << std::endl;
        std::cout << buffer.shm_ctime << std::endl;
        std::cout << buffer.shm_nattch << std::endl;
        std::cout << ToHex(buffer.shm_perm.__key) << std::endl;
        std::cout << "################" << std::endl;
    }

private:
    int _shmid;
    key_t _key;
    void *_addr;
};
// 临时
ShareMemory shm;

struct data
{
    char status[32];
    char lasttime[48];
    char image[4000];
};
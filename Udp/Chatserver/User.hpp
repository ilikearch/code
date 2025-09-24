#pragma once

#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>

#include "InetAddr.hpp"
#include "Log.hpp"
#include "Mutex.hpp"

using namespace LogModule;
using namespace LockModule;

class UserInterface
{
public:
    virtual ~UserInterface() = default;
    virtual void SendTo(int sockfd, const std::string &message) = 0;
    virtual bool operator==(const InetAddr &u) = 0;
    virtual std::string Id() = 0;
};

class User : public UserInterface
{
public:
    User(const InetAddr &id) : _id(id)
    {
    }
    void SendTo(int sockfd, const std::string &message) override
    {
        LOG(LogLevel::DEBUG) << "send message to " << _id.Addr() << " info: " << message;
        int n = ::sendto(sockfd, message.c_str(), message.size(), 0, _id.NetAddr(), _id.NetAddrLen());
        (void)n;
    }
    bool operator==(const InetAddr &u) override
    {
        return _id == u;
    }
    std::string Id() override
    {
        return _id.Addr();
    }
    ~User()
    {
    }

private:
    InetAddr _id;
};

// 对用户消息进行路由
//  UserManager 把所有的用户先管理起来！
//  观察者模式！observer
class UserManager
{
public:
    UserManager()
    {
    }
    void AddUser(InetAddr &id)
    {
        LockGuard lockguard(_mutex);
        for (auto &user : _online_user)
        {
            if (*user == id)
            {
                LOG(LogLevel::INFO) << id.Addr() << "用户已经存在";
                return;
            }
        }
        LOG(LogLevel::INFO) << " 新增该用户: " <<  id.Addr();
        _online_user.push_back(std::make_shared<User>(id));
        PrintUser();
    }
    void DelUser(InetAddr &id)
    {
        // v1
        auto pos = std::remove_if(_online_user.begin(), _online_user.end(), [&id](std::shared_ptr<UserInterface> &user){
            return *user == id;
        });
        _online_user.erase(pos, _online_user.end());
        PrintUser();

        //v2
        // for(auto user : _online_user)
        // {
        //     if(*user == id)
        //     {
        //         _online_user.erase(user);
        //         break; // 迭代器失效的问题
        //     }
        // }
    }
    void Router(int sockfd, const std::string &message)
    {
        LockGuard lockguard(_mutex);
        for (auto &user : _online_user)
        {
            user->SendTo(sockfd, message);
        }
    }
    void PrintUser()
    {
        for(auto user : _online_user)
        {
            LOG(LogLevel::DEBUG) <<"在线用户-> "<<  user->Id();
        }
    }
    ~UserManager()
    {
    }

private:
    std::list<std::shared_ptr<UserInterface>> _online_user;
    Mutex _mutex;
};

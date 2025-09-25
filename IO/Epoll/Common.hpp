#pragma once

#include <iostream>

#define Die(code)  \
    do             \
    {              \
        exit(code);\
    }while (0)

#define CONV(v) (struct sockaddr *)(v)

enum 
{
    USAGE_ERR = 1,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    EPOLL_CREATE_ERR,
    EPOLL_CTL_ERR
};

const int gdefaultsockfd = -1;
const int gbacklog = 8;

// 1. 正常字符串 2. out空串&&返回值是true  3. out空串&&返回值是flase
bool ParseOneLine(std::string &str,std::string *out,const std::string &sep)
{
    auto pos=str.find(sep);
    if(pos==std::string::npos){
        return false;
    }
    *out=str.substr(0,pos);
    str.erase(0,pos+sep.size());
    return true;
}

// Connection: keep-alive
bool SplitString(const std::string &header,const std::string &sep,std::string *key,std::string *value)
{
    auto pos=header.find(sep);
    if (pos==std::string::npos)return false;
    *key=header.substr(0,pos);
    *value=header.substr(pos+sep.size());
    return true;
}
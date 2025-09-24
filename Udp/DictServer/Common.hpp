#pragma once

#include <iostream>
#include <string>


#define Die(code)   \
    do              \
    {               \
        exit(code); \
    } while (0)

#define CONV(v) (struct sockaddr *)(v)

enum 
{
    USAGE_ERR = 1,
    SOCKET_ERR,
    BIND_ERR
};

// happy: 快乐的
bool SplitString(const std::string &line, std::string *key, std::string *value, const std::string &sep)
{
    auto pos = line.find(sep);
    if(pos == std::string::npos) return false;
    *key = line.substr(0, pos);
    *value = line.substr(pos+sep.size());
    if(key->empty() || value->empty()) return false;
    return true;
}
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
    LISTEN_ERR
};
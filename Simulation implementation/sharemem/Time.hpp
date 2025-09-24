#pragma once

#include <iostream>
#include <string>
#include <ctime>

std::string GetCurrTime()
{
    time_t t = time(nullptr);
    struct tm *cuur = ::localtime(&t);

    char currtime[32];
    snprintf(currtime, sizeof(currtime), "%d-%d-%d %d:%d:%d",
             cuur->tm_year + 1900,
             cuur->tm_mon + 1,
             cuur->tm_mday,
             cuur->tm_hour,
             cuur->tm_min,
             cuur->tm_sec);
    return currtime;
}
#pragma once

#include <iostream>
#include <string>
#include <functional>
#include "Log.hpp"

using namespace LogMudule;

using task_t = std::function<void(std::string name)>;

void Push(std::string name)
{
    LOG(LogLevel::DEBUG) << "我是一个推送数据到服务器的一个任务, 我正在被执行" << "[" << name << "]";
}
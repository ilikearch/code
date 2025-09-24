#include "Log.hpp"

using namespace LogMudule;

int main()
{
    ENABLE_FILE_LOG();

    LOG(LogLevel::DEBUG) << "hello file";
    LOG(LogLevel::DEBUG) << "hello file";
    LOG(LogLevel::DEBUG) << "hello file";
    LOG(LogLevel::DEBUG) << "hello file";

    ENABLE_CONSOLE_LOG();
    LOG(LogLevel::DEBUG) << "hello world";
    LOG(LogLevel::DEBUG) << "hello world";
    LOG(LogLevel::DEBUG) << "hello world";
    LOG(LogLevel::DEBUG) << "hello world";
    return 0;
}
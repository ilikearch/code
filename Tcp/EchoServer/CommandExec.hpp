#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <set>

const int line_size = 1024;

class Command
{
public:
    Command()
    {
        _white_list.insert("ls");
        _white_list.insert("ll");
        _white_list.insert("ls -l");
        _white_list.insert("pwd");
        _white_list.insert("touch");
        _white_list.insert("who");
        _white_list.insert("whoami");
    }
    bool SafeCheck(const std::string &cmdstr)
    {
        auto iter = _white_list.find(cmdstr);
        return iter == _white_list.end() ? false : true;
    }

    // 给你一个命令字符串"ls -l", 让你执行，执行完，把结果返回
    std::string Execute(std::string cmdstr)
    {
        if (!SafeCheck(cmdstr))
            return std::string(cmdstr + "不支持");
        FILE *fp = ::popen(cmdstr.c_str(), "r");
        if (nullptr == fp)
            return std::string("FAILED");
        char buffer[line_size];
        std::string result;
        while (true)
        {
            char *ret = ::fgets(buffer, sizeof(buffer), fp);
            if (!ret)
                break;
            result += ret;
        }
        pclose(fp);
        return result.empty() ? std::string("Done") : result;
    }

private:
    std::set<std::string> _white_list;
};
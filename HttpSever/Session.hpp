#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdint>
class Session
{
private:
    std::string name;
    bool islogin;
    uint64_t session_id;
    // 其他信息
};


class SessionManager
{
public:
    void CreateSession(uint64_t session_id)
    {
    }
    void DeleteSession(uint64_t session_id)
    {}
    void SearchSession(uint64_t session_id)
    {
    }
private:
    std::unordered_map<uint64_t, Session*> _session;
};
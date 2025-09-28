#pragma once
#include <iostream>
#include "Protocol.hpp"

class Calculator
{
public:
    Calculator()
    {
    }
    Response Execute(const Request &req)
    {
        // 我们拿到的都是结构化的数据，拿到的不就是类对象吗！！！
        Response resp;
        switch (req.Oper())
        {
        case '+':
            resp.SetResult(req.X() + req.Y());
            break;
        case '-':
            resp.SetResult(req.X() - req.Y());
            break;
        case '*':
            resp.SetResult(req.X() * req.Y());
            break;
        case '/':
        {
            if (req.Y() == 0)
            {
                resp.SetCode(1); // 1 就是除0
            }
            else
            {
                resp.SetResult(req.X() / req.Y());
            }
        }
        break;
        case '%':
        {
            if (req.Y() == 0)
            {
                resp.SetCode(2); // 2 就是mod 0
            }
            else
            {
                resp.SetResult(req.X() % req.Y());
            }
        }
        break;
        default:
            resp.SetCode(3); // 3 用户发来的计算类型，无法识别
            break;
        }
        return resp;
    }
    ~Calculator()
    {
    }
}cal;

// inbuffer: len\r\n{XXX}len\r
std::string HandlerRequest(std::string &inbuffer)
{
    std::string request_str;
    std::string result_str;
    while (Decode(inbuffer, &request_str))
    {
        std::string resp_str;
        // 一定拿到了一个完整报文
        // 1. 反序列化
        if (request_str.empty())
            break;
        Request req;
        if (!req.Deserialize(request_str))
            break;
        std::cout << "#############" << std::endl;
        req.Print();
        std::cout << "#############" << std::endl;
        // 2. 业务处理
        Response resp = cal.Execute(req); // 线程池？？
        // 3. 序列化
        resp.Serialize(resp_str);
        // 4. 添加长度说明 -- 协议
        Encode(resp_str);
        // 5. 添加所有的应答
        result_str += resp_str;
    }
    return result_str;
}

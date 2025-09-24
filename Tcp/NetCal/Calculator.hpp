#pragma once
#include <iostream>
#include "Protocol.hpp"

class Calculator
{
public:
    Calculator(){}
    Response Execute(const Request &req)
    {
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
    ~Calculator(){}
};
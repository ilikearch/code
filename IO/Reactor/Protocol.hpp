#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <jsoncpp/json/json.h>

const std::string Sep = "\r\n";
// {json} -> len\r\n{json}\r\n
bool Encode(std::string &message)
{
    if (message.size() == 0)
        return false;
    std::string package = std::to_string(message.size()) + Sep + message + Sep;
    message = package;
    return true;
}
// len\r\n{json}\r\n
// 123\r\n{json}\r\n -> {json}
// 123\r\n
// 123\r\n{json
// 123\r\n{json}\r
// 123\r\n{json}\r\n123\r\n{json}\r\n123\r\n{js
bool Decode(std::string &package, std::string *content)
{
    auto pos = package.find(Sep);
    if (pos == std::string::npos)
        return false;
    std::string content_length_str = package.substr(0, pos);
    int content_length = std::stoi(content_length_str);
    int full_length = content_length_str.size() + content_length + 2 * Sep.size();

    if (package.size() < full_length)
        return false;

    *content = package.substr(pos + Sep.size(), content_length);

    // package erase
    package.erase(0, full_length); // 后续写网络代码再来研究
    return true;
}

// _x _oper _y
class Request
{
public:
    Request() : _x(0), _y(0), _oper(0)
    {
    }
    Request(int x, int y, char oper) : _x(x), _y(y), _oper(oper)
    {
    }
    bool Serialize(std::string &out_string)
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;

        Json::StreamWriterBuilder wb;
        std::unique_ptr<Json::StreamWriter> w(wb.newStreamWriter());
        std::stringstream ss;
        w->write(root, &ss);
        out_string = ss.str();
        return true;
    }
    bool Deserialize(std::string &in_string)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(in_string, root);
        if (!parsingSuccessful)
        {
            std::cout << "Failed to parse JSON: " << reader.getFormattedErrorMessages() << std::endl;
            return false;
        }

        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _oper = root["oper"].asInt();

        return true;
    }
    void Print()
    {
        std::cout << _x << std::endl;
        std::cout << _oper << std::endl;
        std::cout << _y << std::endl;
    }
    int X() const { return _x; }
    int Y() const { return _y; }
    char Oper() const { return _oper; }

private:
    int _x;
    int _y;
    char _oper;
};

class Response
{
public:
    Response() : _result(0), _code(0)
    {
    }
    Response(int result, int code) : _result(result), _code(code)
    {
    }
    bool Serialize(std::string &out_string)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;

        Json::StreamWriterBuilder wb;
        std::unique_ptr<Json::StreamWriter> w(wb.newStreamWriter());
        std::stringstream ss;
        w->write(root, &ss);
        out_string = ss.str();
        return true;
    }
    bool Deserialize(std::string &in_string)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(in_string, root);
        if (!parsingSuccessful)
        {
            std::cout << "Failed to parse JSON: " << reader.getFormattedErrorMessages() << std::endl;
            return false;
        }

        _result = root["result"].asInt();
        _code = root["code"].asInt();

        return true;
    }
    
    int Result() const { return _result; }
    int Code() const { return _code; }
    void SetResult(int res) { _result = res; }
    void SetCode(int c) { _code = c; }

private:
    int _result; // 结果
    int _code;   // 出错码，0,1,2,3,4
};

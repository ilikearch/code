#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include "Common.hpp"
#include "Log.hpp"

const std::string Sep = "\r\n";
const std::string LineSep = " ";
const std::string HeaderLineSep = ": ";
const std::string BlankLine = Sep;
const std::string defaulthomepage = "wwwroot";
const std::string http_version = "HTTP/1.0";
const std::string page404 = "wwwroot/404.html";
const std::string firstpage = "index.html";

using namespace LogModule;

class HttpRequest
{
private:
    // GET /favicon.ico HTTP/1.1 -> GET    /favicon.ico      HTTP/1.1
    void ParseReqLine(std::string &_req_line, const std::string sep)
    {
        (void)sep;
        std::stringstream ss(_req_line);
        ss >> _method >> _uri >> _version;
    }
    std::string _req_line;
    std::vector<std::string> _req_header;
    std::string _blank_line;
    std::string _body; // body内部可能会包含参数

    // 在反序列化的过程中，细化我们解析出来的字段
    std::string _method;
    std::string _uri; // 用户想要这个 // /login.html || /login?xxxxx
    std::string _path;
    std::string _args;
    std::string _version;
    std::unordered_map<std::string, std::string> _headerkv;
    bool _isexec = false;

public:
    HttpRequest() {}
    ~HttpRequest() {}
    bool IsHasArgs() { return _isexec; };
    // GET /favicon.ico HTTP/1.1\r\n
    // Host: 8.137.19.140:8080
    // Connection: keep-alive
    // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0
    // Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
    // Referer: http://8.137.19.140:8080/?msg=i_have_sent_a_message_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_he_
    // Accept-Encoding: gzip, deflate
    // Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6
    // dnt: 1
    // sec-gpc: 1
    //
    bool ParseHeaderkv()
    {
        std::string key, value;
        for (auto &header : _req_header)
        {
            // Connection: keep-alive
            if (SplitString(header, HeaderLineSep, &key, &value))
            {
                _headerkv.insert(std::make_pair(key, value));
            }
        }
        return true;
    }
    bool ParseHeader(std::string &request_str)
    {
        std::string line;
        while (true)
        {
            bool r = ParseOneLine(request_str, &line, Sep);
            if (r && !line.empty())
            {
                _req_header.push_back(line);
            }
            else if (r && line.empty())
            {
                _blank_line = Sep;
                break;
            }
            else
            {
                return false;
            }
        }
        ParseHeaderkv();
        return true;
    }
    void Deserialize(std::string &request_str)
    {
        if (ParseOneLine(request_str, &_req_line, Sep))
        {
            // 提取请求行中的详细字段
            ParseReqLine(_req_line, LineSep);
            ParseHeader(request_str);
            _body = request_str;

            // 分析请求中，是否含有参数..
            if (_method == "POST")
            {
                _isexec = true; // 参数在正文
                _args = _body;
                _path = _uri;
                LOG(LogLevel::DEBUG) << "POST: _path: " << _path;
                LOG(LogLevel::DEBUG) << "POST: _args: " << _args;
            }
            else if (_method == "GET")
            {
                auto pos = _uri.find("?");
                if (pos != std::string::npos)
                {
                    _isexec = true;
                    // /login?name=zhang&passwd=123456
                    _path = _uri.substr(0, pos);
                    _args = _uri.substr(pos + 1);
                    LOG(LogLevel::DEBUG) << "_path: " << _path;
                    LOG(LogLevel::DEBUG) << "_args: " << _args;
                }
            }
        }
    }
    std::string GetContent(const std::string path)
    {
        std::string content;
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open())
            return std::string();
        in.seekg(0, in.end);
        int filesize = in.tellg();
        in.seekg(0, in.beg);

        content.resize(filesize);
        in.read((char *)content.c_str(), filesize);
        in.close();
        LOG(LogLevel::DEBUG) << "content length: " << content.size();
        return content;
    }
    void Print()
    {
        std::cout << "_method: " << _method << std::endl;
        std::cout << "_uri: " << _uri << std::endl;
        std::cout << "_version: " << _version << std::endl;

        for (auto &kv : _headerkv)
        {
            std::cout << kv.first << " # " << kv.second << std::endl;
        }
        std::cout << "_blank_line: " << _blank_line << std::endl;
        std::cout << "_body: " << _body << std::endl;
    }
    std::string Uri()
    {
        return _uri;
    }
    void SetUri(const std::string newuri)
    {
        _uri = newuri;
    }
    std::string Path() { return _path; }
    std::string Args() { return _args; }
    std::string Suffix()
    {
        // _uri -> wwwroot/index.html wwwroot/image/1.jpg wwwroot/login.html
        auto pos = _uri.rfind(".");
        if (pos == std::string::npos)
            return std::string(".html");
        else
            return _uri.substr(pos);
    }
};

// 对于http，任何请求，都要有应答
class HttpResponse
{
public:
    HttpResponse() : _verion(http_version), _blank_line(Sep)
    {
    }
    ~HttpResponse()
    {
    }
    void Build(HttpRequest &req)
    {
        // std::string uri = req.Uri(); // wwwroot/   wwwroot/a/b/
        // if (uri.back() == '/')
        // {
        //     _status_code = 301;
        //     _status_desc = Code2Desc(_status_code); // 和状态码是强相关的！
        //     SetHeader("Location", "http://8.137.19.140:8888/register.html");
        //     for (auto &header : _header_kv)
        //     {
        //         _resp_header.push_back(header.first + HeaderLineSep + header.second);
        //     }
        //     LOG(LogLevel::DEBUG) << "------客户端在请求: " << req.Uri();
        //     return;
        // }

        std::string uri = defaulthomepage + req.Uri(); // wwwroot/   wwwroot/a/b/
        if (uri.back() == '/')
        {
            uri += firstpage; // wwwroot/index.html
            // req.SetUri(uri);
        }
        LOG(LogLevel::DEBUG) << "------客户端在请求: " << req.Uri();
        req.Print();
        LOG(LogLevel::DEBUG) << "----------------------------";

        _content = req.GetContent(uri);
        // LOG(LogLevel::DEBUG) << "content length: " << _content.size();
        if (_content.empty())
        {
            // 用户请求的资源并不存在
            _status_code = 404;
            // req.SetUri(page404);
            _content = req.GetContent(page404);
        }
        else
        {
            _status_code = 200;
        }

        _status_desc = Code2Desc(_status_code); // 和状态码是强相关的！
        if (!_content.empty())
        {
            SetHeader("Content-Length", std::to_string(_content.size()));
        }
        std::string mime_type = Suffix2Desc(req.Suffix());

        SetHeader("Content-Type", mime_type);
        _body = _content;
    }

    void SetCode(int code)
    {
        _status_code = code;
        _status_desc = Code2Desc(_status_code);
    }
    void SetBody(const std::string &body)
    {
        _body = body;
    }
    void SetHeader(const std::string &k, const std::string &v)
    {
        _header_kv[k] = v;
    }
    void Serialize(std::string *resp_str)
    {
        for (auto &header : _header_kv)
        {
            _resp_header.push_back(header.first + HeaderLineSep + header.second);
        }
        _resp_line = _verion + LineSep + std::to_string(_status_code) + LineSep + _status_desc + Sep;

        // 序列化
        *resp_str = _resp_line;
        for (auto &line : _resp_header)
        {
            *resp_str += (line + Sep);
        }
        *resp_str += _blank_line;
        *resp_str += _body;
    }

private:
    std::string Code2Desc(int code)
    {
        switch (code)
        {
        case 200:
            return "OK";
        case 404:
            return "Not Found";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        default:
            return std::string();
        }
    }
    std::string Suffix2Desc(const std::string &suffix)
    {
        if (suffix == ".html")
            return "text/html";
        else if (suffix == ".jpg")
            return "application/x-jpg";
        else
            return "text/html";
    }
    // 必备的要素
    std::string _verion;
    int _status_code;
    std::string _status_desc;
    std::string _content;
    std::unordered_map<std::string, std::string> _header_kv;

    // 最终要这4部分，构建应答
    std::string _resp_line;
    std::vector<std::string> _resp_header;
    std::string _blank_line;
    std::string _body;
};
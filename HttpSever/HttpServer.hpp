#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include "TcpServer.hpp"
#include "HttpProtocol.hpp"

using namespace TcpServerModule;

using http_handler_t = std::function<void(HttpRequest &, HttpResponse &)>;

class HttpServer
{
private:
    std::unique_ptr<TcpServer> _tsvr;
    std::unordered_map<std::string, http_handler_t> _route; // 功能路由
public:
    HttpServer(int port)
        : _tsvr(std::make_unique<TcpServer>(port)) {}
    ~HttpServer() {}
    void Resgiter(std::string funcname, http_handler_t func)
    {
        _route[funcname] = func;
    }
    bool SafeCheck(const std::string &service)
    {
        auto iter = _route.find(service);
        return iter != _route.end();
    }
    void Start()
    {
        _tsvr->InitServer([this](SockPtr sockfd, InetAddr client)
                          { return this->HandlerHttpRequest(sockfd, client); });
        _tsvr->Loop();
    }
    bool HandlerHttpRequest(SockPtr sockfd, InetAddr client)
    {
        LOG(LogLevel::DEBUG) << "HttpServer: get a new client: " << sockfd->Fd() << " addr info: " << client.Addr();
        std::string http_request;
        sockfd->Recv(&http_request); // 在HTTP这里，我们不做报文完整性的处理
        HttpRequest req;
        req.Deserialize(http_request);
        HttpResponse resp;
        // 请求应该被分成两类: 1. 请求一般的静态资源 2. 提交参数，携带参数，需要我们进行交互设置
         if (req.IsHasArgs())
        {
            std::cout << "-----IsHasArgs()" << std::endl;
            // 2. 提交参数，携带参数，需要我们进行交互设置
            std::string service = req.Path();
            if (SafeCheck(service))
                _route[req.Path()](req, resp); // /login
            else
                resp.Build(req); // debug
        }
        else
        {
            std::cout << "-----Non IsHasArgs()" << std::endl;
            resp.Build(req);
        }
        std::string resp_str;
        resp.Serialize(&resp_str);
        sockfd->Send(resp_str);
        return true;
    }
};

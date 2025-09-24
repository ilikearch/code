#include "HttpServer.hpp"
#include "Session.hpp"

using namespace LogModule;

void Login(HttpRequest &req, HttpResponse &resp)
{
    // req.Path(): /login
    // 根据req，动态构建username=lisi&password=abcdefg
    LOG(LogLevel::DEBUG) << "进入登录模块" << req.Path() << ", " << req.Args();
    std::string req_args = req.Args();
    // 1. 解析参数格式，得到要的参数

    // 2. 访问数据库，验证对应的用户是否是合法的用户，其他工作....
    // TODO
    // SessionManager m;
    // session_id = m.CreateSession(XXXXX);

    // 3. 登录成功
    std::string body = req.GetContent("wwwroot/success.html");
    resp.SetCode(200);
    resp.SetHeader("Content-Length", std::to_string(body.size()));
    resp.SetHeader("Content-Type", "text/html");
    resp.SetHeader("Set-Cookie", "usename=zhangsan&passwd=11111111");
    resp.SetBody(body);

    // resp.SetCode(302);
    // resp.SetHeader("Location", "/");
}
void Register(HttpRequest &req, HttpResponse &resp)
{
    // 根据req，动态构建resp
    LOG(LogLevel::DEBUG) << "进入注册模块" << req.Path() << ", " << req.Args();
}

void Search(HttpRequest &req, HttpResponse &resp)
{
    // 根据req，动态构建resp
    LOG(LogLevel::DEBUG) << "进入搜索模块" << req.Path() << ", " << req.Args();
}

void Test(HttpRequest &req, HttpResponse &resp)
{
    // 根据req，动态构建resp
    LOG(LogLevel::DEBUG) << "进入测试模块" << req.Path() << ", " << req.Args();
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }
    auto httpserver = std::make_unique<HttpServer>(std::stoi(argv[1]));
    // 要让服务器具有登录功能
    httpserver->Resgiter("/login", Login); // restful风格的网络请求接口
    httpserver->Resgiter("/register", Register);
    httpserver->Resgiter("/search", Search);
    httpserver->Resgiter("/test", Test);
    httpserver->Start();
    return 0;
}
#include "TcpServer.hpp"
// #include "CommandExec.hpp"
#include "Protocol.hpp"
#include "Calculator.hpp"
#include "Daemon.hpp"
#include <memory>

using namespace LogModule;

using cal_fun = std::function<Response(const Request &req)>;

// package一定会有完整的报文吗？？不一定把
// 不完整->继续读
// 完整-> 提取 -> 反序列化 -> Request -> 计算模块，进行处理
class Parse
{
public:
    Parse(cal_fun c) : _cal(c)
    {}
    std::string Entry(std::string &package)
    {
        // 1. 判断报文的完整性！
        std::string message;
        std::string respstr;
        while (Decode(package, &message))
        {
            LOG(LogLevel::DEBUG) << "Content: \n" << message;
            if (message.empty())
                break;

            // 2. 反序列化， message是一个曾经被序列化的request
            Request req;
            if (!req.Deserialize(message))
                break;

            std::cout << "#############" << std::endl;
            req.Print();
            std::cout << "#############" << std::endl;

            // 3. 计算
            Response resp = _cal(req);

            // 4. 序列化
            std::string res;
            resp.Serialize(res);
            LOG(LogLevel::DEBUG) << "序列化: \n" << res;



            // 5. 添加长度报头字段！
            Encode(res);

            LOG(LogLevel::DEBUG) << "Encode: \n" << res;

            // 6. 拼接应答
            respstr += res;
        }
        LOG(LogLevel::DEBUG) << "respstr: \n" << respstr;
        return respstr;
    }
private:
    cal_fun _cal;
};

int main()
{
    ENABLE_FILE_LOG();
    // Command cmd;

    // std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>([&cmd](std::string cmdstr){
    //     return cmd.Execute(cmdstr);
    // });
    Daemon(false, false);
    // 1. 计算模块
    Calculator mycal;
    // 2. 解析对象
    Parse myparse([&mycal](const Request &req){
        return mycal.Execute(req);
    });
    // 3. 通信模块
    // 只负责进行IO
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>([&myparse](std::string &package){
        return myparse.Entry(package);
    });

    tsvr->InitServer();
    tsvr->Start();

    return 0;
}
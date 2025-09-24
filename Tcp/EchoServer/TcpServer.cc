#include "TcpServer.hpp"
#include "CommandExec.hpp"
#include <memory>

using namespace LogModule;

int main()
{
    ENABLE_CONSOLE_LOG();
    Command cmd;
    std::unique_ptr<TcpServer> tsvr=std::make_unique<TcpServer>([&cmd](std::string cmdstr){
        return cmd.Execute(cmdstr);
    });

    tsvr->InitServer();
    tsvr->Start();

    return 0;
}
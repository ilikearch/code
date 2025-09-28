#include <iostream>
#include <string>
#include "Log.hpp"
#include "Listener.hpp"
#include "Connection.hpp"
#include "Reactor.hpp"

using namespace LogModule;

int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        std::cout<<"Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }
    ENABLE_CONSOLE_LOG();
    uint16_t local_port=std::stoi(argv[1]);
    Reactor reactor;
    auto conn = std::make_shared<Listener>(local_port);
    reactor.InsertConnection(conn);
    reactor.Loop();
    
    return 0;
}
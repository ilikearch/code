#include "EpollServer.hpp"

// ./select_server 8080
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }
    ENABLE_CONSOLE_LOG();
    uint16_t local_port = std::stoi(argv[1]);

    std::unique_ptr<EpollServer> ssvr = std::make_unique<EpollServer>(local_port);
    ssvr->Init();
    ssvr->Loop();

    return 0;
}

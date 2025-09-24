#include "UdpServer.hpp"

// ./server_udp localport
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " localport" << std::endl;
        Die(USAGE_ERR);
    }
    // std::string ip = argv[1];
    uint16_t port = std::stoi(argv[1]);

    ENABLE_CONSOLE_LOG();

    std::unique_ptr<UdpServer> svr_uptr = std::make_unique<UdpServer>(port);
    svr_uptr->InitServer();
    svr_uptr->Start();

    return 0;
}
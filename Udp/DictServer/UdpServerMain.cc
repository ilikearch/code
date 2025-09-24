#include "UdpServer.hpp"
#include "Dictionary.hpp"

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
    std::shared_ptr<Dictionary> dict_sptr = std::make_shared<Dictionary>();

    std::unique_ptr<UdpServer> svr_uptr = std::make_unique<UdpServer>([&dict_sptr](const std::string &word){
        std::cout << "|" << word << "|" << std::endl;
        return dict_sptr->Translate(word);
    }, port);

    // std::unique_ptr<UdpServer> svr_uptr = std::make_unique<UdpServer>(std::bind(&Dictionary::Translate,\
    //             dict_sptr.get(), std::placeholders::_1), port);

    svr_uptr->InitServer();
    svr_uptr->Start();

    return 0;
}
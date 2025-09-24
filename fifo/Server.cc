#include "Server.hpp"
#include <iostream>

int main()
{
    Server server;
    // std::cout << "pos 1" << std::endl;
    server.OpenPipeForRead();
    // std::cout << "pos 2" << std::endl;

    std::string message;
    while (true)
    {
        if (server.RecvPipe(&message) > 0)
        {
            std::cout << "client say# " << message << std::endl;
        }
        else
            break;
        // std::cout<<"pos 3"<<std::endl;
    }

    std::cout << "client quit, me too!" << std::endl;
    server.ClosePipe();
    return 0;
}
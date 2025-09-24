#include "Client.hpp"
#include <iostream>

int main()
{
    Client client;
    client.OpenPipeForWrite();

    std::string message;
    while(true)
    {
        std::cout<<"please enter# ";
        std::getline(std::cin,message);
        client.SendPipe(message);
    }

    client.ClosePipe();
    return 0;
}
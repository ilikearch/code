#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "Common.hpp"
// ./client_tcp server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage:./client_tcp server_ip server_port" << std::endl;
        return 1;
    }
    std::string server_ip = argv[1]; // "192.168.1.1"
    int server_port = std::stoi(argv[2]);
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "create socket failed" << std::endl;
        return 2;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // client 不需要显示的进行bind, tcp是面向连接的, connect 底层会自动进行bind
    int n = ::connect(sockfd, CONV(&server_addr), sizeof(server_addr));
    if(n < 0)
    {
        std::cout << "connect failed" << std::endl;
        return 3;
    }
    // echo client
    std::string message;
    while(true)
    {
        char inbuffer[1024];
        std::cout << "input message: ";
        std::getline(std::cin, message);

        n = ::write(sockfd, message.c_str(), message.size());
        if(n > 0)
        {
            int m = ::read(sockfd, inbuffer, sizeof(inbuffer));
            if(m > 0)
            {
                inbuffer[m] = 0;
                std::cout << inbuffer << std::endl;
            }
            else
                break;
        }
        else 
            break;
    }

    ::close(sockfd);
    return 0;
}
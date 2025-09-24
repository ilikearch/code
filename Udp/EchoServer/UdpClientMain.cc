#include "UdpClient.hpp"
#include "Common.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// CS
// ./client_udp serverip serverport
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " serverip serverport" << std::endl;
        Die(USAGE_ERR);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    // 1. 创建socket
    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        Die(SOCKET_ERR);
    }

    // 1.1 填充server信息
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = ::htons(serverport);
    server.sin_addr.s_addr = ::inet_addr(serverip.c_str());

    // 2. clientdone
    while(true)
    {
        std::cout << "Please Enter# ";
        std::string message;
        std::getline(std::cin, message);
        // client 不需要bind吗？socket <-> socket
        // client必须也要有自己的ip和端口！但是客户端，不需要自己显示的调用bind！！
        // 而是，客户端首次sendto消息的时候，由OS自动进行bind
        // 1. 如何理解client自动随机bind端口号？ 一个端口号，只能被一个进程bind
        // 2. 如何理解server要显示的bind？服务器的端口号，必须稳定！！必须是众所周知且不能改变轻易改变的!
        int n = ::sendto(sockfd, message.c_str(),message.size(), 0, CONV(&server), sizeof(server));
        (void)n;

        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        char buffer[1024];
        n = ::recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, CONV(&temp), &len);
        if(n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer << std::endl;
        }
    }

    return 0;
}
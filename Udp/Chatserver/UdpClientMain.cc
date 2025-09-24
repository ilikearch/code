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
#include <pthread.h>
#include <signal.h>

int sockfd = -1;
struct sockaddr_in server;

void ClientQuit(int signo)
{
    (void)signo;
    const std::string quit = "QUIT";
    int n = ::sendto(sockfd, quit.c_str(), quit.size(), 0, CONV(&server), sizeof(server));
    exit(0);
}

void *Recver(void *args)
{
    while (true)
    {
        (void)args;
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        char buffer[1024];
        int n = ::recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, CONV(&temp), &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cerr << buffer << std::endl; // 代码没问题，重定向也没问题，管道读写同时打开，才会继续向后运行
            // fprintf(stderr, "%s\n", buffer);
            // fflush(stderr);
        }
    }
}

// CS
// ./client_udp serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " serverip serverport" << std::endl;
        Die(USAGE_ERR);
    }
    signal(2, ClientQuit);
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    // 1. 创建socket
    sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        Die(SOCKET_ERR);
    }

    // 1.1 填充server信息
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = ::htons(serverport);
    server.sin_addr.s_addr = ::inet_addr(serverip.c_str());

    pthread_t tid;
    pthread_create(&tid, nullptr, Recver, nullptr);

    // 1.2 启动的时候，给服务器推送消息即可
    const std::string online = " ... 来了哈!";
    int n = ::sendto(sockfd, online.c_str(), online.size(), 0, CONV(&server), sizeof(server));

    // 2. clientdone
    while (true)
    {
        std::cout << "Please Enter# ";
        std::string message;
        std::getline(std::cin, message);
        // client 不需要bind吗？socket <-> socket
        // client必须也要有自己的ip和端口！但是客户端，不需要自己显示的调用bind！！
        // 而是，客户端首次sendto消息的时候，由OS自动进行bind
        // 1. 如何理解client自动随机bind端口号？ 一个端口号，只能被一个进程bind
        // 2. 如何理解server要显示的bind？服务器的端口号，必须稳定！！必须是众所周知且不能改变轻易改变的!
        int n = ::sendto(sockfd, message.c_str(), message.size(), 0, CONV(&server), sizeof(server));
        (void)n;
    }

    return 0;
}
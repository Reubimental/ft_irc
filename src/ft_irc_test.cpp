
#include <arpa/inet.h>
#include "ft_irc.hpp"
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <bitset>

#define BUFF_SIZE 512

int main(int ac, char** av)
{
    (void)ac, (void)av;

    int sockfd, connfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cout << "Error creating socket" << std::endl;
        exit(1);
    }

    std::memset(&servaddr, '\0', sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6060);

    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        std::cout << "Error in bind" << std::endl;
        exit(1);
    }

    if ((listen(sockfd, 5)) != 0)
    {
        std::cout << "Error in listen" << std::endl;
        exit(1);
    }

    len = sizeof(cli);

    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0)
    {
        std::cout << "Connection failed" << std::endl;
        exit(1);
    }

    std::cout << "Connection success!" << std::endl;

    pollfd a;
    char buff[BUFF_SIZE];
    ssize_t bread;
    int pollreturn;

    std::memset(&a, 0, sizeof(a));
    a.fd = connfd;
    a.events = POLLIN | POLLHUP;

    while (true)
    {
        pollreturn = poll(&a, 1, 1000);
        std::bitset<16> hello(a.revents);
        std::cout << pollreturn << std::endl;
        std::cout << hello << std::endl;
        if (pollreturn)
        {
            if (a.revents & POLLHUP)
                break ;
            if (a.revents & POLLIN)
            {
                bread = read(connfd, buff, BUFF_SIZE - 1);
                buff[bread] = 0;
                if (bread == 0)
                    break;
                std::cout << buff << std::endl;
            }
        }
    }
    close(connfd);
    close(sockfd);
}
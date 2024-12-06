
#include "Server.hpp"
#include <stdexcept>
#include <poll.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

Server::Server()
    : Server(6667, std::string("password"))
{
}

Server::Server(int port)
    : Server(port, std::string("password"))
{
}

Server::Server(int port, const std::string& password)
    : _password(password)
{
    unsigned int len;
    struct sockaddr_in servAddr, bindPort;

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1)
    {
        throw std::runtime_error("Error creating socket");
    }

    std::memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = port;

    if (bind(_sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)))
    {
        close(_sockfd);
        throw std::runtime_error("Error binding socket");
    }

    if (listen(_sockfd, 5))
    {
        close(_sockfd);
        throw std::runtime_error("Error listening for connections");
    }
}

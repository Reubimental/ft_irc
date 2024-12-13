
#include "ft_irc.hpp"
#include "Server.hpp"
#include <stdexcept>
#include <poll.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

void Server::_Server(int port)
{
    struct sockaddr_in servAddr;

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

Server::Server()
    : _password("password")
{
    _Server(6667);
}

Server::Server(int port)
    : _password("password")
{
    _Server(port);
}

Server::Server(int port, const std::string& password)
    : _password(password)
{
    _Server(port);
}

Server::~Server()
{
    close(_sockfd);
    for (std::vector<struct pollfd>::iterator it = _sockets.begin();
            it != _sockets.end(); ++it)
    {
        close(it->fd);
    }
}

void Server::newClient(struct pollfd& pollresult)
{
    if (pollresult.revents & POLLIN)
    {
        std::cout << "Attempting new connection..." << std::endl;
        struct sockaddr_in cli;
        unsigned int len = sizeof(cli);
        int connfd = accept(_sockfd, (struct sockaddr *)&cli, &len);
        if (connfd < 0)
        {
            std::cout << "Connection failed" << std::endl;
            return ;
        }
        std::cout << "Connection success!" << std::endl;
        _clients.push_back(Client(this, connfd));
        _nicknames.push_back(_clients[_clients.size() - 1].getNicknameAddr());
        _sockets.push_back((struct pollfd){.fd = connfd, .events = 0, .revents = 0});
    }
}

bool    Server::authenticate(const std::string& password)
{
    return (password == _password);
}

bool    Server::nicknameAvailable(const std::string& nick)
{
    for (std::vector<std::string*>::iterator it = _nicknames.begin();
            it != _nicknames.end(); ++it)
    {
        if (**it == nick)
            return false;
    }
    return true;
}

Channel* Server::getChannelByName(const std::string& channel)
{
    for (std::vector<Channel>::iterator it = _channels.begin();
            it != _channels.end(); ++it)
    {
        if (it->getChannelName() == channel)
            return &(*it);
    }
    return NULL;
}

Client* Server::getClientByNick(const std::string& nick)
{
    for (std::vector<Client>::iterator it = _clients.begin();
            it != _clients.end(); ++it)
    {
        if (it->getNickname() == nick)
            return &(*it);
    }
    return NULL;
}

unsigned int	Server::findIdByNick(std::string nick)
{
	int	ID;
	for (std::vector<Client>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (nick == _clients.getNickname())
			return (_clients.getClientId());
	}
	std::cout << "Nickname does not exist in database." << std::endl;
    return NULL;
}

void    Server::removeSocket(int fd)
{
    for (std::vector<struct pollfd>::iterator it = _sockets.begin();
            it != _sockets.end(); ++it)
    {
        if (it->fd == fd)
            _sockets.erase(it);
    }
}

void   Server::removeClient(Client& client)
{
    // remove from all channels
    for (std::vector<Channel>::iterator it = _channels.begin();
            it != _channels.end(); ++it)
        it->removeClient(client.getNickname());

    // remove the socket
    removeSocket(client.getSocket());

    // remove the client from _clients
    for (std::vector<Client>::iterator it = _clients.begin();
            it != _clients.end(); ++it)
    {
        if (&(*it) == &client)
            _clients.erase(it);
    }
}

void    Server::privmsgCommand(t_message message, Client& sender)
{
    message.prefix = sender.getNickname();

    // loop through parameters, skipping the first one
    for (std::vector<std::string>::iterator param = message.params.begin() + 1;
            param != message.params.end(); ++param)
    {
        if ((*param)[0] == '#')
        {
            // get channel, send privmsg
            getChannelByName(param->substr(1));
        }
        else
        {
            // get user, send privmsg
        }
    }
}

void    Server::quitCommand(t_message message, Client& sender)
{
    (void)message;
    removeClient(sender);
}

void    Server::pongCommand(t_message message, Client& sender)
{
    (void)message;
    sender.pongCommand();
}

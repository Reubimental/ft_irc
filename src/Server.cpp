
#include "ft_irc.hpp"
#include "Server.hpp"
#include "num_responses.hpp"
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
            Channel* chan = getChannelByName(param->substr(1));
            if (!chan)
                sender.queueMessage(ERR_CANNOTSENDTOCHAN(*param));
            // send message to channel
        }
        else
        {
            Client* client = getClientByNick(*param);
            if (!client)
                sender.queueMessage(ERR_NOSUCHNICK);
            else
                client->queueMessage(message);
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

int	Channel::handleCommands(std::string input, Client& client)
{
	std::istringstream iss(input);
	std::vector<std::string> tokens;
	std::vector<std::string>::iterator it;
	std::string token;
	std::string	prefix;

	if (iss >> token)
	{
		if (token[0] == ':')
			prefix = token;
		else
			tokens.push_back(token);
	}

	while (iss >> token)
	{
		if (token[0] == ':')
		{
			std::getline(iss >> std::ws, token);
			break ;
		}
		else
			tokens.push_back(token);
	}
	if (tokens[0] == "KICK")
	{
		this->kickCommand(tokens, client);
	}
	else if (tokens[0] == "INVITE")
	{
		this->inviteCommand(tokens, client);
	}
	else if (tokens[0] == "TOPIC")
	{
		
	}
	else if (tokens[0] == "MODE")
	{
		
	}
	else if (tokens[0] == "PASS")
	{
		
	}
	else if (tokens[0] == "NICK")
	{
		
	}
	else if (tokens[0] == "USER")
	{
		
	}
	else if (tokens[0] == "JOIN")
	{
		
	}
	else if (tokens[0] == "PRIVMSG")
	{
		
	}
	else if (tokens[0] == "PONG") 
	{
		
	}
	else if (tokens[0] == "PART")
	{
		
	}
	else if (tokens[0] == "QUIT")
	{
		
	}
	else
	{
		ERR_UNKNOWNCOMMAND(tokens[0]);
	}
}
/*
			ERR_NEEDMOREPARAMS*              ERR_NOSUCHCHANNEL*
           ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
           ERR_NOTONCHANNEL
*/
void	Server::kickCommand(std::vector<std::string> tokens, Client& client)
{
	Channel* channel;

	if (tokens.size() < 3)
	{
		client.queueMessage(ERR_NEEDMOREPARAMS("KICK"));
		return ;
	}
	channel = this->getChannelByName(tokens[1]);
	if (!channel)
	{
		client.queueMessage(ERR_NOSUCHCHANNEL(tokens[1]));
		return ;
	}
	if (!channel.checkOp(client.getNickname()))
	{
		client.queueMessage(ERR_CHANOPRIVSNEEDED(tokens[1]));
		return ;
	}
	if (!channel->checkClient(client.getNickname()))
	{
		client.queueMessage(ERR_NOTONCHANNEL(tokens[1]));
		return ;
	}
	channel.removeClient(tokens[2]);
	std::cout << "User " << tokens[2] << " has been kicked from channel " << tokens[1] << "." << std::endl;
	if (tokens[3])
		std::cout << "Reason: " << tokens[3] << std::endl;
}

/*
		ERR_NEEDMOREPARAMS*              ERR_NOSUCHNICK*
        ERR_NOTONCHANNEL                ERR_USERONCHANNEL
        ERR_CHANOPRIVSNEEDED
        RPL_INVITING                    RPL_AWAY
*/
void	Server::inviteCommand(std::vector<std::string> tokens, Client& client)
{
	Channel*	channel;
	std::string	targetName = tokens[1];
	std::string	channelName = tokens[2];
	Client*		target = this->getClientByNuck(targetName);

	if (tokens.size() < 3)
	{
		client.queueMessage(ERR_NEEDMOREPARAMS("INVITE"))
		return ;
	}
	if (!target)
	{
		client.queueMessage(ERR_NOSUCHNICK);
		return ;
	}
	if (!channel.checkClient(client.getNickname()))
	{
		client.queueMessage(ERR_NOTONCHANNEL(channel->getChannelName()));
		return ;
	}
	if (channel.checkClient(targetName))
	{
		client.queueMessage(ERR_USERONCHANNEL(targetName, channelName));
		return ;
	}
	if (channel.checkInviteOnly() && !channel.checkOp(client.getNickname()))
	{
		client.queueMessage(ERR_CHANOPRIVSNEEDED(channelName));
		return ;
	}
	target->queueMessage(RPL_INVITING(channelName, targetName));
	client.queueMessage(RPL_INVITING(channelName, targetName));
	channel.addInvite(this->findIdByNick(targetName));
}

/*
	The token index that is multi-string for each:
		KICK <3>
		TOPIC <2>
		USER <4>
		PRIVMSG <2>
		QUIT <1>
*/
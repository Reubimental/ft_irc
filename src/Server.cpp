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

int	Server::handleCommands(std::string input, Client& client)
{
	std::istringstream iss(input);
	std::vector<std::string>::iterator it;
	std::string	token;
	t_message	message;

	if (iss >> token)
	{
		if (token[0] == ':')
			message.prefix = token;
		else
			message.params.push_back(token);
	}

	while (iss >> token)
	{
		if (token[0] == ':')
		{
			std::getline(iss >> std::ws, message.suffix);
			break ;
		}
		else
			message.params.push_back(token);
	}
	if (message.params[0] == "KICK")
	{
		this->kickCommand(message, client);
	}
	else if (message.params[0] == "INVITE")
	{
		this->inviteCommand(message, client);
	}
	else if (message.params[0] == "TOPIC")
	{
		this->topicCommand(message, client);
	}
	else if (message.params[0] == "MODE")
	{
		
	}
	else if (message.params[0] == "PASS")
	{
		
	}
	else if (message.params[0] == "NICK")
	{
		
	}
	else if (message.params[0] == "USER")
	{
		
	}
	else if (message.params[0] == "JOIN")
	{
		
	}
	else if (message.params[0] == "PRIVMSG")
	{
		this->privmsgCommand(message, client);
	}
	else if (message.params[0] == "PONG") 
	{
		this->pongCommand(message, client);
	}
	else if (message.params[0] == "PART")
	{
		
	}
	else if (message.params[0] == "QUIT")
	{
		this->quitCommand(message, client);
	}
	else
	{
		ERR_UNKNOWNCOMMAND(message.params[0]);
	}
}
/*
			ERR_NEEDMOREPARAMS*              ERR_NOSUCHCHANNEL*
           ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
           ERR_NOTONCHANNEL
*/
void	Server::kickCommand(t_message message, Client& sender) // KICK <#channel> <nickname> [<comment>]
{
	Channel* channel;

	if (message.params.size() < 3)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS("KICK"));
		return ;
	}
	channel = this->getChannelByName(message.params[1]);
	if (!channel)
	{
		sender.queueMessage(ERR_NOSUCHCHANNEL(message.params[1]));
		return ;
	}
	if (!channel.checkOp(sender.getNickname()))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(message.params[1]));
		return ;
	}
	if (!channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(message.params[1]));
		return ;
	}
	channel.removeClient(message.params[2]);
	std::cout << "User " << message.params[2] << " has been kicked from channel " << message.params[1] << "." << std::endl;
	if (message.suffix)
		std::cout << "Reason: " << message.suffix << std::endl;
}

/*
		ERR_NEEDMOREPARAMS*              ERR_NOSUCHNICK*
        ERR_NOTONCHANNEL                ERR_USERONCHANNEL
        ERR_CHANOPRIVSNEEDED
        RPL_INVITING                    RPL_AWAY
*/
void	Server::inviteCommand(t_message message, Client& sender) // INVITE <nickname> <#channelname>
{
	Channel*	channel;
	std::string	targetName = message.params[1];
	std::string	channelName = message.params[2];
	Client*		target = this->getClientByNick(targetName);

	if (message.params.size() < 3)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS("INVITE"))
		return ;
	}
	if (!target)
	{
		sender.queueMessage(ERR_NOSUCHNICK);
		return ;
	}
	channel = this->getChannelByName(channelName);
	if (!channel || !channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(channel->getChannelName()));
		return ;
	}
	if (channel->checkClient(targetName))
	{
		sender.queueMessage(ERR_USERONCHANNEL(targetName, channelName));
		return ;
	}
	if (channel.checkInviteOnly() && !channel.checkOp(sender.getNickname()))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(channelName));
		return ;
	}
	target->queueMessage(RPL_INVITING(channelName, targetName));
	sender.queueMessage(RPL_INVITING(channelName, targetName));
	channel.addInvite(this->findIdByNick(targetName));
}

void	Server::topicCommand(t_message message, Client& sender) // TOPIC <#channel> [<topic>]
{
	std::string	channelName = message.params[1];
	Channel*	channel;

	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS("TOPIC"));
		return ;
	}
	channel = this->findChannelByName(channelName);
	if (!channel || !channel->checkClient(sender.getNickname))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(channel->getChannelName()));
		return ;
	}
	if (!message.suffix)
	{
		if (channel->getTopic)
		{
			sender.queueMessage(RPL_TOPIC(channel->getChannelName(), channel->getTopic()));
			return ;
		}
		else
		{
			sender.queueMessage(RPL_NOTOPIC(channel->getChannelName()));
			return ;
		}
	}
	if (channel->getTopicOpAccess() && !channel->checkOp(sender.getNickname))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(channel->getChannelName()));
		return ;
	}
	channel->setTopic(message.suffix);
}
/*
		ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
        RPL_NOTOPIC                     RPL_TOPIC
        ERR_CHANOPRIVSNEEDED
*/

void	Server::passCommand(t_message message, Client& sender)
{
	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS("PASS"));
		return ;
	}
	if (sender.isRegistered())
	{
		sender.queueMessage(ERR_ALREADYREGISTRED);
		return ;
	}
	if (message.params[0] == this->_password)
	{
		sender.authenticate(message.params[0]);
		return ;
	}
}
/*
		ERR_NEEDMOREPARAMS				ERR_ALREADYREGISTRED
*/

/*
	The token index that is multi-string for each:
		KICK <3>
		TOPIC <2>
		USER <4>
		PRIVMSG <2>
		QUIT <1>
*/
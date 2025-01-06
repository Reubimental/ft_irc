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
#include <fcntl.h>

#define CREATE_POLLFD(_f, _e) (struct pollfd){.fd = (_f), .events = static_cast<short>(_e), .revents = 0}

void Server::_Server(int port)
{
    struct sockaddr_in servAddr;

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1)
    {
        throw std::runtime_error("Error creating socket");
    }
	fcntl(_sockfd, F_SETFL, O_NONBLOCK);

    std::memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

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

	std::cout << "Listening on port " << port << std::endl;
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

// note: it's best practice to avoid having more than one allocated pointer
// in a class, but I don't wanna implement something else 💀
// there also shouldn't be any explicit errors in our code inside the
// deconstructor, so it *shouldn't* leak any memory ever
Server::~Server()
{
	for (client_iter cit = _clients.begin(); cit < _clients.end(); ++cit)
		delete *cit;
	
	for (channel_iter chit = _channels.begin(); chit < _channels.end(); ++chit)
		delete *chit;

    close(_sockfd);
    for (std::vector<struct pollfd>::iterator it = _sockets.begin();
            it != _sockets.end(); ++it)
    {
        close(it->fd);
    }
}

int Server::pollSockets()
{
	_sockets.clear();

	// sockets[0] is STDIN
	_sockets.push_back(CREATE_POLLFD(0, POLLIN));
	
	// sockets[1] is _sockfd/listen
	_sockets.push_back(CREATE_POLLFD(_sockfd, POLLIN));
	for (client_iter it = _clients.begin(); it < _clients.end(); ++it)
	{
		int pollmode = POLLIN;
		if ((*it)->isQueueWaiting()) {pollmode |= POLLOUT; std::cout<<"attempting queue message"<<std::endl;}

		// sockets[>1] are client fds
	    _sockets.push_back(CREATE_POLLFD((*it)->getSocket(), pollmode));
	}
	return poll(_sockets.data(), _sockets.size(), 60000);
}

void Server::run()
{
	int numEvent;

	while (1)
	{
		// gets all pollfds and runs poll
		numEvent = pollSockets();

		if (numEvent < 0) // poll error
		{
			std::cout << "POLL ERROR" << std::endl;
			break ;
		}

		if (numEvent == 0) // poll timeout; 60 seconds without poll
		{
			std::cout << "POLL TIMEOUT" << std::endl;
			// ping clients
		}
		else // poll return!
		{
			std::cout << "POLL RETURNED" << std::endl;
			// if stdin has data, quit -- maybe add input handling later?
			if (_sockets[0].revents & POLLIN) {std::cin.ignore(); break ;}
			// if new client waiting, add it
			if (_sockets[1].revents & POLLIN) newClient();

			Client* client;
			for (socket_iter it = _sockets.begin() + 2; it < _sockets.end(); ++it)
			{
				std::cout << it->fd << ": " << it->revents << std::endl;
				if (it->revents)
				{
					client = getClientBySocket(it->fd);
					if (client->readSocket(*it) < 0)
						removeClient(*client);
				}
			}
			// ping clients
		}
	}

}

void Server::newClient()
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
	fcntl(connfd, F_SETFL, O_NONBLOCK);
	std::cout << "Connection success!" << std::endl;
	_clients.push_back(new Client(this, connfd));
	_nicknames.push_back((*_clients[_clients.size() - 1]).getNicknameAddr());
	_sockets.push_back((struct pollfd){.fd = connfd, .events = 0, .revents = 0});
}

bool    Server::nicknameAvailable(const std::string& nick)
{
    for (nick_iter it = _nicknames.begin(); it != _nicknames.end(); ++it)
    {
        if (**it == nick)
            return false;
    }
    return true;
}

Channel* Server::getChannelByName(const std::string& channel)
{
    for (channel_iter it = _channels.begin();
            it != _channels.end(); ++it)
    {
        if ((*it)->getChannelName() == channel)
            return *it;
    }
    return NULL;
}

Client* Server::getClientByNick(const std::string& nick)
{
    for (client_iter it = _clients.begin();
            it != _clients.end(); ++it)
    {
        if ((*it)->getNickname() == nick)
            return *it;
    }
    return NULL;
}

Client* Server::getClientBySocket(int socket)
{
	for (client_iter it = _clients.begin();
            it != _clients.end(); ++it)
    {
        if ((*it)->getSocket() == socket)
            return *it;
    }
    return NULL;
}

unsigned int	Server::findIdByNick(std::string nick)
{
	for (client_iter it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (nick == (*it)->getNickname())
			return ((*it)->getClientId());
	}
	std::cout << "Nickname does not exist in database." << std::endl;
    return 0;
}

void   Server::removeClient(Client& client)
{
    // remove from all channels
    for (channel_iter it = _channels.begin(); it != _channels.end(); ++it)
        (*it)->removeClient(client.getNickname());

    // remove the socket
	close(client.getSocket());

    // remove the client from _clients
    for (client_iter it = _clients.begin(); it < _clients.end(); ++it)
    {
        if (*it == &client)
		{
			delete *it;
            _clients.erase(it);
		}
    }
}

void    Server::privmsgCommand(t_message message, Client& sender)
{
    message.prefix = sender.getNickname();

    // loop through parameters, skipping the first one
    for (param_iter param = message.params.begin() + 1;
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

void	Server::handleCommands(std::string input, Client& client)
{
	std::istringstream iss(input);
	std::string	token;
	t_message	message;

	std::cout << input << std::endl;

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
	if (message.params.size() == 0)
		return ;
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
		this->passCommand(message, client);
	}
	else if (message.params[0] == "NICK")
	{
		this->nickCommand(message, client);
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
		client.queueMessage(ERR_UNKNOWNCOMMAND(message.params[0]));
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
	if (!channel->checkOp(sender.getNickname(), 0))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(message.params[1]));
		return ;
	}
	if (!channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(message.params[1]));
		return ;
	}
	channel->removeClient(message.params[2]);
	std::cout << "User " << message.params[2] << " has been kicked from channel " << message.params[1] << "." << std::endl;
	if (message.suffix.size() > 0)
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
		sender.queueMessage(ERR_NEEDMOREPARAMS("INVITE"));
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
	if (channel->checkInviteOnly() && !channel->checkOp(sender.getNickname(), 0))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(channelName));
		return ;
	}
	target->queueMessage(RPL_INVITING(channelName, targetName));
	sender.queueMessage(RPL_INVITING(channelName, targetName));
	channel->addInvite(this->findIdByNick(targetName));
}

void	Server::topicCommand(t_message message, Client& sender) // TOPIC <#channel> [<topic>]
{
	Channel*	channel;

	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS("TOPIC"));
		return ;
	}

	std::string	channelName = message.params[1];

	channel = this->getChannelByName(channelName);
	if (!channel || !channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(channel->getChannelName()));
		return ;
	}
	if (message.suffix.size() == 0)
	{
		if (channel->getTopic().size() > 0)
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
	if (channel->getTopicOpAccess() && !channel->checkOp(sender.getNickname(), 0))
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
	if (message.params[1] == this->_password)
	{
		sender.authenticate();
		return ;
	}
}
/*
		ERR_NEEDMOREPARAMS				ERR_ALREADYREGISTRED
*/


void	Server::nickCommand(t_message message, Client& sender)
{
	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NONICKNAMEGIVEN);
		return ;
	}

	std::string nickname = message.params[1];

	for (std::string::iterator it = nickname.begin(); it != nickname.end(); ++it)
	{
		if (!std::isalnum(*it) && !(*it == '_'))
		{
			sender.queueMessage(ERR_ERRONEUSNICKNAME(nickname));
			return ;
		}
	}
	if (getClientByNick(nickname))
	{
		sender.queueMessage(ERR_NICKNAMEINUSE(nickname));
		return ;
	}
}

/*
	ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
    ERR_NICKNAMEINUSE
*/

void	Server::userCommand(t_message message, Client& sender)
{
	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS("USER"));
		return ;
	}

}

/*
	ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED
	 Command: USER
   Parameters: <username> <hostname> <servername> <realname>
*/
/*
	The token index that is multi-string for each:
		KICK <3>
		TOPIC <2>
		USER <4>
		PRIVMSG <2>
		QUIT <1>
*/

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
	: _sockfd(0)
	, _clients()
	, _channels()
	, _sockets()
	, _numSockets(0)
    , _password("password")
	, _motd()
{
    _Server(6667);
	_motd.push_back("This is an MOTD!!!");
}

Server::Server(int port)
	: _sockfd(0)
	, _clients()
	, _channels()
	, _sockets()
	, _numSockets(0)
    , _password("password")
	, _motd()
{
    _Server(port);
	_motd.push_back("This is an MOTD!!!");
}

Server::Server(int port, const std::string& password)
	: _sockfd(0)
	, _clients()
	, _channels()
	, _sockets()
	, _numSockets(0)
    , _password(password)
	, _motd()
{
    _Server(port);
	_motd.push_back("This is an MOTD!!!");
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
		if ((*it)->isQueueWaiting()) pollmode |= POLLOUT;

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
		// remove clients marked for removing
		while (_clients_to_delete.size() > 0)
		{
			removeClient(_clients_to_delete.top());
			_clients_to_delete.pop();
		}

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
}

bool    Server::nicknameAvailable(const std::string& nick)
{
    for (client_iter it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->getNickname() == nick)
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
    return 0;
}

void	Server::removeClient(int client_id)
{
	for (client_iter it = _clients.begin(); it < _clients.end(); ++it)
	{
		if ((*it)->getClientId() == client_id)
		{
			removeClient(**it);
			return ;
		}
	}
}

void   Server::removeClient(Client& client)
{
    // remove from all channels
    for (channel_iter it = _channels.begin(); it != _channels.end(); ++it)
        (*it)->removeClient(client.getNickname());

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
	// incorrect, split the second command by comma, use that as recievers
    for (param_iter param = message.params.begin() + 1;
            param != message.params.end(); ++param)
    {
        if ((*param)[0] == '#')
        {
            Channel* chan = getChannelByName(*param);
            if (!chan)
                sender.queueMessage(ERR_CANNOTSENDTOCHAN(sender.getNickname(), *param));
            else
				chan->printMessage(message, sender);
        }
        else
        {
            Client* client = getClientByNick(*param);
            if (!client)
                sender.queueMessage(ERR_NOSUCHNICK(sender.getNickname()));
            else
                client->queueMessage(message);
        }
    }
}

void    Server::quitCommand(t_message message, Client& sender)
{
    (void)message;
	_clients_to_delete.push(sender.getClientId());
    // removeClient(sender);
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

	std::cout << "RECIEVED COMMAND :: " << input << std::endl;

	if (iss >> token)
	{
		if (token[0] == ':')
			message.prefix = token;
		else
			message.params.push_back(token);
	}

	while (iss.good())
	{
		if ((iss >> std::ws).peek() == ':')
		{
			iss.get();
			std::getline(iss, message.suffix);
			break ;
		}
		iss >> token;
		message.params.push_back(token);
	}
	if (message.params.size() == 0)
		return ;

	if (!client.isRegistered())
	{
		if (message.params[0] == "PASS")
			this->passCommand(message, client);
		else if (message.params[0] == "NICK" && client.isAuthenticated())
			this->nickCommand(message, client);
		else if (message.params[0] == "USER" && client.isAuthenticated())
			this->userCommand(message, client);
		else if (message.params[0] == "QUIT")
			this->quitCommand(message, client);
		else
			std::cout << "**UNREGISTERED USER ATTEMPTED NONE REGISTERING COMMAND**" << std::endl;
		return ;
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
		this->passCommand(message, client);
	}
	else if (message.params[0] == "NICK")
	{
		this->nickCommand(message, client);
	}
	else if (message.params[0] == "USER")
	{
		this->userCommand(message, client);
	}
	else if (message.params[0] == "JOIN")
	{
		this->joinCommand(message, client);
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
		client.queueMessage(ERR_UNKNOWNCOMMAND(client.getNickname(), message.params[0]));
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
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "KICK"));
		return ;
	}
	channel = this->getChannelByName(message.params[1]);
	if (!channel)
	{
		sender.queueMessage(ERR_NOSUCHCHANNEL(sender.getNickname(), message.params[1]));
		return ;
	}
	if (!channel->checkOp(sender.getNickname(), 0))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(sender.getNickname(), message.params[1]));
		return ;
	}
	if (!channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(sender.getNickname(), message.params[1]));
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

	if (message.params.size() < 3)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "INVITE"));
		return ;
	}

	std::string	targetName = message.params[1];
	std::string	channelName = message.params[2];
	Client*		target = this->getClientByNick(targetName);

	if (!target)
	{
		sender.queueMessage(ERR_NOSUCHNICK(sender.getNickname()));
		return ;
	}
	channel = this->getChannelByName(channelName);
	if (!channel || !channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(sender.getNickname(), channel->getChannelName()));
		return ;
	}
	if (channel->checkClient(targetName))
	{
		sender.queueMessage(ERR_USERONCHANNEL(sender.getNickname(), targetName, channelName));
		return ;
	}
	if (channel->checkInviteOnly() && !channel->checkOp(sender.getNickname(), 0))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(sender.getNickname(), channelName));
		return ;
	}
	target->queueMessage(RPL_INVITING(sender.getNickname(), channelName, targetName));
	sender.queueMessage(RPL_INVITING(sender.getNickname(), channelName, targetName));
	channel->addInvite(this->findIdByNick(targetName));
}

void	Server::topicCommand(t_message message, Client& sender) // TOPIC <#channel> [<topic>]
{
	Channel*	channel;

	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "TOPIC"));
		return ;
	}

	std::string	channelName = message.params[1];

	channel = this->getChannelByName(channelName);
	if (!channel || !channel->checkClient(sender.getNickname()))
	{
		sender.queueMessage(ERR_NOTONCHANNEL(sender.getNickname(), channel->getChannelName()));
		return ;
	}
	if (message.suffix.size() == 0)
	{
		if (channel->getTopic().size() > 0)
		{
			sender.queueMessage(RPL_TOPIC(sender.getNickname(), channel->getChannelName(), channel->getTopic()));
			return ;
		}
		else
		{
			sender.queueMessage(RPL_NOTOPIC(sender.getNickname(), channel->getChannelName()));
			return ;
		}
	}
	if (channel->getTopicOpAccess() && !channel->checkOp(sender.getNickname(), 0))
	{
		sender.queueMessage(ERR_CHANOPRIVSNEEDED(sender.getNickname(), channel->getChannelName()));
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
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "PASS"));
		return ;
	}
	if (sender.isRegistered())
	{
		sender.queueMessage(ERR_ALREADYREGISTRED(sender.getNickname()));
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
	if (!sender.isAuthenticated())
		return ;
	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NONICKNAMEGIVEN(sender.getNickname()));
		return ;
	}

	std::string nickname = message.params[1];

	for (std::string::iterator it = nickname.begin(); it != nickname.end(); ++it)
	{
		if (!std::isalnum(*it) && !(*it == '_'))
		{
			sender.queueMessage(ERR_ERRONEUSNICKNAME(sender.getNickname(), nickname));
			return ;
		}
	}
	if (getClientByNick(nickname))
	{
		sender.queueMessage(ERR_NICKNAMEINUSE(sender.getNickname(), nickname));
		return ;
	}
	sender.setNickname(nickname);
	if (!sender.getUsername().empty())
		sender.beRegistered();
}

/*
	ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
    ERR_NICKNAMEINUSE
*/

void	Server::userCommand(t_message message, Client& sender)
{
	if (sender.isRegistered())
	{
		sender.queueMessage(ERR_ALREADYREGISTRED(sender.getNickname()));
		return ;
	}
	if (!sender.isAuthenticated())
		return ;
	
	// check correct params
	if (!(message.params.size() >= 4 && message.suffix.size() != 0)
		|| (message.params.size() >= 5 && message.suffix.size() == 0))
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "USER"));
		return ;
	}
	sender.setUsername(message.params[1]);
	if (message.suffix.size() != 0)
		sender.setRealname(message.suffix);
	else
		sender.setRealname(message.params[4]);

	if (!sender.getNickname().empty())
		sender.beRegistered();
}

/*
	ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED
	 Command: USER
   Parameters: <username> <hostname> <servername> <realname>
*/

void	Server::partCommand(t_message message, Client& sender)
{
	Channel* channel;

	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "PART"));
		return ;
	}
	for (size_t i = 1; i != message.params.size(); i++)
	{
		channel = this->getChannelByName(message.params[i]);
		if (!channel)
		{
			sender.queueMessage(ERR_NOSUCHCHANNEL(sender.getNickname(), message.params[i]));
			return ;
		}
		if (!channel->checkClient(sender.getNickname()))
		{
			sender.queueMessage(ERR_NOTONCHANNEL(sender.getNickname(), message.params[i]));
			return ;
		}
		channel->removeClient(sender.getNickname());
	}
}

/*
	ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
    ERR_NOTONCHANNEL
*/
std::vector<std::pair<std::string, std::string> > Server::joinTokenizer(const std::string& channels_str, const std::string& passwords_str, Client& sender)
{
	std::vector<std::string> channels;
	std::vector<std::string> passwords;
	std::vector<std::pair<std::string, std::string> > tokenized;

	std::istringstream channels_stream(channels_str);
	std::string channel;
	while (std::getline(channels_stream, channel, ','))
		channels.push_back(channel);

	if (!passwords_str.empty())
	{
		std::istringstream passwords_stream(passwords_str);
		std::string password;
		while (std::getline(passwords_stream, password, ','))
			passwords.push_back(password);
	}
	else if (channels.size() > passwords.size())
	{
		passwords.resize(channels.size(), "");
	}
	if (channels.size() < passwords.size())
	{
		sender.queueMessage("Error: Provided too many passwords. Please review.");
		return (tokenized);
	}

	for (std::size_t i = 0; i < channels.size(); ++i)
	{
		std::string password = i < passwords.size() ? passwords[i] : "";
		tokenized.push_back(std::make_pair(channels[i], password));
	}
	return (tokenized);
}

void	Server::joinedChannel(Channel& channel, Client& sender, t_message joinMessage)
{
	channel.printMessage(joinMessage);
	sender.queueMessage(RPL_TOPIC(sender.getNickname(), channel.getChannelName(), channel.getTopic()));
	sender.queueMessage(RPL_NAMREPLY(sender.getNickname(), channel.getChannelName(), channel.getAllUsers()));
}

void	Server::joinCommand(t_message message, Client& sender)
{
	std::vector<std::pair<std::string, std::string> > tokenizedInput;
	std::string	channels;

	message.prefix = sender.getNickname();

	if (message.params.size() < 2)
	{
		sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "JOIN"));
		return ;
	}
	channels = message.params[1];
	std::string	passwords = message.params.size() < 2 ? message.params[2] : "";
	tokenizedInput = this->joinTokenizer(channels, passwords, sender);
	for (size_t i = 0; i < tokenizedInput.size(); i++)
	{
		if (tokenizedInput[i].first[0] != '#')
		{
			sender.queueMessage(ERR_NOSUCHCHANNEL(sender.getNickname(), tokenizedInput[i].first));
			return ;
		}
		Channel* channel = this->getChannelByName(tokenizedInput[i].first);
		if (!channel)
		{
			std::cout << "Creating channel: " << tokenizedInput[i].first << std::endl;
			Channel* newChannel = new Channel(tokenizedInput[i].first, &sender);
			this->_channels.push_back(newChannel);
			joinedChannel(*newChannel, sender, message);
			return ;
		}
		if (channel->checkInviteOnly() && !(channel->canClientJoin(sender.getClientId())))
		{
			sender.queueMessage(ERR_INVITEONLYCHAN(sender.getNickname(), tokenizedInput[i].first));
			return ;
		}
		if (channel->getPassword().size() > 0 && channel->getPassword() != tokenizedInput[i].second)
		{
			sender.queueMessage(ERR_BADCHANNELKEY(sender.getNickname(), tokenizedInput[i].first));
			return ;
		}
		if (channel->getUserLimit() && channel->getUserCount() >= channel->getUserLimit() && channel->checkOp(sender.getNickname(), 0))
		{
			sender.queueMessage(ERR_CHANNELISFULL(sender.getNickname(), tokenizedInput[i].first));
			return ;
		}
		channel->addClient(sender);
		joinedChannel(*channel, sender, message);
	}
	return ;
}

/*
Command: JOIN
   Parameters: <channel>{,<channel>} [<key>{,<key>}]

Numeric Replies:

           ERR_NEEDMOREPARAMS*              ERR_NOSUCHCHANNEL*
           ERR_INVITEONLYCHAN*               ERR_BADCHANNELKEY*
           ERR_CHANNELISFULL*
           RPL_TOPIC

		   haha also RPL_NAMREPLY :)

		   and also possible RPL_NOTOPIC cause fuck you :D
*/

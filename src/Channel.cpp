#include "Channel.hpp"
#include <iostream>
#include <vector>

static unsigned int	channelID = 1;

Channel::Channel(): _channelId(channelID)
{
	std::stringstream ss;
	ss << "Channel " << channelID;
	_channelName = ss.str();
	channelID++;
	std::cout << "Channel of ID " << getChannelId() << " has been initiated." << std::endl;
}

Channel::Channel(std::string channelName): _channelName(channelName), _channelId(channelID)
{
	channelID++;
	std::cout << "Channel of ID " << getChannelId() << " has been initiated." << std::endl;
}

Channel::~Channel()
{
	std::cout << "Channel of ID " << getChannelId() << " has been closed." << std::endl;
}

int Channel::getChannelId() const
{
	return (this->_channelId);
}

// void	Channel::setChannelId(int id)  ### REDUNDANT ###
// {
// 	this->_channelId = id;
// }

std::string	Channel::getChannelName() const
{
	return (this->_channelName);
}

void	Channel::setChannelName(const std::string& name)
{
	this->_channelName = name;
}

bool	Channel::checkClient(std::string nickname)
{
	unsigned int ID = this->findIdByNick(nickname);
	std::vector<Client>::iterator it;

	for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getClientId() == ID)
			return (true);
	}
	return (false);
}

bool	Channel::checkOp(std::string nickname, int change)
{
	unsigned int ID = findIdByNick(nickname);
	std::vector<Client>::iterator it;

	for (it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (it->getClientId() == ID)
		{
			if (change == -1)
				_operators.erase(it);
			return (true);
		}
	}
	if (change == 1)
		this->_operators.push_back(it);
	return (false);
}

void	Channel::printMessage(const std::string& message, const std::string& targetName)
{
	std::cout << targetName << ": " << message << std::endl;
}

void	Channel::broadcastMessage(const std::string& message, const std::string& targetName)
{
	std::cout << "Broadcast From " << targetName << ": " << message << std::endl;
}

bool	Channel::isFull() const
{
	if (this->_currentUserCount >= this->_userLimit)
		return (true);
	return (false);
}

void	Channel::addClient(const Client& client)
{
	if (!this->isFull())
	{
		this->_clients.push_back(client);
		this->_currentUserCount++;
		return ;
	}
	std::cout << "Sorry, Channel \"" << this->getChannelName() << "\" is full." << this->getUserCount() << "/" << this->getUserLimit() << std::endl;
}

void	addInvite(const Client& client)
{
	this->_invited.push_back(client);
	return ;
}

void	Channel::removeClient(std::string client)
{
	unsigned int clientId = findIdByNick(client);
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getClientId() == clientId)
		{
			_clients.erase(it);
			_currentUserCount--;
			break ;
		}
	}
}

void	Channel::setUserLimit(int limit)
{
	this->_userLimit = limit;
}

int	Channel::getUserCount() const
{
	return (this->_currentUserCount);
}

int	Channel::getUserLimit() const
{
	return (this->_userLimit);
}

void	Channel::setTopic(const std::string& topic)
{
	this->_channelTopic = topic;
}

std::string	Channel::getTopic() const
{
	return (this->_channelTopic);
}

bool	Channel::implementMode(char toggle, char mode)
{
	switch(mode)
	{
		case('i'):
		{
			if (toggle == '-')
				this->_inviteOnly = false;					//	unset if channel mode Invite-Only is Active.
			else if (toggle == '+')
				this->_inviteOnly = true;					//	set if channel mode Invite-Only is Inactive.
			break ;
		}
		case('t'):
		{
			if (toggle == '-' && this->_topicOpAccess)
				this->_topicOpAccess = false;
			else if (toggle == '+' && !this->_topicOpAccess)
				this->_topicOpAccess = true;
			break ;
		}
		case('k'):
		{
			std::cout << "Please input a password with this command." << std::endl;
			break ;
		}
		case('o'):
		{
			std::cout << "Please provide a NICK to op." << std::endl;
			break ;
		}
		case('l'):
		{
			std::cout << "Please input the number of users you'd like to limit this channel to." << std::endl;
			break ;
		}
		default:
		{
			return (false);
		}
	}
	return (true);
}

bool	Channel::implementMode(char toggle, char mode, std::vector<std::string> variables)
{
	switch(mode)
	{
		case('i'): // Set/Remove Invite Only channel
		{
			std::cout << "Please do not provide any further variables for this MODE." << std::endl;
		}
		case('t'): // Set/Remove The ability to use TOPIC command to Operator Only.
		{
			std::cout << "Please do not provide any further variables for this MODE." << std::endl;
		}
		case('k'): // Set/Remove the channel key (password)
		{
			if (toggle == '-' && !this->getPassword().empty())
				this->getPassword().clear();
			else if (toggle == '+')
				this->setPassword(variables[0]);
			break ;
		}
		case('o'): // Give/Take Operator privilege
		{
			if (toggle == '-')
				this->checkOp( /*   needs variable-input implementation   */  );
			else if (toggle == '+')
				this->checkOp( /*   needs variable-input implementation   */  );
			break ;
		}
		case('l'): // Set/Remove user limit on Channel
		{
			if (this->getUserLimit())
			{
				if (toggle == '-')
					this->setUserLimit(0);
				else if (toggle == '+' && std::isdigit(std::stoi(variables[0])))
					this->setUserLimit(std::stoi(variables[0]));
			}
			break ;
		}
		default:
		{
			return (false);
		}
	}
	return (true);
}

void	Channel::setPassword(std::string password)
{
	this->_password = password;
}

std::string	Channel::getPassword() const
{
	return (this->_password);
}

unsigned int	Channel::findIdByNick(std::string nick)
{
	int	ID;
	for (std::vector<Client>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (nick == _clients.getNickname())
			return (_clients.getClientId());
	}
	std::cout << "Nickname does not exist in database." << std::endl;
}

bool	Channel::checkInviteOnly()
{
	return (this->_inviteOnly);
}

//////////////////////////////////////*   SERVER STUFF   *//////////////////////////////////////

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
		client.sendMessage(ERR_NEEDMOREPARAMS("KICK"));
		return ;
	}
	channel = this->getChannelByName(tokens[1]);
	if (!channel)
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(tokens[1]));
		return ;
	}
	if (!channel.checkOp(client.getNickname()))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(tokens[1]));
		return ;
	}
	if (!channel->checkClient(client.getNickname()))
	{
		client.sendMessage(ERR_NOTONCHANNEL(tokens[1]));
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
		client.sendMessage(ERR_NEEDMOREPARAMS("INVITE"))
		return ;
	}
	if (!target)
	{
		client.sendMessage(ERR_NOSUCHNICK);
		return ;
	}
	if (!channel.checkClient(client.getNickname()))
	{
		client.sendMessage(ERR_NOTONCHANNEL(channel->getChannelName()));
		return ;
	}
	if (channel.checkClient(targetName))
	{
		client.sendMessage(ERR_USERONCHANNEL(targetName, channelName));
		return ;
	}
	if (channel.checkInviteOnly() && !channel.checkOp(client.getNickname()))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(channelName));
		return ;
	}
	target->sendMessage(RPL_INVITING(channelName, targetName));
	client.sendMessage(RPL_INVITING(channelName, targetName));
	channel.
}

/*
	The token index that is multi-string for each:
		KICK <3>
		TOPIC <2>
		USER <4>
		PRIVMSG <2>
		QUIT <1>
*/
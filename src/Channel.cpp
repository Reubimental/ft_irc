#include "Channel.hpp"
#include <iostream>
#include <vector>

static unsigned int	channelID = 1;

Channel::Channel(): _channelName("Channel " + std::to_string(channelID)), _channelId(channelID)
{
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

void	Channel::printMessage(const std::string& message, const std::string& sender)
{
	std::cout << sender << ": " << message << std::endl;
}

void	Channel::broadcastMessage(const std::string& message, const std::string& sender)
{
	std::cout << "Broadcast From " << sender << ": " << message << std::endl;
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

//////////////////////////////////////*   SERVER STUFF   *//////////////////////////////////////

int	Channel::handleCommands(std::string input)
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
			std::getline(iss >> std::ws, token);
		else
			tokens.push_back(token);
	}
	if (tokens[0] == "KICK")
	{
		this->kickCommand(tokens);
	}
	else if (tokens[0] == "INVITE")
	{

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

void	Server::kickCommand(std::vector<std::string> params)
{
	Channel* channel;
	if (tokens.size() < 3)
		client.sendMessage(ERR_NEEDMOREPARAMS("KICK"));
	channel = this->getChannelByName(tokens[1]);
	if (!channel)
		client.sendMessage(ERR_NOSUCHCHANNEL(tokens[1]));
	channel.removeClient(tokens[2]);
	std::cout << "User " << tokens[2] << " has been kicked from channel " << tokens[1] << "." << std::endl;
		if (tokens[3])
			std::cout << "Reason: " << tokens[3] << std::endl;
}
/*
	The token index that is multi-string for each:
		KICK <3>
		TOPIC <2>
		USER <4>
		PRIVMSG <2>
		QUIT <1>
*/
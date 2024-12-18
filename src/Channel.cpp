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
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		it.queueMessage(message);
	}
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

void	Channel::addInvite(const int clientId)
{
	this->_invited.push_back(clientId);
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
			break ;
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
	return (NULL);
}

bool	Channel::checkInviteOnly()
{
	return (this->_inviteOnly);
}

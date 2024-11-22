#include "Channel.hpp"
#include <iostream>

static unsigned int	channelID = 1;

Channel::Channel()
{
	setChannelId(channelID++);
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

void	Channel::setChannelId(int id)
{
	this->_channelId = id;
}

std::string	Channel::getChannelName() const
{
	return (this->_channelName);
}

void	Channel::setChannelName(const std::string& name)
{
	this->_channelName = name;
}

bool	Channel::checkClient(int clientId) const
{
	for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getClientId() == clientId)
			return (true);
	}
	return (false);
}

bool	Channel::checkOp(int clientId) const
{
	for (std::vector<Client>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (it->getClientId() == clientId)
			return (true);
	}
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
	std::cout << "Sorry, Channel " << this->getChannelName() << " is full." << this->getUserCount() << "/" << this->getUserLimit() << std::endl;
}

void	Channel::removeClient(int clientId)
{
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

void	Channel::setMode(char mode)
{
	if (!this->hasMode(mode))
	{
		this->implementMode(mode);
		return ;
	}
	this->unsetMode(mode);
}

void	Channel::unsetMode(char mode)
{
	 _modes.erase(std::remove(_modes.begin(), _modes.end(), mode), _modes.end());
}

bool	Channel::hasMode(char mode) const
{
	return (std::find(_modes.begin(), _modes.end(), mode) != _modes.end());
}

bool	Channel::implementMode(char mode)
{
	/* Implement all Mode functions here */
	switch(mode)
	{
		case('i'): // Set/Remove Invite Only channel
		{
			break ;
		}
		case('t'): // Set/Remove The ability to use TOPIC command to Operator Only.
		{
			break ;
		}
		case('k'): // Set/Remove the channel key (password)
		{
			break ;
		}
		case('o'): // Give/Take Operator privilege
		{
			break ;
		}
		case('l'): // Set/Remove user limit on Channel
		{
			break ;
		}
		default:
		{
			return ;
		}
	}
	this->_modes.push_back(mode);
}

void	Channel::setPassword(std::string password)
{
	this->_password = password;
}

std::string	Channel::getPassword() const
{
	return (this->_password);
}
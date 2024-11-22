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
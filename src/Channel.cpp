#include "Channel.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include "num_responses.hpp"
#include <string>

static unsigned int	channelID = 1;

Channel::Channel()
	: _clients()
	, _operators()
	, _invited()
	, _channelId(channelID)
	, _channelName()
	, _channelTopic()
	, _inviteOnly(false)
	, _topicOpAccess(false)
	, _password()
	, _userLimit()
{
	std::ostringstream ss;
	ss << "Channel " << channelID;
	_channelName = ss.str();
	channelID++;
	std::cout << "Channel of ID " << getChannelId() << " has been initiated." << std::endl;
}

Channel::Channel(std::string channelName, Client* sender)
	: _clients()
	, _operators()
	, _invited()
	, _channelId(channelID)
	, _channelName(channelName)
	, _channelTopic()
	, _inviteOnly(false)
	, _topicOpAccess(false)
	, _password()
	, _userLimit()
{
	channelID++;
	this->_operators.push_back(sender);
	this->_clients.push_back(sender);
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

bool Channel::getTopicOpAccess() const
{
    return _topicOpAccess;
}

std::string Channel::getAllUsers() const
{
	std::string nicks;

	for (std::vector<Client*>::const_iterator it = _clients.begin(); it < _clients.end(); ++it)
	{
		nicks.append((*it)->getNickname());
		if (it + 1 < _clients.end())
			nicks.append(" ");
	}
	return nicks;
}

void	Channel::setChannelName(const std::string& name)
{
	this->_channelName = name;
}

bool	Channel::checkClient(std::string nickname)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
			return (true);
	}
	return (false);
}

bool	Channel::checkOp(std::string nickname, int change)
{
	std::vector<Client*>::iterator it;

	for (it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			if (change == -1)
				_operators.erase(it);
			return (true);
		}
	}
	if (change == 1)
		this->_operators.push_back(*it);
	return (false);
}

void	Channel::printMessage(t_message& message)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it < _clients.end(); ++it)
	{
		(*it)->queueMessage(message);
	}
}

void	Channel::printMessage(t_message& message, Client& sender)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it < _clients.end(); ++it)
	{
		if (&sender != *it)
			(*it)->queueMessage(message);
	}
}

bool	Channel::isFull() const
{
	if (this->_userLimit &&  this->_clients.size() >= this->_userLimit)
		return (true);
	return (false);
}

void	Channel::addClient(Client& client)
{
	if (!this->isFull())
	{
		this->_clients.push_back(&client);
		return ;
	}
	std::cout << "Sorry, Channel \"" << this->getChannelName() << "\" is full." << this->getUserCount() << "/" << this->getUserLimit() << std::endl;
}

void	Channel::addInvite(unsigned int clientId)
{
	std::vector<unsigned int>::iterator it = std::find(_invited.begin(), _invited.end(), clientId);
	if (it == _invited.end())
		this->_invited.push_back(clientId);
}

void	Channel::removeClient(std::string clientNickname)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNickname() == clientNickname)
		{
			_clients.erase(it);
			break ;
		}
	}
	for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getNickname() == clientNickname)
		{
			_operators.erase(it);
			break ;
		}
	}
}

void	Channel::setUserLimit(int limit)
{
	this->_userLimit = limit;
}

unsigned int	Channel::getUserCount() const
{
	return (this->_clients.size());
}

unsigned int	Channel::getUserLimit() const
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

void	Channel::implementMode(char toggle, char mode, std::string params, Client& sender)
{
	switch(mode)
	{
		case('i'): // Set/Remove Invite Only channel

			if (toggle == '-')
				this->_inviteOnly = false;
			else if (toggle == '+')
				this->_inviteOnly = true;
			break ;

		case('t'): // Set/Remove The ability to use TOPIC command to Operator Only.

			if (toggle == '-' && this->_topicOpAccess)
				this->_topicOpAccess = false;
			else if (toggle == '+' && !this->_topicOpAccess)
				this->_topicOpAccess = true;
			break ;

		case('k'): // Set/Remove the channel key (password)

			if (toggle == '-' && !this->getPassword().empty())
				this->getPassword().clear();
			else if (toggle == '+')
				this->setPassword(params);
			break ;

		case('o'): // Give/Take Operator privilege

            modeOperator(params, sender, toggle);
			break;

        case('l'): // Set/Remove user limit on Channel

			if (toggle == '-')
			{
				this->setUserLimit(0);
			}
			else if (toggle == '+')
			{
				char *end;
				int newLimit = std::strtol(params.c_str(), &end, 10);
				if (*end == '\0')
				{
					this->setUserLimit(newLimit);
				}
				else
				{
					std::cout << "Error: Failed to set User Limit. Please check you have correctly entered a number." << std::endl;
				}
			}
			break ;

		default:
			sender.queueMessage(ERR_UNKNOWNMODE(sender.getNickname(), mode));
	}
}

void Channel::modeOperator(std::string params, Client &sender, char toggle)
{
    if (params.empty())
    {
        sender.queueMessage(ERR_NEEDMOREPARAMS(sender.getNickname(), "MODE"));
		return;
    }
    if (toggle == '-')
    {
		for (std::vector<Client*>::iterator cit = this->_operators.begin(); cit != this->_operators.end(); ++cit)
		{
			if ((*cit)->getNickname() == params)
			{
				this->_operators.erase(cit);
				return;
			}
		}
		sender.queueMessage(ERR_NOSUCHNICK(sender.getNickname()));
	}
    else if (toggle == '+')
    {
		for (std::vector<Client*>::iterator opit = _operators.begin(); opit != _operators.end(); ++opit)
		{
			if ((*opit)->getNickname() == params)
				return ;
		}
		for (std::vector<Client*>::iterator cit = _clients.begin(); cit != _clients.end(); ++cit)
		{
			if ((*cit)->getNickname() == params)
			{
				_operators.push_back(*cit);
				return ;
			}
        }
		sender.queueMessage(ERR_NOSUCHNICK(sender.getNickname()));
    }
}

void	Channel::setPassword(std::string password)
{
	this->_password = password;
}

std::string	Channel::getPassword() const
{
	return (this->_password);
}

bool	Channel::checkInviteOnly()
{
	return (this->_inviteOnly);
}

bool Channel::canClientJoin(unsigned int clientID)
{
	if (!_inviteOnly)
		return (true);
	std::vector<unsigned int>::iterator it = std::find(_invited.begin(), _invited.end(), clientID);
	return (it != _invited.end());
}

void	Channel::modeIs(Client &sender)
{
	std::ostringstream	mode_s;
	std::ostringstream	details_s;
	std::string mode = "+";
	std::string details = "";
	int userLimit = this->getUserLimit();

	if (this->checkInviteOnly())
		mode_s << "i";
	if (this->getPassword() != "")
		mode_s << "k";
	if (userLimit != 0)
		mode_s << "l";
	for (uint i = 0; i < _operators.size(); ++i) mode_s << "o";
	if (this->getTopicOpAccess())
		mode_s << "t";
	if (userLimit != 0)
		details_s << userLimit;
	for (std::vector<Client *>::iterator it = this->_operators.begin(); it != this->_operators.end(); ++it)
	{
		if (it == _operators.begin())
			details_s << (*it)->getNickname();
		else
			details_s << " " << (*it)->getNickname();
	}
	mode.append(mode_s.str());
	details.append(details_s.str());
	sender.queueMessage(RPL_CHANNELMODEIS(sender.getNickname(), this->getChannelName(), mode, details));
}

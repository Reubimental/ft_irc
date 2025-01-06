#include "Channel.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include "num_responses.hpp"

static unsigned int	channelID = 1;

Channel::Channel(): _channelId(channelID)
{
	std::ostringstream ss;
	ss << "Channel " << channelID;
	_channelName = ss.str();
	channelID++;
	std::cout << "Channel of ID " << getChannelId() << " has been initiated." << std::endl;
}

Channel::Channel(std::string channelName): _channelId(channelID), _channelName(channelName)
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

bool Channel::getTopicOpAccess() const
{
    return _topicOpAccess;
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

void	Channel::printMessage(const std::string& message, const std::string& targetName)
{
	std::cout << targetName << ": " << message << std::endl;
}

void	Channel::broadcastMessage(const std::string& message, const std::string& targetName)
{
	(void)targetName;
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		(*it)->queueMessage(message);
	}
}

bool	Channel::isFull() const
{
	if (this->_clients.size() >= this->_userLimit)
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
}

void	Channel::setUserLimit(int limit)
{
	this->_userLimit = limit;
}

int	Channel::getUserCount() const
{
	return (this->_clients.size());
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

void	Channel::implementMode(char toggle, char mode, std::vector<std::string> params, Client& sender)
{
	switch(mode)
	{
		case('i'): // Set/Remove Invite Only channel

			if (toggle == '-')
				this->_inviteOnly = false;
			else if (toggle == '+')
				this->_inviteOnly = true;
			sender.queueMessage(RPL_CHANNELMODEIS(_channelName, mode, _inviteOnly));
			break ;

		case('t'): // Set/Remove The ability to use TOPIC command to Operator Only.

			if (toggle == '-' && this->_topicOpAccess)
				this->_topicOpAccess = false;
			else if (toggle == '+' && !this->_topicOpAccess)
				this->_topicOpAccess = true;
			sender.queueMessage(RPL_CHANNELMODEIS(_channelName, mode, _topicOpAccess));
			break ;

		case('k'): // Set/Remove the channel key (password)

			if (toggle == '-' && !this->getPassword().empty())
				this->getPassword().clear();
			else if (toggle == '+')
				this->setPassword(params[0]);
			sender.queueMessage(RPL_CHANNELMODEIS(_channelName, mode, (_password.size() != 0)));
			break ;

		case('o'): // Give/Take Operator privilege

            modeOperator(params, sender, toggle, mode);
			break;

        case('l'): // Set/Remove user limit on Channel

			if (this->getUserLimit())
			{
				if (toggle == '-')
				{
					this->setUserLimit(0);
					// sender recieves RPL_CHANNELMODEIS
				}
				else if (toggle == '+')
				{
					char *end;
					int newLimit = std::strtol(params[0].c_str(), &end, 10);
					if (!end)
					{
						this->setUserLimit(newLimit);
						// sender recieves RPL_CHANNELMODEIS
					}
					else
					{} // sender recieves ??? I feel like this should have an error, but I can't find what applies
				}
			}
			break ;

		default:
			sender.queueMessage(ERR_UNKNOWNMODE(mode));
	}
}

void Channel::modeOperator(std::vector<std::string> &params, Client &sender, char toggle, char &mode)
{
    if (params.size() < 1)
    {
        sender.queueMessage(ERR_NEEDMOREPARAMS("MODE"));
		return;
    }
    if (toggle == '-')
    {
        for (std::vector<std::string>::iterator opNick = params.begin(); opNick != params.end(); ++opNick)
        {
            for (std::vector<Client*>::iterator cit = _operators.begin(); cit != _operators.end(); ++cit)
            {
                if ((*cit)->getNickname() == *opNick)
                {
                    _operators.erase(cit);
                    break;
                }
            }
        }
    }
    else if (toggle == '+')
    {
		for (std::vector<std::string>::iterator opNick = params.begin(); opNick != params.end(); ++opNick)
        {
			bool willAdd = true;
            for (std::vector<Client*>::iterator opit = _operators.begin(); opit != _operators.end(); ++opit)
            {
                if ((*opit)->getNickname() == *opNick)
                {
					willAdd = false;
                    break;
                }
            }
			if (willAdd)
			{
				for (std::vector<Client*>::iterator cit = _clients.begin(); cit != _clients.end(); ++cit)
				{
					if ((*cit)->getNickname() == *opNick)
					{
						_operators.push_back(*cit);
						break ;
					}
				}
			}
        }
    }
    sender.queueMessage(RPL_CHANNELMODEIS(_channelName, mode, "There are " << _operators.size() << " operators"));
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

void Channel::modeCommand(t_message message, Client &sender)
{
	char toggle = 0;
	char mode;
	
	if (message.params[1].size() == 1)
		mode = message.params[1][0];
	else if (message.params[1].size() == 2)
	{
		toggle = message.params[1][0];
		mode = message.params[1][1];
	}
	else
		return ;
	std::vector<std::string> params(message.params.begin() + 2, message.params.end());
	implementMode(toggle, mode, params, sender);
}

#pragma once

#include <vector>
#include <queue>
#include "Client.hpp"
#include <string>
#include <algorithm>

std::vector<std::string> validOpCommands =
	{ "KICK",
	"INVITE",
	"TOPIC",
	"MODE",
	"PASS",
	"NICK",
	"USER",
	"JOIN",
	"PRIVMSG",
	"PONG",
	"PART",
	"QUIT" };

struct Commands
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
};

class Channel
{
	private:
		std::vector<Client>		_clients;
		std::vector<Client>		_operators;
		std::vector<int>		_invited;
		std::queue<std::string>	_messageBuffer;
        unsigned int			_channelId;
		std::string				_channelName;
		std::string				_channelTopic;
		unsigned int			_currentUserCount;
		/*   MODES   */
		bool		_inviteOnly;
		bool		_topicOpAccess;
		std::string	_password;
		int			_userLimit;
	public:
		Channel();
		Channel(std::string channelName);
		~Channel();
		/*   Setters   */
		// void	setChannelId(int id); ### REDUNDANT ###
		void	setChannelName(const std::string& name);
		/*   Getters   */
		int			getChannelId() const;
		std::string	getChannelName() const;
		/*   Client Auth Check   */
		bool	checkOp(std::string nickname, int change);
		bool	checkClient(std::string nickname);
		/*   Print sent messages   */
		void	printMessage(const std::string& message, const std::string& sender);
		void	broadcastMessage(const std::string& message, const std::string& sender);
		/*   Client Manipulation   */
		bool	isFull() const;
		void	addClient(const Client& client);
		void	addInvite(const int clientId);
		void	removeClient(std::string client);
		void	setUserLimit(int limit);
		int		getUserCount() const;
		int		getUserLimit() const;
		/*   Topic   */
		void		setTopic(const std::string& topic);
		std::string	getTopic() const;
		/*   Modes   */
		bool	checkInviteOnly();
		bool	implementMode(char toggle, char mode);
		bool	implementMode(char toggle, char mode, std::vector<std::string> variables);
		/*   Passwords   */
		void		setPassword(std::string password);
		std::string	getPassword() const;
		/*   Utility   */
		unsigned int	findIdByNick(std::string nick);

/////////////////*   SERVER STUFF   */////////////////
		int		handleCommands(std::string input);
};

#pragma once

#include <vector>
#include <queue>
#include "Client.hpp"
#include <string>
#include <algorithm>

struct Commands
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
};

class Channel
{
	private:
		Channel();

		std::vector<Client*>		_clients;
		std::vector<Client*>		_operators;
		std::vector<unsigned int>	_invited;
        unsigned int				_channelId;
		std::string					_channelName;
		std::string					_channelTopic;
		/*   MODES   */
		bool		_inviteOnly;
		bool		_topicOpAccess;
		std::string	_password;
		unsigned int			_userLimit;

        void modeOperator(std::vector<std::__cxx11::string> &params, Client &sender, char toggle, char &mode);

	public:
		Channel(std::string channelName, Client* sender);
		~Channel();
		/*   Setters   */
		// void	setChannelId(int id); ### REDUNDANT ###
		void	setChannelName(const std::string& name);
		/*   Getters   */
		int			getChannelId() const;
		std::string	getChannelName() const;
		bool		getTopicOpAccess() const;
		/*   Client Auth Check   */
		bool	checkOp(std::string nickname, int change);
		bool	checkClient(std::string nickname);
		/*   Print sent messages   */
		void	printMessage(const std::string& message, const std::string& sender);
		void	broadcastMessage(const std::string& message, const std::string& sender);
		/*   Client Manipulation   */
		bool	isFull() const;
		void	addClient(Client& client);
		void	addInvite(unsigned int clientId);
		void	removeClient(std::string clientNickname);
		void	setUserLimit(int limit);
		int		getUserCount() const;
		int		getUserLimit() const;
		/*   Topic   */
		void		setTopic(const std::string& topic);
		std::string	getTopic() const;
		/*   Modes   */
		bool	checkInviteOnly();
		bool	canClientJoin(unsigned int clientID);
		void	modeCommand(t_message message, Client& sender);
        void	implementMode(char toggle, char mode, std::vector<std::string> params, Client &sender);
        /*   Passwords   */
		void		setPassword(std::string password);
		std::string	getPassword() const;
};

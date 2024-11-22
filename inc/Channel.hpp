#pragma once

#include <vector>
#include <queue>
#include "Client.hpp"
#include <string>
#include <algorithm>

class Channel
{
	private:
		std::vector<Client>		_clients;
		std::vector<Client>		_operators;
		std::queue<std::string>	_messageBuffer;
        int						_channelId;

		std::string				_channelName;
		std::string				_channelTopic;
		std::vector<char>		_modes;
		std::string				_password;
		int						_userLimit;
		int						_currentUserCount;
	public:
		Channel();
		~Channel();
		/*   Setters   */
		void	setChannelId(int id);
		void	setChannelName(const std::string& name);
		/*   Getters   */
		int			getChannelId() const;
		std::string	getChannelName() const;
		/*   Client Auth Check   */
		bool	checkOp(int clientId) const;
		bool	checkClient(int clientId) const;
		/*   Print sent messages   */
		void	printMessage(const std::string& message, const std::string& sender);
		void	broadcastMessage(const std::string& message, const std::string& sender);
		/*   Client Manipulation   */
		bool	isFull() const;
		void	addClient(const Client& client);
		void	removeClient(int clientId);
		void	setUserLimit(int limit);
		int		getUserLimit() const;
		/*   Topic   */
		void		setTopic(const std::string& topic);
		std::string	getTopic() const;
		/*   Modes   */
		void	setMode(char mode);
		void	unsetMode(char mode);
		bool	hasMode(char mode);
		/*   Passwords   */
		void		setPassword(std::string password);
		std::string	getPassword() const;
};
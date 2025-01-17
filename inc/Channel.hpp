#pragma once

#include <vector>
#include <queue>
#include "Client.hpp"
#include <string>
#include <algorithm>

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
		bool			_inviteOnly;
		bool			_topicOpAccess;
		std::string		_password;
		unsigned int	_userLimit;

        void modeOperator(std::string params, Client &sender, char toggle);

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
		std::string getAllUsers() const;
		/*   Client Auth Check   */
		bool	checkOp(std::string nickname, int change=0);
		bool	checkClient(std::string nickname);
		/*   Print sent messages   */
		void	printMessage(t_message& message);
		void	printMessage(t_message& message, Client& sender);
		/*   Client Manipulation   */
		bool	isFull() const;
		void	addClient(Client& client);
		void	addInvite(unsigned int clientId);
		void	removeClient(std::string clientNickname);
		void	setUserLimit(int limit);
		unsigned int		getUserCount() const;
		unsigned int		getUserLimit() const;
		/*   Topic   */
		void		setTopic(const std::string& topic);
		std::string	getTopic() const;
		/*   Modes   */
		bool	checkInviteOnly();
		bool	canClientJoin(unsigned int clientID);
        void	implementMode(char toggle, char mode, std::string param, Client &sender);
		void	modeIs(Client &sender);
		void	modeChanged(std::string changer, char toggle, char mode, std::string param=std::string());
        /*   Passwords   */
		void		setPassword(std::string password);
		void		clearPassword();
		std::string	getPassword() const;
};

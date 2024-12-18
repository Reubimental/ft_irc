
#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "ft_irc.hpp"
#include <vector>

/* temparary line */
class Channel;

class Server
{
private:
	Server(const Server&);
	Server& operator=(const Server&);

	void _Server(int port);
	void removeSocket(int fd);
	void removeClient(Client& client);
	
	int							_sockfd;
	std::vector<Client>			_clients; // needs to change to vector<Client*>
	std::vector<Channel>		_channels;
	std::vector<struct pollfd>	_sockets;
	int							_numSockets;
	std::string					_password;
	std::vector<std::string*>	_nicknames;
	std::vector<std::string>	_motd;


public:
	Server();
	Server(int port);
	Server(int port, const std::string& password);

	~Server();

	void run();

	void doCommand(const std::string& cmdstr, Client&);
	void newClient(struct pollfd& pollresult);
	bool nicknameAvailable(const std::string& nick);
	Channel* getChannelByName(const std::string& channel);
	Client* getClientByNick(const std::string& nick);
	unsigned int	findIdByNick(std::string nick);
	void		handleCommands(std::string input, Client& client);

	// SERVER COMMANDS
	void    passCommand     (t_message message, Client& sender);
	void    nickCommand     (t_message message, Client& sender);
	void    userCommand     (t_message message, Client& sender);
	void    joinCommand     (t_message message, Client& sender);
	void    privmsgCommand  (t_message message, Client& sender);
	void    quitCommand     (t_message message, Client& sender);

	// CHANNEL COMMANDS
	void    kickCommand     (t_message message, Client& sender);
	void    inviteCommand   (t_message message, Client& sender);
	void    topicCommand    (t_message message, Client& sender);
	void    modeCommand     (t_message message, Client& sender);
	void    partCommand     (t_message message, Client& sender);

	// literally nothing command
	void    pongCommand     (t_message message, Client& sender);
};

// { "KICK",
// "INVITE",
// "TOPIC",
// "MODE",
// "PASS",
// "NICK",
// "USER",
// "JOIN",
// "PRIVMSG",
// "PONG",
// "PART",
// "QUIT" };

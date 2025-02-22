
#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "ft_irc.hpp"
#include <vector>
#include <stack>

/* temparary line */
class Channel;

class Server
{
private:
	Server(const Server&);
	Server& operator=(const Server&);

	void _Server(int port);
	void removeSocket(int fd);
	void removeClient(int client_id);
	void removeClient(Client& client);
    int  pollSockets();
	
	int							_sockfd;
	std::vector<Client*>		_clients;
	std::vector<Channel*>		_channels;
	std::vector<struct pollfd>	_sockets;
	int							_numSockets;
	std::string					_password;
	std::vector<std::string>	_motd;
	std::stack<int>			_clients_to_delete;

    typedef std::vector<Client*>::iterator client_iter;
    typedef std::vector<Channel*>::iterator channel_iter;
    typedef std::vector<struct pollfd>::iterator socket_iter;

public:
	Server();
	Server(int port);
	Server(int port, const std::string& password);

	~Server();

	void run();

	void            newClient();
	bool            nicknameAvailable(const std::string& nick);
	Channel*        getChannelByName(const std::string& channel);
	Client*         getClientByNick(const std::string& nick);
    Client*         getClientBySocket(int fd);
	unsigned int    findIdByNick(std::string nick);
	void            handleCommands(std::string input, Client& client);

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

	// COMMAND HELPERS
	void	joinedChannel	(Channel& channel, Client& sender, t_message joinMessage);
	std::vector<std::pair<std::string, std::string> > joinTokenizer(const std::string& channels_str, const std::string& passwords_str, Client& sender);
	std::vector<std::pair<char, std::string> > modeTokenizer(char& toggle, Channel& channel, Client& sender, std::vector<std::string> params);
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

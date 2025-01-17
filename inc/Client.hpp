
#pragma once

#include "ft_irc.hpp"
#include <poll.h>
#include <string>
#include <queue>

class Server; // defined in "Server.hpp"

class Client
{
	private:
		Server*					_server;
		int						_connfd;
		bool					_isAuthenticated;
		bool					_isRegistered;
		time_t					_lastSeen;
		unsigned int			_clientId;
		std::string				_recieveBuffer;
		std::string				_nickname;
		std::string				_username;
		std::string 			_realname;
		std::queue<std::string>	_sendBuffer;
		bool					_isPinging;
		time_t					_pingSentAt;

		void	sendMessage(const std::string& message);
		void	seen(); // sets lastSeen to the current time

	public:
		Client(Server* server, int connfd);
		Client(const Client&);
		~Client();

		Client& operator=(const Client&);

		int					getSocket() const;
		bool				isAuthenticated() const;
		bool				isRegistered() const;
		time_t				getLastSeen() const;
		int					getClientId() const;
		const std::string& 	getMessageBuffer() const;
		const std::string& 	getNickname() const;
		std::string*		getNicknameAddr();
		const std::string& 	getUsername() const;
		const std::string& 	getRealname() const;
		bool				isPinging() const;
		time_t				getPingSentAt() const;
		bool				isQueueWaiting() const;// returns true if the 
												   // message queue is not empty

		void	authenticate();
		void	beRegistered(); // may be done when both nick & user commands
								// have been recieved, instead of as a setter


		// void	setClientId(int newId); // shouldn't be changed?

		void	setRecieveBuffer(const std::string&);

		// setNickname: returns 1 if nickname was successfully set, or -1 if
		// there was an error, e.g. nickname collision
		int		setNickname(const std::string&);
		void	setUsername(const std::string& newUsername);
		void	setRealname(const std::string& newRealname);

		void	sendPing();
		void	pongCommand();
		
		// return negative marks client as disconnected
		int		readSocket(struct pollfd& pollresult);

		// queues a message to send
		void	queueMessage(const std::string&);
		void	queueMessage(t_message message);
};

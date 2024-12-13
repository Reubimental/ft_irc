
#pragma once

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
		bool					_isRegestered;
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
		bool				isRegestered() const;
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

		void	authenticate(const std::string& password);
		void	beRegistered(); // may be done when both nick & user commands
								// have been recieved, instead of as a setter


		// void	setClientId(int newId); // shouldn't be changed?

		void	setRecieveBuffer(const std::string&);

		// setNickname: returns 1 if nickname was successfully set, or -1 if
		// there was an error, e.g. nickname collision
		int		setNickname(const std::string&);

		// void	setUsername(const std::string& newUsername); // I believe
				// this shouldn't be changable

		// void`setRealname(const std::string& newRealname); // do we even use 
				// this member? also I don't think it's meant to change

		void	sendPing();
		void	pongCommand();
		
		void	readSocket(struct pollfd& pollresult);

		// queues a message to send
		void	queueMessage(const std::string&);
};

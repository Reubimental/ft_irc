
#pragma once

#include <poll.h>
#include <string>
#include <queue>

class Client
{
	private:
		struct pollfd&			_socket;
		bool					_isAuthenticated;
		bool					_isRegestered;
		time_t					_lastPing;
		unsigned int			_clientId;
		std::string				_messageBuffer;
		std::string				_nickname;
		std::string				_username;
		std::string 			_realname;
		std::queue<std::string>	_sendBuffer;

	public:
		Client(struct pollfd& socket);
		Client(const Client&);
		~Client();

		Client& operator=(const Client&);

		const struct pollfd&		getSocket() const;
		bool				isAuthenticated() const;
		bool				isRegestered() const;
		time_t				getLastPing() const;
		int					getClientId() const;
		const std::string 	getMessageBuffer() const;
		const std::string 	getNickname() const;
		const std::string 	getUsername() const;
		const std::string 	getRealname() const;

		void	authenticate(); // not sure if this will need a parameter
		void	beRegistered(); // may be done when both nick & user commands
								// have been recieved, instead of as a setter

		void	ping(); // sets lastPing to the current time

		// void	setClientId(int newId); // shouldn't be changed?

		void	setMessageBuffer(const std::string& newBuffer);
		void	setNickname(const std::string& newNickname);

		// void	setUsername(const std::string& newUsername); // I believe
				// this shouldn't be changable

		// void`setRealname(const std::string& newRealname); // do we even use 
				// this member? also I don't think it's meant to change
		

		void	queueMessage(const std::string& message);
		/*
		attempts to send a message, or if it is unable, add it to the
		send queue
		*/


};

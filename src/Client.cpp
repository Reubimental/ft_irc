
#include <poll.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "Client.hpp"
#include "Server.hpp"

static unsigned int numClients = 0;

Client::Client(Server& server, struct pollfd& socket)
    // : _server(server)
    : _socket(socket)
    , _lastPing(time(NULL))
    , _clientId(++numClients)
{
    std::cout << "Initializing Client ID " << _clientId << std::endl;
}

Client::Client(const Client& a)
    // : _server(a._server)
    : _socket(a._socket)
    , _isAuthenticated(a._isAuthenticated)
    , _isRegestered(a._isRegestered)
    , _lastPing(a._lastPing)
    , _clientId(a._clientId)
    , _recieveBuffer(a._recieveBuffer)
    , _nickname(a._nickname)
    , _username(a._username)
    , _realname(a._realname)
    , _sendBuffer(a._sendBuffer)
{
    std::cout << "Copying Client -- don't do this haha" << std::endl;
}

Client::~Client()
{
    std::cout << "Removing Client ID " << _clientId << std::endl;
}

Client& Client::operator=(const Client& a)
{
    if (this == &a)
        return (*this);

    std::cout << "Assigning Client -- don't do this haha -- OBJECT LEFT UNTOUCHED" << std::endl;

    return (*this);
}


const pollfd &Client::getSocket() const
{
    return (_socket);
}

bool Client::isAuthenticated() const
{
    return _isAuthenticated;
}

bool Client::isRegestered() const
{
    return _isRegestered;
}

time_t Client::getLastPing() const
{
    return _lastPing;
}

int Client::getClientId() const
{
    return _clientId;
}

const std::string Client::getMessageBuffer() const
{
    return _recieveBuffer;
}

const std::string Client::getNickname() const
{
    return _nickname;
}

const std::string Client::getUsername() const
{
    return _username;
}

const std::string Client::getRealname() const
{
    return _realname;
}


void Client::authenticate(const std::string& password)
{
    // if (!_isAuthenticated)
    //     _isAuthenticated = _server.authenticate(password)
    // else
        // respod already registered
        return ;
}

void Client::beRegistered()
{
    _isRegestered = true;
}

void Client::ping()
{
    time(&_lastPing);
}

void Client::setRecieveBuffer(const std::string& newBuffer)
{
    std::cout << "Setting message buffer -- did you mean to do this?" << std::endl;
    _recieveBuffer = newBuffer;
}

int Client::setNickname(const std::string& newNickname)
{
//    if (_server.nicknameAvailable(newNickname))
    if (true)
    {
        _nickname = newNickname;
        return (1);
    }
    else
    {
        // return error to sender
        return (-1);
    }
}

void Client::readSocket()
{
    // read from _socket

    if (false /*a full message is obtained -- ends with either \n or \r*/)
        // _server.do_command
        std::cout << "message recieved!" << std::endl;
}

void    Client::queueMessage(const std::string& message)
{
    std::cout << "NOT FULLY IMPLEMENTED, NO MESSAGES SENT" << std::endl;
    
    if (true /*check socket*/)
    {
        if (!_sendBuffer.empty())
        {
            while (!_sendBuffer.empty())
            {
                // send message
                _sendBuffer.pop();
            }
            // attempt to send old messages
        }
        sendMessage(message);
    }
    else
        // can't send messages, add to queue instead
        _sendBuffer.push(message);
}

void    Client::clearQueue()
{
    if (true /*check socket*/ && !_sendBuffer.empty())
    {
        while (!_sendBuffer.empty())
        {
            // send message
            _sendBuffer.pop();
        }

    }
}

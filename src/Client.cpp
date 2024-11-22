
#include <poll.h>
#include <string>
#include <iostream>
#include "Client.hpp"

static unsigned int numClients = 0;

Client::Client(struct pollfd& socket)
    : _socket(socket)
    , _lastPing(time(NULL))
    , _clientId(++numClients)
{
    std::cout << "Constructing Client" << std::endl;
}

Client::Client(const Client& a)
    : _socket(a._socket)
    , _isAuthenticated(a._isAuthenticated)
    , _isRegestered(a._isRegestered)
    , _lastPing(a._lastPing)
    , _clientId(a._clientId)
    , _messageBuffer(a._messageBuffer)
    , _nickname(a._nickname)
    , _username(a._username)
    , _realname(a._realname)
    , _sendBuffer(a._sendBuffer)
{
    std::cout << "Copying Client -- don't do this haha" << std::endl;
}

Client::~Client()
{
    std::cout << "Destroying Client" << std::endl;
}

Client& Client::operator=(const Client& a)
{
    if (this == &a)
        return (*this);

    std::cout << "Assigning Client -- don't do this haha" << std::endl;

    // this makes no sense btw
    _socket = a._socket;
    _isAuthenticated = a._isAuthenticated;
    _isRegestered = a._isRegestered;
    _lastPing = a._lastPing;
    _clientId = a._clientId;
    _messageBuffer = a._messageBuffer;
    _nickname = a._nickname;
    _username = a._username;
    _realname = a._realname;
    _sendBuffer = a._sendBuffer;

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
    return _messageBuffer;
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



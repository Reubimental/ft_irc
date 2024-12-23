
#include <poll.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Client.hpp"
#include "Server.hpp"

static unsigned int numClients = 0;

Client::Client(Server* server, int connfd)
    : _server(server)
    , _connfd(connfd)
    , _lastSeen(time(NULL))
    , _clientId(++numClients)
{
    std::cout << "Initializing Client ID " << _clientId << std::endl;
}

Client::Client(const Client& a)
    : _server(a._server)
    , _connfd(a._connfd)
    , _isAuthenticated(a._isAuthenticated)
    , _isRegistered(a._isRegistered)
    , _lastSeen(a._lastSeen)
    , _clientId(a._clientId)
    , _recieveBuffer(a._recieveBuffer)
    , _nickname(a._nickname)
    , _username(a._username)
    , _realname(a._realname)
    , _sendBuffer(a._sendBuffer)
    , _isPinging(a._isPinging)
    , _pingSentAt(a._pingSentAt)
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


int Client::getSocket() const
{
    return (_connfd);
}

bool Client::isAuthenticated() const
{
    return _isAuthenticated;
}

bool Client::isRegistered() const
{
    return _isRegistered;
}

time_t Client::getLastSeen() const
{
    return _lastSeen;
}

int Client::getClientId() const
{
    return _clientId;
}

const std::string& Client::getMessageBuffer() const
{
    return _recieveBuffer;
}

const std::string& Client::getNickname() const
{
    return _nickname;
}

std::string* Client::getNicknameAddr()
{
    return &_nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

const std::string& Client::getRealname() const
{
    return _realname;
}

bool Client::isPinging() const
{
    return _isPinging;
}

time_t Client::getPingSentAt() const
{
    return _pingSentAt;
}

bool Client::isQueueWaiting() const
{
    return _sendBuffer.size();
}

void Client::authenticate()
{
    _isAuthenticated = true;
}

void Client::beRegistered()
{
    _isRegistered = true;
}

void Client::seen()
{
    time(&_lastSeen);
}

void Client::setRecieveBuffer(const std::string& newBuffer)
{
    std::cout << "Setting message buffer -- did you mean to do this?" << std::endl;
    _recieveBuffer = newBuffer;
}

int Client::setNickname(const std::string& newNickname)
{
    if (_server->nicknameAvailable(newNickname))
    // if (true)
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

void Client::sendPing()
{
    queueMessage("PING");
}

void Client::pongCommand()
{
    _isPinging = false;
}

// ****************************************** TODO ************
void Client::readSocket(struct pollfd& pollresult)
{
    if (pollresult.revents & POLLIN)
        return ; // read here // also set lastSeen
    // read from _socket

    if (pollresult.revents & POLLOUT)
        return ; // send here

    if (false /*a full message is obtained -- ends with either \n or \r*/)
        _server->doCommand(NULL, *this);
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

void Client::queueMessage(t_message message)
{
    std::ostringstream oss;

    if (message.prefix.size() > 0)
        oss << ':' << message.prefix;
    for (std::vector<std::string>::iterator it = message.params.begin(); it != message.params.end(); ++it)
    {
        oss << *it;
    }
    if (message.suffix.size() > 0)
        oss << ':' << message.suffix;
    queueMessage(oss.str());
}

void    Client::sendMessage(const std::string& message)
{
    (void)message;
}

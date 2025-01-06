
#include <poll.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Client.hpp"
#include "Server.hpp"
#include <unistd.h>

static unsigned int numClients = 0;

Client::Client(Server* server, int connfd)
    : _server(server)
    , _connfd(connfd)
    , _isAuthenticated(false)
    , _isRegistered(false)
    , _lastSeen(time(NULL))
    , _clientId(++numClients)
    , _recieveBuffer()
    , _nickname()
    , _username()
    , _realname()
    , _sendBuffer()
    , _isPinging(false)
    , _pingSentAt()
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
    return _sendBuffer.size() != 0;
}

void Client::authenticate()
{
    std::cout << "CLIENTID " << _clientId << " IS NOW AUTHENTICATED" << std::endl;
    _isAuthenticated = true;
}

void Client::beRegistered()
{
    std::cout << "CLIENT " << _nickname << " IS NOW REGISTERED" << std::endl;
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

void	Client::setUsername(const std::string& newUsername)
{
    this->_username = newUsername;
}

void	Client::setRealname(const std::string& newRealname)
{
    this->_realname = newRealname;
}

void Client::sendPing()
{
    queueMessage("PING");
}

void Client::pongCommand()
{
    _isPinging = false;
}

int Client::readSocket(struct pollfd& pollresult)
{
    _lastSeen = time(NULL);

    if (pollresult.revents & POLLIN)
    {
        std::string newInput = getInput(_connfd);
        if (newInput.size() == 0)
            return -1;
        _recieveBuffer.append(newInput);
        int m_len;
        while ((m_len = _recieveBuffer.find('\n')) >= 0
            || (m_len = _recieveBuffer.find('\r')) >= 0)
        {
            _server->handleCommands(_recieveBuffer.substr(0, m_len), *this);
            _recieveBuffer = _recieveBuffer.substr(m_len + 1);
        }
    }

    if (pollresult.revents & POLLOUT)
    {
        std::string send;
        while (!_sendBuffer.empty())
        {
            send.append(_sendBuffer.front());
            send.append("\n");
            _sendBuffer.pop();
        }
        write(_connfd, send.c_str(), send.size());
    }
    return 0;
}

void    Client::queueMessage(const std::string& message)
{
    _sendBuffer.push(message);
}

void Client::queueMessage(t_message message)
{
    std::ostringstream oss;

    if (message.prefix.size() > 0)
        oss << ':' << message.prefix << ' ';
    for (std::vector<std::string>::iterator it = message.params.begin();
        it < message.params.end(); ++it)
    {
        oss << *it;
        if (it + 1 < message.params.end())
            oss << ' ';
    }
    if (message.suffix.size() > 0)
        oss << " :" << message.suffix;
    queueMessage(oss.str());
}

void    Client::sendMessage(const std::string& message)
{
    (void)message;
}

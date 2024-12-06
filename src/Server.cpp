
#include "Server.hpp"

Server::Server()
    : Server(6667, std::string("password"))
{
}

Server::Server(int port)
    : Server(port, std::string("password"))
{
}

Server::Server(int port, const std::string& password)
    : _password(password)
{
    
}

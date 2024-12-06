
#include "Client.hpp"
#include "Channel.hpp"
#include <vector>

/* temparary line */
class Channel;

class Server
{
private:
    Server(const Server&);
    Server& operator=(const Server&);
    
    int                         _sockfd;
    std::vector<Client>         _clients;
    std::vector<Channel>        _channels;
    struct pollfd*              _sockets;
    int                         _numSockets;
    std::string                 _password;
    std::vector<std::string*>   _nicknames;
    std::vector<std::string>    _motd;

    void newClient(/* things? */);

public:
    Server();
    Server(int port);
    Server(int port, const std::string& password);

    ~Server();

    void run();
    void doCommand(const std::string& cmd, Client&);
    bool authenticate(const std::string& password);
    bool nicknameAvailable(const std::string& nick);
};


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

    void _Server(int port);
    
    int                         _sockfd;
    std::vector<Client>         _clients;
    std::vector<Channel>        _channels;
    std::vector<struct pollfd>  _sockets;
    int                         _numSockets;
    std::string                 _password;
    std::vector<std::string*>   _nicknames;
    std::vector<std::string>    _motd;


public:
    Server();
    Server(int port);
    Server(int port, const std::string& password);

    ~Server();

    void run();

    void doCommand(const std::string& cmdstr, Client&);
    void newClient(struct pollfd& pollresult);
    bool authenticate(const std::string& password);
    bool nicknameAvailable(const std::string& nick);
    Channel* getChannelByName(const std::string& channel);
    Client* getClientByNick(const std::string& nick);
};

#include "ft_irc.hpp"
#include <iostream>
#include "Server.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout << "Incorrect args" << std::endl;
        return 1;
    }

    int port = std::atoi(av[1]);

    Server server(port, av[1]);

    server.run();
}

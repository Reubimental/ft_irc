#include "ft_irc.hpp"
#include <iostream>
#include "Server.hpp"
#include <unistd.h>
#include <cerrno>

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout << "Incorrect args" << std::endl;
        return 1;
    }

    int port = std::atoi(av[1]);

    try
    {
        Server server(port, av[2]);
        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to initialize server:\n";
        std::cerr << e.what() << '\n';
    }
}

std::string getInput(int fd)
{
    std::string output;
    char buf[256];
    int  read_bytes;

    while ((read_bytes = read(fd, buf, 255)))
    {
        if (read_bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            break ;
        else if (read_bytes < 0)
            throw std::runtime_error("READ ERROR");
        buf[read_bytes] = 0;
        output.append(buf);
    }
    return output;
}

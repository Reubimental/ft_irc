
#pragma once

#include <string>
#include <vector>

std::string getInput(int fd);

typedef struct s_message
{
    std::string prefix;
    std::vector<std::string> params;
    std::string suffix;
}   t_message;
typedef std::vector<std::string>::iterator param_iter;


#pragma once

#include <string>
#include <vector>

typedef struct s_message
{
    std::string prefix;
    std::vector<std::string> params;
    std::string suffix;
}   t_message;

NAME		= ircserv
CPP			= c++
CPPFLAGS	= -Wall -Wextra -Werror -std=c++98

SRCS_DIR	= srcs
OBJS_DIR	= objs

SRCS		= ft_irc.cpp
OBJS		= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

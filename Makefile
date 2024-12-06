NAME		:= ircserv
CXX			:= c++
CXXFALGS	:= -Wall -Wextra -Werror -std=c++98

DEBUG_FLAGS := -g -DDEBUG=1

SRC_DIR	:= src/
OBJ_DIR	:= obj/
DEBUG_DIR := debug_obj/
INC_DIR := inc/

FILES := Channel \
		 Client \
		 Server \

SRC := $(addsuffix .cpp, $(addprefix $(SRC_DIR), $(FILES)))
OBJ := $(addsuffix .o, $(addprefix $(OBJ_DIR), $(FILES)))
DEP := $(OBJ:.o=.d)
DEBUG_OBJ := $(patsubst $(OBJ_DIR)%, $(DEBUG_DIR)%, $(OBJ))
DEBUG_DEP := $(DEBUG_OBJ:.o=.d)

IFLAGS := -I$(INC_DIR)


C_GRAY := \033[1;30m
C_ORANGE := \033[0;33m
C_RED := \033[0;31m
C_CYAN := \033[0;36m
NC := \033[0m

PREFIX := $(C_ORANGE)<$(NAME)>$(NC)


all: $(NAME)

$(NAME): $(OBJ) $(FLIBS)
	@printf "$(PREFIX) CREATING $(C_CYAN)$@$(NC)\n"
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)

$(NAME)_debug: $(DEBUG_OBJ) $(FLIBS)
	@printf "$(PREFIX) CREATING $(C_CYAN)$@$(NC)\n"
	@$(CXX) $(CXXFLAGS) -o $@ $(DEBUG_OBJ) $(LFLAGS)

-include $(DEP)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR:/=)
	$(CXX) $(CXXFALGS) $(IFLAGS) -c $< -o $@

$(OBJ_DIR:/=):
	@mkdir $(OBJ_DIR)

$(DEBUG_DIR)%.o: $(SRC_DIR)%.c | $(DEBUG_DIR:/=)
	@printf "$(PREFIX) $(C_GRAY)COMPILING $(C_CYAN)$@$(NC)\n"
	@$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(IFLAGS) -MMD -c $< -o $@

$(DEBUG_DIR:/=):
	@mkdir $(DEBUG_DIR)

debug: _debug $(NAME)_debug

_debug:
	@printf "$(PREFIX) $(RED)***DEBUG MODE***$(NC)\n"

clean:
	@printf "$(PREFIX) $(C_RED)REMOVING OBJECT FILES$(NC)\n"
	@rm -rf $(OBJ_DIR) $(DEBUG_DIR)

fclean: clean
	@printf "$(PREFIX) $(C_RED)REMOVING BINARY$(NC)\n"
	@rm -f $(NAME) $(NAME)_debug

re: fclean all

.PHONY: all clean fclean re debug _debug

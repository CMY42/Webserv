COLOR_RESET = \033[0m
COLOR_GREEN = \033[1;32m
COLOR_YELLOW = \033[1;33m
COLOR_CYAN = \033[1;36m
COLOR_RED = \033[1;31m
COLOR_ORANGE = \033[1;38;5;208m

CC = g++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

SRCS = main.cpp $(wildcard srcs/Configuration/*.cpp) $(wildcard srcs/Server/*.cpp) $(wildcard Partie_C/*.cpp)

OBJS = $(SRCS:.cpp=.o)

NAME = webserv

all: $(NAME)
	@echo "$(COLOR_GREEN)Compilation completed successfully!$(COLOR_RESET)"

$(NAME): $(OBJS)
	@echo "$(COLOR_YELLOW)Compiling srcs/main.cpp..$(COLOR_RESET)"
	@$(CC) $(CPPFLAGS) $^ -o $@
	@echo "$(COLOR_CYAN)Linking $(NAME)...$(COLOR_RESET)"

%.o: %.cpp
	@echo "$(COLOR_CYAN)Compiling $<...$(COLOR_RESET)"
	@$(CC) $(CPPFLAGS) -c $< -o $@

.PHONY: all clean fclean re

clean:
	@rm -f $(OBJS)
	@echo "$(COLOR_ORANGE)Object files removed!$(COLOR_RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(COLOR_RED)Executable removed!$(COLOR_RESET)"

re: fclean all

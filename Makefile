MAKEFLAGS += -j

NAME	:= webserv
SRCS	:= server/main.cpp\
		   server/Server.cpp\
		   server/request.cpp\
		   server/utils.cpp\
		   server/status_code.cpp
HEADERS := server/include.hpp\
		   server/Server.hpp
CC 		:= c++
CFLAGS  := -Wall -Wextra -Werror -std=c++98 -g
OBJS    := $(SRCS:.cpp=.o)

all: $(NAME)

run: all
	./${NAME}

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

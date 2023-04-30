NAME	= webserv
SRCS	= server/main.cpp \
		  server/Server.cpp\
		  server/request.cpp\
		  server/status_code.cpp
CC 		= c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98 -g
OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

run: all
	./${NAME}

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.cpp=.o)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

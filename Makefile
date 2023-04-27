# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ulayus <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/23 10:28:52 by ulayus            #+#    #+#              #
#    Updated: 2023/04/27 16:57:31 by averdon          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv
SRCS	= parsing/parseConfFile.cpp \
		  parsing/parseLine.cpp \
		  parsing/initializeStruct.cpp \
		  parsing/utils.cpp \
		  server/main.cpp \
		  server/Server.cpp
CC 		= c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98 -g
OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

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

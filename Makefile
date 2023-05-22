MAKEFLAGS += -j

NAME	:= webserv
SRCS	:= main.cpp\
		   Server.cpp\
		   Request.cpp\
		   parseRequest.cpp\
		   directoryListing.cpp\
		   statusCode.cpp\
		   parsing/attributeFunction.cpp\
		   parsing/initializeStruct.cpp\
		   parsing/parseConfFile.cpp\
		   parsing/parseLine.cpp\
		   parsing/utils.cpp\

CC 		:= g++
CFLAGS  := -Wall -Wextra -Werror -std=c++98 -g -MMD
OBJS    := $(addprefix objs/, $(SRCS:.cpp=.o))

all: $(NAME)

run: all
	./${NAME} ConfigFiles/easy.conf

vg: all
	valgrind --track-fds=yes --show-leak-kinds=all --leak-check=full ./${NAME} ConfigFiles/easy.conf

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

objs/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf objs 

fclean: clean
	rm -f $(NAME)

re: fclean
	make

.PHONY: all clean fclean re

-include $(OBJS:.o=.d)

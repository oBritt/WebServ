NAME = Webserve
FLAGS = -Wall -Werror -Wextra
SRC = \
	Files/Parser/parse.cpp\
	Files/Parser/Class_Server.cpp\
	\
	Files/Class_Connection.cpp\
	Files/Class_Request.cpp\
	\
	Files/Webserver.cpp\
	Files/socket_create.cpp

OBJS = $(SRC:.cpp=.o)

$(NAME) : all

all: 
	rm socket_create.o;make all2;
all2: $(OBJS)
	c++ $(FLAGS) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
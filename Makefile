CC = cc

CFLAGS = -Wall -pthread

SRCS = utils/parser.c utils/parser_helper.c

OBJS = $(SRCS:.c=.o)

NAME = main

all:$(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(NAME).c -o $(NAME) 

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: re fclean clean all
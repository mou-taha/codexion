CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = parser/parser.c parser/parser_helper.c initialization/init.c  initialization/destroy.c logs/logs.c routines/coder_routine.c routines/helper.c routines/monitor.c simulation/simulation.c

OBJS = $(SRCS:.c=.o)

NAME = codexion

all:$(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(NAME).c -g -o $(NAME) 

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: re fclean clean all
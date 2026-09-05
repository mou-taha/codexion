# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/05 11:24:21 by tmousnia          #+#    #+#              #
#    Updated: 2026/09/05 12:13:56 by tmousnia         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread -g
 
SRCS = initialization/destroy.c \
	   initialization/init.c \
	   logs/logs.c \
	   parser/parser.c \
	   parser/parser_helper.c \
	   routines/coder_routine.c \
	   routines/helper.c \
	   routines/monitor.c \
	   simulation/simulation.c \
	   simulation/dongle.c \
	   simulation/heap.c \
	   simulation/heapHelper.c \

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
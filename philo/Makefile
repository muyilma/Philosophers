NAME=philo

CFLAGS=-g  -Wall -Wextra -Werror -fsanitize=thread

SRC=philosopher.c actions.c utils.c util2.c

OBJ=$(SRC:.c=.o)

all:$(NAME)

$(NAME):$(OBJ)
	cc $(CFLAGS) $(OBJ) -o  $(NAME)  

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re:	fclean all
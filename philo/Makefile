NAME=philo
CFLAGS=-Wall -Wextra -Werror -fsanitize=thread
SRC=philosopher.c actions.c utils.c

all:$(NAME)

$(NAME):$(SRC)
	cc  -g  -o  $(NAME) $(CFLAGS)  $(SRC)

clean:
	rm -rf $(NAME)

re:	clean all
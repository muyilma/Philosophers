NAME=philo
CFLAGS=-Wall -Wextra -Werror
SRC=philosopher.c actions.c

all:$(NAME)

$(NAME):$(SRC)
	cc  -g  -o  $(NAME) $(CFLAGS)  $(SRC)

clean:
	rm -rf $(NAME)

re:	clean all
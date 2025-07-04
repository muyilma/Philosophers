NAME=philo

SRC=philosopher.c actions.c
CFLAGS=-Wall -Wextra -Werror

all:$(NAME)

$(NAME):$(SRC)
	cc  -g  -o  $(NAME) $(CFLAGS)  $(SRC)

clean:
	rm -rf $(NAME)

re:	clean all
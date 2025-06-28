NAME=philo

SRC=philosopher.c
CFLAGS=

all:$(NAME)

$(NAME):$(SRC)
	cc  -o $(NAME) $(CFLAGS)  $(SRC)

clean:
	rm -rf $(NAME)

re:	clean all
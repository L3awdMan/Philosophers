NAME    = philo

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g3
INC     = -I includes

SRC_DIR = src
UTL_DIR = utils

SRC     = $(SRC_DIR)/main.c \
          $(SRC_DIR)/parse.c

UTL     = $(UTL_DIR)/error_utils.c

OBJ     = $(SRC:.c=.o) $(UTL:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -lpthread -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

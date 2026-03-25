NAME    = philo

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g3 -pthread -MMD -MP
INC     = -I includes

SRC_DIR = src
UTL_DIR = utils

SRC     = $(SRC_DIR)/main.c \
          $(SRC_DIR)/parse.c \
          $(SRC_DIR)/init.c \
          $(SRC_DIR)/routine.c \
          $(SRC_DIR)/monitor.c \
          $(SRC_DIR)/print.c \
          $(SRC_DIR)/time.c

UTL     = $(UTL_DIR)/error_utils.c \
          $(UTL_DIR)/utils.c

OBJ     = $(SRC:.c=.o) $(UTL:.c=.o)
DEP     = $(OBJ:.o=.d)

all: $(NAME)

-include $(DEP)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ) $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

asan: CFLAGS += -fsanitize=address,undefined
asan: re

tsan: CFLAGS += -fsanitize=thread
tsan: re

.PHONY: all clean fclean re asan tsan

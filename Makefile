NAME    = philo

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g3 -pthread -MMD -MP
INC     = -I includes

SRC_DIR = src
UTL_DIR = utils
OBJ_DIR = obj

SRC     = $(SRC_DIR)/main.c \
          $(SRC_DIR)/parse.c \
          $(SRC_DIR)/init.c \
          $(SRC_DIR)/routine.c \
          $(SRC_DIR)/monitor.c \
          $(SRC_DIR)/print.c \
          $(SRC_DIR)/time.c

UTL     = $(UTL_DIR)/error_utils.c \
          $(UTL_DIR)/utils.c

OBJ     = $(SRC:%.c=$(OBJ_DIR)/%.o) $(UTL:%.c=$(OBJ_DIR)/%.o)
DEP     = $(OBJ:.o=.d)

all: $(NAME)

-include $(DEP)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re asan tsan

CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

TARGET = s21_grep

SRC = s21_grep.c \
      grep_parser.c \
      grep_process.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
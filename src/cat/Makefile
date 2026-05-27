CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGET = s21_cat

SOURCES = s21_cat.c cat_process.c cat_parser.c

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

clean:
	rm -f *.o $(TARGET)

rebuild: clean all
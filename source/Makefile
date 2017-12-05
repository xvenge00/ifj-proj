#DEPLOY Makefile
CC = gcc
CFLAGS = -Wall -pedantic -Wextra -Werror -std=c99
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
PROJ = ifj
EXECUTABLE = $(PROJ)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS)

pack:
	zip -j xvenge00.zip source/Makefile source/*.c source/*.h rozdeleni rozsireni

clean_all:
	rm -f $(OBJS) $(EXECUTABLE)

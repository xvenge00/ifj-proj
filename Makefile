#DEV MAKEFILE
CC = gcc
CFLAGS = -Wall -pedantic -Wextra -std=c99
SRCS = $(wildcard source/*.c)   #pri odovzdani vymazat source/
OBJS = $(SRCS:.c=.o)
PROJ = ifj
EXECUTABLE = $(PROJ)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean pack clean_all debug
clean:
	rm -f $(OBJS)

pack:
	zip -j xvenge00.zip source/Makefile source/*.c source/*.h rozdeleni rozsireni

clean_all:
	rm -f $(OBJS) $(EXECUTABLE)


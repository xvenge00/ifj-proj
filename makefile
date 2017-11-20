CC = gcc
CFLAGS = -Wall -pedantic -Wextra -Werror
SRCS = $(wildcard source/*.c)   #pri odovzdani vymazat source/
OBJS = $(SRCS:.c=.o)
PROJ = ifj
EXECUTABLE = $(PROJ)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean 
clean:
	rm -f $(OBJS)
	
pack:
	zip -j xvenge00.zip makefile source/*.c source/*.h

clean_all:
	rm -f $(OBJS) $(EXECUTABLE)

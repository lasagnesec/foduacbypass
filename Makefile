CC=x86_64-w64-mingw32-gcc
CFLAGS=-c -Wall -masm=intel -O2 -fPIC
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o

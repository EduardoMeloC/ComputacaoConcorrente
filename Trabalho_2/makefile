CC		:= gcc
CFLAGS	:= -Wall
LDFLAGS := -lpthread
SRC		:= *.c
PROGRAM := main.out

.PHONY: all clean

all: main.out

main.out:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(PROGRAM)

clean:
	rm -rf *.o *.out

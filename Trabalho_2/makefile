CC		:= gcc
CFLAGS	:= -Wall
LDFLAGS := -lpthread
SRC		:= src/*.c
INC		:= -I include
BINDIR  := bin
TARGET  := $(BINDIR)/main.out

.PHONY: all clean

all: main.out verbose.out

main.out:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) $(SRC) -o $(TARGET)

verbose.out:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) $(SRC) -DVERBOSE -o $(BINDIR)/verbose.out

clean:
	rm -rf bin/*.o bin/*.out

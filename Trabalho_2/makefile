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
	@ mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) $(SRC) -o $(TARGET)

verbose.out:
	@ mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) $(SRC) -DVERBOSE -o $(BINDIR)/verbose.out

clean:
	rm -rf bin/*.o bin/*.out

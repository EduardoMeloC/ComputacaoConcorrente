CC		:= gcc
CFLAGS	:= -Wall
LDFLAGS := -lpthread
SRC		:= $(filter-out ../src/main.c ../src/RnWriter.c, $(wildcard ../src/*.c)) ./*.c
INC		:= -I ../include
TARGET  := test.out

.PHONY: all clean

all: main.out

main.out:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) $(SRC) -o $(TARGET)

clean:
	rm -rf *.o *.out

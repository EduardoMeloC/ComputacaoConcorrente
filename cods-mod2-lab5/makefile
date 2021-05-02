all:
	gcc -o main.o main.c -lpthread; gcc -o main_test.o main_test.c -lpthread -lm

run: 
	@make --no-print-directory all; echo; ./main.o

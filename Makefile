CC=gcc

all: main.c
	cpp -CP main.c main_expanded.c
	$(CC) -g -o main main_expanded.c
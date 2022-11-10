CC=gcc
CFLAGS= -I. -g
all: wordpuzzle randomgen
wordpuzzle: wordpuzzle.c 
	gcc -pthread -o wordpuzzle wordpuzzle.c $(CFLAGS)

randomgen: randomgen.c
	gcc -o randomgen randomgen.c

CC=gcc 
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -ggdb
LDFLAGS=-lm -lraylib

meowdo: main.c
	$(CC) $(CFLAGS) -o meowdo main.c $(LDFLAGS)

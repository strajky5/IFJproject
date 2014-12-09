NAME=main

CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra

all: $(NAME)

str.o: str.c str.h list.h
	$(CC) $(CFLAGS) -c str.c -o str.o
scaner.o: scaner.c scaner.h str.h
	$(CC) $(CFLAGS) -c scaner.c -o scaner.o
ial.o: ial.c ial.h str.h errors.h list.h 
	$(CC) $(CFLAGS) -c ial.c -o ial.o
parser.o: parser.c parser.h str.h errors.h expressions.h scaner.h ial.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o
list.o: list.c list.h str.h errors.h 
	$(CC) $(CFLAGS) -c list.c -o list.o

expressions.o: expressions.c expressions.h str.h errors.h scaner.h list.h ial.h
	$(CC) $(CFLAGS) -c expressions.c -o expressions.o

main.o: main.c errors.h scaner.h parser.h 
	$(CC) $(CFLAGS) -c main.c -o main.o

main: str.o scaner.o expressions.o list.o parser.o ial.o main.o  
	$(CC) $(CFLAGS) str.o scaner.o parser.o ial.o list.o expressions.o main.o -o main

clean:
	rm -f *.o $(OBJFILES) 

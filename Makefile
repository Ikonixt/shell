CC=gcc

all: icsh

icsh: icsh.c
	$(CC) -o icsh icsh.c
	
clean:
	rm -f icsh.o

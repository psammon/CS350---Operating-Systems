sleep_seconds=3
ecode=0

all: myshell sne
	
myshell: main.o parser.o
	gcc -static parser.o main.o -o $@

main.o: main.c parser.h
	gcc -c main.c

parser.o: parser.c parser.h
	gcc -c parser.c

sne: sleep_and_echo.c Makefile
	gcc -static -DSECS=$(sleep_seconds) -DECODE=$(ecode) sleep_and_echo.c -o $@

clean:
	rm -f myshell sne *.o

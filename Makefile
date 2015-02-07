CFLAGS=-DUNIX -lcurses 
DEBUG=-g
#DEBUG=

all: shell


shell:	shell.c parse.c parse.h
	gcc $(CFLAGS) $(DEBUG) shell.c parse.c -o shell -lreadline
clean:
	rm -f shell *~

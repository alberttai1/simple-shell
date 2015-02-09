CFLAGS=-DUNIX -lcurses 
DEBUG=-g

all: shell

shell:	shell.c parse.c parse.h
	gcc $(CFLAGS) $(DEBUG) shell.c shell.h parse.c -o shell -lreadline
clean:
	rm -f shell *~

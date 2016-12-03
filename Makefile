all:
	gcc -o webserv webserv.c
	gcc -o webcli webcli.c

clean:
	rm -f webserv webcli

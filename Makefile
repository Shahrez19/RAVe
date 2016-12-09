all:
	gcc -o webserv webserv.c

debug:
	gcc -g -o webserv webserv.c

clean:
	rm -f webserv

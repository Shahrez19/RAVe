#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * get_suffix(char * filename) {
    int i;
    for (i = 0; i < strlen(filename); i++) {
        if (filename[i] == '.') {
            return &filename[i+1];
        }
    }
    return "";
}

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"usage: %s port\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);
    int sd, new_sd;
    struct sockaddr_in name, cli_name;
    int sock_opt_val = 1;
    int cli_len;

    // TODO DMA
    char data[100];		// receive data buffer

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
        exit (-1);
    }

    if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_opt_val,
    sizeof(sock_opt_val)) < 0) {
        perror ("Failed to set SO_REUSEADDR on INET socket");
        exit (-1);
    }

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind (sd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        perror ("bind() error");
        exit (-1);
    }

    listen (sd, 5);

    for (;;) {
        cli_len = sizeof (cli_name);
        new_sd = accept (sd, (struct sockaddr *) &cli_name, &cli_len);
        // printf ("Assigning new socket descriptor:  %d\n", new_sd);

        if (new_sd < 0) {
            perror ("accept() error");
            exit (-1);
        }

        if (fork () == 0) {	                // child process
            close (sd);
            read (new_sd, &data, 100);       // read our request
            char * q = strtok(data, " ");   // start to tokenize
            int i;
            for (i = 0; i!=1; i++) {    // get second token (request)
                q = strtok(NULL, " ");
            }

            // parse and execute request
            if (strcmp(get_suffix(q), "cgi") == 0) {
                char * req = (char *) malloc(sizeof(q) + sizeof(char));
                strcpy(req, ".");
                strcat(req, q);
                char *args[] = {req, NULL};
                if (execvp(req, args) == -1) {
    				fprintf(stderr, "execution error %s: %s\n", strerror(errno), req);
                    close(new_sd);
    				exit(-1);
    			}
            }


            close(new_sd);
            exit (0);
        }
    }

    return 0;
}

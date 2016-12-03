#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#define BACKLOG 10

// child process handler
void sigchld_handler(int s) {
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// get ip addr (IPV4)
void *get_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"usage: %s port\n", argv[0]);
        exit(0);
    }

    const char * PORT = argv[1];
    int sockfd, new_sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage requester_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // get address info and put in &servinfo
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return(1);
    }

    // find first usable address in struct and bind to it
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // done with this addrinfo struct

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;    // track and clean up dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Listening on %s...\n", PORT);

    while (true) {
        sin_size = sizeof(requester_addr);
        new_sockfd = accept(sockfd, (struct sockaddr *) &requester_addr, &sin_size);
        if (new_sockfd == -1) {
            perror("accept);");
            continue;
        }

        inet_ntop(requester_addr.ss_family, get_addr((struct sockaddr *) &requester_addr), s, sizeof(s));
        printf("server: incoming connection from %s\n", s);

        pid_t pid = fork();
		if (pid == -1) {
			fprintf(stderr, "error: %s\n", strerror(errno));
		}
		else if (pid == 0) { // child
            close(sockfd);   // close server socket in child
            if (send(new_sockfd, "testing connection from child", 36, 0) == -1) {
                perror("send failure");
            }
            close(new_sockfd);  // cleanup and exit child
            exit(0);
		}
		else {	// parent
            close(new_sockfd);  // close unneeded socket in parent
        }
    }
}

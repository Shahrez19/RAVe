#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define MAXDATA 100 // max bytes that can be received at once

// get ip addr (IPV4)
void *get_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: %s address port\n", argv[0]);
        exit(0);
    }

    const char * ADDR = argv[1];
    const char * PORT = argv[2];
    int sockfd, numbytes;
    char buf[MAXDATA];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // get address info and put in &servinfo
    if ((rv = getaddrinfo(ADDR, PORT, &hints, &servinfo)) !=0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return(1);
    }

    // find first usable address in struct and bind to it
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connection failure");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return(2);
    }

    inet_ntop(p->ai_family, get_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    printf("connecting to %s\n", s);

    freeaddrinfo(servinfo); // done with this addrinfo struct

    if ((numbytes = recv(sockfd, buf, MAXDATA-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("received: '%s'\n", buf);

    close(sockfd);

    return(0);
}

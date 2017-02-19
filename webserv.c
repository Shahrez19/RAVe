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
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>

#define BLKDEV 0     // block device
#define CHRDEV 1     // character device
#define DREG   2     // directory
#define FIFO   3     // FIFO
#define SLINK  4     // symlink
#define FREG   5     // regular file
#define FSOCK  6     // socket file

FILE *popen(const char *command, const char *type);

static char *hargs[10];

char * get_suffix(char * filename) {
    int i;
    for (i = 0; i < strlen(filename); i++) {
        if (filename[i] == '.') {
            return &filename[i+1];
        }
    }
    return "";
}

void notfound(int client, char *fname) {
    char buf[1024];
    char rbuf[1024];
    sprintf(buf, "HTTP/1.0 404 File Not Found\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Server: webserv/0.1.0\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: ");
    send(client, buf, strlen(buf), 0);

    // read content
    strcat(rbuf, "<HTML><HEAD><TITLE>404: File Not Found\r\n");
    strcat(rbuf, "\r\n</TITLE></HEAD>\r\n");
    strcat(rbuf, "<BODY><H1 style='text-align: center;'>404: File Not Found:</H1>\r\n<p style='text-align: center;'>");
    strcat(rbuf, fname);
    strcat(rbuf, "</p></HTML>\r\n");

    unsigned long rsize = strlen(rbuf);
    sprintf(buf, "%lu\r\n", rsize);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

    // send response
    send(client, rbuf, strlen(rbuf), 0);

    memset(buf, 0, 1024);
    close(client);
}

void unimplemented(int client) {

    char buf[1024];
    char * resp = "<HTML><HEAD><TITLE>Method Not Implemented\r\n</TITLE></HEAD>\r\n<BODY><P>HTTP request method not supported.</p>\r\n</BODY></HTML>\r\n";
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Server: webserv/0.1.0\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: 125\r\n\r\n");
    send(client, buf, strlen(buf), 0);
    send(client, resp, strlen(resp), 0);
    memset(buf, 0, 1024);
}

// checks dir for file fname and returns its type
int checkcwd (char * fname) {
    DIR *dir;
    struct dirent *dp;
    struct stat st;

    if((dir = opendir(".")) == NULL) {
        perror("\nUnable to open directory.");
        exit(1);
    }

    while ((dp = readdir(dir)) != NULL) {
        if(strcmp(dp->d_name, fname) == 0) {    // match
            if (stat(fname, &st) == -1) {
                fprintf(stderr, "failed to stat %s", fname);
                perror("");
                exit(-1);
            }

            switch (st.st_mode & S_IFMT) {
                case S_IFBLK:   closedir(dir);   return (BLKDEV);   break;
                case S_IFCHR:   closedir(dir);   return (CHRDEV);   break;
                case S_IFDIR:   closedir(dir);   return (DREG);     break;
                case S_IFIFO:   closedir(dir);   return (FIFO);     break;
                case S_IFLNK:   closedir(dir);   return (SLINK);    break;
                case S_IFREG:   closedir(dir);   return (FREG);     break;
                case S_IFSOCK:  closedir(dir);   return (FSOCK);    break;
                default:        closedir(dir);   return (-1);     break;
            }
        }
    }
}

void rlsdir (const char* fname, int client) {
    DIR *dir;
    struct dirent *dp;

    char * dirname = (char *) malloc(sizeof(fname) + (2*sizeof(char)));
    strcpy(dirname, "./");
    strcat(dirname, fname);

    if((dir = opendir(dirname)) == NULL) {
        perror("\nUnable to open directory.");
        exit(1);
    }

    fprintf(stderr, "200: %s\n", fname);

    char buf[1024];
    char rbuf[1024];
    // headers
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Server: webserv/0.1.0\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: ");
    send(client, buf, strlen(buf), 0);

    // read response
    strcat(rbuf, "<HTML><HEAD><TITLE>Index of ");
    strcat(rbuf, fname);
    strcat(rbuf, "\r\n");
    strcat(rbuf, "</TITLE></HEAD>\r\n<BODY><H1>Index of ");
    strcat(rbuf, fname);
    strcat(rbuf, "</H1>\r\n<hr>\r\n<pre>\r\n");

    //read directory contents
    while ((dp = readdir(dir)) != NULL) {
        if ((strcmp(dp->d_name,".") != 0)) {
            // write (sockfd, , )); // NEED TO WRITE TO SOCKET
            // printf("%s\n", dp->d_name);
            strcat(rbuf, "<a href="">\r\n");
            strcat(rbuf, dp->d_name);
            strcat(rbuf, "</a>\r\n");
        }
    }

    strcat(rbuf, "</pre>\r\n<hr>\r\n</HTML>\r\n");

    unsigned long rsize = strlen(rbuf);
    // printf("%s", buf);
    // printf("CONTENT LEN: %lu\n", rsize);
    sprintf(buf, "%lu\r\n", rsize);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

    // send response
    send(client, rbuf, strlen(rbuf), 0);

    // char *reply =
    //     "HTTP/1.1 200 OK\n"
    //     "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
    //     "Server: Apache/2.2.3\n"
    //     "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
    //     "ETag: \"56d-9989200-1132c580\"\n"
    //     "Content-Type: text/html\n"
    //     "Content-Length: 15\n"
    //     "Accept-Ranges: bytes\n"
    //     "Connection: close\n"
    //     "\n"
    //     "sdfkjsdnbfkjbsf";
    //
    // send(client, reply, strlen(reply), 0);

    memset(buf, 0, 1024);
    memset(rbuf, 0, 1024);

    closedir(dir);
    close(client);
}

void send_file(const char * fname, int client, char * extension) {
    if (strcmp(extension, "html") == 0) {
        FILE * fp;
        fp = fopen(fname, "r");
        int fdp;
        struct stat sb;
        if ((fdp = open (fname, O_RDONLY)) != -1) {
            if (fstat(fdp, &sb) != -1) {
            }
            else {
                perror("Error getting file size: ");
                exit(1);
            }
        }
        else {
            perror("Error opening file: ");
            exit(1);
        }
        char pbuf[65536];
        fread(pbuf, sizeof(char), sb.st_size + 1, fp);
        fclose(fp);

        fprintf(stderr, "200: %s\n", fname);

        char buf[1024];
        sprintf(buf, "HTTP/1.0 200\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Server: webserv/0.1.0\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Content-Type: text/html\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "%s\r\n", extension);
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Content-Length: ");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "%lu\r\n", sb.st_size);
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "\r\n");
        send(client, buf, strlen(buf), 0);

        write (client, pbuf, sb.st_size);
        close(client);
        close(fdp);
    }
    else {
        FILE * fp;
        fp = fopen(fname, "r");
        int fdp;
        struct stat sb;
        if ((fdp = open (fname, O_RDONLY)) != -1) {
            if (fstat(fdp, &sb) != -1) {
            }
            else {
                perror("Error getting file size: ");
                exit(1);
            }
        }
        else {
            perror("Error opening file: ");
            exit(1);
        }
        char pbuf[65536];
        fread(pbuf, sizeof(char), sb.st_size + 1, fp);
        fclose(fp);

        fprintf(stderr, "200: %s\n", fname);

        char buf[1024];
        sprintf(buf, "HTTP/1.0 200\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Server: webserv/0.1.0\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Content-Type: text/plain\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "%s\r\n", extension);
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Content-Length: ");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "%lu\r\n", sb.st_size);
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "\r\n");
        send(client, buf, strlen(buf), 0);

        write (client, pbuf, sb.st_size);
        close(client);
        close(fdp);
    }
}

void send_img(const char * fname, int client, char * extension) {

    FILE *pic;
    struct stat sb;
    pic = fopen(fname, "r");
    int fdp;

    if ((fdp = open (fname, O_RDONLY)) != -1) {
        if (fstat(fdp, &sb) != -1) {
        }
        else {
            perror("Error getting pic size: ");
            exit(1);
        }
    }
    else {
        perror("Error opening pic: ");
        exit(1);
    }

    fprintf(stderr, "200: %s\n", fname);

    char buf[1024];
    sprintf(buf, "HTTP/1.0 200\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Server: webserv/0.1.0\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: image/");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "%s\r\n", extension);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: ");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "%lu\r\n", sb.st_size);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

    char pbuf[65536];
    fread(pbuf, sizeof(char), sb.st_size + 1, pic);
    fclose(pic);
    write (client, pbuf, sb.st_size);
    close(client);
    close(fdp);
}

int handle_request(char * request, int sockfd) {
    // parse and execute request r
    if (request[0] == '/') {
        request++;
    }
    if (request[strlen(request)-1] == '/') {
        request[strlen(request)-1] = 0;
    }

    // prefixed request
    char * pr = (char *) malloc(sizeof(request) + (2*sizeof(char)));
    strcpy(pr, "./");
    strcat(pr, request);

    // stat file
    struct stat sb;
    if (stat(pr, &sb) == -1) {   // 404
        fprintf(stderr, "404: %s\n", pr);
        notfound(sockfd, pr);
        exit(1);
    }

    if (strcmp(pr, "./") == 0) {    // root dir
        rlsdir("./", sockfd);
    }
    else if (strcmp(get_suffix(request), "cgi") == 0) {    // cgi script

        pid_t pid;
        if ((pid = fork ()) > 0) {

        }
        else if (pid == 0) {
            hargs[0] = pr;
            int devNull = open("/dev/null", O_WRONLY);
            int saved_stdout = dup(STDOUT_FILENO);
            dup2(sockfd, devNull);
            if (execvp(pr, hargs) == -1) {
                fprintf(stderr, "execution error %s: %s\n", strerror(errno), pr);
                close(sockfd);
                exit(-1);
            }
            dup2(saved_stdout, STDOUT_FILENO);
        }
        else {
            perror("");
            exit(1);
        }
        if (strcmp(request, "graph.cgi") == 0) {
            char * resp = "<!DOCTYPE html><html><body><div style='text-align:center'><br><p style='color:red; font-size:16pt'>CS410 Webserver</p><br><img src='histogram.jpeg'></div></body></html>";
            char buf[1024];
            sprintf(buf, "HTTP/1.0 200 OK\r\n");
            send(sockfd, buf, strlen(buf), 0);
            sprintf(buf, "Server: webserv/0.1.0\r\n");
            send(sockfd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Type: text/html\r\n");
            send(sockfd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Length: 168\r\n\r\n");
            send(sockfd, buf, strlen(buf), 0);
            send(sockfd, resp, strlen(resp), 0);
            memset(buf, 0, 1024);
        }
        else {
            char buf[1024];
            char rbuf[1024];
            sprintf(buf, "HTTP/1.0 200 OK\r\n");
            send(sockfd, buf, strlen(buf), 0);
            sprintf(buf, "Server: webserv/0.1.0\r\n");
            send(sockfd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Type: text/html\r\n");
            send(sockfd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Length: 0\r\n\r\n");
            send(sockfd, buf, strlen(buf), 0);
        }
        fprintf(stderr, "200: %s\n", request);
    }
    else if ((strcmp(get_suffix(request), "gif") == 0) ||     // image file (gif or jpeg)
             (strcmp(get_suffix(request), "jpg") == 0) ||
             (strcmp(get_suffix(request), "jpeg") == 0)) {
                 send_img(request, sockfd, get_suffix(request));
    }
    else if (checkcwd(request) == DREG) {   // directory to list
        rlsdir(pr, sockfd);
    }
    else if (checkcwd(request) == FREG) {   // file to send
        send_file(request, sockfd, get_suffix(request));
    }
    else {
        printf("UNCAUGHT\n");
    }
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

        pid_t pid;
        if ((pid = fork ()) > 0) {
        }
        else if (pid == 0) {
            close (sd);
            read (new_sd, &data, 100);       // read our request
            char * rs = strtok(data, " ?");    // start to tokenize
            char r[1024];
            strcpy(r, rs);
            int i;
            char METHOD[1024];
            strcpy(METHOD, r);
            rs = strtok(NULL, " ?");
            strcpy(r, rs);

            if(strcmp(METHOD, "GET") != 0) {
                fprintf(stderr, "501: %s\n", METHOD);
                unimplemented(new_sd);
                close(new_sd);
                exit(0);
            }

            // get args
            int it = 1;
            while ((hargs[it] =strtok(NULL, " ?")) != NULL && it < 10) {
                //printf("ARG: %s\n", hargs[it]);
                it++;
            }

            handle_request(r, new_sd);
            close(new_sd);
            close(sd);
            exit (0);
        }
        else {
            perror("");
            continue;
        }
    }

    return 0;
}

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

#define BLKDEV 0     // block device
#define CHRDEV 1     // character device
#define DREG   2     // directory
#define FIFO   3     // FIFO
#define SLINK  4     // symlink
#define FREG   5     // regular file
#define FSOCK  6     // socket file

char * get_suffix(char * filename) {
    int i;
    for (i = 0; i < strlen(filename); i++) {
        if (filename[i] == '.') {
            return &filename[i+1];
        }
    }
    return "";
}

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
                perror("stat");
                exit(1);
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

void rlsdir (const char* fname, int sockfd) {
    DIR *dir;
    struct dirent *dp;

    char * dirname = (char *) malloc(sizeof(fname) + (2*sizeof(char)));
    strcpy(dirname, "./");
    strcat(dirname, fname);

    if((dir = opendir(dirname)) == NULL) {
        perror("\nUnable to open directory.");
        exit(1);
    }

    while ((dp = readdir(dir)) != NULL) {
        if ((strcmp(dp->d_name,".") != 0) && (strcmp(dp->d_name,"..") != 0)) {
            // write (sockfd, , )); // NEED TO WRITE TO SOCKET
            printf("%s\n", dp->d_name);
        }
    }
    closedir(dir);
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
            char * r = strtok(data, " ");    // start to tokenize
            int i;
            for (i = 0; i!=1; i++) {    // get second token (request)
                r = strtok(NULL, " ");
            }

            // parse and execute request r
            if (r[0] == '/') {
                r++;
            }

            // stat file
            struct stat sb;
            if (stat(r, &sb) == -1) {   // 404
                perror("stat");
                exit(1);
            }

            // prefixed
            char * req = (char *) malloc(sizeof(r) + (2*sizeof(char)));
            strcpy(req, "./");
            strcat(req, r);

            if (strcmp(get_suffix(r), "cgi") == 0) {    // cgi script
                char *args[] = {req, NULL};
                if (execvp(req, args) == -1) {
    				fprintf(stderr, "execution error %s: %s\n", strerror(errno), req);
                    close(new_sd);
    				exit(-1);
    			}
            }
            else if (strcmp(get_suffix(r), "gif") == 0 ||     // image file (gif or jpeg)
                     strcmp(get_suffix(r), "jpg") == 0 ||
                     strcmp(get_suffix(r), "jpeg") == 0) {

            }
            else if (checkcwd(r) == DREG) {    // directory to list
                rlsdir(req, new_sd);
            }
            else {
                printf("UNCAUGHT");
            }

            close(new_sd);
            exit (0);
        }
    }

    return 0;
}

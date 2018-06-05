#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include "Timer.h"

#define N_THREADS 10

char * files[] = {
    //"/home/klefstad/public_html/public/rcp/files/cblas.h", 
    //"/home/klefstad/public_html/public/rcp/files/curses.h", 
    //"/home/klefstad/public_html/public/rcp/files/cursesw.h", 
    //"/home/klefstad/public_html/public/rcp/files/elf.h", 
    //"/home/klefstad/public_html/public/rcp/files/expat.h", 
    //"/home/klefstad/public_html/public/rcp/files/gd.h", 
    //"/home/klefstad/public_html/public/rcp/files/geos_c.h", 
    //"/home/klefstad/public_html/public/rcp/files/gmp-x86_64.h", 
    //"/home/klefstad/public_html/public/rcp/files/gmpxx.h", 
    //"/home/klefstad/public_html/public/rcp/files/H5overflow.h", 
    //"/home/klefstad/public_html/public/rcp/files/jpeglib.h", 
    //"/home/klefstad/public_html/public/rcp/files/kdb.h", 
    //"/home/klefstad/public_html/public/rcp/files/ldap.h", 
    //"/home/klefstad/public_html/public/rcp/files/mpfr.h", 
    //"/home/klefstad/public_html/public/rcp/files/sqlite3.h", 
    //"/home/klefstad/public_html/public/rcp/files/tclDecls.h", 
    //"/home/klefstad/public_html/public/rcp/files/tcl.h", 
    //"/home/klefstad/public_html/public/rcp/files/words1", 
    //"/home/klefstad/public_html/public/rcp/files/words2", 
    //"/home/klefstad/public_html/public/rcp/files/words3",

    "/Users/jeffomidvaran/Desktop/hw9/files/H5overflow.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/cblas.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/copy.sh", 
    "/Users/jeffomidvaran/Desktop/hw9/files/curses.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/cursesw.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/elf.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/expat.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/gd.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/geos_c.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/gmp-x86_64.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/gmpxx.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/jpeglib.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/kdb.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/ldap.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/mpfr.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/sqlite3.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/tcl.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/tclDecls.h", 
    "/Users/jeffomidvaran/Desktop/hw9/files/words1", 
    "/Users/jeffomidvaran/Desktop/hw9/files/words2",
    "/Users/jeffomidvaran/Desktop/hw9/files/words3",

};

#define files_length() (sizeof files / sizeof files[0])

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

struct sockaddr_in make_server_addr(char *host, short port)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof addr);
    struct hostent *hp = gethostbyname(host);
    if ( hp == 0 )
        error(host);
    addr.sin_family = AF_INET;
    bcopy(hp->h_addr_list[0], &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    return addr;
}

int connect_socket(char *host, short port)
{
    int status;
    int tries = 3;
    struct sockaddr_in addr = make_server_addr(host, port);
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if ( s == -1 )
        error("socket()");
    status = connect(s, (struct sockaddr*)&addr, sizeof addr);
    if ( status < 0 )
        error("connect refused");
    return s;
}

void request_file_from_server(int server_socket, char *file)
{
    int len = strlen(file);
    int n = write(server_socket, file, len);
    if ( n != len )
        error("short write");
}

void read_file_from_server(int server_socket, char *file)
{
    char buf[BUFSIZ];
    int n;
    mode_t mode = 0666;
    int ofd = open(file, O_WRONLY | O_CREAT, mode);
    if ( ofd == -1 )
        perror("open()");
    while ( (n = read(server_socket, buf, BUFSIZ)) > 0 )
        write(ofd, buf, n);
    close(ofd);
}


struct Thread_data
{
    int id;
    pthread_t thread_id;
    char * host;
    short port;
    char path[BUFSIZ];
};

void make_file_name(char *local_name, char *dir, char *original_path)
{
    char *p = rindex(original_path, '/');
    if ( !p )
        error("rindex()");
    sprintf(local_name, "%s/%s", dir, p+1);
}

int remote_copy(struct Thread_data * data, char * file)
{
    int server_socket = connect_socket(data->host, data->port);
    request_file_from_server(server_socket, file);
    char local_name[BUFSIZ];
    make_file_name(local_name, data->path, file);
    read_file_from_server(server_socket, local_name);
    close(server_socket);
    return 0; 
}

void make_empty_dir_for_copies(struct Thread_data * data)
{
    mode_t mode = 0777;
    sprintf(data->path, "./Thread_%d", data->id);
    mkdir(data->path, mode);
}

#define N_FILES_TO_COPY files_length() // copy them all
// #define N_FILES_TO_COPY 1 // just copy one when testing

void *thread_work(void *arg)
{
    struct Thread_data * data = (struct Thread_data *)arg;
    make_empty_dir_for_copies(data);
    for ( int i=0; i < N_FILES_TO_COPY; ++i )
        remote_copy(data, files[i]);
    pthread_exit(0);
}

void start_threads(char *host, short port, struct Thread_data thread_args[])
{
    for ( int i = 0; i < N_THREADS; ++i )
    {
        struct Thread_data * t = &thread_args[i];
        t->id = i+1;
        t->host = host;
        t->port = port;
        pthread_create(&t->thread_id, NULL, thread_work, t);
    }
}

void join_threads(struct Thread_data thread_args[])
{
    for ( int i=0; i < N_THREADS; i++ )
        pthread_join(thread_args[i].thread_id, NULL);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    if ( argc != 3 )
    {
        fprintf(stderr, "Usage: %s host port\n", argv[0]);
        exit(-1);
    }
    double eTime;
    struct Thread_data thread_args[N_THREADS];
    char *host = argv[1];
    short port = atoi(argv[2]);
    start_threads(host, port, thread_args);
    join_threads(thread_args);
}

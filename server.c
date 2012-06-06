#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "global.h"

/* 服务器ip和端口号 */
#define _addr_ "127.0.0.1"
#define _port_ 8888

typedef struct client {
    char nick[32];
    int con_fd;
}Client;

static pthread_t cli_thrd[1024];    /* 服务器并发线程 */
Client cli[1024];   /* 客户端 */
static int cli_que_len;
static char buf[MAX_BUF_LEN];

void *recv_thrd(void *arg);   /* server for client */
void remove_client(const int client_id);    /* deal with client quit */
void get_dst_nick(const char *msg, char *ret);  /* 客户与客户之间的通信 */
int client_connected(const char *nick); /* 此昵称的客户在线 */
void get_filename(const char *cli_file_name, char *ret);    /* get the filename */
int get_file_length(const char *cli_file_name); /* get the file size */
void save_file(const int fd, const char *filename, const int total);    /* save file */

int main(int argc, char *argv[])
{
    struct sockaddr_in srv;
    int sock_fd, chk, con_fd;
    pthread_attr_t attr;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error!");
        return -1;
    }

    bzero(&srv, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(_port_);

    /* inet_pton: support ipv6 and ipv4, set the ipaddr */
    if (inet_pton(AF_INET, _addr_, &srv.sin_addr) < 0)
    {
        printf("inet_pton error!");
        return -1;
    }

    chk = bind(sock_fd, (struct sockaddr*)&srv, sizeof(srv));
    if (chk < 0)
    {
        printf("bind error!");
        return -1;
    }

    chk = listen(sock_fd, MAX_LISTEN_QUEUE);
    if (chk < 0)
    {
        printf("listen error!");
        return -1;
    }

    /* 初始化线程参数 */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    cli_que_len = 0;
    while (1)
    {
        con_fd = accept(sock_fd, (struct sockaddr*)NULL, NULL);
        printf("a client connected!\n");
        
        /* get the nickname of the client */
        cli[cli_que_len].con_fd = con_fd;
        printf("fd = %d\n", con_fd);
        strcpy(cli[cli_que_len].nick, "fuck");

        /* create server thread for a new client */
        chk = pthread_create(cli_thrd + cli_que_len, &attr, recv_thrd, cli + cli_que_len);
        if (chk)
        {
            printf("create thread error!");
            continue;
        }
        ++cli_que_len;
    }
    close(sock_fd);

    return 0;
}

void *recv_thrd(void *arg)   /* server for client */
{
    int sock_fd, index;
    int rec_bytes, left;
    Client *cli_ptr = NULL;
    char *ptr = NULL;
    char filename[128], res[128];
    int total_bytes;
    FILE *fp = NULL;

    cli_ptr = (Client *)arg;
    sock_fd = cli_ptr->con_fd;
    printf("thread nick: %s\nthread fd: %d\n", cli_ptr->nick, cli_ptr->con_fd);

    while ((rec_bytes = recv(sock_fd, buf, sizeof(buf), 0)) > 0)
    {
        ptr = strstr(buf, "\r\n\r\n");
        index = ptr - buf;
        strncpy(res, buf, index);
        res[index] = '\0';
        get_filename(res, filename);
        total_bytes = get_file_length(res);
        printf("please enter the folder to save\"%s\":", filename);
        scanf("%s", res);
        index = strlen(res);
        if (res[index - 1] != '/')
        {
            strcat(res, "/");
        }
        else
        {
            strcat(res, filename);
        }

        if ((fp = fopen(filename, "r")) != NULL)
        {
            puts("error, file exist!");
            fclose(fp);
            return NULL;
        }

        printf("filename = %s\r\n", filename);
        fp = fopen(filename, "a+");

        left = rec_bytes - (ptr - buf + 4);
        fwrite(ptr + 4, left, 1, fp);
        fflush(fp);
        fclose(fp);
        total_bytes -= left;

        save_file(sock_fd, filename, total_bytes);
        printf("file %s saved, bytes = %d\r\n", filename, total_bytes + left);
    }

    close(sock_fd);
    remove_client(sock_fd);
    printf("a client(%d) quit!\n", cli_ptr->con_fd);

    pthread_exit(NULL);
}

void remove_client(const int client_id)    /* deal with client quit */
{
    int i, index;

    index = -1;
    for (i = 0; i < cli_que_len; ++i)
    {
        if (cli[i].con_fd == client_id)
        {
            index = i;
            break;
        }
    }

    if (index > 0)
    {
        for (i = index + 1; i < cli_que_len; ++i)
        {
            cli[i - 1].con_fd = cli[i].con_fd;
            strcpy(cli[i - 1].nick, cli[i].nick);
        }
    }
    --cli_que_len;
}

/* 此昵称的客户在线 */
int client_connected(const char *nick)
{
    int i;

    for (i = 0; i < cli_que_len; ++i)
    {
        if (strcpy(cli[i].nick, nick) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/* get the filename */
void get_filename(const char *cli_file_name, char *ret)
{
    const char *ptr = NULL;

    assert(cli_file_name != NULL && ret != NULL);

    ptr = strrchr(cli_file_name, '/');
    if (NULL == ptr)
    {
        sscanf(cli_file_name, "%s", ret);
    }
    else
    {
        sscanf(ptr + 1, "%s", ret);
    }
}

/* get the file size */
int get_file_length(const char *cli_file_name)
{
    int len;

    assert(cli_file_name != NULL);

    sscanf(cli_file_name, "%*s%d", &len);


    return len;
}

/* save file */
void save_file(const int fd, const char *filename, const int total)
{
    int w_bytes, chk;
    FILE *fp = NULL;
    static char buf[1500];

    assert(filename != NULL && total >= 0);

    fp = fopen(filename, "a+");
    if (NULL == fp)
    {
        printf("open file error!\r\n");
        return;
    }
    w_bytes = 0;
    while (w_bytes != total)
    {
        chk = recv(fd, buf, sizeof(buf), 0);
        fwrite(buf, chk, 1, fp);
        w_bytes += chk;
    }
    fflush(fp);
    fclose(fp);
    printf("%s: save!", filename);
}

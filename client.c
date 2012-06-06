#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "global.h"
#include "progress.h"

static char buf[MAX_BUF_LEN];

void *recv_thrd(void *arg); /* 客户端接收信息线程 */
void send_file(const int fd, const char *path); /* 向目的端发送文件字节流 */
static Progress bar;

int main(int argc, char *argv[])
{
    struct sockaddr_in srv;
    int sock_fd, chk;
    pthread_t tid;
    pthread_attr_t attr;

    if (argc != 3)
    {
        printf("usage: %s ipaddr port", argv[0]);
        return -1;
    }
    
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error!");
        return -1;
    }

    bzero(&srv, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(atoi(argv[2])); /* port */
    chk = inet_pton(AF_INET, argv[1], &srv.sin_addr); /* x.x.x.x to network ipaddr */
    if (chk < 0)
    {
        printf("illegal ip address!");
        return -1;
    }

    chk = connect(sock_fd, (struct sockaddr *)&srv, sizeof(srv));
    if (chk < 0)
    {
        printf("connect error!");
        return -1;
    }

    /* 接收消息的线程 */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    chk = pthread_create(&tid, &attr, &recv_thrd, (void *)sock_fd);    /* 接收线程 */
    if (chk)
    {
        printf("create thread error!");
        return -1;
    }

    while (printf("Enter Path(eg:\"./from/trans.tar.bz2\"):"), scanf("%s", buf) != EOF)
    {
        puts("sending....");
        send_file(sock_fd, buf);
        puts("finished!");
    }

    close(sock_fd);

    chk = pthread_join(tid, NULL);
    pthread_attr_destroy(&attr);

    
    return 0;
}

void *recv_thrd(void *arg) /* 接收线程 */
{
    int sock_fd = (int)arg;
    int rec_bytes;

    while ((rec_bytes = recv(sock_fd, buf, sizeof(buf), 0)) > 0)
    {
        buf[rec_bytes] = '\0';
        printf("\r%s", buf);
    }
    
    pthread_exit(NULL);
}

/* 向目的端发送文件字节流 */
void send_file(const int fd, const char *path)
{
    int total_bytes, send_bytes, chk;
    FILE *fp = NULL;
    static char buf[1500], res[128], filename[128];

    assert(path != NULL && strlen(path) >= 0);

    printf("filename = %s\r\n", filename);
    strcpy(filename, path);
    fp = fopen(filename, "r");
    if (NULL == fp)
    {
        printf("file open error, make sure the absolve path is right!\r\n");
        return;
    }

    /* point to the end of the file */
    fseek(fp, 0, SEEK_END);
    /* get the file size */
    total_bytes = ftell(fp);
    /* point to the header */
    fseek(fp, 0, SEEK_SET);

    /* initial progress bar */
    init_progress(&bar, total_bytes);

    /* file messages */
    strcpy(buf, filename);
    strcat(buf, " ");
    m_ntoa(total_bytes, res);
    strcat(buf, res);
    strcat(buf, " \r\n\r\n");
    send(fd, buf, strlen(buf), 0);

    /* the bytes the client send */
    send_bytes = 0;
    printf("file = %s, bytes = %d\r\n", filename, total_bytes);
    while (fread(buf, sizeof(buf), 1, fp))
    {
        send(fd, buf, sizeof(buf), 0);
        send_bytes += sizeof(buf);

        /* update and display progress bar */
        update_progress(&bar, sizeof(buf));
        display_image(&bar);
    }
    chk = send(fd, buf, total_bytes - send_bytes, 0);
    if (total_bytes - send_bytes == chk)
    {
        send_bytes = total_bytes;

        /* update and display progress bar */
        update_progress(&bar, chk);
        display_image(&bar);
    }
}

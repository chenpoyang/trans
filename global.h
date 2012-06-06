#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* 充许在特定fd上请求连接的客户队列数 */
#define MAX_LISTEN_QUEUE 1000

/* max buf len */
#define MAX_BUF_LEN 1024

#define m_error(msg) \
    fprintf(stderr, "%s:%s() %d: %s\r\n", __FILE__, __func__, __LINE__, msg);
#define GLOBAL extern
/* 哈希表的最大长度 */
#define HALEN    256
/* 以太网MTU(bytes) */
#define MAX_ETH_MTU    1500
#define bzero(base,n) memset(base, 0, n)
#define u_int unsigned int
#define M_HIDE 0
#define M_SHOW 1
#define FILENAME_LEN 10

/* current thread wait sec seconds  */
void thread_wait(const int sec);
/* convert number to char array */
void m_ntoa(const int num, char *ret);

void m_tolower(char *str);
void m_toupper(char *str);

/* max length to store error message */
#define MAXLINE 4096
/* max buf to store bytes */
#define MAXBUF 4096

/* logical error */
void merr_msg(const char *fmt, ...);

/* system error */
void merr_sys(const char *fmt, ...);

/* 字符串转换成整型数 */
unsigned int m_atou(const char *str);
/* 整数转换成字符串 */
void m_utoa(int num, char *str);

/* fatal system call error */
void merr_dump(const char *fmt, ...);

/* show error messages and quit */
void merr_quit(const char *fmt, ...);

/* deal with error arglist */
void merr_deal(const int flg, const char *fmt, va_list ap);

/* show tips, frequently show messages */
void mydebug(const char *fmt, ...);

#endif /* __GLOBAL_H__ */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/* 充许在特定fd上请求连接的客户队列数 */
#define MAX_LISTEN_QUEUE 1000

/* max buf len */
#define MAX_BUF_LEN 1024

void bzero(void *base, unsigned int size);

void m_tolower(char *str);

void thread_wait(const int sec);

void m_ntoa(const int num, char *ret);

#endif /* __GLOBAL_H__ */

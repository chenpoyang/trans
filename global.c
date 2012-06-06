#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include <pthread.h>
#include "global.h"

/**
 * @brief    current thread wait some time
 *
 * @param    sec, the internal the thread wait
 */
void thread_wait(const int sec)
{
    struct timespec timeout;
    pthread_mutex_t mutex;
    pthread_cond_t cond;


    /* there is no need to wait */
    if (sec <= 0)
    {
        return;
    }

    /**
     * @param    mutex: for synchronization
     * @param    cond: wait sec seconds
     */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_mutex_lock(&mutex);

    /* wait till : current time + sec */
    timeout.tv_sec = time(NULL) + sec;
    timeout.tv_nsec = 0;
    pthread_cond_timedwait(&cond, &mutex, &timeout);

    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
}

void m_ntoa(const int num, char *ret)
{
    int i, j, flg;
    assert(ret != NULL && num >= 0);

    if (num <= 0)
    {
        strcpy(ret, "0");
        return;
    }

    flg = num;
    i = 0;
    while (flg)
    {
        ret[i++] = flg % 10 + '0';
        flg /= 10;
    }

    j = i - 1;
    i = 0;
    while (i < j)
    {
        flg = ret[i];
        ret[i] = ret[j];
        ret[j] = flg;
        ++i;
        --j;
    }
}

void m_tolower(char *str)
{
    int i, len; 

    assert(str != NULL);

    len = strlen(str);
    for (i = 0; i < len; ++i)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = 'a' + str[i] - 'A';
        }
    }
}

void m_toupper(char *str)
{
    int i;

    assert(str != NULL);

    i = strlen(str);
    while (--i >= 0)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = 'A' + str[i] - 'a';
        }
    }
}

/* logical error */
void merr_msg(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    merr_deal(M_HIDE, fmt, ap);
    va_end(ap);
    return;
}

/* system error */
void merr_sys(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    merr_deal(M_SHOW, fmt, ap);
    va_end(ap);
    exit(-1);
}

/* fatal system call error */
void merr_dump(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    merr_deal(M_SHOW, fmt, ap);
    va_end(ap);
    abort();
    exit(-1);
}

/* show error messages and quit */
void merr_quit(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    merr_deal(M_SHOW, fmt, ap);
    va_end(ap);
    exit(1);
}

/* deal with error arglist */
void merr_deal(const int flg, const char *fmt, va_list ap)
{
    char buf[MAXLINE];
    int m_errno = 0, len = 0;

    snprintf(buf, sizeof(buf), "TIPS on %s, line %d:\r\n\t", __FILE__, __LINE__);

    len = strlen(buf);
    vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);

    m_errno = errno;    /* get the latest types of error */
    len = strlen(buf);
    if (flg == M_SHOW)    /* show lastest error message */
    {
        snprintf(buf + len, sizeof(buf) - len, ": %s", strerror(m_errno));
    }

    len = strlen(buf);
    strncat(buf, "\r\n", sizeof(buf) - len);

    fflush(stdout);    /* in case stderr and stdout are the same */
    fputs(buf, stderr);
    fflush(stderr);
}

/* show tips, frequently show messages */
void mydebug(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    merr_deal(M_HIDE, fmt, ap);
    va_end(ap);
}

/* 字符串转换成整型数 */
unsigned int m_atou(const char *str)
{
    unsigned int num, i, len;

    assert(str != NULL);
    num = 0;
    len = strlen(str);
    for (i = 0; i < len; ++i)
    {
        num = num * 10 + (str[i] - '0');
    }

    return num;
}

/* 整数转换成字符串 */
void m_utoa(int num, char *str)
{
    int i, j;
    char ch;

    j = 0;
    if (num < 0)
    {
        str[j++] = '-';
        i = -num;
    }
    else
    {
        i = num;
    }

    if (num == 0)
    {
        strcpy(str, "0");
        return;
    }
    while (i)
    {
        str[j++] = i % 10 + '0';
        i /= 10;
    }
    str[j] = '\0';

    if (num < 0)
    {
        i = 1;
    }
    else
    {
        i = 0;
    }

    while (i < j)
    {
        ch = str[i];
        str[i] = str[j - 1];
        str[j - 1] = ch;

        ++i;
        --j;
    }
}

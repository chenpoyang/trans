#include <string.h>
#include <assert.h>
#include <time.h>

#include <pthread.h>
#include "global.h"

void bzero(void *base, unsigned int size)
{
    assert(size >= 0);
    memset(base, 0, size);
}

void m_tolower(char *str)
{
    assert(str != NULL);

    int i, len;

    len = strlen(str);

    for (i = 0; i < len; ++i)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = 'a' + str[i] - 'A';
        }
    }
}

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

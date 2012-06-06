#ifndef _PROGRESS_H_
#define _PROGRESS_H_

#include <time.h>

#define DEFAULT_WIDTH 85
#define MAX_STR 128
/*
|<04>=<--------------------45--------------------->=<----12----><--08--><------14---->|
|____[-------------------------------------------->] ---,---,--- -------  ------------|
|100%[-------------------------------------------->] 219,324,416 29.1M/s  in 5.5s     |
|100%[---------------------------------->] x,xxx,xxx,219,324,416 2.1 G/s  in 0.1s     |
|  1%[->                                           ] 219,324,416 1018K/s  in 12m 5s   |
| 80%[---------------------->                      ] ___,___,___ ____+/+  eta 03m 16s |
*/
typedef struct progress {
    char buf[MAX_STR];
    unsigned int done;    /* 完成的字节数 */
    unsigned int total;    /* 总大小 */
    unsigned int width;    /* 默认宽度为85 */
    time_t beg_sec;        /* 任务开始时间 */
    time_t end_sec;        /* 从任务开始到现在的时间间隔 */
}Progress;

void init_progress(Progress *pro, unsigned int total);
void update_progress(Progress *pro, int recv_buf);

void get_percent(const int done, const int totals, char *ptr_buf);
void get_spd(const int tm_stm, const int got_bytes, const int totals, char *ret_spd);
void get_rec_bytes(const int done, char *ret);
void get_ptr_bar(int perc, const int width, char *ret);
void get_status(const int tm_stm, const int done, const int totals, char *ret);
void create_image(Progress *pro, const int width);

void display_image(Progress *pro);

#endif //_PROGRESS_H_

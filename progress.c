#include <stdio.h>
#include <assert.h>
#include "progress.h"
#include "global.h"

void init_progress(Progress *pro, unsigned int total)
{
    char *p = NULL;
    int i;

    assert(pro != NULL);

    pro->done = 0;
    pro->total = total;
    pro->width = DEFAULT_WIDTH;
    pro->beg_sec = time(NULL);

    p = pro->buf;

    /* {"  0%"}-->4 ch! */ 
    *p++ = ' ';
    *p++ = '0';
    *p++ = '%';
    *p++ = ' ';

    /* {"[...]"}-->47 ch! */
    *p++ = '[';
    i = 44;
    *p++ = '>';
    while (--i >= 0)
    {
        *p++ = ' ';
    }
    *p++ = ']';

    /* {" ---,---,---"}-->12 ch! */
    *p++ = ' ';
    *p++ = '0';
    i = 10;
    while (--i >= 0)
    {
        *p++ = ' ';
    }

    /* {" -------"}-->8 ch!, defaule: " --.-K/s" */
    *p++ = ' ';
    *p++ = '-';
    *p++ = '-';
    *p++ = '.';
    *p++ = '-';
    *p++ = 'K';
    *p++ = '/';
    *p++ = 's';

    /* {"  ------------"}-->14 ch! */
    *p++ = ' ';
    *p++ = ' ';
    *p++ = 'e';
    *p++ = 't';
    *p++ = 'a';
    *p++ = ' ';
    *p++ = '-';
    *p++ = '-';
    *p++ = 'm';
    *p++ = ' ';
    *p++ = '-';
    *p++ = '-';
    *p++ = 's';
    *p++ = ' ';

    *p++ = '\0';
}

void update_progress(Progress *pro, int recv_buf)
{
    if (pro == NULL || recv_buf < 0)
    {
        merr_msg("pro == NULL || recv_buf < 0");
        return;
    }

    pro->done += recv_buf;
    pro->end_sec = time(NULL);

    create_image(pro, DEFAULT_WIDTH);    /* 默认宽度: 85 */
}

/**
 * @brief    根据任务完成的份额, 计算百分额, 用ptr_buf[](占四字节)返回
 *
 * @param    done:    任务在总任务的完成额
 * @param    totals:    总任务份额
 * @param    ptr_buf: 返回数组
 */
void get_percent(const int done, const int totals, char *ptr_buf)
{
    int perc;

    assert(ptr_buf != NULL);

    if (totals <= 0 || done <= 0)
    {
        perc = 0;
    }
    else
    {
        perc = (int)(done * 100.0 / totals);
    }

    if (perc != 100)
    {
        if (perc < 10)
        {
            ptr_buf[0] = ' ';
            ptr_buf[1] = perc + '0';
            ptr_buf[2] = '%';
            ptr_buf[3] = ' ';
        }
        else
        {
            ptr_buf[0] = ' ';
            ptr_buf[1] = perc / 10 + '0';
            ptr_buf[2] = perc % 10 + '0';
            ptr_buf[3] = '%';
        }
    }
    else
    {
        ptr_buf[0] = '1';
        ptr_buf[1] = '0';
        ptr_buf[2] = '0';
        ptr_buf[3] = '%';
    }
    ptr_buf[4] = '\0';
}

/**
 * @brief    根据完成的任务量got_bytes及时间间隔, 计算速度
 *
 * @param    tm_stm: 任务开始到现在的时间间隔
 * @param    got_bytes: 完成的任务量
 * @param    ret_spd:    用字符串返回速度(占八个字符, 包括第一个空格字符!)
 */
void get_spd(const int tm_stm, const int got_bytes, const int totals, char *ret_spd)
{
    /* B,K,M,G/s */
    int head, tail, unit, res, index, i, j;
    char *ch = "BKMG", spd;
    char tmp_str[32];

    assert(ret_spd != NULL);
    if (tm_stm <= 0 || got_bytes == totals)
    {
        return;
    }

    head = got_bytes / tm_stm;
    unit = 1;
    index = 0;
    while (head >= 1024)
    {
        head /= 1024;
        unit *= 1024;
        ++index;
    }
    if (index >= 4)
    {
        merr_sys("速度计算值过太, 请注意检查程序");
        return;
    }
    spd = ch[index];

    res = head * unit;
    tail = got_bytes / tm_stm - res;

    i = 0;
    ret_spd[i++] = ' ';
    m_utoa(head, ret_spd + i);
    i = strlen(ret_spd);

    while (--index > 0)    /* 小数点后的数处理, 比最开始的index值少一循环 */
    {
        tail /= 1024;
    }

    if (spd == 'M' || spd == 'G')
    {
        ret_spd[i++] = '.';
        m_utoa(tail, tmp_str);
        j = strlen(tmp_str);
        ch = tmp_str;
        while (i < 5 && ch != tmp_str + j)
        {
            ret_spd[i++] = *ch++;
        }
    }
    while (i < 5)
    {
        ret_spd[i++] = ' ';
    }

    ret_spd[i++] = spd;
    ret_spd[i++] = '/';
    ret_spd[i++] = 's';
    ret_spd[i++] = '\0';
}

void get_rec_bytes(const int done, char *ret)
{
    char str[9], *ptr = NULL;    /* | ___,___,___| */
    int index, len;

    index = 0;
    ret[index++] = ' ';
    m_utoa(done, str);
    len = strlen(str);
    ptr = str;
    while (len > 3)
    {
        strncpy(ret + index, ptr, 3);
        index += 3;
        ret[index++] = ',';
        len -= 3;
        ptr += 3;
    }
    strncpy(ret + index, ptr, len);
    index += len;
    ret[index] = '\0';
}

/* get the bar: [------->        ] 
 * width = 4 + 1 + left + 1 + 12 + 8 + 14, default width: 85, left = 45!
 * width: 总的宽度
 * perc: 百分整, eg: the interger of 80%, 85%, 75% etc
 */
void get_ptr_bar(int perc, const int width, char *ret)
{
    int index, left, head, tail;

    index = 0;
    ret[index++] = '[';

    left = width - 4 - 1 - 1 - 12 - 8 - 14;
    head = (int)(perc * left * 1.0 / 100);
    tail = left - head;
    while (--head > 0)
    {
        ret[index++] = '-';
    }
    ret[index++] = '>';

    while (--tail >= 0)
    {
        ret[index++] = ' ';
    }

    ret[index++] = ']';
    ret[index] = '\0';
}
/*
 *|<------14---->|
 *|  ------------|
 *|  in  --.--   |
 *|  in  01s     |
 *|  in  12m 05s |
 *|  eta 01d 16h |
 *|<------14---->|
 *显示状态, 如剩余时间, 完成任务的总花时等, 占14个字符, 包括前面两空格字符!
 */
void get_status(const int tm_stm, const int done, const int totals, char *ret)
{
    int index, h, m, s;

    index = 0;
    ret[index++] = ' ';
    ret[index++] = ' ';
    if (done >= 0 && done != totals)
    {
        ret[index++] = 'i';
        ret[index++] = 'n';
        ret[index++] = ' ';
        ret[index++] = ' ';

        /* 时分秒 */
        s = tm_stm;
        h = s / 60 / 60;
        s -= h * 60 * 60;
        m = s / 60;
        s -= m * 60;
    }
    else if (done >= 0 && done < totals)
    {
        ret[index++] = 'e';
        ret[index++] = 't';
        ret[index++] = 'a';
        ret[index++] = ' ';
    }

    while (index < 14)
    {
        ret[index++] = ' ';
    }
    ret[index] = '\0';
}

void create_image(Progress *pro, const int width)
{
    char *p = NULL;
    char spd[8], rec_bytes[12], per[4];
    int len, per_int, tm_stm;

    assert(pro != NULL && width > 0);

    p = pro->buf;
    get_percent(pro->done, pro->total, per);    /* 接收的百分率 */
    strncpy(p, per, 4);

    per_int = (int)(pro->done * 100.0 / pro->total);
    get_ptr_bar(per_int, width, pro->buf + 4);    /* 显示状态条 */

    get_rec_bytes(pro->done, rec_bytes);    /* 接收的数据量 */
    strncpy(p + strlen(p), rec_bytes, 12);

    tm_stm = pro->end_sec - pro->beg_sec;
    if (tm_stm <= 0)
    {
        return;
    }
    get_spd(tm_stm, pro->done, pro->total, spd);    /* 传输平均速度 */
printf("  %s", spd);    /* 宽度动态改变时, 输出字符混乱! */
    len = strlen(p);
    //strncpy(p + len, spd, 8);
    //p[len + 8] = '\0';
}

void display_image(Progress *pro)
{
    fprintf(stdout, "%s", "\r");
    fprintf(stdout, "%s", pro->buf);
}

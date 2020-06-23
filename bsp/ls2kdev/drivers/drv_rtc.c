/*
 * Copyright 2020, Du Huanpeng <548708880@qq.com>
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <rtthread.h>

struct loongson_rtc {
    rt_uint32_t sys_toytrim;
    rt_uint32_t sys_toywrite0;
    rt_uint32_t sys_toywrite1;
    rt_uint32_t sys_toyread0;
    rt_uint32_t sys_toyread1;
    rt_uint32_t sys_toymatch0;
    rt_uint32_t sys_toymatch1;
    rt_uint32_t sys_toymatch2;
    rt_uint32_t sys_rtcctrl;
    rt_uint32_t __pad4[3];
    rt_uint32_t __pad5[4];
    rt_uint32_t sys_rtctrim;
    rt_uint32_t sys_rtcwrite0;
    rt_uint32_t sys_rtcread0;
    rt_uint32_t sys_rtcmatch0;
    rt_uint32_t sys_rtcmatch1;
    rt_uint32_t sys_rtcmatch2;
};

/* bit field helpers. */
#define __M(n)               (~(~0<<(n)))
#define __RBF(number, n)     ((number)&__M(n))
#define __BF(number, n, m)   __RBF((number>>m), (n-m+1))
#define BF(number, n, m)     (m<n ? __BF(number, n, m) : __BF(number, m, n))


struct rtctime {
    rt_uint32_t sys_toyread0;
    rt_uint32_t sys_toyread1;
    rt_uint32_t sys_rtcread0;
};
typedef struct rtctime rtctime_t;

void pr_time(struct tm *time)
{
    static int cnt = 0;

    rt_kprintf("tm_year - tm_mon - tm_mday %d-%d-%d\n",
                time->tm_year, time->tm_mon, time->tm_mday);
    rt_kprintf("tm_hour:tm_min:tm_sec [%d:%d:%d] \n",
                time->tm_hour, time->tm_min, time->tm_sec);
}


struct tm *localrtctime(const rtctime_t *rtctp)
{
    static struct tm time;
    int msec;

    msec = BF(rtctp->sys_toyread0, 3, 0);
    msec *= 100;

    time.tm_sec   = BF(rtctp->sys_toyread0,  9,  4);
    time.tm_min   = BF(rtctp->sys_toyread0, 15, 10);
    time.tm_hour  = BF(rtctp->sys_toyread0, 20, 16);
    time.tm_mday  = BF(rtctp->sys_toyread0, 21, 25);
    time.tm_mon   = BF(rtctp->sys_toyread0, 26, 31);
//  time.tm_isdst = 0;
//  time.tm_wday  = 0;
//  time.tm_yday  = 0;

    time.tm_mon  -= 1;

    time.tm_year  = rtctp->sys_toyread1;

    return &time;
}


rtctime_t mkrtctime(struct tm *tm)
{
    rtctime_t rtctm;
    struct tm tmptime;


    rt_kprintf("%s %d\n", __func__, __LINE__);
    pr_time(tm);


    rtctm.sys_toyread0 <<= 31 - 26 + 1;
    rtctm.sys_toyread0   = tm->tm_mon + 1;


    rtctm.sys_toyread0 <<= 25 - 21 + 1;
    rtctm.sys_toyread0  |= tm->tm_mday;

    rtctm.sys_toyread0 <<= 20 - 16 + 1;
    rtctm.sys_toyread0  |= tm->tm_hour;

    rtctm.sys_toyread0 <<= 15 - 10 + 1;
    rtctm.sys_toyread0  |= tm->tm_min;

    rtctm.sys_toyread0 <<= 9 - 4 + 1;
    rtctm.sys_toyread0  |= tm->tm_sec;
    /* Fixme: 0.1 second */
    rtctm.sys_toyread0 <<= 3 - 0 + 1;
    rtctm.sys_toyread0  |= 0;

    rtctm.sys_toyread1 = tm->tm_year;

    rt_kprintf("%s %d\n", __func__, __LINE__);
    tmptime = *localrtctime(&rtctm);
    pr_time(&tmptime);

    return rtctm;
}



void hw_rtc_dump(void)
{
    rt_uint32_t t;
    volatile struct loongson_rtc *hw_rtc = (void *)0xFFFFFFFFBFE07820;

    t = hw_rtc->sys_toyread0;
    rt_kprintf("1.   [%p]: DEC[%10d], HEX[%8x]\n", & hw_rtc->sys_toyread0, t, t);

    t = hw_rtc->sys_toyread1;
    rt_kprintf("2.   [%p]: DEC[%10d], HEX[%8x]\n", & hw_rtc->sys_toyread1, t, t);

    t = hw_rtc->sys_rtcread0;
    rt_kprintf("3.   [%p]: DEC[%10d], HEX[%8x]\n", & hw_rtc->sys_rtcread0, t, t);

    rt_kprintf("-------------------\n");

}


static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    rt_kprintf("%s\n", __func__);
    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_kprintf("%s\n", __func__);
    return 0;
}



static rt_err_t rt_rtc_ioctl(rt_device_t dev, int cmd, void *args)
{
    rt_err_t err = RT_ENOSYS;

    static int count = 0;

    struct loongson_rtc *hw_rtc;
    rtctime_t rtctm;
    struct tm time;
    struct tm tmptime;
    time_t *t;

    rt_kprintf("---------------------------------\n");


    hw_rtc = dev->user_data;

    t = (time_t *)args;
    time = *localtime(t);

    rtctm.sys_toyread0 = hw_rtc->sys_toyread0;
    rtctm.sys_toyread1 = hw_rtc->sys_toyread1;
    rtctm.sys_rtcread0 = hw_rtc->sys_rtcread0;
    tmptime = *localrtctime(&rtctm);

    pr_time(&time);


    switch (cmd) {

    case RT_DEVICE_CTRL_RTC_GET_TIME:
        *t = mktime(&tmptime);
        rt_kprintf("RT_DEVICE_CTRL_RTC_GET_TIME\n");
    break;

    case RT_DEVICE_CTRL_RTC_SET_TIME:
            tmptime.tm_hour = time.tm_hour;
            tmptime.tm_min  = time.tm_min;
            tmptime.tm_sec  = time.tm_sec;

            tmptime.tm_year = time.tm_year;
            tmptime.tm_mon  = time.tm_mon;
            tmptime.tm_mday = time.tm_mday;

            rtctm = mkrtctime(&tmptime);
            /* write to hw RTC */
            hw_rtc->sys_toywrite0 = rtctm.sys_toyread0;
            hw_rtc->sys_toywrite1 = rtctm.sys_toyread1;
            rt_kprintf("RT_DEVICE_CTRL_RTC_SET_TIME\n");
        break;

    case RT_DEVICE_CTRL_RTC_GET_ALARM:
        rt_kprintf("RT_DEVICE_CTRL_RTC_GET_ALARM\n");
    break;

    case RT_DEVICE_CTRL_RTC_SET_ALARM:
        rt_kprintf("RT_DEVICE_CTRL_RTC_SET_ALARM\n");
    break;

    default:
        rt_kprintf("ENOSYS\n");
    break;
    }

    return RT_EOK;
}


int rt_hw_rtc_init(void)
{
    static struct rt_device rtc = {
        .type      = RT_Device_Class_RTC,
        .init      = RT_NULL,
        .open      = rt_rtc_open,
        .close     = RT_NULL,
        .read      = rt_rtc_read,
        .write     = RT_NULL,
        .control   = rt_rtc_ioctl,
        .user_data = (void *)0xFFFFFFFFBFE07820,
    };

    rt_kprintf("%s\n", __func__);

    /* need initial operation ? */

    rt_device_register(&rtc, "rtc", RT_DEVICE_FLAG_RDWR);
}

INIT_DEVICE_EXPORT(rt_hw_rtc_init);

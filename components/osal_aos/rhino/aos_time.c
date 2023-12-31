/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "rhino_p.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

static long long start_time_ms = 0;

void aos_calendar_time_set(uint64_t now_ms)
{
    start_time_ms = now_ms - krhino_sys_time_get();
}

uint64_t aos_calendar_time_get(void)
{
    return krhino_sys_time_get() + start_time_ms;
}

uint64_t aos_calendar_localtime_get(void)
{
    if ((int64_t)(aos_calendar_time_get() - 8 * 3600 * 1000) < 0) {
        return aos_calendar_time_get();
    }
    return aos_calendar_time_get() + 8 * 3600 * 1000;
}

uint64_t aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}

uint64_t aos_now_ms(void)
{
    return krhino_sys_time_get();
}


aos_status_t aos_now_time_str(char *buffer, size_t len)
{

    uint64_t ms = aos_now_ms();

    if (buffer == NULL || len == 0) {
        return -EINVAL;
    }

    time_t actualTime = { ms / 1000 };
    memset(buffer, 0, len);
    strftime(buffer, len, "%m-%d %H:%M:%S", gmtime(&actualTime));
    if ((int)(len - strlen(buffer) - 1) > 0) {
        const int milli    = ms % 1000;
        char      msStr[8] = {0};
        snprintf(msStr, sizeof(msStr), ".%03d", milli);
        strncat(buffer, msStr, len - strlen(buffer) - 1);
    } else {
        return -EINVAL;
    }

    return 0;
}

#define POSIX_MAX_DELAY_NS 50000000UL

void aos_ndelay(uint32_t nsecs)
{
    uint64_t start = HR_COUNT_GET();
    uint64_t freq = HR_FREQ_GET();

    /* Avoid cycly count overflow. */
    if (nsecs > POSIX_MAX_DELAY_NS)
        return;

    if (!start || !freq)
        return;

    /* Loop until enough nanoseconds.
     * The resolution depends on HR_FREQ_GET implementation in soc.
     */
    while ((HR_COUNT_GET() - start) < (nsecs * freq) / 1000000000ULL)
        ;
}

void aos_udelay(uint32_t usecs)
{
    aos_ndelay(usecs * 1000);
}

void aos_mdelay(uint32_t msecs)
{
    aos_udelay(msecs * 1000);
}

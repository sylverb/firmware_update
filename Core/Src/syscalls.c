#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "gw_rtc.h"

extern uint32_t log_idx;
extern char logbuf[1024 * 4];

int _write(int file, char *ptr, int len)
{
    uint32_t idx = log_idx;
    if (idx + len + 1 > sizeof(logbuf))
    {
        idx = 0;
    }

    memcpy(&logbuf[idx], ptr, len);
    idx += len;
    logbuf[idx] = '\0';

    log_idx = idx;

    return len;
}

int _gettimeofday(struct timeval *tv, void *tzvp)
{
    if (tv)
    {
        // get epoch UNIX time from RTC
        time_t unixTime = GW_GetUnixTime();
        tv->tv_sec = unixTime;

        // get millisecondes from rtc and convert them to microsecondes
        uint64_t millis = GW_GetCurrentMillis();
        tv->tv_usec = (millis % 1000) * 1000;
        return 0;
    }

    errno = EINVAL;
    return -1;
}

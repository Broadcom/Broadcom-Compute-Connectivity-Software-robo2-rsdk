/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *    sal_time.c
 * * Purpose: Time management
 * */


#include <sys/types.h>
#include <sys/time.h>
#if defined(linux) || (__STRICT_ANSI__)
#include <time.h>
#endif
#include <sal_time.h>

/*
 * Function:
 *	sal_time_usecs
 * Purpose:
 *	Returns the relative time in microseconds modulo 2^32.
 * Returns:
 *	Time in microseconds modulo 2^32
 * Notes:
 *	The precision is limited to the Unix clock period.
 *	Time is monotonic if supported by the O/S.
 *
 */

sal_usecs_t
sal_time_usecs(void)
{
    struct timeval	ltv;
#ifdef CLOCK_MONOTONIC
    struct timespec	lts;
    if (clock_gettime(CLOCK_MONOTONIC, &lts) == 0) {
        return (lts.tv_sec * SECOND_USEC + lts.tv_nsec / 1000);
    }
#endif
    /* Fall back to RTC if monotonic clock unavailable */
    gettimeofday(&ltv, 0);
    return (ltv.tv_sec * SECOND_USEC + ltv.tv_usec);
}

/*
 * Function:
 *	sal_time
 * Purpose:
 *	Return the current time in seconds since 00:00, Jan 1, 1970.
 * Returns:
 *	Time in seconds
 * Notes:
 *	This routine must be implemented so it is safe to call from
 *	an interrupt routine.  It is used for timestamping and other
 *	purposes.
 */

sal_time_t
sal_time(void)
{
    /* "Interrupt safe" since intrs are fake for PLISIM */
    return (sal_time_t) time(0);
}
/*
 * Function:
 *  sal_sleep
 * Purpose:
 *  Suspend calling thread for a specified number of seconds.
 * Parameters:
 *  sec - number of seconds to suspend
 * Notes:
 *  Other tasks are free to run while the caller is suspended.
 */

void
sal_sleep(int sec)
{
    struct timeval tv;
    tv.tv_sec = (time_t) sec;
    tv.tv_usec = 0;
    select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
}

/*
 * Function:
 *  sal_usleep
 * Purpose:
 *  Suspend calling thread for a specified number of microseconds.
 * Parameters:
 *  usec - number of microseconds to suspend
 * Notes:
 *  The actual delay period depends on the resolution of the
 *  Unix select routine, whose precision is limited to the
 *  the period of the scheduler tick, generally 1/60 or 1/100 sec.
 *  Other tasks are free to run while the caller is suspended.
 */

void
sal_usleep(uint32 usec)
{
    struct timeval tv;

    if (usec < (2 * SECOND_USEC)/HZ) {
        sal_usecs_t now;
        sal_usecs_t earlier;
        sal_usecs_t delta;
        sal_usecs_t max_time = SAL_UINT32_MAX;

        earlier = sal_time_usecs();
        do {
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
            now = sal_time_usecs();
            if (now < earlier) {
                delta = max_time - earlier + now;
            } else {
                delta = now - earlier;
            }
        } while (delta < usec);
    }
    else {
        tv.tv_sec = (time_t) (usec / SECOND_USEC);
        tv.tv_usec = (long) (usec % SECOND_USEC);
        select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
    }
}

/*
 * Function:
 *  sal_nsleep
 * Purpose:
 *  Suspend calling thread for a specified number of nanoseconds.
 * Parameters:
 *  nsec - number of nanoseconds to suspend
 */

void
sal_nsleep(uint64 nsec)
{
    struct timespec sleep_time;
    uint64_t tot_nsec;

    sal_memset(&sleep_time,0,sizeof(struct timespec));
    if (clock_gettime(CLOCK_MONOTONIC, &sleep_time) == 0) {
        tot_nsec = sleep_time.tv_nsec + nsec;
        if (tot_nsec >= 1000000000) {
            sleep_time.tv_sec++;
            sleep_time.tv_nsec = tot_nsec - 1000000000;
        } else {
            sleep_time.tv_nsec = tot_nsec;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &sleep_time, NULL);
    }
}

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File: sal_time.h
 * Purpose: SAL time definitions
 *
 * Microsecond Time Routines
 *
 *  The main clock abstraction is sal_usecs_t, an unsigned 32-bit
 *  counter that increments every microsecond and wraps around every
 *  4294.967296 seconds (~71.5 minutes)
 */

#ifndef _SAL_TIME_H
#define _SAL_TIME_H

#include <stdlib.h>
#include "sal_types.h"
#include "sal_task.h"

#ifdef OPENRTOS
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#endif

/*
 * Constants
 */

#define MILLISECOND_USEC       (1000)
#define SECOND_USEC            (1000000)
#define MINUTE_USEC            (60 * SECOND_USEC)
#define HOUR_USEC              (60 * MINUTE_USEC)

#define SECOND_MSEC            (1000)
#define MINUTE_MSEC            (60 * SECOND_MSEC)
#ifndef HZ
#define HZ                     (100)
#endif
#define SAL_UINT32_MAX         (0xffffffff)


typedef unsigned long sal_time_t;
typedef uint32 sal_usecs_t;

#ifdef OPENRTOS
typedef xTimerHandle sal_tmr_id;
#else
typedef void* sal_tmr_id;
#endif

typedef struct _timer_info {
    void (*callback)(void *data);
    void *data;
    sal_thread_t tid;
} timer_info_t;

/* Timer - sleep for micro seconds */
extern void sal_usleep(uint32 usec);

/* Timer - sleep for number of seconds (background tasks would be executed) */
extern void sal_sleep(int sec);

/* Timer - sleep for nano seconds */
extern void sal_nsleep(uint64 nsec);

/* Relative time in seconds from last tick */
extern sal_time_t sal_time_last_tick(sal_time_t last_tick);

/* Relative time in microseconds from last tick */
extern sal_usecs_t sal_time_usecs_last_tick(sal_time_t last_tick);

/* Relative time in microseconds */
extern sal_usecs_t sal_time_usecs(void);

/* Timer - Get current ticks */
extern sal_time_t sal_tick_get(void);

/* Get tick duration in micro seconds */
extern uint32_t sal_get_us_per_tick(void);

extern sal_tmr_id sal_timer_add
(
    int8 *timer_name,
    int32 interval,
    int8  periodic,
    void (*callback)(void *),
    void *data
);

extern int32 sal_timer_stop(sal_tmr_id timer_id);
extern int32 sal_timer_restart(sal_tmr_id timer_id);
extern int32 sal_timer_delete(sal_tmr_id timer_id);

#endif    /* !_SAL_TIME_H */

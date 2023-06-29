/*
 * $Copyright: (c) 2017 Broadcom Limited.
 * All Rights Reserved.$
 *
 * File:
 *     timer.c
 * Purpose:
 *     Timer module
 */

#ifdef __C51__
#ifdef CODE_USERCLASS
#pragma userclass (code = krntime)
#endif /* CODE_USERCLASS */
#endif /* __C51__ */

#include "system.h"
#include "sal_console.h"
#include "sal_time.h"
#if 0
#include "FreeRTOS.h"
#include "sal_task.h"
#include "sal_sync.h"
#endif

/* Note: #if 0 code in the file are compiling appropriately with freertos OS code.
 * The #if 0 code will be resolved when openrtos is added to repository.
 */


#define CFG_TIMER_CALLBACK_SUPPORT  1
#define CFG_MAX_REGISTERED_TIMERS   16

#if CFG_TIMER_CALLBACK_SUPPORT

struct timer_t {
    tick_t      interval;   /* In ticks */
    tick_t      start;      /* In ticks */
    TIMER_FUNC  cbk;
    void *      arg;
    BOOL        inprogress; /* To avoid reentrance */
};

STATIC struct timer_t timer[CFG_MAX_REGISTERED_TIMERS];
STATIC tick_t previous_tick;
#if 0
sal_thread_t  tmr_task_hdl;
sal_sem_t tmr_semid = NULL;
#endif

/* Forwards */
APISTATIC void sys_timer_task(void *param) REENTRANT;

extern void sys_timer_init(void) REENTRANT;

APISTATIC void
APIFUNC(sys_timer_task)(void *param) REENTRANT {
    while(1)
    {
        /* Check and run timers only once per tick */
        if (previous_tick != sal_tick_get()) {

            uint8 i;
            TIMER_FUNC cbk;
            tick_t curr;
            struct timer_t *pt = &timer[0];

            UNREFERENCED_PARAMETER(param);

#if 0
            /* Acquire semaphore before accessing timer database */
            sal_sem_take(tmr_semid, 0);
#endif

            for(i=0; i<CFG_MAX_REGISTERED_TIMERS; i++, pt++) {
                cbk = pt->cbk;
                if (cbk == NULL) {
                    break;
                }
                if (!pt->inprogress) {
                    curr = sal_tick_get();

                    if (pt->interval == 0 ||
                            curr - pt->start >= pt->interval) {

                        pt->inprogress = TRUE;
                        (*cbk)(pt->arg);

                        /* Double check in case it's removed during execution */
                        if (pt->cbk == cbk) {
                            pt->inprogress = FALSE;
                            pt->start = curr;
                        }
                    }
                }
            }
#if 0
            /* Release timer database semaphore */
            sal_sem_give(tmr_semid);
#endif

            previous_tick = sal_tick_get();
        }
        /* TBD: Tune the sleep time to accomodate smallest timer in the code.
         * Sleep for 100 milli seconds (for now) to yield for other tasks to execute
         * in the system.
         */
        /* sal_sleep(10000); */
    }
}

void
APIFUNC(sys_timer_init)(void) REENTRANT {
    uint8 i;
    for(i=0; i<CFG_MAX_REGISTERED_TIMERS; i++)
    {
        timer[i].cbk = NULL;
    }

    /* Get current ticks */
    previous_tick = sal_tick_get();

#if 0
    /* Create timer semaphore */
    if (!tmr_semid)
    {
        tmr_semid = sal_sem_create("sem",0,10);
    }

    /* Register background process for handling timer callback */
    /* task_add(sys_timer_task, NULL); */
    tmr_task_hdl = sal_thread_create("timer_task", SAL_THREAD_STKSZ,
    tskIDLE_PRIORITY,
    sys_timer_task, NULL);
#endif
}

BOOL
APIFUNC(timer_add)(TIMER_FUNC func, void *arg, uint32 usec) REENTRANT {
    uint8 i;
    tick_t curr = sal_tick_get();

    SAL_DEBUGF(("timer_add: %p\n", func));
    if (func == NULL)
    {
        return FALSE;
    }
#if 0
    /* Acquire semaphore before accessing timer database */
    sal_sem_take(tmr_semid, 0);
#endif

    /* Check if it has been registered */
    for(i=0; i<CFG_MAX_REGISTERED_TIMERS; i++)
    {
        if (timer[i].cbk != NULL) {

            if (func == timer[i].cbk) {

                /* Already registered */
                timer[i].interval = usec / (1000 * sal_tick_get());
                timer[i].start = curr;
                timer[i].arg = arg;
#if 0
                /* Release timer database semaphore */
                sal_sem_give(tmr_semid);
#endif

                return TRUE;
            }
        }
    }

    /* Found an empty slot */
    for(i=0; i<CFG_MAX_REGISTERED_TIMERS; i++)
    {
        if (timer[i].cbk == NULL) {

            timer[i].cbk = func;
            timer[i].interval = usec / (1000 * sal_tick_get());
            timer[i].start = curr;
            timer[i].arg = arg;
            timer[i].inprogress = FALSE;
#if 0
            /* Release timer database semaphore */
            sal_sem_give(tmr_semid);
#endif

            return TRUE;
        }
    }
#if 0
    /* Release timer database semaphore */
    sal_sem_give(tmr_semid);
#endif

    return FALSE;
}
void
APIFUNC(timer_remove)(TIMER_FUNC func) REENTRANT {
    uint8 i;
    SAL_DEBUGF(("timer_remove: %p\n", func));
#if 0
    /* Acquire semaphore before accessing timer database */
    sal_sem_take(tmr_semid, 0);
#endif
    for(i=0; i<CFG_MAX_REGISTERED_TIMERS; i++)
    {
        if (timer[i].cbk == func) {
            break;
        }
    }

    if (i == CFG_MAX_REGISTERED_TIMERS)
    {
#if 0
        /* Release timer database semaphore */
        sal_sem_give(tmr_semid);
#endif
        return;
    }

    for (; i < CFG_MAX_REGISTERED_TIMERS - 1; i++)
    {
        timer[i].cbk = timer[i + 1].cbk;
        timer[i].interval = timer[i + 1].interval;
        timer[i].start = timer[i + 1].start;
        timer[i].arg = timer[i + 1].arg;
        timer[i].inprogress = timer[i + 1].inprogress;
    }
    timer[i].cbk = NULL;
#if 0
    /* Release timer database semaphore */
    sal_sem_give(tmr_semid);
#endif

}

#endif /* CFG_TIMER_CALLBACK_SUPPORT */

/*
 * $Copyright: (c) 2017 Broadcom Limited.
 * All Rights Reserved.$
 * File:
 *    timer.h
 * Description
 *    timer.h
 *
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef UM_KEIL_BUILD
#include <reg51.h>
#else
#include <8051.h>  // Special Function Registers of 80C51 MCU
#endif

/**********************************************************************************************************************
 Public Type Definitions
**********************************************************************************************************************/
#define CFG_MAX_REGISTERED_TIMERS       (6)
#define FREQUENCY_IN_MHZ                (250)
#define MAX_MEASURABLE_TIME_IN_TICKS    (0xffff) //(0x10000u) - This will create trouble for lsw = 0;

/* Type of system ticks */
typedef uint32_t tick_t;

/* Timer registration/notification */
typedef void (*TIMER_FUNC)(void *);

typedef struct time {
    uint32_t msw; /* MSW of Time Interval In ticks */
    uint32_t lsw; /* LSW of Time Interval In ticks */
} time_t;

typedef struct tmr {
    time_t      abs_intvl;    /* Absolute Interval In ticks */
    time_t      rel_intvl;    /* Relative Interval In ticks */
    TIMER_FUNC  cbk;          /* Callback Function */
    uint16_t    inprogress;   /* To avoid reentrance */
    uint16_t    nxt;          /* Index of the next timer object in either free or busy list */
} timer_t;

/**********************************************************************************************************************
 Public APIs
**********************************************************************************************************************/
extern volatile timer_t timer[CFG_MAX_REGISTERED_TIMERS];
extern volatile uint16_t freehead;
extern volatile uint16_t busyhead;
#if 0
/* For Testing / Debuging */
extern volatile uint16_t expirycount;
extern volatile uint16_t addedathead;
extern volatile uint32_t currenttime;
extern volatile uint32_t newtime;
extern volatile uint32_t residue;
extern volatile uint32_t curtime1;
extern volatile uint32_t curtime2;
#endif
extern void disable_interrupts(void);
extern void enable_interrupts(void);
extern void disable_timer_interrupts(uint16_t hwtimer);
extern void enable_timer_interrupts(uint16_t hwtimer);
extern void start_timer(uint16_t hwtimer);
extern void stop_timer(uint16_t hwtimer);
extern void initialize_timers(void);
extern uint16_t add_timer(TIMER_FUNC func, void *arg, uint32_t usec);
extern uint16_t remove_timer(uint16_t timer_id);
//extern void test_timer(uint32 interval, TIMER_FUNC cbk);

#endif /* _TIMER_H_ */

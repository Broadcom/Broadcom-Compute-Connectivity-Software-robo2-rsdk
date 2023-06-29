/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File
 *    wdog.h
 * Description
 *    Watchdog management defs
 */

#ifndef __WDOG_H

#include <soc/robo2/bcm53158/init.h>

#ifdef CORTEX_M7

#define WATCHDOG_TIMER_CLOCK         (80)        /* 80ns Based on 12.5Mhz clock */
#define WATCHDOG_TIMER_MULTIPLIER    (12500000UL)  /* 1/TIMER_CLOCK */
#define WATCHDOG_TIMEOUT_VALUE       (BCM53158_WATCHDOG_TIMEOUT_SECS * WATCHDOG_TIMER_MULTIPLIER)

#define WATCHDOG_LOCK_UNLOCK         0x1ACCE551
#define WATCHDOG_LOCK_LOCK           0x0ACCE550
#define WATCHDOG_LOCK_UNLOCKED       0x00000000
#define WATCHDOG_LOCK_LOCKED         0x00000001

#define WATCHDOG_INTR_CLEAR          0x0000000F
#define WATCHDOG_INTR_ENABLE         0x00000001
#define WATCHDOG_RESET_ENABLE        0x00000002

#define WATCHDOG_CNTR_LOAD_OFFSET    0x00000000
#define WATCHDOG_CNTR_VALUE_OFFSET   0x00000004
#define WATCHDOG_CONTROL_OFFSET      0x00000008
#define WATCHDOG_INTR_CLR_OFFSET     0x0000000C
#define WATCHDOG_LOCK_OFFSET         0x00000C00

#define WATCHDOG_RESET_CTRL_RESET    0x1
#define WATCHDOG_RESET_CTRL_INTR     0x0

/* Declarations */
extern void watchdog_timer_cancel(void);
extern void watchdog_unlock(void);
extern uint32 watchdog_get_reset_reason(void);
extern void watchdog_set_reset_reason(uint32 reason);
extern int watchdog_init(uint32 userdata, int timeout);
extern void watchdog_testmode(uint32 mode);
extern void watchdog_reload(uint32 timer);
extern void watchdog_reboot(void);


#endif /* CORTEX_M7 */

#endif /* __WDOG_H */

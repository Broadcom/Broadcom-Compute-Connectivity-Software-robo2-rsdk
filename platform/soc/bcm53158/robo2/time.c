/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    time.c
 * Description:
 *    This file contains Systick code for bcm53158_a0
 */

#ifdef CORTEX_M7

#include <sal_types.h>
#include <soc/robo2/bcm53158/config.h>
#include <soc/cpu_m7.h>

#if (__Vendor_SysTickConfig == 0U)

static volatile uint32 sysTicks = 0;

int init_systime(void)
{
     int rc = 0;
     rc = SysTick_Config( BCM53158_A0_M7CORE_CLOCK / BCM53158_TICKS_RATE );
     if (rc != 0) {
         /* TBD Error Handling */
     }
     return rc;
}

void systick_handler(void) {
    sysTicks++;
}

#endif /* Vendor_SysTick */
#endif /* Cortex M7 */

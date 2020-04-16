/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    system_bcm53158.c
 * Description:
 *    Implementation for bcm53158 system routines
 */

#include <sal_types.h>
#include <soc/robo2/bcm53158/config.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/common/console.h>
#include <soc/cpu_m7.h>
#include <sal_libc.h>
#include <sal_console.h>


/* This should probably be set based on detecting the chip id */
uint32 SystemCoreClock = BCM53158_A0_M7CORE_CLOCK;

void sysdelay(int usecs)
{
    int u;
    u = usecs;
    while(u > 0) {u--;}
    u = usecs;
    while(u > 0) {u--;}
    u = usecs;
    while(u > 0) {u--;}
}


/*
 * sysdbgwait
 * Hold system boot till debugger connection is active
 * Use debugger to get boot moving out of this routine
 */
void sysdbgwait(void)
{
    wait_for_debugger();
}

/**
 *  @brief SystemEarlyInit
 *  System early init
 */
inline
void SystemEarlyInit()
{
    //sysprint_setup(UART_DEFAULT_BAUDRATE);
#ifdef BCM_53158_A0
    bcm53158_early_init();
#endif
}


/**
 *  @brief SystemInit
 *  Initialize M7SS
 */
void SystemInit()
{
#ifdef BCM_53158_A0
    bcm53158_core_init();
#endif
}

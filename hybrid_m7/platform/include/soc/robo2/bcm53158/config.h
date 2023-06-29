/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 */

/* 
 * BCM53158 Clocks and Memmap Configs 
 */
#ifdef BCM_53158_A0 

#include <soc/robo2/bcm53158/memmap_bcm53158_a0.h>

#ifdef EMULATION_BUILD
/* CPU Clock */
#define BCM53158_A0_M7CORE_CLOCK (100*1000*1000)                  /* 100Mhz */

/* UART Clock */
#define BCM53158_A0_UART_CLOCK   (153600)                        /* 153.6Khz */ 

/* XGXS Sleep time */
#define XGXS_SLEEP_US            (50000)

#else /* EMULATION_BUILD */

/* CPU Clock */

#define BCM53158_A0_M7CORE_CLOCK_DEF (200*1000*1000)                  /* 200Mhz */
#define BCM53158_A0_M7CORE_CLOCK_MAX (500*1000*1000)                  /* 500Mhz */
#define BCM53158_A0_M7CORE_CLOCK     (BCM53158_A0_M7CORE_CLOCK_DEF)

/* UART Clock */
#define BCM53158_A0_UART_CLOCK   (25*1000*1000)                       /* 25 Mhz */

/* XGXS Sleep time */
#define XGXS_SLEEP_US            (1000)

#endif /* EMULATION_BUILD */

#define BCM53158_TICKS_RATE  (1000)

#endif /* BCM_53158_A0 */

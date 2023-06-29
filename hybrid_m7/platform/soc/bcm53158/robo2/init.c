/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    init.c
 * Description:
 *    This file contains initialization code for bcm53158_a0
 */

#ifdef CORTEX_M7

#include <sal_types.h>
#include <sal_console.h>
#include <soc/cpu_m7.h>
#include <soc/robo2/bcm53158/config.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/mpu.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/irq.h>
#include <soc/robo2/bcm53158/memmap_bcm53158_a0.h>
#include <soc/error.h>

/* Round up to MB */
#define ROUNDUP_TO_MB(v) (((v) + 0x100000) & ~0xfffff)

extern int 
int2str(char *buffer, int dec, unsigned num, int size, char pad);

/*
 * Init CPU config
 */
static void init_cpu(void)
{
    SCB->CCR = 0x00040201;
    SCB->SHCSR = 0x70000;
}

/*
 * Initialize MPU
 */
int init_mpu(void)
{
    int rv = 0;
    uint32 region_no = 0;

    rv += m7_mpu_set_region(region_no++, BCM53158_A0_LOCAL_ITCM_BASE_ADDR, BCM53158_A0_LOCAL_ITCM_SIZE,
                            M7_MPU_TEX_NORMAL_MEM | M7_MPU_AP_ALL_ACCESS);


    rv += m7_mpu_set_region(region_no++, BCM53158_A0_LOCAL_QSPI_FLASH_BASE_ADDR,
                            BCM53158_A0_LOCAL_QSPI_FLASH_SIZE, M7_MPU_CACHEABLE | M7_MPU_AP_ALL_ACCESS);

    rv += m7_mpu_set_region(region_no++, BCM53158_A0_LOCAL_DTCM_BASE_ADDR, BCM53158_A0_LOCAL_DTCM_SIZE,
                            M7_MPU_TEX_NORMAL_MEM | M7_MPU_AP_ALL_ACCESS);
    if (rv == 0) {
        m7_mpu_enable();
        sysprint("MPU enabled\n");
    }
    return rv;
}

/*
 * Initialize the TCM
 */
void init_tcm(void)
{
    m7_tcm_enable();
}

/*
 * Set priority for Interrupts
 */
void init_irq(void)
{
    IRQn_Type irq;
    for (irq=UC_TO_M7_SW_IRQn; irq < MAX_NUM_IRQn; irq++) {
        m7_set_irq_priority(irq, BCM53158_A0_IRQ_DEFAULT_PRIORITY);
    }
    m7_set_irq_grouping(BCM53158_A0_IRQ_DEFAULT_GROUPING);
}

/*
 * Enable the ICACHE and DCACHE
 */
void init_caches()
{
    m7_dcache_invalidate_all();
    m7_dcache_enable();
    m7_icache_invalidate_all();
    m7_icache_enable();
}


/**
 * @brief bcm53158_early_init
 * Setup the M7 CPU and enable TCM
 */ 
int bcm53158_early_init(void) 
{
    init_cpu();
    init_tcm();
    return SOC_E_NONE;
}

/**
 * @brief bcm53158_core_init
 * Setup the M7SS subsystem, Uart and Hw Blocks 
 */ 
int bcm53158_core_init(void) 
{
    int rv = 0;
    init_caches();
    init_irq();
    return SOC_E_NONE;
}

#endif /* CORTEX_M7 */

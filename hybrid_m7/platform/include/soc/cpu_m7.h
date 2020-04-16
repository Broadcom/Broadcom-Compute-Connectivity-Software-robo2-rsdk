/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 * $Id: $
 *
 * File:
 *     cpu_m7.h
 * Description:
 *     This file contains the paramters that reflect the M7 Core
 */

#ifdef CORTEX_M7

#ifndef __CPU_M7_H
#define __CPU_M7_H

#ifdef BCM_53158_A0
#include <soc/robo2/bcm53158/irq.h>
#include <soc/robo2/bcm53158/config.h>
#endif

/**
 * | M7 CPU Configuration parameters
 * |----------------------------------------------------------------------------------------------------
 * | Configuration          |  Range   |  Default |  Description
 * | ---------------------------------------------------------------------------------------------------
 * | __CM7_REV	            |  0x0000  |  0x0000  |  Core revision number ([15:8] revision number, [7:0] patch number)
 * | ---------------------------------------------------------------------------------------------------
_* | __MPU_PRESENT          |  0 .. 1  |   0      |  Defines if a MPU is present or not
 * | ---------------------------------------------------------------------------------------------------
_* | __NVIC_PRIO_BITS       |  2 .. 8  |   4      |  Number of priority bits implemented in the NVIC (device specific)
 * |
 * | ---------------------------------------------------------------------------------------------------
_* | __Vendor_SysTickConfig | 0 .. 1   |   0	  |  If this define is set to 1,
 * |                                              |  then the default SysTick_Config function is excluded.
 * | ---------------------------------------------------------------------------------------------------
_* | __FPU_PRESENT          | 0 .. 1   |   0	  | Defines if a FPU is present or not. See __FPU_DP description below.
 * | ---------------------------------------------------------------------------------------------------
_* | __FPU_DP               | 0 .. 1   |   0	  | The combination of the defines __FPU_PRESENT and __FPU_DP
 * |                                                determine the whether the FPU is with single or double precision
 * |                                                as shown in the table below.
_* |                                                 _FPU_PRESENT  __FPU_DP  Description
 * |                                                      0	      x      Processor has no FPU.
 * |                                                      1	      0	     Processor with FPU with single precision.
 * |                                                      1           1	     Processor with FPU with double precision.
 * | ---------------------------------------------------------------------------------------------------
_* | __ICACHE_PRESENT	    |  0 .. 1  |   1	   |  Instruction Chache present or not
 * |----------------------------------------------------------------------------------------------------
_* | __DCACHE_PRESENT	    |  0 .. 1  |   1	   |  Data Chache present or not
 * | ---------------------------------------------------------------------------------------------------
_* |  __DTCM_PRESENT	    |  0 .. 1  |   1	   |  Data Tightly Coupled Memory is present or not
 * |----------------------------------------------------------------------------------------------------
 */


#ifdef BCM_53158_A0

#define __CM7_REV                   0x0001  /**< r0p1 */
#define __MPU_PRESENT               1
#define __NVIC_PRIO_BITS            3
#define __FPU_PRESET                0
#define __ICACHE_PRESENT            1
#define __DCACHE_PRESENT            1
#define __DTCM_PRESENT              1
#define __ITCM_PRESENT              1


#define __Vendor_SysTickConfig      0

#endif /* BCM_53158_A0 */

#include <core_cm7.h>

static inline
int m7_cpu_in_isr(void)
{
    return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0);
}

/* Declarations */
void m7_tcm_enable(void);
void m7_tcm_disable(void);
void m7_vector_table_set(uint32_t base);
void m7_enable_intr(void);
void m7_disable_intr(void);
void m7_enable_irq(IRQn_Type irq);
void m7_disable_irq(IRQn_Type irq);
void m7_set_irq_priority(IRQn_Type irq, uint32 priority);
uint32 m7_get_irq_priority(IRQn_Type irq);
int m7_mpu_set_region(uint32 region, uint32 region_base, uint32 region_size, uint32 region_attr);
inline int m7_mpu_enable(void);
inline int m7_mpu_disable(void);
void m7_mpu_enable_region(uint32 region);
void m7_mpu_disable_region(uint32 region);
void m7_dcache_invalidate(uint32 *addr, int len);
void m7_dcache_flush(uint32 *addr, int len);
void m7_dcache_invalidate_all(void);
void m7_icache_invalidate_all(void);
void m7_dcache_enable(void);
void m7_dcache_disable(void);
void m7_icache_enable(void);
void m7_icache_disable(void);
void m7_set_irq_grouping(uint32 grouping);




#endif /* __CPU_M7_H */

#endif /* CORTEX_M7 */

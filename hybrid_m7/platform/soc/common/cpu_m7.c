/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 * $Id: $
 *
 * File:
 *     cpu_m7.c
 * Description:
 *     Helper routines for the M7 Core
 */

#ifdef CORTEX_M7


#include <sal_types.h>
#include <sal_console.h>
#include <soc/error.h>
#include <soc/cpu_m7.h>


/**
 * @brief  TCM memory enable
 * The function enables TCM memories
 * 
 */
inline void m7_tcm_enable(void) 
{
    __DSB();
    __ISB();
    SCB->ITCMCR = (SCB_ITCMCR_EN_Msk  | SCB_ITCMCR_RMW_Msk 
                    | SCB_ITCMCR_RETEN_Msk);
    SCB->DTCMCR = ( SCB_DTCMCR_EN_Msk | SCB_DTCMCR_RMW_Msk 
                    | SCB_DTCMCR_RETEN_Msk);
    __DSB();
    __ISB();
}

/** 
 * @brief  TCM memory Disable
 * The function enables TCM memories
 */
inline void m7_tcm_disable(void) 
{
    __DSB();
    __ISB();
    SCB->ITCMCR &= ~(SCB_ITCMCR_EN_Msk);
    SCB->DTCMCR &= ~(SCB_ITCMCR_EN_Msk);
    __DSB();
    __ISB();
}

/**
 * @brief Update m7 vector table
 * The function updates m7 VTOR
 */
inline void m7_vector_table_set(uint32_t base) 
{
    SCB->VTOR = (base & SCB_VTOR_TBLOFF_Msk);
}

/**
 * @brief  Enable an irq
 * The function enables given irq
 */
inline void m7_enable_irq(IRQn_Type irq)
{
    NVIC_EnableIRQ(irq);
}

/**
 * @brief  Disable an irq
 * The function disables given irq
 */
inline void m7_disable_irq(IRQn_Type irq)
{
    NVIC_DisableIRQ(irq);
}

/**
 * @brief  Enable interrupts
 * The function enables interrupt processing, the irqmask is required
 */
inline void m7_enable_intr(void)
{
    __enable_irq();
}

/**
 * @brief  Disable interrupts
 * The function disables interrupts processing
 * returns the current interrupts enabled mask
 */
inline void m7_disable_intr(void)
{
    __disable_irq();
}

/**
 * @brief  Set Irq Priority Grouping
 */
inline void m7_set_irq_grouping(uint32 grouping)
{
    if (grouping > 7) {
        /* Log an error */
        return;
    }
    NVIC_SetPriorityGrouping(grouping);
}

/**
 * @brief  Set Irq Priority
 * The function sets prority for a given irq
 */
inline void m7_set_irq_priority(IRQn_Type irq, uint32 priority)
{
    if (priority > 7) {
        /* Log an error */
        return;
    }
    NVIC_SetPriority(irq, priority);
}

/**
 * @brief  Get IRQ priority
 * The function gets the priority of given irq
 */
inline uint32 m7_get_irq_priority(IRQn_Type irq)
{
    return (NVIC_GetPriority(irq));
}

/**
 * @brief  Mpu config
 * The function configures MPU region
 */
int m7_mpu_set_region(uint32 region, uint32 region_base, uint32 region_size, uint32 region_attr)
{
    uint32 size, tmpsize = 0;

    if (MPU->TYPE == 0) {
        return SOC_E_INIT;
    }
    if (region_size < 32) {
        return SOC_E_PARAM;
    }
    if (region_attr & 0xE8C000C0) {
        /* Reserved bits set */
        return SOC_E_PARAM;
    }
    
    tmpsize = 1;
    size = 0;
    if (region_size == 0xffffffff) {
        size = 32;
    } else {
        while ( tmpsize <= region_size ) { size++; tmpsize <<= 1; }
    }

    /* Disable ints */
    m7_disable_intr();

    /* Sync */
    __DSB();
    __ISB();

    /* program the region addr and attr */
    MPU->RNR = region;
    MPU->RASR &= ~MPU_RASR_ENABLE_Msk;
    MPU->RBAR = region_base & ~((1 << size)-1); /* Align to size */
    MPU->RASR = (size - 1) << MPU_RASR_SIZE_Pos | region_attr | MPU_RASR_ENABLE_Msk;
    

    /* Sync */
    __DSB();
    __ISB();

    /* Enable ints */
    m7_enable_intr();

    return SOC_E_NONE;
}

/**
 * @brief  Mpu Enable
 * The function Enables MPU operation
 */
inline int m7_mpu_enable(void)
{
    if (MPU->TYPE == 0) {
        return SOC_E_INIT;
    }
    /* Enable MPU with no default map and mpu disabled for fault */
    //MPU->CTRL |= ((~MPU_CTRL_PRIVDEFENA_Msk) | (~MPU_CTRL_HFNMIENA_Msk) | MPU_CTRL_ENABLE_Msk);
    MPU->CTRL = ((1 << MPU_CTRL_PRIVDEFENA_Pos) | (1 << MPU_CTRL_ENABLE_Pos));
    return SOC_E_NONE;
}

/**
 * @brief  Mpu Disable
 * The function Disables MPU operation
 */
inline int m7_mpu_disable(void)
{
    if (MPU->TYPE == 0) {
        return SOC_E_INIT;
    }
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;
    return SOC_E_NONE;
}

/**
 * @brief  Mpu Enable
 * The function Enables MPU operation
 */
void m7_mpu_enable_region(uint32 region)
{
    /* Disable ints */
    m7_disable_intr();

    /* Sync */
    __DSB();
    __ISB();

    /* Enable the region */
    MPU->RNR = region;
    MPU->RASR |= MPU_RASR_ENABLE_Msk;

    /* Sync */
    __DSB();
    __ISB();

    /* Enable ints */
    m7_enable_intr();
    
}

/**
 * @brief  Mpu Disable
 * The function Disables MPU operation
 */
void m7_mpu_disable_region(uint32 region)
{
    /* Disable ints */
    m7_disable_intr();

    /* Sync */
    __DSB();
    __ISB();

    /* Enable the region */
    MPU->RNR = region;
    MPU->RASR &= ~MPU_RASR_ENABLE_Msk;

    /* Sync */
    __DSB();
    __ISB();

    /* Enable ints */
    m7_enable_intr();
    
}

/**
 * @brief DCache Invalidate
 * Function invalidates Dcache by address
 * this will mark an entry if hit as invalid
 */
void m7_dcache_invalidate(uint32 *addr, int len)
{
    uint32 *mod_addr;
    int mod_len;
    
    if ((uint32)addr & 0x1F) {
        mod_addr = (uint32 *)((uint32)addr - ((uint32)addr & 0x1F));
        mod_len = len + ((uint32)addr - (uint32)mod_addr);
    } else {
        mod_addr = addr;
        mod_len = len;
    }
    
    SCB_InvalidateDCache_by_Addr(mod_addr, mod_len);
    __DSB();
    __ISB();
}

/**
 * @brief DCache Flush
 * Function Clean Dcache and flush by address
 * This will push cache data to mem if dirty
 */
void m7_dcache_flush(uint32 *addr, int len)
{
    uint32 *mod_addr;
    int mod_len;
    
    if ((uint32)addr & 0x1F) {
        mod_addr = (uint32 *)((uint32)addr - ((uint32)addr & 0x1F));
        mod_len = len + ((uint32)addr - (uint32)mod_addr);
    } else {
        mod_addr = addr;
        mod_len = len;
    }
    
    SCB_CleanDCache_by_Addr(mod_addr, mod_len);
    __DSB();
    __ISB();
}

/**
 * @brief DCache Invalidate All
 * Function invalidates entire dcache
 */
void m7_dcache_invalidate_all(void)
{
    SCB_InvalidateDCache();
    __DSB();
    __ISB();
}

/**
 * @brief ICache Invalidate All
 * Function invalidates entire Icache 
 */
void m7_icache_invalidate_all(void)
{
    
    SCB_InvalidateICache();
    __DSB();
    __ISB();
}

/**
 * @brief DCache Enable
 * Function Enable the DCACHE
 */
void m7_dcache_enable(void)
{
    SCB_EnableDCache();
    __DSB();
    __ISB();
}

/**
 * @brief DCache Disable
 * Function Disable the DCACHE
 */
void m7_dcache_disable(void)
{
    SCB_DisableDCache();
    __DSB();
    __ISB();
}

/**
 * @brief ICache Enable
 * Function Enable the ICACHE
 */
void m7_icache_enable(void)
{
    SCB_EnableICache();
    __DSB();
    __ISB();
}

/**
 * @brief ICache Disable
 * Function Disable the ICACHE
 */
void m7_icache_disable(void)
{
    SCB_DisableICache();
    __DSB();
    __ISB();
}

#endif /* CORTEX_M7 */

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
#include <soc/drv.h>
#include <soc/cpu_m7.h>
#include <soc/robo2/bcm53158/config.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/mpu.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/irq.h>
#include <soc/robo2/bcm53158/memmap_bcm53158_a0.h>
#include <shared/version.h>

/* Round up to MB */
#define ROUNDUP_TO_MB(v) (((v) + 0x100000) & ~0xfffff)

extern int
cb_init_sram(uint32 start_page, uint32 total_size, uint32 buffer_mem_size);
extern int 
int2str(char *buffer, int dec, unsigned num, int size, char pad);

void check_reset()
{
    sysprintf("RESET0: %08x\n", GET_REG(BCM53158_A0_CRU_SYSRESET_R1_REG));
    sysprintf("RESET1: %08x\n", GET_REG(BCM53158_A0_CRU_SYSRESET_R2_REG));
    sysprintf("RESET2: %08x\n", GET_REG(BCM53158_A0_CRU_SYSRESET_R3_REG));
    sysprintf("RESET3: %08x\n", GET_REG(BCM53158_A0_CRU_SYSRESET_R4_REG));
    sysprintf("RESET4: %08x\n", GET_REG(BCM53158_A0_CRU_SYSRESET_R5_REG));
    sysprintf("STRAPS RAW: %08x\n", GET_REG(BCM53158_A0_CRU_STRAP_RAW_REG));
    sysprintf("STRAPS STS: %08x\n", GET_REG(BCM53158_A0_CRU_STRAP_STS_REG));
}

/*
 * Init CPU config
 */
static void init_cpu(void)
{
    SCB->CCR = 0x00040201;
    SCB->SHCSR = 0x70000;
}

/*
 * API to update QSPI flash MPU settings
 */
int set_qspi_flash_mpu(uint32_t size)
{
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(6, BCM53158_A0_LOCAL_QSPI_FLASH_BASE_ADDR, size,
                          (M7_MPU_CACHEABLE | M7_MPU_AP_READ_ONLY)));

    return SOC_E_NONE;
}

/*
 * Initialize MPU
 */
int init_mpu(void)
{
    /* These MPU settings are configured as per recommendation from hardware team - DO NOT CHANGE */
#ifdef CONFIG_CASCADED_MODE
    // Default Region
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(0, 0x0, 0xffffffff,
                          M7_MPU_AP_NO_ACCESS));
    // Register/Configuration Address Space for Avenger - 0
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(1, 0x40000000, 0x02000000,
                          (2 << MPU_RASR_TEX_Pos) | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    // Register/Configuration Address Space for Avenger – 1 (Remote Access via SPIm)
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(2, 0x44000000, 0x02000000,
                          (2 << MPU_RASR_TEX_Pos) | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    // Remote Packet Buffer Memory - MPU region can be used instead for remote ITCM or remote DTCM, with appropriate settings
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(3, BCM53158_A0_REMOTE_PACKET_RAM_BASE_ADDR, BCM53158_A0_PACKET_RAM_AVAIL,
                          (2 << MPU_RASR_TEX_Pos) | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    // Packet Buffer Memory
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(4, BCM53158_A0_LOCAL_PACKET_RAM_BASE_ADDR, BCM53158_A0_PACKET_RAM_AVAIL,
                          M7_MPU_TEX_NORMAL_MEM | M7_MPU_CACHEABLE | M7_MPU_BUFFERABLE | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    // ITCM
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(5, BCM53158_A0_LOCAL_ITCM_BASE_ADDR, BCM53158_A0_LOCAL_ITCM_SIZE,
                          M7_MPU_TEX_NORMAL_MEM | M7_MPU_AP_ALL_ACCESS));
    // QSPI - set access to max supported size, this will be re-sized after flash is enumerated
    SOC_IF_ERROR_RETURN(set_qspi_flash_mpu(256*1024*1024));
    // DTCM
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(7, BCM53158_A0_LOCAL_DTCM_BASE_ADDR, BCM53158_A0_LOCAL_DTCM_SIZE,
                          M7_MPU_TEX_NORMAL_MEM | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
#else
    // Default Region
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(0, 0x0, 0xffffffff,
                          M7_MPU_AP_NO_ACCESS));
    // Register/Configuration Address Space
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(1, 0x40200000, 0x00200000,
                          (2 << MPU_RASR_TEX_Pos) | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(2, 0x41000000, 0x00040000,
                          (2 << MPU_RASR_TEX_Pos) | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(3, 0x41100000, 0x00100000,
                          (2 << MPU_RASR_TEX_Pos) | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    // Packet Buffer Memory
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(4, BCM53158_A0_LOCAL_PACKET_RAM_BASE_ADDR, BCM53158_A0_PACKET_RAM_AVAIL,
                          M7_MPU_TEX_NORMAL_MEM | M7_MPU_CACHEABLE | M7_MPU_BUFFERABLE | M7_MPU_AP_ALL_ACCESS | M7_MPU_XN));
    // ITCM
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(5, BCM53158_A0_LOCAL_ITCM_BASE_ADDR, BCM53158_A0_LOCAL_ITCM_SIZE,
                          M7_MPU_TEX_NORMAL_MEM | M7_MPU_AP_ALL_ACCESS));
    // QSPI - set access to max supported size, this will be re-sized after flash is enumerated
    SOC_IF_ERROR_RETURN(set_qspi_flash_mpu(256*1024*1024));
    // DTCM
    SOC_IF_ERROR_RETURN(
        m7_mpu_set_region(7, BCM53158_A0_LOCAL_DTCM_BASE_ADDR, BCM53158_A0_LOCAL_DTCM_SIZE,
                          M7_MPU_TEX_NORMAL_MEM | M7_MPU_AP_ALL_ACCESS));
#endif
    m7_mpu_enable();
#ifndef CONFIG_BOOTLOADER
    sysprint("MPU enabled\n");
#endif
    return SOC_E_NONE;
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

/*
 * Init and Enable SRAM
 */
int init_sram()
{
    int rv = 0;
    /* Init SRAM for Processor Access */
    rv = cb_init_sram(0, BCM53158_A0_LOCAL_PACKET_RAM_SIZE, BCM53158_A0_PACKET_RAM_AVAIL);
    return rv;
}

void get_device_id(int unit, uint32 *chipid, uint32 *revid, uint32 *modelid)
{
    uint32 regid = 0;
    if (chipid) {
        regid = drv_robo2_map_to_device_addr(unit, BCM53158_A0_CRU_CHIP_ID_REG);
        *chipid = GET_REG(regid);
        *chipid &= 0xFFFFFF; /* 24 bit chipid */
    }
    if (revid) {
        regid = drv_robo2_map_to_device_addr(unit, BCM53158_A0_CRU_CHIP_REVID_REG);
        *revid = GET_REG(regid);
        *revid &= 0xFF; /* 8 bit revid */
    }
    if (modelid) {
        regid = drv_robo2_map_to_device_addr(unit, BCM53158_A0_CRU_CHIP_MODID_REG);
        *modelid = GET_REG(regid);
    }
}

/**
 * @brief Banner
 * 
 */ 
void display_banner(void)
{
#ifndef CONFIG_BOOTLOADER
    uint32 chipid, revid, mdlid;
#endif
    sysprint("\n\nCopyright 2015-2017 Broadcom Limited\n");
    sysprint("         All rights reserved\n\n");
#ifndef CONFIG_BOOTLOADER
    get_device_id(0, &chipid, &revid, &mdlid);
    sysprintf("ChipId: BCM%d RevId: %02d \n", (int)chipid, (int)revid);
    sysprint("Version Regs: " REGS_VERSION "\n");
    sysprint("Software: " SW_VERSION " Build: " SW_BUILD_DATE "\n");
#endif
}

/**
 * @brief bcm53158_early_init
 * Setup the M7 CPU and enable TCM
 */ 
int bcm53158_early_init(void) 
{
    int rc = SOC_E_NONE;

    init_cpu();
    init_tcm();
    rc = init_sram();
    return rc;
}

/**
 * @brief bcm53158_core_init
 * Setup the M7SS subsystem, Uart and Hw Blocks 
 */ 
int bcm53158_core_init(void) 
{
    int rv = 0;
    display_banner();
    rv = init_mpu();
    if (rv != 0) {
        /* Log an error */
        sysprint("Failed enabling MPU\n");
    }
    init_caches();
    init_irq();
    return SOC_E_NONE;
}


/**
 * @brief bcm53158_chip_init
 * Direct the chip driver to initialize
 * Do we need attach and init, can we do only init and move attach code to init?? 
 */ 
int bcm53158_chip_init(void) 
{
    int rv;
    rv = soc_robo2_essential_init(0);
#ifdef CONFIG_CASCADED_MODE
    if (SOC_IS_CASCADED(0)) {
        if (rv == SOC_E_NONE) {
            rv = soc_robo2_essential_init(1);
        }
    }
#endif
    return rv;
}

#endif /* CORTEX_M7 */

/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    cb.c
 * Description:
 *    Coronado Bridge Helper routines
 *    For use in MC environment.
 */

#ifdef CORTEX_M7

#include <sal_types.h>
#include <sal_console.h>
#include <cpu_m7.h>
#include <soc/robo2/common/robo2_types.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/bcm53158/config.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/intr.h>

__attribute__((section(".dtcmdata")))
uint8_t bcm53158_rev_id = 0x0;

#ifdef BCM_53158_A0

#define BCM5318_A0_CB_PMI_BASE           (BCM53158_A0_CB_BASE_ADDR + 0xD0000)
#define BCM5318_A0_CB_BMU_BASE           (BCM53158_A0_CB_BASE_ADDR + 0x50000)
#define BCM5318_A0_CB_CMM_BASE           (BCM53158_A0_CB_BASE_ADDR + 0x60000)

#define CB_SRAM_PAGE_SIZE                256
#define CB_BMU_LLC_FREE_PAGE_THRESHOLD   32
#ifdef EMULATION_BUILD
#define CB_SW_DELAY                      100
#define CB_SW_RETRIES                    5
#else
#define CB_SW_DELAY                      100
#define CB_SW_RETRIES                    10
#endif

/* Table IA access Defs */
#define CB_IA_CONFIG_OP_POS       28
#define CB_IA_CONFIG_OP_MSK       0xF
#define CB_IA_CONFIG_ADDR_POS     0
#define CB_IA_STATUS_MSK          0x7
#define CB_IA_STATUS_POS          29
#define CB_PMI_PT_ENTRY_VALID_POS    13
#define CB_PMI_PT_ENTRY_ADDR_POS     0
#define CB_PMI_PT_ENTRY_ADDR_MSK     0xFFF

/* PMI PAGETABLE Defs */
#define CB_PMI_PT_CONTROL_REG         (BCM5318_A0_CB_PMI_BASE + 0x8C)
#define CB_PMI_PT_HW_INIT_SADDR_REG   (CB_PMI_PT_CONTROL_REG + 4)
#define CB_PMI_PT_HW_INIT_EADDR_REG   (CB_PMI_PT_CONTROL_REG + 8) 
#define CB_PMI_PT_HW_INIT_SPAGE_REG   (CB_PMI_PT_CONTROL_REG + 12)
#define CB_PMI_PT_HW_INIT_STATUS_REG  (CB_PMI_PT_CONTROL_REG + 16)
#define CB_PMI_PT_IA_CAPABILITY_REG   (CB_PMI_PT_CONTROL_REG + 20)
#define CB_PMI_PT_IA_STATUS_REG       (CB_PMI_PT_CONTROL_REG + 24)
#define CB_PMI_PT_IA_WDATA_REG        (CB_PMI_PT_CONTROL_REG + 28)
#define CB_PMI_PT_IA_CONFIG_REG       (CB_PMI_PT_CONTROL_REG + 32)

#define CB_PMIPT_HW_INIT         1
#define CB_PMIPT_SW_DONE         2
#define CB_PMIPT_STS_INIT_DONE   1

#define CB_PMIPT_IA_CONFIG_ADDR_POS     0
#define CB_PMIPT_IA_CONFIG_ADDR_MSK     0x1FFF

/* BMU LLC Defs */
#define CB_BMU_LLC_CONTROL_REG        (BCM5318_A0_CB_BMU_BASE +                 \
                                        ((bcm53158_rev_id == BCM53158_A0_REV_ID) \
                                            ? 0x648 : 0x650))
#define CB_BMU_LLC_SELF_INIT_REG      (CB_BMU_LLC_CONTROL_REG + 4)
#define CB_BMU_LLC_STATUS_REG         (CB_BMU_LLC_CONTROL_REG + 8)

#define CB_BMULLC_STS_PAGE_ERR     0x0100
#define CB_BMULLC_STS_INIT_ERR     0x0200
#define CB_BMULLC_STS_INIT_DONE    0x0400

#define CB_BMULLC_INIT_SPAGE_POS        0
#define CB_BMULLC_INIT_SPAGE_MSK        0x1FFF
#define CB_BMULLC_INIT_EPAGE_POS        15
#define CB_BMULLC_INIT_EPAGE_MSK        0x1FFF
#define CB_BMULLC_INIT_SELF_INIT_POS    30
#define CB_BMULLC_INIT_SELF_INIT_MSK    1
#define CB_BMULLC_INIT_MANUAL_INIT_POS  31
#define CB_BMULLC_INIT_MANUAL_INIT_MSK  1

#define CB_BMULLC_CONTROL_ENABLE_POS                 10
#define CB_BMULLC_CONTROL_ENABLE_MSK                 1
#define CB_BMULLC_CONTROL_FREE_PAGE_THRESHOLD_POS    0
#define CB_BMULLC_CONTROL_FREE_PAGE_THRESHOLD_MSK    0x3FF

/* CMM Defs */
#define CB_CMM_INIT_CTRL_REG       (BCM5318_A0_CB_CMM_BASE + 0x200)
#define CB_CMM_INIT_STATUS_REG     (CB_CMM_INIT_CTRL_REG + 4)

#define CB_CMM_STS_INIT_DONE          1

/* Helpers */

#define CB_PMIPT_SET_IA_CONFIG(op, addr)    \
              SET_REG(CB_PMI_PT_IA_CONFIG_REG, \
                      (((op) << CB_IA_CONFIG_OP_POS) | ((addr) << CB_PMIPT_IA_CONFIG_ADDR_POS)))
#define CB_PMIPT_GET_IA_STATUS()    \
              (((GET_REG(CB_PMI_PT_IA_STATUS_REG)) >> CB_IA_STATUS_POS) & CB_IA_STATUS_MSK)

#define CB_BMULLC_SELFINIT_CONFIG(start, end)  \
               ((((start) & CB_BMULLC_INIT_SPAGE_MSK) << CB_BMULLC_INIT_SPAGE_POS) | \
                (((end)  & CB_BMULLC_INIT_EPAGE_MSK) << CB_BMULLC_INIT_EPAGE_POS) | \
                (1 <<  CB_BMULLC_INIT_SELF_INIT_POS))

#define CB_BMULLC_MANUALINIT_CONFIG(start, end) \
               ((((start) & CB_BMULLC_INIT_SPAGE_MSK) << CB_BMULLC_INIT_SPAGE_POS) | \
                (((end)  & CB_BMULLC_INIT_EPAGE_MSK) << CB_BMULLC_INIT_EPAGE_POS) | \
                (1 <<  CB_BMULLC_INIT_MANUAL_INIT_POS))

int
cb_wait_for_status(uint32 regaddr, int status_value, char *msg)
{
    register int times=CB_SW_RETRIES;
    register int d;

    while((GET_REG(regaddr) & status_value) == 0) {
        if (msg) {sysprint(msg); sysprintf("Read %08x: %08x\n", regaddr, GET_REG(regaddr));}
        for (d = CB_SW_DELAY; d > 0; d--);
        if (--times <= 0) return SOC_E_TIMEOUT;
    }
    return SOC_E_NONE;
}

/*
 * @brief cb_page_tables_init
 * cb_page_tables_init initializes the CB PMI page tables
 */
int
cb_page_tables_init(uint32 start_page, uint32 num_pages)
{
    int rc = 0;

    CB_PMIPT_SET_IA_CONFIG(ROBO2_IA_OP_TABLE_ENABLE, 0);
    __DSB();

    sysdelay(100);
    while (CB_PMIPT_GET_IA_STATUS() ==  ROBO2_IA_STATUS_BUSY) {
        sysprint("PMI Page Table is busy...\n");
        sysdelay(10000);
    }

    if (CB_PMIPT_GET_IA_STATUS() != ROBO2_IA_STATUS_READY) {
        sysprintf("Enabling page tables failed 0x%x\n", CB_PMIPT_GET_IA_STATUS());
        return SOC_E_INTERNAL;
    }

    SET_REG(CB_PMI_PT_HW_INIT_SADDR_REG, 0);
    SET_REG(CB_PMI_PT_HW_INIT_EADDR_REG, num_pages - 1);
    SET_REG(CB_PMI_PT_HW_INIT_SPAGE_REG, start_page);
    SET_REG(CB_PMI_PT_CONTROL_REG, 3);
    __DSB(); __ISB();

    rc = cb_wait_for_status(CB_PMI_PT_HW_INIT_STATUS_REG, CB_PMIPT_STS_INIT_DONE, NULL);
    if (rc != SOC_E_NONE) {
        sysprint("PMI Pagetable HW Init: Busy Timeout\n");
        return rc;
    }

    /*
     * sysprintf("Current Config: HW SADDR: %08x EADDR: %08x SPAGE: %08x STS: %08x\n",
     *         GET_REG(CB_PMI_PT_HW_INIT_SADDR_REG), GET_REG(CB_PMI_PT_HW_INIT_EADDR_REG),
     *         GET_REG(CB_PMI_PT_HW_INIT_SPAGE_REG), GET_REG(CB_PMI_PT_HW_INIT_STATUS_REG));
     */

    return SOC_E_NONE;   
}
    
    

/*
 * @brief cb_pkt_buffer_init
 * cb_pkt_buffer_init initializes the CB BMU LLC for packet buffer
 */
int
cb_pkt_buffer_init(uint32 start_page, uint32 num_pages)
{
    uint32 regval = 0;
    int rc = 0;

    regval = CB_BMULLC_SELFINIT_CONFIG(start_page, (start_page + num_pages -1));
    SET_REG(CB_BMU_LLC_SELF_INIT_REG, regval);

    regval = CB_BMU_LLC_FREE_PAGE_THRESHOLD << CB_BMULLC_CONTROL_FREE_PAGE_THRESHOLD_POS;
    regval |= 1 << CB_BMULLC_CONTROL_ENABLE_POS;
    SET_REG(CB_BMU_LLC_CONTROL_REG, regval);

    rc = cb_wait_for_status(CB_BMU_LLC_STATUS_REG, 
                (CB_BMULLC_STS_INIT_DONE | CB_BMULLC_STS_INIT_ERR | CB_BMULLC_STS_PAGE_ERR), NULL);

    if (rc != SOC_E_NONE) {
        sysprintf("BMU LLC hw init: Busy timeout\n");
#if 0 /* LLC init done not set in hw*/
        return rc;
    } else {
        regval = GET_REG(CB_BMU_LLC_STATUS_REG);
        if (regval & CB_BMULLC_STS_INIT_ERR) {
            sysprint("BMU LLC hw init: Init Error\n");
            return SOC_E_PARAM;
        } else if (regval & CB_BMULLC_STS_PAGE_ERR) {
            sysprint("BMU LLC hw init: Page Limit Error\n");
            return SOC_E_PARAM;
        }
#endif
    }

    /*
     * sysprintf("Current BMU LLC Config: CTRL: %08x SELFINIT: %08x STS: %08x\n",
     *          GET_REG(CB_BMU_LLC_CONTROL_REG), GET_REG(CB_BMU_LLC_SELF_INIT_REG),
     *          GET_REG(CB_BMU_LLC_STATUS_REG));
     */
    return SOC_E_NONE;   
}


/*
 * @brief cb_init_sram
 * Carve out BMU and PMI regions
 */
int
cb_init_sram(uint32 start_page, uint32 total_size, uint32 host_mem_size)
{
     int rc = 0;
     uint32 regval = 0;
     uint32 numpages = total_size / CB_SRAM_PAGE_SIZE;
     uint32 num_pmi_pages = host_mem_size / CB_SRAM_PAGE_SIZE;
     uint32 num_bmu_pages = numpages - num_pmi_pages - 10;
     
     /* The chip revision needs to be evaluated before
      * any register is accessed for CB SRAM init 
      * This works with the assumption that the chip rev id
      * register address remains the same for all bcm53158 
      * revisions, which is true for A0 and B0 */
     bcm53158_rev_id = GET_REG(BCM53158_A0_CRU_CHIP_REVID_REG) & 0xFF;

     /* Reset CB before starting */
     regval = GET_REG(BCM53158_A0_CRU_SYSRESET_R4_REG);
     regval &= ~BCM53158_A0_R4_CB_RESETN;
     SET_REG(BCM53158_A0_CRU_SYSRESET_R4_REG, regval);
     regval |= BCM53158_A0_R4_CB_RESETN;
     SET_REG(BCM53158_A0_CRU_SYSRESET_R4_REG, regval);

     /* Init page tables BMU buffer manager */
     rc = cb_page_tables_init(start_page, num_pmi_pages);
     if (rc == SOC_E_NONE) {
         rc = cb_pkt_buffer_init(start_page + num_pmi_pages, num_bmu_pages);
         if (rc == SOC_E_NONE) {
              SET_REG(CB_CMM_INIT_CTRL_REG, 1);
              rc = cb_wait_for_status(CB_CMM_INIT_STATUS_REG, CB_CMM_STS_INIT_DONE, NULL);
              if (rc != SOC_E_NONE) {
                  sysprint("CB CMM Hw Init failed: Busy timeout\n");
                  return rc;
              }
              /*
               * sysprintf("Current CMM Config: CTRL: %08x STS: %08x\n",
               *         GET_REG(CB_CMM_INIT_CTRL_REG), GET_REG(CB_CMM_INIT_STATUS_REG));
               */
         }
     } 

#ifdef INCLUDE_MEMTEST
     if (rc == SOC_E_NONE) {
          int inc = 128;
          sysprint("Starting Memtest...");
          uint32 wdata0 = 0xAAAAAAAA;
          uint32 wdata1 = 0x55555555;
          uint32 *r, *p = (uint32*)BCM53158_A0_LOCAL_PACKET_RAM_BASE_ADDR;
          uint32 *q = (uint32*)((BCM53158_A0_LOCAL_PACKET_RAM_BASE_ADDR + host_mem_size) & ~0x3);
          for (r = p; r < q ; r+=inc) {
              sysprintf("Writing to addr %x\n", r);
              if ( (uint32)r & (0x8 * inc)) {
                  *r = wdata0;
              } else {
                  *r = wdata1;
              }
          }
          sysprint("write done!\n");
          for (r = p; r < q; r+=inc ) {
              sysprintf("Reading back addr %x\n", r);
              if ( (uint32)r & (0x8 * inc)) {
                 if (*r != wdata0) {
                     sysprint("Failed verifying data access0\n");
                 }
              } else {
                 if (*r != wdata1) {
                     sysprint("Failed verifying data access1\n");
                 }
                 
              }
          }
          sysprint(" Done\n");
     }
#endif
     return rc;
}
#endif /* BCM_53158_A0 */
#endif /* CORTEX_M7 */

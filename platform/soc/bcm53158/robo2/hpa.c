/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    hpa.c
 * Description:
 *    Host Port Adapter Driver for BCM53158_A0
 */

#include <sal_types.h>
#include <sal_alloc.h>
#include <bsl_log/bsl.h>
#include <fsal_int/types.h>
#include <soc/drv.h>
#include <soc/cpu_m7.h>
#include <soc/robo2/common/regacc.h>
#include <fsal_int/pkt.h>
#include <sal_console.h>

#ifndef CONFIG_BOOTLOADER
#ifdef CORTEX_M7

#define HPA_RX         CBXI_PKT_RX
#define HPA_TX         CBXI_PKT_TX

#define HPA_OWN_CPU    0x0
#define HPA_OWN_HPA    0x1

/* Intr defs */
#define HPA_INTR_TX_DESC_DONE    0x00001
#define HPA_INTR_TX_DESC_EMPTY   0x00002
#define HPA_INTR_TX_DESC_AXIERR  0x00004
#define HPA_INTR_RX_DESC_DONE    0x10000
#define HPA_INTR_RX_DESC_EMPTY   0x20000
#define HPA_INTR_RX_DESC_AXIERR  0x40000

#define HPA_MAX_HW_RX_DESCR  8
#define HPA_MAX_HW_TX_DESCR  8
#define HPA_MIN_TX_SIZE   60



/* DMA Control */
#define HPA_DMA_CONTROL_TX_EN          0x00000001
#define HPA_DMA_CONTROL_RX_EN          0x00000010
#define HPA_DMA_CONTROL_TX_RESET       0x00000100
#define HPA_DMA_CONTROL_RX_RESET       0x00001000
#define HPA_DMA_CONTROL_LE_EN          0x00010000


/* DMA STATUS */
#define HPA_DMA_STATUS_STOPPED            0
#define HPA_DMA_STATUS_WAIT_FOR_DESCR     1
#define HPA_DMA_STATUS_AXI_REQUEST        2
#define HPA_DMA_STATUS_AXI_DATA_XFER      3
#define HPA_DMA_STATUS_AXI_RESPONSE       4
#define HPA_DMA_STATUS_AXI_ERR_RECOVERY   5
#define HPA_DMA_STATUS_CLOSE_DESCR        6
#define HPA_DMA_STATUS_INCR_DESCR_PTR     7


struct hpa_desc_s
{
    uint32 len:14;
    uint32 rsvd0:2;
    uint32 sop:1;
    uint32 eop:1;
    uint32 err:1;
    uint32 axierr:1;
    uint32 ioc:1;
    uint32 rsvd1:11;
    uint32 address;
    uint32 rsvd3:31;
    uint32 own:1;
};

typedef union
{
    struct hpa_desc_s desc;
    uint32 words[3];
} hpa_desc_t;


#ifdef CONFIG_PACKET_DEBUG
extern int tx_dump_packet;
#endif
int init = 0, rx_pkt_reaped = 0;
int chain_active = 0;
int enq_done_hw = 0;
int enq_hw_tx = 0, intr = 0;

uint32 tx_cnt = 0 , rx_cnt = 0;
uint32 tx_err = 0 , rx_err = 0;
uint32 alloc_fails = 0;

void
hpa_dump_stats(void)
{
    sal_printf("HPA Tx Pkts: %d Rx Pkts: %d\n", tx_cnt, rx_cnt);
    sal_printf("HPA Tx Errs: %d Rx Errs: %d\n", tx_err, rx_err);
    sal_printf("HPA Intr: %d Alloc fails: %d\n", intr, alloc_fails);
}

void
hpa_desc_dump(int id, hpa_desc_t *hpadesc)
{
    sal_printf("DESC%d OWN:%s ADDR:%08x SOP:%d EOP:%d IOC:%d LEN:%d\n",
                id, (hpadesc->desc.own == 1) ? "HPA" : "CPU", hpadesc->desc.address,
                hpadesc->desc.sop, hpadesc->desc.eop, hpadesc->desc.ioc, hpadesc->desc.len);
}

/*
 * Function:
 *    hpa_read_desc
 * Description:
 *    Read a Tx or Rx Desc
 */
void 
hpa_read_desc(int descid, hpa_desc_t *desc, int dir)
{
    desc->words[0] = 0;
    desc->words[1] = 0;
    desc->words[2] = 0;
    /*
     * Always read the OWN flag first, so that if it indicates that control of
     * the descriptor has been transferred to the CPU, the contents of the
     * other fields will always be read afterward here and thus be valid.
     */
    if (dir == HPA_TX) {
        switch(descid) {
        case 0:
            REG_READ_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 1:
            REG_READ_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 2:
            REG_READ_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 3:
            REG_READ_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 4:
            REG_READ_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 5:
            REG_READ_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 6:
            REG_READ_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 7:
            REG_READ_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        }
    } else {
        switch(descid) {
        case 0:
            REG_READ_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 1:
            REG_READ_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 2:
            REG_READ_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 3:
            REG_READ_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 4:
            REG_READ_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 5:
            REG_READ_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 6:
            REG_READ_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        case 7:
            REG_READ_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_READ_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            break;
        }
    }
}

/*
 * Function:
 *    hpa_write_desc
 * Description:
 *    Write a Tx or Rx Desc
 */
void 
hpa_write_desc(int descid, hpa_desc_t *desc, int dir)
{
    if (dir == HPA_TX) {
        switch(descid) {
        case 0:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 1:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 2:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 3:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 4:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 5:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 6:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 7:
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        default:
            return;
        }
    } else {
        switch(descid) {
        case 0:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 1:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 2:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 3:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 4:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 5:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 6:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        case 7:
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_31_0r(CBX_AVENGER_PRIMARY, &desc->words[0]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_63_32r(CBX_AVENGER_PRIMARY, &desc->words[1]);
            REG_WRITE_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_127_96r(CBX_AVENGER_PRIMARY, &desc->words[2]);
            break;
        default:
            return;
        }
    }
    
}

void
hpa_dma_start(int dir)
{
    uint32 regval = 0;
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(CBX_AVENGER_PRIMARY, &regval);
    regval |= (dir == HPA_TX) ? (HPA_DMA_CONTROL_TX_EN) : (HPA_DMA_CONTROL_RX_EN);
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(CBX_AVENGER_PRIMARY, &regval);
}

void
hpa_dma_stop(int dir)
{
    uint32 regval = 0;
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(CBX_AVENGER_PRIMARY, &regval);
    regval &= (dir == HPA_TX) ? (~HPA_DMA_CONTROL_TX_EN) : (~HPA_DMA_CONTROL_RX_EN);
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(CBX_AVENGER_PRIMARY, &regval);
}

uint32
hpa_dma_status_get(void)
{
    uint32 regval = 0;
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_STSr(CBX_AVENGER_PRIMARY, &regval);
    return regval;
}

void
hpa_dma_status_dump(void)
{
    uint32 regval = 0;
    regval = hpa_dma_status_get();
    if (regval == HPA_DMA_STATUS_STOPPED) {
        sal_printf("Tx DMA is stopped\n");
    } else if (regval == HPA_DMA_STATUS_AXI_REQUEST) {
        sal_printf("Tx Waiting for AXI Request\n");
    } else if (regval == HPA_DMA_STATUS_AXI_RESPONSE) {
        sal_printf("Tx Waiting for AXI Request\n");
    } else if (regval == HPA_DMA_STATUS_AXI_DATA_XFER) {
        sal_printf("Tx DMA in progress\n");
    } else if (regval == HPA_DMA_STATUS_AXI_ERR_RECOVERY) {
        sal_printf("Tx recovering from the Error state\n");
    } else if (regval == HPA_DMA_STATUS_WAIT_FOR_DESCR) {
        sal_printf("Tx waiting for the processor to create the descriptor\n");
    } else if (regval == HPA_DMA_STATUS_INCR_DESCR_PTR) {
        sal_printf("Tx updating the Descriptor pointer\n");
    } else if (regval == HPA_DMA_STATUS_CLOSE_DESCR) {
        sal_printf("Tx closing descriptor and hand-over back to processor\n");
    }

    regval >>= 4;
    if (regval == HPA_DMA_STATUS_STOPPED) {
        sal_printf("Rx DMA is stopped\n");
    } else if (regval == HPA_DMA_STATUS_AXI_REQUEST) {
        sal_printf("Rx Waiting for AXI Request\n");
    } else if (regval == HPA_DMA_STATUS_AXI_RESPONSE) {
        sal_printf("Rx Waiting for AXI Request\n");
    } else if (regval == HPA_DMA_STATUS_AXI_DATA_XFER) {
        sal_printf("Rx DMA in progress\n");
    } else if (regval == HPA_DMA_STATUS_AXI_ERR_RECOVERY) {
        sal_printf("Rx recovering from the Error state\n");
    } else if (regval == HPA_DMA_STATUS_WAIT_FOR_DESCR) {
        sal_printf("Rx waiting for the processor to create the descriptor\n");
    } else if (regval == HPA_DMA_STATUS_INCR_DESCR_PTR) {
        sal_printf("Rx updating the Descriptor pointer\n");
    } else if (regval == HPA_DMA_STATUS_CLOSE_DESCR) {
        sal_printf("Rx closing descriptor and hand-over back to processor\n");
    }
}

/*
 * Function:
 *    hpa_init_buffers
 * Description:
 *    Init Rx Buffers
 */
int
hpa_init_buffers(void)
{
    void *ptr;
    hpa_desc_t hpadesc;
    int i;
    for (i=0; i < HPA_MAX_HW_RX_DESCR; i++) {
        sal_memset(&hpadesc, 0, sizeof(hpadesc));
        hpa_read_desc(i, &hpadesc, HPA_RX);
        if (hpadesc.desc.own == HPA_OWN_CPU) {
            ptr = (void*)cbxi_rx_buffer_alloc();
            if (ptr) {
                sal_memset(&hpadesc, 0, sizeof(hpadesc));
                hpadesc.desc.address = (uint32)ptr;
                hpadesc.desc.own = HPA_OWN_HPA;
                hpadesc.desc.ioc = 1;
                hpa_write_desc(i, &hpadesc, HPA_RX);
            } else {
                alloc_fails++;
                /* To refresh buffers */
#ifndef CONFIG_STATIC_ALLOC
                hpa_intr_disable(HPA_RX);
                wake_up_rxtask();
#endif
                return SOC_E_MEMORY;
            }
        }
    }
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_CPU_CUR_DESC_PTRr(CBX_AVENGER_PRIMARY, (uint32*)&i);
    hpa_dma_start(HPA_RX);
    return SOC_E_NONE;
}

#ifndef CONFIG_STATIC_PKTBUF
/*
 * Function:
 *    hpa_release_buffers
 * Description:
 *    Release Rx Buffers
 */
void
hpa_release_buffers(void)
{
    hpa_desc_t hpadesc;
    int i;
    hpa_dma_stop(HPA_RX);
    for (i=0; i < HPA_MAX_HW_RX_DESCR; i++) {
        hpa_read_desc(i, &hpadesc, HPA_RX);
        cbxi_pkt_free((void*)hpadesc.desc.address);
    }
    i = 0;
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_CPU_CUR_DESC_PTRr(CBX_AVENGER_PRIMARY, (uint32*)&i);
}
#endif


/*
 * Function:
 *    hpa_process_rx_desc
 * Description:
 *    Handle and refill one desc
 */
STATIC int
hpa_process_rx_desc(int descid)
{
    hpa_desc_t hpadesc;
    void *addr;
    hpa_read_desc(descid, &hpadesc, HPA_RX);
    //hpa_desc_dump(descid, &hpadesc);
    if ((hpadesc.desc.own == HPA_OWN_CPU) && (hpadesc.desc.address != 0)) {
        /* Process chains only in order */
        if ((hpadesc.desc.sop == 0) && (chain_active == 0)) {
            return SOC_E_EMPTY;
        }
        /* Process only if we can refill the buffer */
        addr = (void*)cbxi_rx_buffer_alloc();
        if (addr) {
            if ((hpadesc.desc.sop) && (hpadesc.desc.eop)) {
                /* Full frame received */
                cbxi_rx_enqueue(0, (void*)hpadesc.desc.address, hpadesc.desc.len,
                                CBXI_PKT_FLAGS_RX_VALID | CBXI_PKT_FLAGS_RX_SOP |
                                CBXI_PKT_FLAGS_RX_EOP);
            } else if (hpadesc.desc.sop) {
               /* First chunk */
                cbxi_rx_enqueue(0, (void*)hpadesc.desc.address, hpadesc.desc.len,
                                CBXI_PKT_FLAGS_RX_VALID | CBXI_PKT_FLAGS_RX_SOP);
                chain_active = 1;
            } else if (hpadesc.desc.eop) {
               /* Last chunk */
                cbxi_rx_enqueue(0, (void*)hpadesc.desc.address, hpadesc.desc.len,
                                CBXI_PKT_FLAGS_RX_VALID | CBXI_PKT_FLAGS_RX_EOP);
                chain_active = 0;
            } else {
                cbxi_rx_enqueue(0, (void*)hpadesc.desc.address, hpadesc.desc.len,
                                CBXI_PKT_FLAGS_RX_VALID | CBXI_PKT_FLAGS_RX_MID);
            }
            rx_cnt++;
            hpadesc.desc.address = (uint32)addr;
            hpadesc.desc.sop = 0;
            hpadesc.desc.eop = 0;
            hpadesc.desc.len = 0;
            hpadesc.desc.own = HPA_OWN_HPA;
            hpadesc.desc.ioc = 1;
            hpa_write_desc(descid, &hpadesc, HPA_RX);
        } else {
            alloc_fails++;
            wake_up_rxtask();
            return SOC_E_MEMORY;
        }
        return SOC_E_NONE;
    } else {
        return SOC_E_EMPTY;
    }
}

/*
 * Function:
 *    hpa_rx_ready
 * Description:
 *    Intr handler for Rx Ready interrupt
 */
int
hpa_rx_ready(void)
{
    int rc, descid, maxidx;
    uint32 npkts = 0, hwcur = 0;
    maxidx = rx_pkt_reaped + 1 + HPA_MAX_HW_RX_DESCR;
    for (descid = rx_pkt_reaped; descid < maxidx; descid++) {
        rc = hpa_process_rx_desc((descid % HPA_MAX_HW_RX_DESCR));
        if (rc == SOC_E_MEMORY) {break;}
        if (rc == SOC_E_NONE) {npkts++;}
    }
    if (npkts > 0) {
        REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_CPU_CUR_DESC_PTRr(
                CBX_AVENGER_PRIMARY, &hwcur);
        hwcur = (hwcur + npkts) % 16;
        REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_CPU_CUR_DESC_PTRr(
                CBX_AVENGER_PRIMARY, &hwcur);
        hpa_dma_start(HPA_RX);
        rx_pkt_reaped = (rx_pkt_reaped + npkts) % 16;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    hpa_rx_empty
 * Description:
 *    Intr handler for Rx empty interrupt
 */
int
hpa_rx_empty(void)
{
    wake_up_rxtask();
    return 0;
}

/*
 * Function:
 *    hpa_rx_error
 * Description:
 *    Intr handler for Rx error interrupt
 */
int
hpa_rx_error(void)
{
    wake_up_rxtask();
    rx_err++;
    return 0;
}

/*
 * Function:
 *    hpa_tx_clean_desc
 * Description:
 *    Release the tx buffer
 */
int
hpa_tx_clean_desc(int idx)
{
    hpa_desc_t hpadesc;
    hpa_read_desc(idx, &hpadesc, HPA_TX);
    //hpa_desc_dump(idx, &hpadesc);
    if ((hpadesc.desc.own == HPA_OWN_CPU) &&
            (hpadesc.desc.address != 0)) {
        sal_dma_free((void*)hpadesc.desc.address);
        sal_memset(&hpadesc, 0, sizeof(hpa_desc_t));
        hpa_write_desc(idx, &hpadesc, HPA_TX);
        return SOC_E_NONE;
    } else {
        return SOC_E_BADID;
    }
}

/*
 * Function:
 *    hpa_tx_done_isr
 * Description:
 *    Handler for tx done intr
 */
int
hpa_tx_done_isr(void)
{
    hpa_tx_done();
    return SOC_E_NONE;
}

/*
 * Function:
 *    hpa_tx_done
 * Description:
 *    Free up sent packet buffers
 */
int
hpa_tx_done(void)
{
    int rc, descid, maxidx;
    uint32 npkts = 0;
    maxidx = enq_done_hw + 1 + HPA_MAX_HW_RX_DESCR;
    for (descid = enq_done_hw + 1; descid < maxidx; descid++, npkts++) {
        rc = hpa_tx_clean_desc((descid % HPA_MAX_HW_TX_DESCR));
        if (rc != SOC_E_NONE) {break;}
    }
    if (npkts) { enq_done_hw = (enq_done_hw + npkts) % 16; }

    hpa_dma_start(HPA_TX);
    return SOC_E_NONE;
}

/*
 * Function:
 *    hpa_tx_empty
 * Description:
 *    Handler for tx empty intr
 */
int
hpa_tx_empty(void)
{
    return (hpa_tx_done_isr());
}

/*
 * Function:
 *    hpa_tx_error
 * Description:
 *    Handler for tx error intr
 */
int
hpa_tx_error(void)
{
    tx_err++;
    return (hpa_tx_done_isr());

}

/*
 * Function:
 *    hpa_tx_packet
 * Description:
 *    Transmit a packet
 */
int
hpa_tx_packet(void *buffer, int buffer_size)
{
    hpa_desc_t  hpadesc;
#ifdef CONFIG_PACKET_DEBUG
    int i;
#endif
    int descid;
    uint32 hwcur;
    int rv = SOC_E_NONE;
    if (!init) {
        return SOC_E_INIT;
    }
    if (!buffer) {
        return SOC_E_PARAM;
    }
    if (buffer_size < HPA_MIN_TX_SIZE) {
        return SOC_E_PARAM;
    }
    sal_memset(&hpadesc, 0, sizeof(hpadesc));
    m7_disable_intr();
    descid = enq_hw_tx;
    hpa_read_desc(descid, &hpadesc, HPA_TX);
    if (hpadesc.desc.own != HPA_OWN_HPA) {
        m7_dcache_flush((uint32*)buffer, buffer_size);
        hpadesc.desc.address = (uint32)buffer;
        hpadesc.desc.len = buffer_size;
        hpadesc.desc.sop = 1;
        hpadesc.desc.eop = 1;
        hpadesc.desc.ioc = 1;
        hpadesc.desc.own = HPA_OWN_HPA;
        hpa_write_desc(descid, &hpadesc, HPA_TX);
        enq_hw_tx = (enq_hw_tx + 1) % HPA_MAX_HW_TX_DESCR;
        tx_cnt++;
        REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_TX_CPU_CUR_DESC_PTRr(CBX_AVENGER_PRIMARY, &hwcur);
        hwcur = (hwcur + 1) % 16;
        REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_TX_CPU_CUR_DESC_PTRr(CBX_AVENGER_PRIMARY, &hwcur);
        hpa_dma_start(HPA_TX);
    } else {
        tx_err++;
        m7_enable_intr();
        return SOC_E_FAIL;
    }
    m7_enable_intr();
#ifdef CONFIG_PACKET_DEBUG
    if (tx_dump_packet) {
        sal_printf("Pkt with CB header:\n");
        for(i=0; i < buffer_size; i++) {
            if ((i % 16) == 0) {
                sal_printf("\n%04x: ", i);
            }
            sal_printf("%02x ", *((char*)buffer + i));
        }
        sal_printf("\n");
    }
#endif
    return rv;

}

/*
 * Function:
 *    hpa_intr_ack
 * Description:
 *    Ack an Intr
 */
void
hpa_intr_ack(int intr)
{
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_STSr(CBX_AVENGER_PRIMARY, &intr);
}


/*
 * Function:
 *    hpa_intr
 * Description:
 *    Intr handler
 */
void
hpa_handler(void)
{
    uint32 intsts;

    intr++;
    m7_disable_intr();
    do {
        intsts = 0;
        REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_STSr(CBX_AVENGER_PRIMARY, &intsts);
        if (intsts == 0) {
            break;
        } else if (intsts & HPA_INTR_TX_DESC_DONE) {
            /* Not handling Tx Empty */
            hpa_intr_ack(HPA_INTR_TX_DESC_EMPTY);
            hpa_intr_ack(HPA_INTR_TX_DESC_DONE);
            hpa_tx_done_isr();
        } else if (intsts & HPA_INTR_TX_DESC_AXIERR) {
            LOG_ERROR(BSL_LS_SOC_PKTIO,
               (BSL_META(
                   "Tx AXI error\n")));
            /* Not handling Tx Empty */
            hpa_intr_ack(HPA_INTR_TX_DESC_EMPTY);
            hpa_intr_ack(HPA_INTR_TX_DESC_AXIERR);
            hpa_tx_error();
        } else if (intsts & HPA_INTR_RX_DESC_DONE) {
            hpa_intr_ack(HPA_INTR_RX_DESC_DONE);
            hpa_rx_ready();
        } else if (intsts & HPA_INTR_RX_DESC_EMPTY) {
            hpa_intr_ack(HPA_INTR_RX_DESC_EMPTY);
            hpa_rx_empty();
        } else if (intsts & HPA_INTR_RX_DESC_AXIERR) {
            LOG_ERROR(BSL_LS_SOC_PKTIO,
               (BSL_META(
                   "Rx AXI error\n")));
            hpa_intr_ack(HPA_INTR_RX_DESC_AXIERR);
            hpa_rx_error();
        }
    } while(1);
    m7_enable_intr();
}

/*
 * Function:
 *    hpa_intr_enable
 * Description:
 *    Intr enable
 */
void
hpa_intr_enable(int intr)
{
    uint32 regval = 0;
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(CBX_AVENGER_PRIMARY, &regval);
    regval |= intr;
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(CBX_AVENGER_PRIMARY, &regval);

}
/*
  * Function:
  *    hpa_intr_disable
  * Description:
  *    Intr disable 
  */
void
hpa_intr_disable(int intr)
{
    uint32 regval = 0;
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(CBX_AVENGER_PRIMARY, &regval);
    regval &= ~intr;
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(CBX_AVENGER_PRIMARY, &regval);
}
/*
  * Function:
  *    hpa_init
  * Description:
  *    HPA module initr 
  */
int
hpa_init(void)
{
    int rv = SOC_E_NONE;
    uint32 regval = 0;

    m7_disable_intr();

    /* Setup Rx Size */
    regval = (CBXI_RX_BUFFER_SIZE >> 8) - 1; /* Multiple of 256 */
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_DESC_SIZEr(CBX_AVENGER_PRIMARY, &regval);

    /* Setup DMA */
    regval = HPA_DMA_CONTROL_RX_RESET | HPA_DMA_CONTROL_TX_RESET | HPA_DMA_CONTROL_LE_EN;
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(CBX_AVENGER_PRIMARY, &regval);
    regval = HPA_DMA_CONTROL_LE_EN;
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(CBX_AVENGER_PRIMARY, &regval);

    /* Check if we need to set AXI and Burst */

    /* Enable Intr */
    regval = (HPA_INTR_RX_DESC_AXIERR | HPA_INTR_RX_DESC_DONE | HPA_INTR_RX_DESC_EMPTY |
                 HPA_INTR_TX_DESC_AXIERR | HPA_INTR_TX_DESC_DONE );
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(CBX_AVENGER_PRIMARY, &regval);

    /* Fill buffers */
    rv = hpa_init_buffers();
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_PKTIO,
           (BSL_META(
               "HPA Init Buffers Failed\n")));
        return rv;
    }
    regval = 0x500;
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_AXI_CONTROLr(CBX_AVENGER_PRIMARY, &regval);

    /* Init complete */
    init = 1;
    m7_enable_irq(HPA_IRQn);
    m7_enable_intr();
    return rv;
}
/*
  * Function:
  *    hpa_uninit
  * Description:
  *    HPA module uninit
  */
void
hpa_uninit(void)
{
    uint32 regval = 0;
    m7_disable_irq(HPA_IRQn);
    m7_disable_intr();
    hpa_dma_stop(HPA_TX);
    hpa_dma_stop(HPA_RX);
    REG_WRITE_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(CBX_AVENGER_PRIMARY, &regval);
#ifndef CONFIG_STATIC_PKTBUF
    hpa_release_buffers();
#endif
    init = 0;
    m7_enable_intr();
}

#endif /* CORTEX_M7 */
#endif /* !CONFIG_BOOTLOADER */


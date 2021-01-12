/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     pkt.h
 * * Purpose:
 * *     This file contains definitions internal to BCM53158 Packet ID.
 */

#ifndef __PKT_H
#define __PKT_H

#include <sal_types.h>
#include <fsal_int/imp.h>

/* define => Use DTCM based buffers
 * undef  => Use SRAM based buffers, dynamically allocated
 */
#define CONFIG_STATIC_PKTBUF

#define CBXI_PKT_RX 0
#define CBXI_PKT_TX 1

#define CBXI_PKT_FLAGS_RX_VALID    0x0001     /* Pkt received ok  */
#define CBXI_PKT_FLAGS_RX_ERROR    0x0002     /* Pkt received with error */
#define CBXI_PKT_FLAGS_RX_SOP      0x0004     /* Pkt buffer has only SOP */
#define CBXI_PKT_FLAGS_RX_EOP      0x0008     /* Pkt buffer has EOP */
#define CBXI_PKT_FLAGS_RX_MID      0x0010     /* Pkt bufffer is part of chained */

#define CBXI_RX_MAX_BUFFER_REGION_SIZE     (25*1024)  /* Limit Rx buffers to 25K */
#define CBXI_RX_BUFFER_SIZE                1536       /* Rx buffer size in multiple of 256 */
#define CBXI_RX_NUM_BUFFERS_MIN            8          /* Need to allocate atleast 8 buffers */
#define CBXI_RX_NUM_BUFFERS_MAX            (CBXI_RX_MAX_BUFFER_REGION_SIZE / CBXI_RX_BUFFER_SIZE)
#define CBXI_RX_NUM_BUFFERS                (CBXI_RX_NUM_BUFFERS_MAX)
#define CBXI_RX_NUM_BUFFERS_LOW_THRESHOLD  (CBXI_RX_NUM_BUFFERS_MIN)

#define CBXI_RX_TASK_STACK_SIZE   1000

#ifdef OPENRTOS
#define CBXI_RX_TASK_PRIO         configMAX_PRIORITIES - 2
#else
#define CBXI_RX_TASK_PRIO         1
#endif

/*
 * CBXI Packet
 */
__attribute__((aligned(32)))
typedef struct cbxi_pkt_s {
    union {
        struct {
            struct cbxi_pkt_s *next;       /* Chain */
            uint32            total_len;   /* Total num of bytes in case of chain */
            uint16            num_bytes;   /* Number of bytes in packet */
            uint16            flags;       /* Flags */
        };
#ifndef CONFIG_STATIC_PKTBUF
        uint8             _align[32];      /* Align start (and actual end) of pktbuffer[] to a D-cache boundary */
#endif
    };

    uint8             pktbuffer[0];
} cbxi_pkt_t;


/* Declarations */
int cbxi_rx_init(void);
int cbxi_rx_uninit(void);
void cbxi_pkt_free(void *ptr);
cbxi_pkt_t* cbxi_pkt_alloc(void);
void * cbxi_rx_buffer_alloc(void);
void cbxi_rx_buffer_free(cbxi_pkt_t *ptr);
void wake_up_rxtask();
void cbxi_dump_pkt(uint8 *buffer, int size);
void cbxi_rx_enqueue(int qid, void *buffer, int size, int flags);
int cbxi_rx_process_pkt(uint8 *buffer, int len, cbxi_egress_imp_header_t *header,
                        cbxi_timestamp_header_t *ts_header,
                        brcm_egress_imp_header_t *bcm_header);
uint8 *cbxi_rx_extract_header(uint8 *buffer, uint32 buffer_size,
                              uint32 size, int *newsize,
                              cbxi_egress_imp_header_t *cb_header,
                              cbxi_timestamp_header_t *ts_header,
                              brcm_egress_imp_header_t *bcm_header);


#ifdef CONFIG_EXTERNAL_HOST
extern int soc_packetio_send(int unit, uint8 *buffer, int buffer_size);
extern int soc_packetio_init(int unit);
extern int soc_packetio_uninit(int unit);
#else
extern int hpa_tx_packet(void *buffer, int buffer_size);
extern void hpa_intr_enable(int);
extern void hpa_intr_disable(int);
extern int hpa_init(void);
extern void hpa_uninit(void);
extern int hpa_rx_ready(void);
extern int hpa_tx_done(void);
#endif /* !CONFIG_EXTERNAL_HOST */

/* Uncomment for debug *|
#define TRACE_PKTBUF 1
|* Uncomment for debug */

void hpa_dump_stats(void);
void tx_dump_stats(void);
void rx_dump_stats(void);

#define CONFIG_PACKET_DEBUG 1
void set_pkt_dump(int dir, int enable);

#endif /* __PKT_H */

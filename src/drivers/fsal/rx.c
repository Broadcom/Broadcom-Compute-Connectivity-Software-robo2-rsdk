/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    rx.c
 * Description:
 *    Receive processor
 */

#include <sal_types.h>
#include <sal_util.h>
#include <soc/drv.h>
#include <sal_task.h>
#include <sal_alloc.h>
#include <sal_console.h>
#include <fsal/error.h>
#include <fsal/cosq.h>
#include <fsal/pktio.h>
#include <fsal_int/pkt.h>
#include <fsal_int/types.h>
#include <bsl_log/bsl.h>
#ifndef CONFIG_EXTERNAL_HOST
#include <soc/cpu_m7.h>
#include <FreeRTOS.h>
#endif



uint32 rx_alloc_fails;
uint32 rx_unknown_etypes;


#ifndef CONFIG_EXTERNAL_HOST
int rx_task_status = 0;
sal_thread_t rxtask;
#ifdef CONFIG_PACKET_DEBUG
extern int rx_dump_packet;
#endif

#define CBXI_BUF_TO_PKT(b) ((cbxi_pkt_t*)((char*)b - offsetof(cbxi_pkt_t, pktbuffer)))
#define CBXI_PKT_TO_BUF(p) ((void*)((char*)p + offsetof(cbxi_pkt_t, pktbuffer)))

/* Task related */
#define CBXI_THREAD_DONE      (0)
#define CBXI_THREAD_STOP      (1)
#define CBXI_THREAD_RUNNING   (2)


typedef struct rx_cb_s {
    volatile cbxi_pkt_t *rx_pkt_queue_head;
    volatile cbxi_pkt_t *rx_pkt_queue_tail;
    volatile cbxi_pkt_t *rx_buffer_pool_head;
    volatile cbxi_pkt_t *rx_buffer_pool_tail;
    volatile cbxi_pkt_t *rx_current_chain;
    volatile int num_buffers_in_pool;
    volatile unsigned int num_pkts_processed;
    volatile unsigned int num_pkts_enqueued;
    volatile unsigned int num_pkts_dropped;
    volatile int rx_current_chain_len;
    volatile int rx_current_chain_num;
    int rx_current_chain_drop;
    sal_sem_t rx_packets_sem;
} cbxi_rx_cb_t;

STATIC cbxi_rx_cb_t rx_cb = {0};
uint32 rx_dma_alloc_fails;
#ifdef CONFIG_STATIC_PKTBUF
__attribute__((section(".dtcmdata")))
uint8 rx_buffer_region[CBXI_RX_NUM_BUFFERS_MAX * (sizeof(cbxi_pkt_t) + CBXI_RX_BUFFER_SIZE)];
#endif

void rx_dump_stats(void)
{
    sal_printf("Rx Num Pkts  Queued   : %u\n", rx_cb.num_pkts_enqueued);
    sal_printf("Rx Num Pkts Handled   : %u\n", rx_cb.num_pkts_processed);
    sal_printf("Rx Num Pkts Dropped   : %u\n", rx_cb.num_pkts_dropped);
    sal_printf("Rx Num Buffers Pool   : %d\n", rx_cb.num_buffers_in_pool);
    sal_printf("Rx BuffAlloc Fails    : %d\n", rx_alloc_fails);
    sal_printf("Rx Dma BuffAlloc Fails: %d\n", rx_dma_alloc_fails);
    sal_printf("Rx Unknown Etypes     : %d\n", rx_unknown_etypes);
}

void wake_up_rxtask()
{
    if (rx_cb.rx_packets_sem)
        sal_sem_give(rx_cb.rx_packets_sem);
}

#ifndef CONFIG_STATIC_PKTBUF
/*
 * Allocate a Packet buffer
 */
cbxi_pkt_t*
cbxi_pkt_alloc(void)
{
    cbxi_pkt_t *pkt = NULL;
    pkt = (cbxi_pkt_t*)sal_dma_alloc(CBXI_RX_BUFFER_SIZE + sizeof(cbxi_pkt_t), "RX");
    if (!pkt) {
        rx_dma_alloc_fails++;
    }
#ifdef TRACE_PKTBUF
    sal_printf("Allocated %08x pktbuffer set to %08x\n", (uint32)pkt, (uint32)pkt->pktbuffer);
#endif
    return pkt;
}

/*
 * Free a Packet buffer
 */
void
cbxi_pkt_free(void *ptr)
{
    cbxi_pkt_t *pkt = NULL;
    if (ptr == NULL) {
        //SAL_ASSERT(0);
        sal_printf("***ERROR: Invalid Memory to free %08x \n", (uint32)ptr);
        return;
    }
    pkt = CBXI_BUF_TO_PKT(ptr);
#ifdef TRACE_PKTBUF
    sal_printf("Freeing %08x Buffer was %08x\n", (uint32)pkt, (uint32)ptr);
#endif
    sal_dma_free(pkt);
}
#endif /* CONFIG_STATIC_PKTBUF */

/*
 * cbxi_rx_dqueue
 *    Dqueue a packet for RX processing
 *    Called from Rx Task
 */
cbxi_pkt_t*
cbxi_rx_dqueue(int qid)
{
    cbxi_pkt_t *pkt = NULL;
#ifdef TRACE_PKTBUF
    sal_printf("Dequeued Head: %08x, Num Enqueued:%u\n",
              rx_cb.rx_pkt_queue_head, rx_cb.num_pkts_enqueued);
#endif
    m7_disable_intr();
    if (rx_cb.num_pkts_enqueued != rx_cb.num_pkts_processed) {
        if (rx_cb.rx_pkt_queue_head != NULL) {
            pkt = (cbxi_pkt_t*)rx_cb.rx_pkt_queue_head;
            rx_cb.rx_pkt_queue_head = pkt->next;
            if (rx_cb.rx_pkt_queue_head == NULL) {
                rx_cb.rx_pkt_queue_tail = rx_cb.rx_pkt_queue_head;
            }
        }
    }
    m7_enable_intr();
#ifdef TRACE_PKTBUF
    if (pkt)
        sal_printf("Dequeued Pkt: %08x, Buffer: %08x\n", pkt, pkt->pktbuffer);
    sal_printf("Dequeued Head after dqueue: %08x\n", rx_cb.rx_pkt_queue_head);
#endif
    return pkt;
}

/*
 * cbxi_rx_enqueue
 *    Enqueue a packet for RX processing
 *    Called from HPA interrupt handler
 */
void
cbxi_rx_enqueue(int qid, void *buffer, int size, int flags)
{
    cbxi_pkt_t *pkt, *tmppkt;
    int num_pkts;

    pkt = CBXI_BUF_TO_PKT(buffer);
    pkt->flags = flags;
    if ((flags & CBXI_PKT_FLAGS_RX_SOP) && !(flags & CBXI_PKT_FLAGS_RX_EOP)) {
        pkt->num_bytes = size;
        rx_cb.rx_current_chain = pkt;
        rx_cb.rx_current_chain_len = size;
        rx_cb.rx_current_chain_num = 1;
        return;
    } else if (!(flags & CBXI_PKT_FLAGS_RX_SOP) && !(flags & CBXI_PKT_FLAGS_RX_EOP)) {
        pkt->num_bytes = size;
        rx_cb.rx_current_chain_len += size;
        tmppkt = (cbxi_pkt_t*)rx_cb.rx_current_chain;
        while (tmppkt->next != NULL) {
            tmppkt = tmppkt->next;
        }
        tmppkt->next = pkt;
        rx_cb.rx_current_chain_num += 1;
        return;
    } else if (!(flags & CBXI_PKT_FLAGS_RX_SOP) && (flags & CBXI_PKT_FLAGS_RX_EOP)) {
        pkt->num_bytes = size - rx_cb.rx_current_chain_len;
        rx_cb.rx_current_chain_num += 1;
        tmppkt = (cbxi_pkt_t*)rx_cb.rx_current_chain;
        while (tmppkt->next != NULL) {
            tmppkt = tmppkt->next;
        }
        tmppkt->next = pkt;
        /* Gather Chain */
        num_pkts = rx_cb.rx_current_chain_num;
        pkt = (cbxi_pkt_t*)rx_cb.rx_current_chain;
        pkt->total_len = size;
        /* Reset Chain data */
        rx_cb.rx_current_chain = NULL;
        rx_cb.rx_current_chain_num = 0;
        rx_cb.rx_current_chain_len = 0;
    } else {
        /* Complete Packet */
        pkt->total_len = size;
        pkt->num_bytes = size;
        num_pkts = 1;
    }

    if (rx_cb.rx_pkt_queue_tail == NULL) {
        rx_cb.rx_pkt_queue_head = rx_cb.rx_pkt_queue_tail = pkt;
    } else {
        rx_cb.rx_pkt_queue_tail->next = pkt;
        rx_cb.rx_pkt_queue_tail = pkt;
    }
#ifdef TRACE_PKTBUF
    if (pkt)
        sal_printf("Enqueued Pkt: %08x, Buffer: %08x\n", pkt, pkt->pktbuffer);
#endif
    rx_cb.num_pkts_enqueued += num_pkts;
    /* To process the packets */
    wake_up_rxtask();
}


#ifdef CONFIG_STATIC_PKTBUF
/*
 * cbxi_rx_buffer_pool_init
 *    Initialize static buffer pool
 */
int
cbxi_rx_buffer_pool_init(void)
{
    int i;
    uint8 *ptr = &rx_buffer_region[0];
    cbxi_pkt_t *pkt;
    for (i = 0; i < CBXI_RX_NUM_BUFFERS_MAX; i++) {
        pkt = (cbxi_pkt_t*)ptr;
        cbxi_rx_buffer_free(pkt);
        ptr += sizeof(cbxi_pkt_t) + CBXI_RX_BUFFER_SIZE;
    }
    return CBX_E_NONE;
}
#else  /* CONFIG_STATIC_PKTBUF */
/*
 * cbxi_rx_buffer_pool_refill
 *    Initialize and allocate buffer pool
 */
int
cbxi_rx_buffer_pool_refill(uint32 num)
{
    int i;
    cbxi_pkt_t *ptr;
    for (i = 0; i < num; i++) {
        ptr = cbxi_pkt_alloc();
        if (ptr) {
            cbxi_rx_buffer_free(ptr);
        } else {
            return CBX_E_MEMORY;
        }
    }
#ifdef TRACE_PKTBUF
    sal_printf("Buffer Pool Head = %x Tail = %x\n", rx_cb.rx_buffer_pool_head, rx_cb.rx_buffer_pool_tail);
#endif
    return CBX_E_NONE;
}

/*
 * cbxi_rx_buffer_pool_release
 *    Release all buffers from the pool
 */
void
cbxi_rx_buffer_pool_release()
{
    uint8 *ptr;
    while(rx_cb.num_buffers_in_pool > 0) {
        m7_disable_intr();
        ptr = cbxi_rx_buffer_alloc();
        m7_enable_intr();
        if (ptr)
            cbxi_pkt_free(ptr);
    }
}
#endif  /* CONFIG_STATIC_PKTBUF */

/*
 * cbxi_rx_buffer_free
 *    Release CBXI packet to the pool
 */
void
cbxi_rx_buffer_free(cbxi_pkt_t *ptr)
{

    m7_disable_intr();

#ifndef CONFIG_STATIC_PKTBUF
    {
        int curr_nbufs;
        /* We are to restrict memory we consume */
        curr_nbufs = rx_cb.num_pkts_enqueued - rx_cb.num_pkts_processed;
        curr_nbufs += rx_cb.num_buffers_in_pool;
        if (curr_nbufs > CBXI_RX_NUM_BUFFERS_MAX) {
            sal_dma_free(ptr);
            m7_enable_intr();
            return;
        }
    }
#endif /* CONFIG_STATIC_PKTBUF */

    sal_memset(ptr->pktbuffer, 0, CBXI_RX_BUFFER_SIZE);
    ptr->num_bytes = 0;
    ptr->flags = 0;
    if (rx_cb.rx_buffer_pool_tail) {
        rx_cb.rx_buffer_pool_tail->next = ptr;
        rx_cb.rx_buffer_pool_tail = ptr;
    } else {
        rx_cb.rx_buffer_pool_tail = ptr;
        rx_cb.rx_buffer_pool_head = ptr;
    }
    rx_cb.num_buffers_in_pool++;
    m7_enable_intr();
}

/*
 * cbxi_rx_buffer_alloc
 *    Allocate a CBXI packet from the pool
 * Note:
 *    This function must be called either from a single-threaded context or
 *    with interrupts disabled.
 */
void *
cbxi_rx_buffer_alloc()
{
    cbxi_pkt_t *ptr;
#ifdef TRACE_PKTBUF
    sal_printf("Num= %d Buffer Pool Head = %x Tail = %x\n",
               rx_cb.num_buffers_in_pool, rx_cb.rx_buffer_pool_head,
               rx_cb.rx_buffer_pool_tail);
#endif
    if (rx_cb.num_buffers_in_pool > 0) {
        ptr = (cbxi_pkt_t*)rx_cb.rx_buffer_pool_head;
        rx_cb.rx_buffer_pool_head = ptr->next;
        if (ptr == rx_cb.rx_buffer_pool_tail) {
            rx_cb.rx_buffer_pool_tail = NULL;
        }
        rx_cb.num_buffers_in_pool--;
        ptr->next = NULL;
#ifndef CONFIG_STATIC_PKTBUF
         /*
         * The buffer has been zeroed upon being freed. Make sure that those
         * writes make it to memory before DMA starts. Also invalidate the
         * same memory range in the cache, so that the packet contents will
         * not be hidden by the cache after DMA has completed.
         */
         m7_dcache_flush_invalidate((uint32*)ptr->pktbuffer, CBXI_RX_BUFFER_SIZE);
#endif
#ifdef TRACE_PKTBUF
            sal_printf("OK Num= %d Buffer Pool Head = %x Tail = %x\n",
               rx_cb.num_buffers_in_pool, rx_cb.rx_buffer_pool_head, rx_cb.rx_buffer_pool_tail);
#endif
            return CBXI_PKT_TO_BUF(ptr);
    }
    return (void*)NULL;
}

/*
 * cbxi_rx_task
 *    Main thread to handle packets
 */
void
cbxi_rx_task(void*params)
{
    uint8 *ptr = NULL;
    uint8 *buffer = NULL;
    cbxi_pkt_t *pkt;
    int pktlen = 0;
    cbxi_egress_imp_header_t cb_header;
    cbxi_timestamp_header_t ts_header;
    brcm_egress_imp_header_t bcm_header;

    rx_task_status = CBXI_THREAD_RUNNING;
    while (rx_task_status != CBXI_THREAD_STOP) {
#ifndef CONFIG_STATIC_PKTBUF
        if (rx_cb.num_buffers_in_pool < CBXI_RX_NUM_BUFFERS_LOW_THRESHOLD) {
            cbxi_rx_buffer_pool_refill(CBXI_RX_NUM_BUFFERS - rx_cb.num_buffers_in_pool);
            hpa_intr_enable(CBXI_PKT_RX);
        }
#endif
        pkt = cbxi_rx_dqueue(0);
        if (!pkt) {
            m7_disable_irq(HPA_IRQn);
            hpa_rx_ready();
            m7_enable_irq(HPA_IRQn);
            if (rx_cb.num_pkts_enqueued == rx_cb.num_pkts_processed) {
                sal_sem_take(rx_cb.rx_packets_sem, 100000);
            }
            continue;
        }
        rx_cb.num_pkts_processed++;
        if (pkt->flags & CBXI_PKT_FLAGS_RX_VALID) {
#ifdef CONFIG_PACKET_DEBUG
            if (rx_dump_packet) {
                cbxi_dump_pkt(pkt->pktbuffer, pkt->num_bytes);
            }
#endif
            if (pkt->flags & CBXI_PKT_FLAGS_RX_SOP) {
                if (pkt->flags & CBXI_PKT_FLAGS_RX_EOP) {
                    buffer = (void*)cbxi_rx_extract_header(pkt->pktbuffer,
                                                    pkt->total_len, pkt->num_bytes,
                                                    &pktlen, &cb_header, &ts_header, &bcm_header);
                    if (!buffer) {
                        LOG_VERBOSE(BSL_LS_FSAL_PKTIO,
                           (BSL_META(
                               "FSAL API : No IMP Header on packet, Dropping\n")));
                    } else {
                        cbxi_rx_process_pkt(buffer, pktlen, &cb_header, &ts_header, &bcm_header);
                    }
                } else {
                    ptr = buffer = (void*)cbxi_rx_extract_header(pkt->pktbuffer,
                                                  pkt->total_len, pkt->num_bytes,
                                                  &pktlen, &cb_header, &ts_header, &bcm_header);
                    if (!buffer) {
                        LOG_ERROR(BSL_LS_FSAL_PKTIO,
                           (BSL_META(
                               "FSAL API : Out of Memory, Dropping chain\n")));
                        rx_cb.rx_current_chain_drop = 1;
                    } else {
                        ptr += pktlen;
                    }
               }
               cbxi_rx_buffer_free(pkt);
           } else if (pkt->flags & CBXI_PKT_FLAGS_RX_MID) {
               if (!rx_cb.rx_current_chain_drop) {
                   sal_memcpy(ptr, pkt->pktbuffer, pkt->num_bytes);
                   ptr += pkt->num_bytes;
                   pktlen += pkt->num_bytes;
               }
               cbxi_rx_buffer_free(pkt);
           } else if (pkt->flags & CBXI_PKT_FLAGS_RX_EOP) {
               if (!rx_cb.rx_current_chain_drop) {
                   sal_memcpy(ptr, pkt->pktbuffer, pkt->num_bytes);
                   pktlen += pkt->num_bytes;
                   cbxi_rx_process_pkt(buffer, pktlen, &cb_header, &ts_header, &bcm_header);
               }
               cbxi_rx_buffer_free(pkt);
               rx_cb.rx_current_chain_drop = 0;
           }
        } else {
            LOG_ERROR(BSL_LS_FSAL_PKTIO,
               (BSL_META(
                   "FSAL API : Received Packet with Valid not set, Dropping\n")));
            sal_printf("VALID NOT SET PKT %x %x %d %x\n", (uint32)pkt, pkt->pktbuffer, pkt->num_bytes, pkt->flags);
#ifdef CONFIG_PACKET_DEBUG
            if (rx_dump_packet)
                cbxi_dump_pkt(pkt->pktbuffer, pkt->num_bytes);
#endif
            rx_cb.num_pkts_dropped++;
            cbxi_rx_buffer_free(pkt);
        }
    }
    rx_task_status = CBXI_THREAD_DONE;
}
#endif /* !CONFIG_EXTERNAL_HOST */

#ifdef CONFIG_PACKET_DEBUG
/*
 * cbxi_dump_pkt
 *    Debug routine to dump packets received
 */
void
cbxi_dump_pkt(uint8 *buffer, int size)
{
    int i;
    sal_printf("\nPktDump  Pkt: %08x Len: %d", buffer, size);
    for (i = 0; i < size; i++) {
        if (i % 16 == 0) sal_printf("\n%04X: ", i);
        sal_printf(" %02x", buffer[i]);
    }
    sal_printf("\n");
}
#endif

/*
 * cbxi_rx_extract_header
 *    Extract CB and TS headers if present, returns a new buffer with these headers removed
 */
uint8 *
cbxi_rx_extract_header(uint8 *buffer, uint32 buflen,
                       uint32 size, int *newsize,
                       cbxi_egress_imp_header_t *cb_header,
                       cbxi_timestamp_header_t *ts_header,
                       brcm_egress_imp_header_t *bcm_header)
{
    uint8 *newbuffer = NULL;
    uint8 *ptr = buffer;
    int hdr_size = 0, pktlen = 0;
    uint16 etype = 0;

    if (!newsize) {
       return NULL;
    }
    if (ts_header) {
        sal_memset(ts_header, 0, sizeof(cbxi_timestamp_header_t));
    }
    if (cb_header) {
        sal_memset(cb_header, 0, sizeof(cbxi_egress_imp_header_t));
    }
    //Skip mac
    ptr += 12;
    sal_memcpy(&etype, ptr, 2);
    etype = sal_ntohs(etype);
    if (etype == cbxi_get_imp_header_type(imp_cb_ing)) {
        if (cb_header) {
            hdr_size = cbxi_unpack_imp_header(ptr, cb_header);
        }
        ptr += hdr_size;
        pktlen = size - hdr_size;
        buflen -= hdr_size;
        hdr_size = ptr - buffer;
#ifdef CONFIG_BCM_TAG
        /* BRCM tag is also passed along with CB tag */
        hdr_size = cbxi_unpack_brcm_header(ptr, bcm_header);
        ptr += hdr_size;
        pktlen = pktlen - hdr_size;
        buflen -= hdr_size;
        hdr_size = ptr - buffer;
#endif /* CONFIG_BCM_TAG */

#ifdef CONFIG_PTP
        sal_memcpy(&etype, ptr, 2);
        etype = sal_ntohs(etype);
        if (etype == cbxi_get_imp_header_type(imp_cb_ts)) {
            if (ts_header) {
                hdr_size = cbxi_unpack_timestamp_header(ptr, ts_header);
            }
            ptr += hdr_size;
            pktlen = pktlen - hdr_size;
            buflen -= hdr_size;
            hdr_size = ptr - buffer;
        }
#endif /* CONFIG_PTP */

        ptr = newbuffer = sal_alloc(buflen, "Rx packet");
        if (newbuffer != NULL) {
            sal_memcpy(ptr, buffer, 12);
            ptr += 12;
            sal_memcpy(ptr, (buffer + hdr_size), pktlen - 12);
            *newsize = pktlen;
        } else {
            rx_alloc_fails++;
        }
    } else {
        rx_unknown_etypes++;
#ifndef CONFIG_EXTERNAL_HOST
        sal_printf("NO CBX HEADER SET PKT %x %d\n", buffer, size);
#ifdef CONFIG_PACKET_DEBUG
        if (rx_dump_packet)
            cbxi_dump_pkt(buffer, size);
#endif
#endif
    }
    return (newbuffer);
}


/*
 * cbxi_rx_init
 *    Initialize the Rx and HPA modules
 */
int
cbxi_rx_init()
{
    int rv = CBX_E_NONE;

#ifdef CONFIG_EXTERNAL_HOST
    rv = soc_packetio_init(0);
#else
    sal_memset(&rx_cb, 0, sizeof(rx_cb));
    rx_cb.rx_packets_sem = sal_sem_create("Rx Ready", 1, 0);
    if (rx_cb.rx_packets_sem == NULL) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "FSAL API : Sem create failed\n")));
        return CBX_E_INIT;
    }
    if (CBXI_RX_NUM_BUFFERS < CBXI_RX_NUM_BUFFERS_MIN) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "FSAL API : Too few buffers\n")));
        return CBX_E_INIT;
    }
#ifdef CONFIG_STATIC_PKTBUF
    rv = cbxi_rx_buffer_pool_init();
    if (rv != CBX_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "FSAL API : Buffer pool Init failed\n")));
        sal_sem_destroy(rx_cb.rx_packets_sem);
        return rv;
    }
#else /* !CONFIG_STATIC_PKTBUF */
    rv = cbxi_rx_buffer_pool_refill(CBXI_RX_NUM_BUFFERS);
    if (rv != CBX_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "FSAL API : Buffer pool refill failed\n")));
        sal_sem_destroy(rx_cb.rx_packets_sem);
        return rv;
    }
#endif /* !CONFIG_STATIC_PKTBUF */
    rv = hpa_init();
    if (rv != CBX_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "FSAL API : HPA init failed %d\n"), rv));
        sal_sem_destroy(rx_cb.rx_packets_sem);
#ifndef CONFIG_STATIC_PKTBUF
        cbxi_rx_buffer_pool_release();
#endif
        return rv;
    }
    rxtask = sal_thread_create("RX", CBXI_RX_TASK_STACK_SIZE,
                                CBXI_RX_TASK_PRIO, cbxi_rx_task, NULL);
    if (rxtask == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "FSAL API : Failed Creating Rx Task\n")));
        sal_sem_destroy(rx_cb.rx_packets_sem);
#ifndef CONFIG_STATIC_PKTBUF
        cbxi_rx_buffer_pool_release();
#endif
        return CBX_E_INIT;
    }
    rx_dma_alloc_fails = 0;
#endif
    rx_alloc_fails = 0;
    rx_unknown_etypes = 0;
    return rv;
}

/*
 * cbxi_rx_uninit
 *    Uninit the Rx and HPA modules
 */
int
cbxi_rx_uninit()
{
#ifdef CONFIG_EXTERNAL_HOST
    soc_packetio_uninit(0);
#else
#ifndef CONFIG_STATIC_PKTBUF
    cbxi_rx_buffer_pool_release();
#endif /* CONFIG_STATIC_PKTBUF */
    hpa_uninit();
    rx_task_status = CBXI_THREAD_STOP;
    while(rx_task_status != CBXI_THREAD_DONE);
    sal_sem_destroy(rx_cb.rx_packets_sem);
    sal_memset(&rx_cb, 0, sizeof(rx_cb));
    rx_dma_alloc_fails = 0;
#endif /* CONFIG_EXTERNAL_HOST */
    rx_alloc_fails = 0;
    return SOC_E_NONE;
}


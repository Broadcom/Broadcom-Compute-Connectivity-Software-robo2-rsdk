/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 *  $Copyright: (c) 2020 Broadcom Inc.
 *  All Rights Reserved$
 *
 *  File:
 *      pktio.c
 *  Description:
 *      Packet IO using HPA
 *
 */

#include <sal_types.h>
#include <sal_alloc.h>
#include <sal_util.h>
#include <bsl_log/bsl.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/regacc.h>
#include <fsal/error.h>
#include <fsal/cosq.h>
#include <fsal/lag.h>
#include <fsal/pktio.h>
#include <fsal/vlan.h>
#include <fsal/exception.h>
#include <fsal_int/types.h>
#include <fsal_int/port.h>
#include <fsal_int/pkt.h>
#include <fsal_int/imp.h>
#include <fsal_int/stt.h>
#include <fsal_int/native_types.h>
#include <fsal_int/lag.h>


/* Based on the CBX_PKTIO_CALLBACK_XX Flags */
#define CBXI_PKTIO_CALLBACK_FILTER_MAX      8
#define CBXI_PKT_LLC_HEADER_LEN             6

#define CBXI_CREATE_DLI(type, port) ((type) << 11 | (port))

typedef enum {
    CBXI_PKT_CB_HEADER,
    CBXI_PKT_BRCM_HEADER,
    CBXI_PKT_CB_DBG_HEADER
} cbxi_pkt_header_t;

typedef struct cbxi_callback_s {
    cbx_packet_io_callback_t   handler;
    cbx_packet_cb_info_t       cbinfo;
    struct cbxi_callback_s    *next;
    struct cbxi_callback_s    *prev;
} cbxi_callback_t;


typedef struct {
    int                       init;
    cbxi_pkt_header_t         header_mode;
    sal_mutex_t               callback_mutex;
    cbxi_callback_t          *callback_handlers[CBXI_PKTIO_CALLBACK_FILTER_MAX];
    int                       num_callbacks;
} cbx_pktio_t;

/* Context */
static cbx_pktio_t cbx_pktio_cb;
uint32 tx_dma_alloc_fails;
uint32 rx_attr_alloc_fails;
uint32 rx_pv_type = 0;
uint32 rx_lin_type = 0;
uint32 rx_trap_type = 0;
#ifdef CONFIG_PACKET_DEBUG
int tx_dump_packet = 0;
int rx_dump_packet = 0;
#endif

void
tx_dump_stats(void)
{
    sal_printf("Tx Dma Alloc Fails: %d\n", tx_dma_alloc_fails);
    sal_printf("Rx Attr Alloc Fails: %d\n", rx_attr_alloc_fails);
    sal_printf("Rx PV: %d Lin: %d Traps: %d\n", rx_pv_type, rx_lin_type, rx_trap_type);
}

/*****************************************
 *  FSAL API HELPERS
 *****************************************/
#ifdef CONFIG_PACKET_DEBUG
void set_pkt_dump(int dir, int enable)
{
    if (dir)  {
        rx_dump_packet = enable;
    } else {
        tx_dump_packet = enable;
    }
}

void print_cb_header(cbxi_imp_header_t *cb_header)
{
    sal_printf("CB Header Dump\n");
    sal_printf("  ethertype     = %04x\n", cb_header->ethertype);
    sal_printf("  trapgrp_lbh_dp= %04x\n", cb_header->trapgrp_lbh_dpp);
    sal_printf("  unit          = %04x\n", cb_header->unit);
    sal_printf("  fwdop         = %04x\n", cb_header->fwdop);
    sal_printf("  dp            = %04x\n", cb_header->dp);
    sal_printf("  tc            = %04x\n", cb_header->tc);
    sal_printf("  spg           = %04x\n", cb_header->spg);
    sal_printf("  dest          = %04x\n", cb_header->dest);
    sal_printf("  vsi           = %04x\n", cb_header->vsi);
    sal_printf("  t             = %04x\n", cb_header->t);
}

void print_bcm_header(brcm_egress_imp_header_t *bcm_header)
{
    sal_printf("BCM Header Dump\n");
    sal_printf("  fwdop         = %04x\n", bcm_header->fwdop);
    sal_printf("  tc            = %04x\n", bcm_header->tc);
    sal_printf("  sport         = %04x\n", bcm_header->sport);
    sal_printf("  exception     = %04x\n", bcm_header->exception);
    sal_printf("  opcode        = %04x\n", bcm_header->opcode);
}

void print_ts_header(cbxi_timestamp_header_t *ts_header)
{
    sal_printf("TS Header Dump\n");
    sal_printf("  ethertype           = %04x\n", ts_header->ethertype);
    sal_printf("  timestamp_h         = %04x\n", ts_header->timestamp_h);
    sal_printf("  timestamp_l         = %04x\n", ts_header->timestamp_l);
}

#endif

/*
 * cbxi_callback_to_idx
 *     Map the Callback to Internal index
 */
enum {
    /* Add new to top */
    CBX_PKTIO_CALLBACK_ECP_ID,
    CBX_PKTIO_CALLBACK_LLC_ID,
    CBX_PKTIO_CALLBACK_LLDP_ID,
    CBX_PKTIO_CALLBACK_UDP_ID,
    CBX_PKTIO_CALLBACK_TCP_ID,
    CBX_PKTIO_CALLBACK_IP_ID,
    CBX_PKTIO_CALLBACK_ALL_ID,
    CBX_PKTIO_CALLBACK_TRAP_ID,
};

STATIC void
cbxi_callback_clear(int idx, uint32 *flags)
{
    switch (idx) {
    case CBX_PKTIO_CALLBACK_ECP_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_ECP;
        break;
    case CBX_PKTIO_CALLBACK_LLC_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_LLC;
        break;
    case CBX_PKTIO_CALLBACK_LLDP_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_LLDP;
        break;
    case CBX_PKTIO_CALLBACK_UDP_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_UDP;
        break;
    case CBX_PKTIO_CALLBACK_TCP_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_TCP;
        break;
    case CBX_PKTIO_CALLBACK_IP_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_IP;
        break;
    case CBX_PKTIO_CALLBACK_ALL_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_ALL;
        break;
    case CBX_PKTIO_CALLBACK_TRAP_ID:
        *flags &= ~CBX_PKTIO_CALLBACK_TRAP;
        break;
    }
}

STATIC int
cbxi_callback_to_idx(int callback_type)
{
    int idx = -1;
    if (callback_type & CBX_PKTIO_CALLBACK_ECP) {
        idx = CBX_PKTIO_CALLBACK_ECP_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_LLC) {
        idx = CBX_PKTIO_CALLBACK_LLC_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_LLDP) {
        idx = CBX_PKTIO_CALLBACK_LLDP_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_UDP) {
        idx = CBX_PKTIO_CALLBACK_UDP_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_TCP) {
        idx = CBX_PKTIO_CALLBACK_TCP_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_IP) {
        idx = CBX_PKTIO_CALLBACK_IP_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_ALL) {
        idx = CBX_PKTIO_CALLBACK_ALL_ID;
    } else if (callback_type & CBX_PKTIO_CALLBACK_TRAP) {
        idx = CBX_PKTIO_CALLBACK_TRAP_ID;
    }

    return idx;
}

static inline
void dump_cb(char *message, cbx_packet_cb_info_t cb_info)
{
    LOG_VERBOSE(BSL_LS_FSAL_PKTIO,
       (BSL_META(
           "PKTIO API: %s %s Flags(%08x)\n"), message, cb_info.name, cb_info.flags));
}

/*
 * cbxi_add_callback_handler
 *     Add another callback to the chain of handlers.
 *     Each type of callback has its own chain, sorted based on priority
 */
int
cbxi_add_callback_handler(int idx,
                          cbx_packet_io_callback_t cb,
                          cbx_packet_cb_info_t info,
                          cbx_pktio_rx_cb_handle *handle)
{
    cbxi_callback_t *cbptr, *cbnew;
    cbptr = cbx_pktio_cb.callback_handlers[idx];

    *handle = NULL;
    cbnew = (cbxi_callback_t*)sal_alloc(sizeof(cbxi_callback_t), "CBXI Callbacks");
    if (cbnew) {
        cbnew->cbinfo = info;
        cbnew->handler = cb;
        cbnew->next = cbnew->prev = NULL;
        if (cbptr == NULL) {
            cbx_pktio_cb.callback_handlers[idx] = cbnew;
        } else {
            while (cbptr->cbinfo.priority <= cbnew->cbinfo.priority) {
                if (cbptr->next == NULL) {
                    break;
                } else {
                    cbptr = cbptr->next;
                }
            }
            if (cbptr->cbinfo.priority < cbnew->cbinfo.priority) {
                if (cbptr->prev) {
                    cbptr->prev->next = cbnew;
                }
                cbnew->prev = cbptr->prev;
                cbnew->next = cbptr;
                cbptr->prev = cbnew;
                if (cbx_pktio_cb.callback_handlers[idx] == cbptr) {
                    cbx_pktio_cb.callback_handlers[idx] = cbnew;
                }
            } else {
                if (cbptr->next) {
                    cbptr->next->prev = cbnew;
                }
                cbnew->next = cbptr->next;
                cbptr->next = cbnew;
                cbnew->prev = cbptr;
            }
        }
        cbx_pktio_cb.num_callbacks++;
        *handle = cbnew;
        return CBX_E_NONE;
    }
    return CBX_E_MEMORY;
}

/*
 * cbxi_remove_callback_handler
 *     Remove the handler if it exists. Returns NOT_FOUND otherwise
 */
int
cbxi_remove_callback_handler(cbx_pktio_rx_cb_handle handle)
{
    int i;
    cbxi_callback_t *cbptr = handle, *iterator;

    for (i = 0; i < CBXI_PKTIO_CALLBACK_FILTER_MAX; i++) {
        for (iterator = cbx_pktio_cb.callback_handlers[i];
             iterator != NULL;
             iterator = iterator->next) {
            if (cbptr == iterator) {
                cbx_pktio_cb.num_callbacks--;
                if (cbptr->next) {
                    cbptr->next->prev =  cbptr->prev;
                }
                if (cbptr->prev) {
                    cbptr->prev->next = cbptr->next;
                }
                if (cbptr == cbx_pktio_cb.callback_handlers[i]) {
                    cbx_pktio_cb.callback_handlers[i] = cbptr->next;
                }
                dump_cb("Callback UnRegistered", cbptr->cbinfo);
                sal_free(cbptr);
                return CBX_E_NONE;
            }
        }
    }

    return CBX_E_NOT_FOUND;
}

/*
 * cbxi_callback
 *     Invoke the Callback
 *     If there are multiple callbacks invoke them in order of priority (low prio is high priority)
 *     Caller is expected to free buffer and packet_attr after usage
 */
int
cbxi_callback(int idx, void *buffer, int buffer_size,
                                   cbx_packet_attr_t *packet_attr)
{
    int rv = CBX_RX_NOT_HANDLED;
    cbxi_callback_t *cbptr;
    if (idx < 0) {
        return rv;
    }
    sal_mutex_take(cbx_pktio_cb.callback_mutex, sal_mutex_FOREVER);
    for (cbptr = cbx_pktio_cb.callback_handlers[idx];
             cbptr != NULL;
                 cbptr = cbptr->next) {
        if (cbptr) {
            //dump_cb("Packet Handled by", cbptr->cbinfo);
            rv = cbptr->handler(buffer, buffer_size, packet_attr, &cbptr->cbinfo);
            if (rv == CBX_RX_HANDLED) {
                break;
            }
        }
    }
    sal_mutex_give(cbx_pktio_cb.callback_mutex);
    return rv;
}

/*
 * cbxi_get_imp_header_type
 *     Get the Etype to use based on the imp header type requested
 */
int
cbxi_get_imp_header_type(imp_header_type_t type)
{
    int etype = 0xFFFF;
    switch (type) {
    case imp_cb_ing:
        etype = CBX_STT_ETYPE_CP_TO_SWITCH;
        break;
    case imp_cb_egr:
        etype = CBX_STT_ETYPE_SWITCH_TO_CP;
        break;
    case imp_br_egr:
    case imp_br_ing:
        etype = CBX_STT_ETYPE_BRCM_TAG;
        break;
    case imp_cb_ts:
        etype = CBX_STT_ETYPE_SWITCH_TO_CP_TS;
        break;
    }
    return etype;
}

/*
 * cbxi_pack_imp_header
 *  Serialize the imp structure
 */
int
cbxi_pack_imp_header(uint8 *buffer, cbxi_ingress_imp_header_t *header)
{
    uint32 w0, w1;
    if (header && buffer) {
        w0 = CBXI_IMP_PACK_W0(header);
        w1 = CBXI_IMP_PACK_W1(header);
        w0 = sal_htonl(w0);
        w1 = sal_htonl(w1);
        /* sal_printf("%08x %08x\n", w0, w1); */
        sal_memcpy(buffer, &w0, 4);
        sal_memcpy(buffer + 4, &w1, 4);
        return 8;
    }
    return 0;
}

/*
 * cbxi_unpack_imp_header
 *  Deserialize the imp structure
 */
int
cbxi_unpack_imp_header(uint8 *buffer, cbxi_egress_imp_header_t *header)
{
    uint32 w0, w1;
    if (header && buffer) {
        sal_memcpy(&w0, buffer, 4);
        sal_memcpy(&w1, buffer + 4, 4);
        /* sal_printf("Before: %08x %08x\n", w0, w1); */
        w0 = sal_ntohl(w0);
        w1 = sal_ntohl(w1);
        /* sal_printf("After: %08x %08x\n", w0, w1); */
        CBXI_IMP_UNPACK_W0(w0, header);
        CBXI_IMP_UNPACK_W1(w1, header);
        return 8;
    }
    return 0;
}

/*
 * cbxi_pack_brcm_header
 *  Serialize the brcm structure
 */
int
cbxi_pack_brcm_header(uint8 *buffer, brcm_ingress_imp_header_t *header)
{
    return CBX_E_UNAVAIL;
}

/*
 * cbxi_unpack_brcm_header
 *  Deserialize the brcm structure
 */
int
cbxi_unpack_brcm_header(uint8 *buffer, brcm_egress_imp_header_t *header)
{
    uint32 w0;

    if (header && buffer) {
        sal_memcpy(&w0, buffer, 4);
        /* sal_printf("Before: %08x %08x\n", w0, w1); */
        w0 = sal_ntohl(w0);
        /* sal_printf("After: %08x %08x\n", w0, w1); */
        CBXI_BRCM_UNPACK_W0(w0, header);
        return 4;
    }
    return 0;
}

/*
 * cbxi_unpack_timestamp_header
 *  Deserialize the Timestamp header
 */
int
cbxi_unpack_timestamp_header(uint8 *buffer, cbxi_timestamp_header_t *header)
{
    uint32 w0, w1;
    if (buffer && header) {
        sal_memcpy(&w0, buffer, 4);
        sal_memcpy(&w1, buffer + 4, 4);
        w0 = sal_ntohl(w0);
        w1 = sal_ntohl(w1);
        CBXI_IMP_TS_UNPACK_W0(w0, header);
        CBXI_IMP_TS_UNPACK_W1(w1, header);
        return 8;
    }
    return 0;
}

/*
 * cbxi_rx_process_pkt
 *     Identify and call the correct call back handler registered with us
 *     Its expected that the handler will free the packet and attr when done
 *     We free the packet when nobody takes it
 */
int
cbxi_rx_process_pkt(uint8 *buffer, int len, cbxi_egress_imp_header_t *header,
                    cbxi_timestamp_header_t *ts_header, brcm_egress_imp_header_t *bcm_header)
{

    uint8 *ptr = buffer;
    int rv = CBX_RX_NOT_HANDLED;
    cbx_packet_attr_t *attr;
#if 0
    lin2vsi_t  linentry;
#endif

    attr = sal_alloc(sizeof(cbx_packet_attr_t), "Rx Packet Attr");
    if (attr == NULL) {
        rx_attr_alloc_fails++;
        sal_free(buffer);
        return CBX_RX_NOT_HANDLED;
    }
    sal_memset(attr, 0, sizeof(cbx_packet_attr_t));
#ifdef CONFIG_PACKET_DEBUG
    if (rx_dump_packet) {
        print_cb_header(header);
#ifdef CONFIG_BCM_TAG /* BCM TAG for SPP info */
        print_bcm_header(bcm_header);
#endif
#ifdef CONFIG_PTP
        print_ts_header(ts_header);
#endif
        cbxi_dump_pkt((void *)buffer, len);
    }
#endif

    attr->tc.int_dp = header->dp;
    attr->tc.int_pri = header->tc;

#ifdef CONFIG_BCM_TAG /* BCM TAG for SPP info */
    /* source physical port is not given by the CB tag, get the SPP from the BRCM tag */
    attr->source_index = bcm_header->sport + header->unit * CBX_MAX_PORT_PER_UNIT;
#else
    attr->source_index = header->spg;
#endif

#ifdef CONFIG_PTP
    attr->ing_ptp_ts = ts_header->timestamp_l;
#endif

#ifdef CONFIG_EXTERNAL_HOST
    attr->dest_index = CBX_PORT_ECPU;
#else
    attr->dest_index = CBX_PORT_ICPU;
#endif
    if (header->t) {
        attr->vlanid = header->vsi;
        CBX_PORTID_LOCAL_SET(attr->source_port, attr->source_index);
        rx_pv_type++;
    } else {
        rx_lin_type++;
#if 0 //TEMP Change, LIN not supported yet, HW returns invalid data
        attr->source_port = 0; //Fixme??
        if (SOC_E_NONE == soc_robo2_lin2vsi_get(header->unit, header->vsi, &linentry)) {
            attr->vlanid = linentry.vsi;      //LIN2VSI(N_VSI)
        } else {
            attr->vlanid = CBX_VLAN_INVALID;
        }
#endif
    }

    if (header->fwdop == CBXI_FWD_OPCODE_EXCEPTION) {
        rx_trap_type++;
        attr->flags |= CBX_PACKET_RX_TRAP;
        if ((header->dest >= CBXI_SLIC_ID_START) &&
              (header->dest <= CBXI_SLIC_ID_END)) {
            attr->trapid = header->trapgrp_lbh_dpp;  /* Slic Group id */
            attr->trap_data = header->dest;          /* Slic Match id */
        } else if ((header->dest >= CBXI_MIRROR_GRP_ID_START) &&
              (header->dest <= CBXI_MIRROR_GRP_ID_END)) {
            attr->trapid = header->dest;
            attr->flags |= CBX_PACKET_RX_MIRROR | CBX_PACKET_RX_TRAP;
        } else if ((header->dest >= CBXI_CFP_ACTION_ID_START) &&
              (header->dest <= CBXI_CFP_ACTION_ID_END)) {
            attr->trapid = header->trapgrp_lbh_dpp;
            attr->trap_data = header->dest;
        } else {
            attr->trapid = header->dest;
            switch (header->dest) {
            case BC_DROP:
            case MLF_DROP:
                attr->flags |= CBX_PACKET_RX_MCAST;
                /* FALL Through */
            case ULF_DROP:
                /* Associated with DLF Failure */
                attr->flags |= CBX_PACKET_RX_DLF;
                break;
            case CA_SA_MOVE:
            case SA_MOVE:
            case MAC_LEARN_LIMIT:
            case MAC_SA_FULL:
            case CA_LEARN_FRAME:
            case LEARN_FRAME:
            case NO_LRN_LKP_FAIL:
                /* Associated with Learning, set Learn */
                attr->flags |= CBX_PACKET_RX_LEARN;
                break;
            }
        }

        rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_TRAP), buffer, len, attr);
        if (rv == CBX_RX_HANDLED) {
            return rv;
        }
        goto default_handler;
    } else {
        attr->dest_port = header->dest;
    }

    ptr = buffer + 12;
    if ((ptr[0] == 0xaa) && (ptr[1] == 0xaa) && (ptr[2] == 0x03)) {
        /* LLC Frame */
        rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_LLC), buffer, len, attr);
        if (rv == CBX_RX_HANDLED) {
            return rv;
        }
        ptr += CBXI_PKT_LLC_HEADER_LEN;
    }
    if ((ptr[0] == 0x89) && (ptr[1] == 0x40)) {
        /* ECP */
        rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_ECP), buffer, len, attr);
        if (rv == CBX_RX_HANDLED) {
            return rv;
        }
        goto default_handler;
    } else if ((ptr[0] == 0x88) && (ptr[1] == 0xcc)) {
        /* LLDP */
        rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_LLDP), buffer, len, attr);
        if (rv == CBX_RX_HANDLED) {
            return rv;
        }
        goto default_handler;
    } else if (((ptr[0] == 0x81) && (ptr[1] == 0x00)) ||
           ((ptr[0] == 0x88) && (ptr[1] == 0xa8))) {
        /* Skip etype */
        ptr += 4;
    }

    /* Is this IP */
    if (((ptr[0] == 0x08) && (ptr[1] == 0x00)) || ((ptr[0] == 0x86) && (ptr[1] == 0xdd))) {
        ptr += 2;
        /* Examine the protocol */
        if (ptr[9] == 0x11) {
            rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_UDP), buffer, len, attr);
            if (rv == CBX_RX_HANDLED) {
                return rv;
            }
        } else if (ptr[9] == 0x6) {
            rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_TCP), buffer, len, attr);
            if (rv == CBX_RX_HANDLED) {
                return rv;
            }
        }
        rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_IP), buffer, len, attr);
        if (rv == CBX_RX_HANDLED) {
                return rv;
        }
    }

default_handler:
    /* Default Handlers */
    rv = cbxi_callback(cbxi_callback_to_idx(CBX_PKTIO_CALLBACK_ALL), buffer, len, attr);
    if (rv == CBX_RX_HANDLED) {
        return rv;
    }
    /* sal_printf("Pkt not handled\n"); */
    /* If we come here the packet was not accepted */
    sal_free(buffer);
    sal_free(attr);
    return rv;
}

/*
 * cbxi_insert_header
 *    Insert a CB header, if required. A new buffer is allocated and the old one freed
 */

STATIC uint8*
cbxi_insert_header(void *buffer, int *size, int insert, cbxi_ingress_imp_header_t *cb_header)
{
    uint8 *ptr, *newbuffer;
    int hlen = 0;
    ptr = newbuffer = sal_dma_alloc(*size + sizeof(cbxi_ingress_imp_header_t), "Tx Packet");
    if (ptr) {
        if (insert) {
            sal_memcpy(ptr, (char*)buffer, 12);
            ptr += 12;
            hlen = cbxi_pack_imp_header(ptr, cb_header);
            ptr += hlen;
            sal_memcpy(ptr, (char*)buffer + 12, *size - 12);
            *size += hlen;
        } else {
            sal_memcpy(ptr, (char*)buffer, *size);
        }
    } else {
        tx_dma_alloc_fails++;
    }
    return newbuffer;
}


int
cbxi_pkt_brcm_send(void *buffer, int buffer_size,
                         cbx_packet_attr_t *packet_attr)
{
    return CBX_E_UNAVAIL;
}

int
cbxi_pkt_cb_dbg_send(void *buffer, int buffer_size,
                            cbx_packet_attr_t *packet_attr)
{
    return CBX_E_UNAVAIL;
}

/*
 * cbxi_pkt_cb_send
 *    Create a CB Tagged packet and send it
 */
int
cbxi_pkt_cb_send(void *buffer, int buffer_size,
                       cbx_packet_attr_t *packet_attr)
{
    int insert_header;
    cbxi_ingress_imp_header_t cb_header;
    cbx_port_t port, lagid;
    cbxi_pgid_t lpg, gpg;
    int unit = 0;
    int rv = CBX_E_NONE;
    dgt_t  dgt_entry;
    cbx_lag_info_t lag_info;
    uint32_t pgid, pp_mask, pp=0;
    svt_t         svt_entry;
    int index, max_index;
    lpg2ppfov_t   ppfov_entry;
#ifdef CONFIG_EXTERNAL_HOST
    uint8 *old_buf = NULL;
#endif

    if (!cbx_pktio_cb.init) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "PKTIO API: Pktio Module Not Initialized\n")));
        return CBX_E_INIT;
    }
#if 0
#ifndef CONFIG_EXTERNAL_HOST
    if (packet_attr->flags != CBX_PACKET_TX_FWD_RAW) {
        /* In ROBO Os only RAW mode is supported */
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "PKTIO API: Unsupported Mode %x, Only RAW mode is supported\n"), packet_attr->flags));
        return SOC_E_PARAM;
    }
#endif
#endif
    if (packet_attr->flags != CBX_PACKET_TX_FWD_MCAST) {
        if (packet_attr->dest_port != 0) {
            rv = cbxi_robo2_port_validate(packet_attr->dest_port, &port, &lpg, &unit);
            if (rv != CBX_E_NONE) {
                return rv;
            }
        } else {
            // Expect DPP to be present in Dest_index
            rv = cbxi_robo2_port_validate(packet_attr->dest_index, &port, &lpg, &unit);
            if (rv != CBX_E_NONE) {
                return rv;
            }
        }
    }
    port = port + (unit * CBX_MAX_PORT_PER_UNIT);

    insert_header = 1; // Default insert CB header
    sal_memset(&cb_header, 0, sizeof(cb_header));
    cb_header.ethertype = cbxi_get_imp_header_type(imp_cb_ing);
    cb_header.tc = packet_attr->tc.int_pri;
    cb_header.dp = packet_attr->tc.int_dp;
    cb_header.spg = packet_attr->source_index; //? Index or source port?
    cb_header.vsi = packet_attr->vlanid;
    cb_header.unit = CBX_AVENGER_PRIMARY;

    switch (packet_attr->flags) {
    case CBX_PACKET_TX_FWD_UCAST:
        cb_header.fwdop = CBXI_FWD_OPCODE_UCAST_DIRECTED;
        if (CBX_PORTID_IS_VLAN_PORT(packet_attr->dest_port)) {
            //cb_header.dest = port; //LIN
            cb_header.dest = CBXI_CREATE_DLI(0, port);
            cb_header.t = 0;
        } else if (CBX_PORTID_IS_MCAST(packet_attr->dest_port)) {
            // Expect MCAST Fwd directive
            return CBX_E_PARAM;

        } else if (cbxi_robo2_lag_member_check(packet_attr->dest_port, &lagid) == 0) {
            /* Destination port is member of LAG, needs to be sent on physical port.
             * As all the physical ports which are members of LAG have same PG,
             * need LBH index to map it to the given physical port. Use the LBH
             * index along with PG to send it on given physical port */
            CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(lagid, &lag_info));
            pgid = (unit == CBX_AVENGER_PRIMARY) ? lag_info.lpgid:lag_info.rlpgid;
            pp =  port - (unit * CBX_MAX_PORT_PER_UNIT);
            CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit, pgid, &ppfov_entry));
            max_index =  soc_robo2_svt_max_index(unit);
            for (index = 0; index <= max_index; index++) {
                /* Read entry from SVT */
                CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));
                pp_mask = svt_entry.svt_mask & ppfov_entry.ppfov;
                if (CBX_PBMP_MEMBER(pp_mask, pp)) {
                    cb_header.trapgrp_lbh_dpp = index;
                    break;
                }
            }
            if (index > max_index) {
                return CBX_E_PARAM;
            }
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg));
            cb_header.dest = CBXI_CREATE_DLI(1, gpg);
            cb_header.t = 1 ;// Type PV
        } else {
            /* Destination port is either LAG or physical port */
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg));
            cb_header.dest = CBXI_CREATE_DLI(1, gpg);
            cb_header.t = 1 ;// Type PV
        }
        break;
    case CBX_PACKET_TX_FWD_MCAST:
        cb_header.fwdop = CBXI_FWD_OPCODE_MCAST_DIRECTED;
        if (CBX_PORTID_IS_MCAST(packet_attr->mcastid)) {
            cb_header.dest = CBX_PORTID_MCAST_ID_GET(packet_attr->mcastid);
            rv = soc_robo2_dgt_get(unit, cb_header.dest, &dgt_entry);
            if (rv == SOC_E_NONE) {
                if ((dgt_entry.lilt_type == 0) && (dgt_entry.lilt_ndx == 0)) {
                    /* L2 Multicast */
                    cb_header.t = 1;
                }
            } else {
                LOG_ERROR(BSL_LS_FSAL_PKTIO,
                   (BSL_META(
                       "PKTIO API: Error reading DGT for mcast id %u rv(%d)\n"),
                        packet_attr->mcastid, rv));
                return CBX_E_PARAM;
            }
        } else {
            LOG_ERROR(BSL_LS_FSAL_PKTIO,
              (BSL_META(
                  "PKTIO API: Invalid mcast id %u\n"), packet_attr->mcastid));
            return CBX_E_PARAM;
        }
        break;
    case CBX_PACKET_TX_FWD_SWITCH:
        insert_header = 0;
        break;
    case CBX_PACKET_TX_FWD_RAW:
        cb_header.fwdop = CBXI_FWD_OPCODE_UCAST_DIRECTED;
        cb_header.trapgrp_lbh_dpp  = port;
        cb_header.dest = 0;
        cb_header.t = 1 ;// Type PV
        break;
    default:
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "PKTIO API: Invalid packet flags attribute %d\n"), packet_attr->flags));
        return CBX_E_PARAM;
    }
#ifdef CONFIG_PACKET_DEBUG
    if (tx_dump_packet) {
        print_cb_header(&cb_header);
        cbxi_dump_pkt(buffer, buffer_size);
    }
#endif
#ifdef CONFIG_EXTERNAL_HOST
    old_buf = (uint8 *)buffer;
#endif
    buffer = (void*)cbxi_insert_header(buffer, &buffer_size,insert_header, &cb_header);
    if (!buffer) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
           (BSL_META(
               "PKTIO API: Packet Memory Alloc failed\n")));
        return CBX_E_MEMORY;
    }
#ifdef CONFIG_CASCADED_MODE
    /* reset ppfov before tx */
    CBX_IF_ERROR_RETURN(cbxi_pqm_cascade_ppfov_modify(CBXI_PQM_CASCADE_PPFOV_DELETE));
#endif
#ifdef CONFIG_EXTERNAL_HOST
    rv = soc_packetio_send(unit, buffer, buffer_size);
#else
    rv = hpa_tx_packet(buffer, buffer_size);
#endif
#ifdef CONFIG_CASCADED_MODE
    /* setback ppfov after tx */
    CBX_IF_ERROR_RETURN(cbxi_pqm_cascade_ppfov_modify(CBXI_PQM_CASCADE_PPFOV_ADD));
#endif

#ifdef CONFIG_EXTERNAL_HOST
    if (buffer) {
        /* Free allocated buffer */
        sal_dma_free(buffer);

        /* Return original buffer to user to free allocated memory */
        buffer = (void *)old_buf;
    }
#endif
    return rv;
}

int
cbxi_pktio_channel_init(void)
{
    uint32 regval = 0;

    /* Enable hisi channel */
    regval = 1;
    REG_WRITE_CB_SIA_HISI_SI_CONFIGr(CBX_AVENGER_PRIMARY, &regval);
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        REG_WRITE_CB_SIA_HISI_SI_CONFIGr(CBX_AVENGER_SECONDARY, &regval);
    }

    /* Enable Epp Transmit */
    regval = 0;
    REG_READ_CB_EPP_EPP_SID_PORT_EN_CONFIGr(CBX_AVENGER_PRIMARY, &regval);
#ifdef CONFIG_EXTERNAL_HOST
    regval |= 1 << CBX_PORT_ECPU;
#else
    regval |= 1 << CBX_PORT_ICPU;
#endif
    REG_WRITE_CB_EPP_EPP_SID_PORT_EN_CONFIGr(CBX_AVENGER_PRIMARY, &regval);

    return CBX_E_NONE;
}

int
cbxi_pktio_stt_init(void)
{
     uint32 status = 0;
     uint32 regval = 0;
     uint32 value = 0;
     int rv = 0;
     stt_t stt_entry;

     REG_READ_CB_IPP_STT_IA_STATUSr(CBX_AVENGER_PRIMARY, &status);
     if ((status & 7) == 0) {
         soc_robo2_stt_enable(CBX_AVENGER_PRIMARY);
         if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
             soc_robo2_stt_enable(CBX_AVENGER_SECONDARY);
         }
     }

     /* CP to SWITCH */
     sal_memset(&stt_entry, 0, sizeof(stt_t));
     stt_entry.etype = cbxi_get_imp_header_type(imp_cb_ing);
     stt_entry.ntype = CBXI_NT_CP2SW;
     stt_entry.length = 4;
     stt_entry.tci = 0;
     stt_entry.ekey_type = 7; /* USER_TPID7 */
     rv = soc_robo2_stt_set(CBX_AVENGER_PRIMARY, CBX_STT_INDEX_CP_TO_SWITCH,
                            &stt_entry, &status);
     if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Failed setting CP_TO_SW Header Ntype\n")));
        return rv;
     }
     if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
         stt_entry.etype = cbxi_get_imp_header_type(imp_cb_ing);
         stt_entry.ntype = CBXI_NT_CP2SW;
         stt_entry.length = 4;
         stt_entry.tci = 0;
         stt_entry.ekey_type = 7; /* USER_TPID7 */
         rv = soc_robo2_stt_set(CBX_AVENGER_SECONDARY, CBX_STT_INDEX_CP_TO_SWITCH,
                                &stt_entry, &status);
         if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_FSAL_PKTIO,
                 (BSL_META("PKTIO API: Failed setting CP_TO_SW Header Ntype\n")));
            return rv;
         }
     }

     regval = 0;
     value = cbxi_get_imp_header_type(imp_cb_ing);
     soc_CB_EPP_ETHERTYPE_CONFIGr_field_set(
         CBX_AVENGER_PRIMARY, &regval, CB_ETYPEf, &value);
     REG_WRITE_CB_EPP_ETHERTYPE_CONFIGr(CBX_AVENGER_PRIMARY, &regval);
     if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
         regval = 0;
         value = cbxi_get_imp_header_type(imp_cb_ing);
         soc_CB_EPP_ETHERTYPE_CONFIGr_field_set(
             CBX_AVENGER_PRIMARY, &regval, CB_ETYPEf, &value);
         REG_WRITE_CB_EPP_ETHERTYPE_CONFIGr(CBX_AVENGER_SECONDARY, &regval);
     }

     /* TimeStamp */
     stt_entry.etype = cbxi_get_imp_header_type(imp_cb_ts);
     stt_entry.ntype = CBXI_NT_EGR_TIMESTAMP;
     stt_entry.length = 3;
     rv = soc_robo2_stt_set(CBX_AVENGER_PRIMARY, CBX_STT_INDEX_SWITCH_TO_CP_TS,
                            &stt_entry, &status);
     if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Failed setting SW_TO_CP TS Header Ntype\n")));
     }
     if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
         stt_entry.etype = cbxi_get_imp_header_type(imp_cb_ts);
         stt_entry.ntype = CBXI_NT_EGR_TIMESTAMP;
         stt_entry.length = 3;
         rv = soc_robo2_stt_set(CBX_AVENGER_SECONDARY, CBX_STT_INDEX_SWITCH_TO_CP_TS,
                                &stt_entry, &status);
         if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_FSAL_PKTIO,
                 (BSL_META("PKTIO API: Failed setting SW_TO_CP TS Header Ntype\n")));
         }
     }
     return rv;
}


/*****************************************
 *  FSAL API IMPLEMENTATION BEGINS
 *****************************************/


/**
 * Packet_IO send function
 *
 * @param  buffer       (IN)  packet buffer
 * @param  buffer_size  (IN)  packet size in bytes
 * @param  packet_attr  (IN)  packet attributess
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_pktio_packet_send (void *buffer, int buffer_size,
                              cbx_packet_attr_t *packet_attr)
{
    if ((!buffer) || (!packet_attr)) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Invalid Parameters\n")));
        return CBX_E_PARAM;
    }

    switch (cbx_pktio_cb.header_mode) {
    case CBXI_PKT_CB_HEADER:
        return (cbxi_pkt_cb_send (buffer, buffer_size, packet_attr));
    case CBXI_PKT_CB_DBG_HEADER:
        return (cbxi_pkt_cb_dbg_send (buffer, buffer_size, packet_attr));
    case CBXI_PKT_BRCM_HEADER:
        return (cbxi_pkt_brcm_send (buffer, buffer_size, packet_attr));
    }

    LOG_ERROR(BSL_LS_FSAL_PKTIO,
        (BSL_META("PKTIO API: Invalid PKTIO header mode\n")));
    return CBX_E_PARAM;

}

/**
 * Initialize Packet attribute parameters.
 *
 * @param  packet_attr    attribute object
 */
void cbx_packet_attr_t_init (cbx_packet_attr_t *packet_attr)
{
    if (packet_attr) {
        sal_memset(packet_attr, 0, sizeof(cbx_packet_attr_t));
    }
#ifdef CONFIG_EXTERNAL_HOST
    packet_attr->source_index = CBX_PORT_ECPU;
#else
    packet_attr->source_index = CBX_PORT_ICPU;
#endif
}


/**
 * Initialize Packet_IO module
 * This routine is used to init pktio.
 *
 * @param pktio_params   (IN)  Pktio parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_pktio_init (cbx_pktio_params_t *pktio_params)
{
    int rv = CBX_E_NONE;
#if 0
    if (pktio_params.flags & CBX_PKTIO_CB_HEADER) {
        cbx_pktio_cb.header_mode = CBXI_PKT_CB_HEADER;
    } else if (pktio_params.flags & CBX_PKTIO_CB_DEBUG_HEADER) {
        cbx_pktio_cb.header_mode = CBXI_PKT_CB_DBG_HEADER;
    } else {
        cbx_pktio_cb.header_mode = CBXI_PKT_BRCM_HEADER;
    }
#endif
    cbx_pktio_cb.header_mode = CBXI_PKT_CB_HEADER;
    if (cbx_pktio_cb.callback_mutex == NULL)
        cbx_pktio_cb.callback_mutex = sal_mutex_create("CBX Callback");
    if (cbx_pktio_cb.callback_mutex == NULL) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Failed Creating Mutex\n")));
        return CBX_E_INIT;
    }
    rv = cbxi_rx_init();
    if (rv != CBX_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Failed Rx Init\n")));
        return CBX_E_INIT;
    }
    /* Setup STT */
    rv = cbxi_pktio_stt_init();
    if (rv != CBX_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Failed Stt Init\n")));
        return CBX_E_INIT;
    }
    /* Enable IO Channels */
    rv = cbxi_pktio_channel_init();
    if (rv != CBX_E_NONE) {
        LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Failed Channels Init\n")));
        return CBX_E_INIT;
    }
    tx_dma_alloc_fails = 0;
    rx_attr_alloc_fails = 0;
    rx_pv_type = 0;
    rx_lin_type = 0;
    rx_trap_type = 0;
    cbx_pktio_cb.init = 1;
    return rv;
}

/**
 * Detach a Packet_IO previously initialized by cbx_pktio_init()
 *
 * @param pktio_params   (IN)  Pktio parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized pktio.
 */
int
cbx_pktio_detach (cbx_pktio_params_t *pktio_params)
{
    int rv = CBX_E_NONE;
    if (cbx_pktio_cb.num_callbacks > 0) {
       LOG_ERROR(BSL_LS_FSAL_PKTIO,
             (BSL_META("PKTIO API: Callbacks are registered\n")));
       return CBX_E_BUSY;
    }
    rv = cbxi_rx_uninit();
    if (rv != CBX_E_NONE) {
        return rv;
    }
    sal_mutex_destroy(cbx_pktio_cb.callback_mutex);
    sal_memset(&cbx_pktio_cb, 0, sizeof(cbx_pktio_t));
    cbx_pktio_cb.init = 0;
    return rv;
}

/**
 * Packet IO receive callback register
 * This routine is used to register a Packet IO callback routine.
 * Register a handler invoked packet receiption.
 *
 * Packets which match the cb_info.flags filter are presented.
 *
 * If multiple handlers are registered, they are invoked on
 * packet receiption ordered based on cb_info.priority (lowest first
 * and clients with the same priority order is not specified.
 *
 * @param cb_info      (IN)  Callback information
 * @param cb           (IN)  Callback function pointer
 *                           see: cbx_packet_io_callback_t
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_pktio_register (cbx_packet_cb_info_t      cb_info,
                        cbx_packet_io_callback_t  cb,
                        cbx_pktio_rx_cb_handle    *handle)
{
    int idx = 0;
    int rv = 0;
    uint32 flags = cb_info.flags;

    if (!cbx_pktio_cb.init) {
        return CBX_E_INIT;
    }

    if (handle == NULL) {
        return CBX_E_PARAM;
    }

    while (flags != 0 ) {
        idx = cbxi_callback_to_idx(flags);
        if (idx < 0) {
            return CBX_E_PARAM;
        }
        cbxi_callback_clear(idx, &flags);
        sal_mutex_take(cbx_pktio_cb.callback_mutex, sal_mutex_FOREVER);
        dump_cb("Callback Registered", cb_info);
        rv = cbxi_add_callback_handler(idx, cb, cb_info, handle);
        sal_mutex_give(cbx_pktio_cb.callback_mutex);
    }

    return rv;

}

/**
 * Packet IO receive callback unregister
 * This routine is used to unregister a Packet IO callback routine.
 *
 * @param cb           (IN)  Packet IO callback routine to unregister
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_pktio_unregister (cbx_pktio_rx_cb_handle  handle)
{
    int rv = CBX_E_NONE;

    sal_mutex_take(cbx_pktio_cb.callback_mutex, sal_mutex_FOREVER);
    rv = cbxi_remove_callback_handler(handle);
    sal_mutex_give(cbx_pktio_cb.callback_mutex);

    return rv;
}

int
soc_tct_init()
{
    int rv, max;
    tct_t tct_entry;
    tct_entry.drop = 0;
    tct_entry.term = 0;
    tct_entry.trap_group = 0;
    max = soc_robo2_tct_max_index(CBX_AVENGER_PRIMARY);
    soc_robo2_tct_enable(CBX_AVENGER_PRIMARY);
    rv = soc_robo2_tct_init(CBX_AVENGER_PRIMARY, 0, max, 0, &tct_entry);
    return rv;
}

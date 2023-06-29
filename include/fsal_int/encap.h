/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     encap.h
 * * Purpose:
 * *     This file contains definitions for Egress Processing Directives.
 */

#ifndef  __ENCAP_H
#define  __ENCAP_H

#include <sal_types.h>
#include <shared/types.h>
#include <soc/robo2/common/tables.h>

#define INCLUDE_LIN_ENCAP

/*
 * EPP Encap processing
 *    Epp can process upto 6 directives and act on it
 *    NOP directive takes one cycle and should be avoided
 *    Encap record can be put in the Port Encap Table
 *    Or in Encap Record Table and DLI Encap Table
 */

#define CBXI_EGR_ENCAP_PET_ENCAP_LEN       8
#define CBXI_EGR_ENCAP_DLIET_ENCAP_LEN     8
#define CBXI_EGR_ENCAP_ERT_ENCAP_LEN      12

#define CBXI_EGR_ENCAP_RECORD_LEN_MAX      \
             (CBXI_EGR_ENCAP_DLIET_ENCAP_LEN + (2 * CBXI_EGR_ENCAP_ERT_ENCAP_LEN))

#define CBXI_EGR_NUM_ERT_PER_LIN_MAX         2
#define CBXI_EGR_NUM_DIRECTIVES_MAX          6
#define CBXI_EGR_ENCAP_EOR                0xFF
#define CBXI_ERT_IDX_INVALID              0xFF
#define CBXI_EGR_NUM_DLIET_MAX            2048
#define CBXI_EGR_NUM_PET_MAX                16
#define CBXI_EGR_NUM_ERT_MAX               255

/* Encap flags */
#define CBXI_ENCAP_AVG_PRI                 0x1
#define CBXI_ENCAP_AVG_SEC                 0x2
#define CBXI_ENCAP_AVG_SYS                 0x4 /* Encap system wide (both Primary and Secondary) */

typedef struct {
    uint8 data[CBXI_EGR_ENCAP_RECORD_LEN_MAX];
    uint8 len;
} cbxi_encap_record_t;

#ifdef INCLUDE_LIN_ENCAP
/*
 * The DLIET has 8 bytes of directives to be applied
 * Then based on traffic type (ucast vs mcast) and if EOR is not found,
 * ERT table is read and upto 24 bytes of directives can be applied.
 * In SW we allow a lin_encap to refer to either a unicast or multicast encap
 */
typedef struct {
    cbx_portid_t  portid;
    uint16        dliet_index;
    uint8         is_mcast;
    uint8         ert_index;
    uint8         n_ert_index;
    uint8         encap_len;
} cbxi_lin_encap_index_info_t;

typedef struct {
    /* index info HAS to be the first element in the lin encap structure */
    cbxi_lin_encap_index_info_t *idx_info;
    dliet_t                     dliet_entry;
    ert_t                       ert_entry[CBXI_EGR_NUM_ERT_PER_LIN_MAX];
} cbxi_lin_encap_t;
#endif

typedef struct {
    pet_t      pet_entry;
    int        encap_len;
} cbxi_port_encap_t;


/*
 * EPP Encap Directives
 */
enum {
        CBX_EGR_DIRECTIVE_NOP,
        CBX_EGR_DIRECTIVE_INSERT_4B_TAG,
        CBX_EGR_DIRECTIVE_INSERT_6B_TAG,
        CBX_EGR_DIRECTIVE_INSERT_8B_TAG,
        CBX_EGR_DIRECTIVE_CVLAN_EDIT_VID_ONLY,
        CBX_EGR_DIRECTIVE_CVLAN_EDIT_PCP_DEI_ONLY,
        CBX_EGR_DIRECTIVE_CVLAN_EDIT,
        CBX_EGR_DIRECTIVE_SVLAN_EDIT_VID_ONLY,
        CBX_EGR_DIRECTIVE_SVLAN_EDIT_PCP_DEI_ONLY,
        CBX_EGR_DIRECTIVE_SVLAN_EDIT,
        CBX_EGR_DIRECTIVE_DSCP_RE_WRITE,
        CBX_EGR_DIRECTIVE_PREPEND_MAC_HEADER,
        CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER,
        CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER_WITH_S_BIT,
        CBX_EGR_DIRECTIVE_TTL_DECREMENT,
        CBX_EGR_DIRECTIVE_UPDATE_PTP_CORRECTION_FIELD,
        CBX_EGR_DIRECTIVE_EGRESS_TIMESTAMP_CONTROL_TLV,
        CBX_EGR_DIRECTIVE_INSERT_BROADCOM_HEADER,
        CBX_EGR_DIRECTIVE_INSERT_CB_IMP_TAG,
        CBX_EGR_DIRECTIVE_INSERT_CB_CASCADE_TAG,
        CBX_EGR_DIRECTIVE_INSERT_IMP_TIMESTAMP_TAG,
        CBX_EGR_DIRECTIVE_INSERT_IPP_DIAGNOSTIC,
        CBX_EGR_DIRECTIVE_INSERT_CB_DIAGNOSTIC,
        CBX_EGR_DIRECTIVE_PREPEND_PTP_TIMESTAMP_TLV,
        CBX_EGR_DIRECTIVE_PREPEND_2B,
        CBX_EGR_DIRECTIVE_PREPEND_4B,
        CBX_EGR_DIRECTIVE_PREPEND_6B,
        CBX_EGR_DIRECTIVE_PREPEND_8B,
        CBX_EGR_DIRECTIVE_PREPEND_10B,
        CBX_EGR_DIRECTIVE_PREPEND_12B,
        CBX_EGR_DIRECTIVE_PREPEND_14B,
        CBX_EGR_DIRECTIVE_PREPEND_16B,
        CBX_EGR_DIRECTIVE_PREPEND_18B,
        CBX_EGR_DIRECTIVE_PREPEND_20B,
        CBX_EGR_DIRECTIVE_EOR,
};

int
cbxi_add_to_encap_record(int directive, cbxi_encap_record_t *encap_record, ...);


#define CBX_EPP_INSERT_NOP(record)                                  \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_NOP, record)

#define CBX_EPP_INSERT_4B_TAG(record, data)                  \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_4B_TAG, record, data)

#define CBX_EPP_INSERT_6B_TAG(record, data)                  \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_6B_TAG, record, data)

#define CBX_EPP_INSERT_8B_TAG(record, data)                  \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_8B_TAG, record, data)

#define CBX_EPP_INSERT_CVLAN_EDIT_VID_ONLY(record, data)            \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_CVLAN_EDIT_VID_ONLY, record, data)

#define CBX_EPP_INSERT_CVLAN_EDIT_PCP_DEI_ONLY(record, data)        \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_CVLAN_EDIT_PCP_DEI_ONLY, record, data)

#define CBX_EPP_INSERT_CVLAN_EDIT(record, data)                     \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_CVLAN_EDIT, record, data)

#define CBX_EPP_INSERT_SVLAN_EDIT_VID_ONLY(record, data)            \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_SVLAN_EDIT_VID_ONLY, record, data)

#define CBX_EPP_INSERT_SVLAN_EDIT_PCP_DEI_ONLY(record, data)        \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_SVLAN_EDIT_PCP_DEI_ONLY, record, data)

#define CBX_EPP_INSERT_SVLAN_EDIT(record, data)                     \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_SVLAN_EDIT, record, data)

#define CBX_EPP_INSERT_DSCP_RE_WRITE(record, data)                  \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_DSCP_RE_WRITE, record, data)

#define CBX_EPP_PREPEND_MAC_HEADER(record, data)             \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_MAC_HEADER, record, data)

#define CBX_EPP_PREPEND_MPLS_HEADER(record, data)            \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER, record, data)

#define CBX_EPP_PREPEND_MPLS_HEADER_WITH_S_BIT(record, data) \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER_WITH_S_BIT, record, data)

#define CBX_EPP_INSERT_TTL_DECREMENT(record)                  \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_TTL_DECREMENT, record)

#define CBX_EPP_INSERT_UPDATE_PTP_CORRECTION_FIELD(record)    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_UPDATE_PTP_CORRECTION_FIELD, record)

#define CBX_EPP_INSERT_EGRESS_TIMESTAMP_CONTROL_TLV(record, data)   \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_EGRESS_TIMESTAMP_CONTROL_TLV, record, data)

#define CBX_EPP_INSERT_BROADCOM_HEADER(record)         \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_BROADCOM_HEADER, record)

#define CBX_EPP_INSERT_CB_IMP_TAG(record)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_CB_IMP_TAG, record)

#define CBX_EPP_INSERT_CB_CASCADE_TAG(record)                \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_CB_CASCADE_TAG, record)

#define CBX_EPP_INSERT_IMP_TIMESTAMP_TAG(record)             \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_IMP_TIMESTAMP_TAG, record)

#define CBX_EPP_INSERT_IPP_DIAGNOSTIC(record)                \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_IPP_DIAGNOSTIC, record)

#define CBX_EPP_INSERT_CB_DIAGNOSTIC(record, data)           \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_INSERT_CB_DIAGNOSTIC, record, data)

#define CBX_EPP_PREPEND_PTP_TIMESTAMP_TLV(record, data)      \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_PTP_TIMESTAMP_TLV, record, data)

#define CBX_EPP_PREPEND_2B(record, data)                     \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_2B, record, data)

#define CBX_EPP_PREPEND_4B(record, data)                     \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_4B, record, data)

#define CBX_EPP_PREPEND_6B(record, data)                     \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_6B, record, data)

#define CBX_EPP_PREPEND_8B(record, data)                     \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_8B, record, data)

#define CBX_EPP_PREPEND_10B(record, data)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_10B, record, data)

#define CBX_EPP_PREPEND_12B(record, data)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_12B, record, data)

#define CBX_EPP_PREPEND_14B(record, data)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_14B, record, data)

#define CBX_EPP_PREPEND_16B(record, data)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_16B, record, data)

#define CBX_EPP_PREPEND_18B(record, data)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_18B, record, data)

#define CBX_EPP_PREPEND_20B(record, data)                    \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_PREPEND_20B, record, data)

#define CBX_EPP_INSERT_EOR(record)                            \
          cbxi_add_to_encap_record(CBX_EGR_DIRECTIVE_EOR, record)

extern int cbxi_encap_init(void);
extern int cbxi_port_encap_set(int unit, int port, cbxi_encap_record_t *record);

#ifdef INCLUDE_LIN_ENCAP
    extern int cbxi_lin_encap_set(cbx_portid_t portid,
                       int dliet_idx, int is_mcast, cbxi_encap_record_t *record, uint32_t flags);
extern int cbxi_lin_encap_release(int dliet_idx);
#endif

#endif /* ENCAP_H */

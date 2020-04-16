/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *    Encap.c
 * Description:
 *    EPP Encap Handling
 */

#include <sal_types.h>
#include <sal_alloc.h>
#include <bsl_log/bsl.h>
#include <fsal_int/types.h>
#include <fsal_int/encap.h>
#include <fsal_int/port.h>
#include <fsal/error.h>
#include <stdarg.h>

#ifdef INCLUDE_LIN_ENCAP
    #define GET_LIN_ENCAP_ENTRY(i)     cbxi_encaps.lin_encap[(i)]
    #define SET_LIN_ENCAP_ENTRY(i, l)  cbxi_encaps.lin_encap[(i)] = (l)
#endif
#define GET_PORT_ENCAP_ENTRY(u, i)    cbxi_encaps.port_encap[(u)][(i)]
#define SET_PORT_ENCAP_ENTRY(u, i, p) cbxi_encaps.port_encap[(u)][(i)] = (p)

typedef struct {
    uint8 opcode;
    uint8 len;
} cbxi_encap_directive_t;

typedef struct {
    cbxi_port_encap_t           *port_encap[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT];
#ifdef INCLUDE_LIN_ENCAP
    cbxi_lin_encap_index_info_t *lin_encap[CBXI_EGR_NUM_DLIET_MAX];
    int                          ert_indexes[CBXI_EGR_NUM_ERT_MAX];
#endif
} cbxi_encap_cb_t;

cbxi_encap_directive_t epp_directives[] = {
        { 0x00, 1 },       /*  CBX_EGR_DIRECTIVE_NOP                              */
        { 0x01, 4 },       /*  CBX_EGR_DIRECTIVE_INSERT_4B_TAG                    */
        { 0x02, 6 },       /*  CBX_EGR_DIRECTIVE_INSERT_6B_TAG                    */
        { 0x03, 8 },       /*  CBX_EGR_DIRECTIVE_INSERT_8B_TAG                    */
        { 0x04, 4 },       /*  CBX_EGR_DIRECTIVE_CVLAN_EDIT_VID_ONLY              */
        { 0x05, 4 },       /*  CBX_EGR_DIRECTIVE_CVLAN_EDIT_PCP_DEI_ONLY          */
        { 0x06, 4 },       /*  CBX_EGR_DIRECTIVE_CVLAN_EDIT                       */
        { 0x07, 4},        /*  CBX_EGR_DIRECTIVE_SVLAN_EDIT_VID_ONLY              */
        { 0x08, 4 },       /*  CBX_EGR_DIRECTIVE_SVLAN_EDIT_PCP_DEI_ONLY          */
        { 0x09, 4 },       /*  CBX_EGR_DIRECTIVE_SVLAN_EDIT                       */
        { 0x0A, 2 },       /*  CBX_EGR_DIRECTIVE_DSCP_RE_WRITE                    */
        { 0x0B, 10 },      /*  CBX_EGR_DIRECTIVE_PREPEND_MAC_HEADER               */
        { 0x0C, 6 },       /*  CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER              */
        { 0x0D, 6 },       /*  CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER_WITH_S_BIT   */
        { 0x0E, 1 },       /*  CBX_EGR_DIRECTIVE_TTL_DECREMENT                    */
        { 0x0F, 1 },       /*  CBX_EGR_DIRECTIVE_UPDATE_PTP_CORRECTION_FIELD      */
        { 0x10, 2 },       /*  CBX_EGR_DIRECTIVE_EGRESS_TIMESTAMP_CONTROL_TLV     */
        { 0x11, 1 },       /*  CBX_EGR_DIRECTIVE_INSERT_BROADCOM_HEADER           */
        { 0x12, 1 },       /*  CBX_EGR_DIRECTIVE_INSERT_CB_IMP_TAG                */
        { 0x13, 1 },       /*  CBX_EGR_DIRECTIVE_INSERT_CB_CASCADE_TAG            */
        { 0x14, 1 },       /*  CBX_EGR_DIRECTIVE_INSERT_IMP_TIMESTAMP_TAG         */
        { 0x15, 1 },       /*  CBX_EGR_DIRECTIVE_INSERT_IPP_DIAGNOSTIC            */
        { 0x16, 1 },       /*  CBX_EGR_DIRECTIVE_INSERT_CB_DIAGNOSTIC             */
        { 0x17, 2 },       /*  CBX_EGR_DIRECTIVE_PREPEND_PTP_TIMESTAMP_TLV        */
        { 0x21, 3 },       /*  CBX_EGR_DIRECTIVE_PREPEND_2B                       */
        { 0x22, 5 },       /*  CBX_EGR_DIRECTIVE_PREPEND_4B                       */
        { 0x23, 7 },       /*  CBX_EGR_DIRECTIVE_PREPEND_6B                       */
        { 0x24, 9 },       /*  CBX_EGR_DIRECTIVE_PREPEND_8B                       */
        { 0x25, 11 },      /*  CBX_EGR_DIRECTIVE_PREPEND_10B                      */
        { 0x26, 13 },      /*  CBX_EGR_DIRECTIVE_PREPEND_12B                      */
        { 0x27, 15 },      /*  CBX_EGR_DIRECTIVE_PREPEND_14B                      */
        { 0x28, 17 },      /*  CBX_EGR_DIRECTIVE_PREPEND_16B                      */
        { 0x29, 19 },      /*  CBX_EGR_DIRECTIVE_PREPEND_18B                      */
        { 0x2A, 21 },      /*  CBX_EGR_DIRECTIVE_PREPEND_20B                      */
        { CBXI_EGR_ENCAP_EOR, 1  },      /*  CBX_EGR_DIRECTIVE_EOR                */
};

cbxi_encap_cb_t cbxi_encaps;

#ifdef INCLUDE_LIN_ENCAP
/*
 * Function
 *    cbxi_free_ert
 * Purpose
 *    Free ERT indexes
 */
int
cbxi_free_ert(int ert_index, int number)
{
    if ((number > 2) || (number < 0)) {
        return CBX_E_PARAM;
    }
    if (ert_index >= CBXI_EGR_NUM_ERT_MAX) {
        return CBX_E_PARAM;
    }
    if (number == 2) {
        if (ert_index + 1 >= CBXI_EGR_NUM_ERT_MAX) {
            return CBX_E_PARAM;
        }
        cbxi_encaps.ert_indexes[ert_index + 1] = CBXI_ERT_IDX_INVALID;
    }
    cbxi_encaps.ert_indexes[ert_index] = CBXI_ERT_IDX_INVALID;
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_alloc_ert
 * Purpose
 *    Alloc ERT indexes, contigous if required
 */
int
cbxi_alloc_ert(int number)
{
    int i;
    if ((number > 2) || (number < 0)) {
        return -1;
    }
    for (i = 0; i < CBXI_EGR_NUM_ERT_MAX; i++) {
        if (cbxi_encaps.ert_indexes[i] == CBXI_ERT_IDX_INVALID) {
            if ((number == 1) ||
                  ((i+1 < CBXI_EGR_NUM_ERT_MAX) &&
                    (cbxi_encaps.ert_indexes[i+1] == CBXI_ERT_IDX_INVALID))) {
                cbxi_encaps.ert_indexes[i] = 0;
                if (number == 2) {
                    cbxi_encaps.ert_indexes[i+1] = 0;
                }
                return i;
            }
        }
    }
    return -1;
}
#endif

/*
 * Function
 *    cbxi_add_to_encap_record
 * Purpose
 *    Add a encap directive to encap record
 *    This routine takes variadic arguments and combines them to
 *    form directives. Even though it allows for upto maximum of
 *    CBXI_EGR_ENCAP_RECORD_LEN_MAX directives, PET encap is only 8 bytes
 *    Packing encap record to PET record will fail if len is > 8.
 *    Final call to set the EOR is mandatory, unless the record
 *    all available bytes and there is no space to add EOR
 */
int
cbxi_add_to_encap_record(int type, cbxi_encap_record_t *encap_record, ...)
{
    va_list args;
    uint8 *ptr;
    uint8 opcode;
    int   len, i;

    /* Validate we have a record to write */
    if (encap_record == NULL) {
        return CBX_E_PARAM;
    }

    va_start(args, encap_record);
    opcode = epp_directives[type].opcode;
    len = epp_directives[type].len - 1;
    if (encap_record->len + len > CBXI_EGR_ENCAP_RECORD_LEN_MAX) {
        va_end(args);
        return CBX_E_FULL;
    }

    LOG_VERBOSE(BSL_LS_FSAL_PORT,
             (BSL_META("Add Encap Type %d, Opcode %d Len %d"), type, opcode, len));

    switch (type) {
    case CBX_EGR_DIRECTIVE_INSERT_6B_TAG:
        break;
    case CBX_EGR_DIRECTIVE_INSERT_8B_TAG:
        break;
    case CBX_EGR_DIRECTIVE_PREPEND_MAC_HEADER:
        break;
    case CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER:
        break;
    case CBX_EGR_DIRECTIVE_PREPEND_MPLS_HEADER_WITH_S_BIT:
        break;
    case CBX_EGR_DIRECTIVE_NOP:
    case CBX_EGR_DIRECTIVE_TTL_DECREMENT:
    case CBX_EGR_DIRECTIVE_UPDATE_PTP_CORRECTION_FIELD:
    case CBX_EGR_DIRECTIVE_INSERT_BROADCOM_HEADER:
    case CBX_EGR_DIRECTIVE_INSERT_CB_IMP_TAG:
    case CBX_EGR_DIRECTIVE_INSERT_CB_CASCADE_TAG:
    case CBX_EGR_DIRECTIVE_INSERT_IMP_TIMESTAMP_TAG:
    case CBX_EGR_DIRECTIVE_INSERT_IPP_DIAGNOSTIC:
    case CBX_EGR_DIRECTIVE_INSERT_CB_DIAGNOSTIC:
    case CBX_EGR_DIRECTIVE_EOR:
        encap_record->data[encap_record->len++] = opcode;
        break;
    case CBX_EGR_DIRECTIVE_EGRESS_TIMESTAMP_CONTROL_TLV:
    case CBX_EGR_DIRECTIVE_PREPEND_PTP_TIMESTAMP_TLV:
    case CBX_EGR_DIRECTIVE_DSCP_RE_WRITE:
        encap_record->data[encap_record->len++] = opcode;
        i = va_arg(args, int);
        encap_record->data[encap_record->len++] = (uint8)i;
        break;
    case CBX_EGR_DIRECTIVE_INSERT_4B_TAG:
    case CBX_EGR_DIRECTIVE_CVLAN_EDIT_VID_ONLY:
    case CBX_EGR_DIRECTIVE_CVLAN_EDIT_PCP_DEI_ONLY:
    case CBX_EGR_DIRECTIVE_CVLAN_EDIT:
    case CBX_EGR_DIRECTIVE_SVLAN_EDIT_VID_ONLY:
    case CBX_EGR_DIRECTIVE_SVLAN_EDIT_PCP_DEI_ONLY:
    case CBX_EGR_DIRECTIVE_SVLAN_EDIT:
    case CBX_EGR_DIRECTIVE_PREPEND_2B:
    case CBX_EGR_DIRECTIVE_PREPEND_4B:
    case CBX_EGR_DIRECTIVE_PREPEND_6B:
    case CBX_EGR_DIRECTIVE_PREPEND_8B:
    case CBX_EGR_DIRECTIVE_PREPEND_10B:
    case CBX_EGR_DIRECTIVE_PREPEND_12B:
    case CBX_EGR_DIRECTIVE_PREPEND_14B:
    case CBX_EGR_DIRECTIVE_PREPEND_16B:
    case CBX_EGR_DIRECTIVE_PREPEND_18B:
    case CBX_EGR_DIRECTIVE_PREPEND_20B:
        encap_record->data[encap_record->len++] = opcode;
        ptr = va_arg(args, uint8*);
        for (i = 0; i < len; i++) {
            encap_record->data[encap_record->len++] = ptr[i];
        }
        break;
    default:
        SAL_ASSERT(0);
    }

    va_end(args);
    return CBX_E_NONE;
}

void
cbxi_dump_encap_record(cbxi_encap_record_t *record)
{
    int i;
    sal_printf("EncapRecord: ");
    for (i = 0; i < record->len; i++) {
        sal_printf("%02x ", record->data[i]);
    }
    sal_printf("\n");
}


/*
 * Function
 *    cbxi_port_encap_pack
 * Purpose
 *    Convert encap record to pet format
 */
int
cbxi_port_encap_pack(cbxi_port_encap_t *portencap, cbxi_encap_record_t *record)
{
    if ((!record) || (!portencap)) {
        return CBX_E_PARAM;
    }
    if (record->len > 8) {
        return CBX_E_FAIL;
    }
    if ((record->len < 8) && (record->data[record->len-1] != CBXI_EGR_ENCAP_EOR)) {
        return CBX_E_PARAM;
    }
    if (record->len == 8) {
        portencap->pet_entry.full = 1;
    }
    portencap->pet_entry.encap0 = record->data[0];
    portencap->pet_entry.encap1 = record->data[1];
    portencap->pet_entry.encap2 = record->data[2];
    portencap->pet_entry.encap3 = record->data[3];
    portencap->pet_entry.encap4 = record->data[4];
    portencap->pet_entry.encap5 = record->data[5];
    portencap->pet_entry.encap6 = record->data[6];
    portencap->pet_entry.encap7 = record->data[7];
    portencap->encap_len = record->len;
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_port_encap_unpack
 * Purpose
 *    Convert pet to encap record
 */
int
cbxi_port_encap_unpack(cbxi_port_encap_t *portencap, cbxi_encap_record_t *record)
{
    if ((!record) || (!portencap)) {
        return CBX_E_PARAM;
    }
    sal_memset(&record, 0, sizeof(cbxi_encap_record_t));
    record->data[0] = portencap->pet_entry.encap0;
    record->data[1] = portencap->pet_entry.encap1;
    record->data[2] = portencap->pet_entry.encap2;
    record->data[3] = portencap->pet_entry.encap3;
    record->data[4] = portencap->pet_entry.encap4;
    record->data[5] = portencap->pet_entry.encap5;
    record->data[6] = portencap->pet_entry.encap6;
    record->data[7] = portencap->pet_entry.encap7;
    record->len     = portencap->encap_len;
    return CBX_E_NONE;
}

#ifdef INCLUDE_LIN_ENCAP
/*
 * Function
 *    cbxi_lin_encap_pack
 * Purpose
 *    Convert encap_record to lin encap entry
 */
int
cbxi_lin_encap_pack(cbxi_lin_encap_t *linencap,
                    cbxi_encap_record_t *record)
{
    int i = 0;
    int use_ert = 0;

    if ((!linencap) || (!linencap->idx_info) || (!record)) {
        return CBX_E_PARAM;
    }
    if ((record->len < 32) &&
           (record->data[record->len-1] != CBXI_EGR_ENCAP_EOR)) {
        return CBX_E_PARAM;
    }

    sal_memset(&linencap->dliet_entry, 0, sizeof(dliet_t));
    sal_memset(&linencap->ert_entry, 0, CBXI_EGR_NUM_ERT_PER_LIN_MAX*sizeof(ert_t));
    linencap->dliet_entry.full = 0;
    linencap->idx_info->ert_index = CBXI_ERT_IDX_INVALID;
    if (record->len > 8) {
        linencap->dliet_entry.full = 1;
        if (record->data[8] != CBXI_EGR_ENCAP_EOR) {
            use_ert = 1;
        }
    }
    linencap->dliet_entry.encap0 = record->data[i++];
    linencap->dliet_entry.encap1 = record->data[i++];
    linencap->dliet_entry.encap2 = record->data[i++];
    linencap->dliet_entry.encap3 = record->data[i++];
    linencap->dliet_entry.encap4 = record->data[i++];
    linencap->dliet_entry.encap5 = record->data[i++];
    linencap->dliet_entry.encap6 = record->data[i++];
    linencap->dliet_entry.encap7 = record->data[i++];

    if (use_ert) {
        linencap->ert_entry[0].encap0 = record->data[i++];
        linencap->ert_entry[0].encap1 = record->data[i++];
        linencap->ert_entry[0].encap2 = record->data[i++];
        linencap->ert_entry[0].encap3 = record->data[i++];
        linencap->ert_entry[0].encap4 = record->data[i++];
        linencap->ert_entry[0].encap5 = record->data[i++];
        linencap->ert_entry[0].encap6 = record->data[i++];
        linencap->ert_entry[0].encap7 = record->data[i++];
        linencap->ert_entry[0].encap8 = record->data[i++];
        linencap->ert_entry[0].encap9 = record->data[i++];
        linencap->ert_entry[0].encap10 = record->data[i++];
        linencap->ert_entry[0].encap11 = record->data[i++];

        if (record->len > 20) {
            linencap->ert_entry[0].full = 1;
            linencap->ert_entry[1].encap0 = record->data[i++];
            linencap->ert_entry[1].encap1 = record->data[i++];
            linencap->ert_entry[1].encap2 = record->data[i++];
            linencap->ert_entry[1].encap3 = record->data[i++];
            linencap->ert_entry[1].encap4 = record->data[i++];
            linencap->ert_entry[1].encap5 = record->data[i++];
            linencap->ert_entry[1].encap6 = record->data[i++];
            linencap->ert_entry[1].encap7 = record->data[i++];
            linencap->ert_entry[1].encap8 = record->data[i++];
            linencap->ert_entry[1].encap9 = record->data[i++];
            linencap->ert_entry[1].encap10 = record->data[i++];
            linencap->ert_entry[1].encap11 = record->data[i++];
            if (record->len > CBXI_EGR_ENCAP_RECORD_LEN_MAX) {
                linencap->ert_entry[1].full = 1;
            }
        }
    }
    linencap->idx_info->encap_len = record->len;
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_lin_encap_unpack
 * Purpose
 *    Convert lin encap entry to encap_record
 */
int
cbxi_lin_encap_unpack(cbxi_lin_encap_t *linencap, cbxi_encap_record_t *record)
{
    int i = 0;
    if ((!record) || (!linencap) || (!linencap->idx_info)) {
        return CBX_E_PARAM;
    }

    sal_memset(&record, 0, sizeof(cbxi_encap_record_t));
    record->data[i++] = linencap->dliet_entry.encap0;
    record->data[i++] = linencap->dliet_entry.encap1;
    record->data[i++] = linencap->dliet_entry.encap2;
    record->data[i++] = linencap->dliet_entry.encap3;
    record->data[i++] = linencap->dliet_entry.encap4;
    record->data[i++] = linencap->dliet_entry.encap5;
    record->data[i++] = linencap->dliet_entry.encap6;
    record->data[i++] = linencap->dliet_entry.encap7;

    if (linencap->dliet_entry.full) {
        record->data[i++] = linencap->ert_entry[0].encap0;
        record->data[i++] = linencap->ert_entry[0].encap1;
        record->data[i++] = linencap->ert_entry[0].encap2;
        record->data[i++] = linencap->ert_entry[0].encap3;
        record->data[i++] = linencap->ert_entry[0].encap4;
        record->data[i++] = linencap->ert_entry[0].encap5;
        record->data[i++] = linencap->ert_entry[0].encap6;
        record->data[i++] = linencap->ert_entry[0].encap7;
        record->data[i++] = linencap->ert_entry[0].encap8;
        record->data[i++] = linencap->ert_entry[0].encap9;
        record->data[i++] = linencap->ert_entry[0].encap10;
        record->data[i++] = linencap->ert_entry[0].encap11;

        if (linencap->ert_entry[0].full) {
            record->data[i++] = linencap->ert_entry[1].encap0;
            record->data[i++] = linencap->ert_entry[1].encap1;
            record->data[i++] = linencap->ert_entry[1].encap2;
            record->data[i++] = linencap->ert_entry[1].encap3;
            record->data[i++] = linencap->ert_entry[1].encap4;
            record->data[i++] = linencap->ert_entry[1].encap5;
            record->data[i++] = linencap->ert_entry[1].encap6;
            record->data[i++] = linencap->ert_entry[1].encap7;
            record->data[i++] = linencap->ert_entry[1].encap8;
            record->data[i++] = linencap->ert_entry[1].encap9;
            record->data[i++] = linencap->ert_entry[1].encap10;
            record->data[i++] = linencap->ert_entry[1].encap11;
            if (linencap->ert_entry[1].full) {
                record->len = CBXI_EGR_ENCAP_RECORD_LEN_MAX;
                return CBX_E_NONE;
            }
        }
    }
    record->len = linencap->idx_info->encap_len;
    return CBX_E_FAIL;
}

/*
 * Function
 *    cbxi_lin_encap_clear
 * Purpose
 *    Clear the lin encap record
 */
int
cbxi_lin_encap_clear(int dliet_idx)
{
    dliet_t dliet_entry;
    uint32 sts = 0;

    /* Not bothered to clear ERT,
     * since references to it from
     * dliet is anyway removed
     */
    sal_memset(&dliet_entry, 0, sizeof(dliet_t));
    CBX_IF_ERROR_RETURN(
        soc_robo2_dliet_set(CBX_AVENGER_PRIMARY, dliet_idx, &dliet_entry, &sts));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(
            soc_robo2_dliet_set(CBX_AVENGER_SECONDARY, dliet_idx, &dliet_entry, &sts));
    }
    return CBX_E_NONE;
}


#ifdef INCLUDE_LIN_ENCAP
int
cbxi_lin_encap_idx_get(int dliet_idx)
{
    int i;
    cbxi_lin_encap_index_info_t *linencap_idxinfo;

    for (i = 0; i < CBXI_EGR_NUM_DLIET_MAX; i++) {
        linencap_idxinfo = GET_LIN_ENCAP_ENTRY(i);
        if ((linencap_idxinfo != NULL) &&(linencap_idxinfo->dliet_index == dliet_idx)) {
            return i;
        }
    }
    return -1;
}

#endif
/*
 * Function
 *    cbxi_lin_encap_release
 * Purpose
 *    Clear the lin encap record and release related memories
 */
int
cbxi_lin_encap_release(int dliet_idx)
{
#ifdef INCLUDE_LIN_ENCAP
    cbxi_lin_encap_index_info_t *linencap_idxinfo;
    int idx = -1;
#endif


    cbxi_lin_encap_clear(dliet_idx);

#ifdef INCLUDE_LIN_ENCAP
    idx = cbxi_lin_encap_idx_get(dliet_idx);
    if (idx < 0) {
        return CBX_E_NONE;
    }

    linencap_idxinfo = GET_LIN_ENCAP_ENTRY(idx);
    cbxi_free_ert(linencap_idxinfo->ert_index, linencap_idxinfo->n_ert_index);
    sal_free(linencap_idxinfo);
    SET_LIN_ENCAP_ENTRY(idx, NULL);
#endif

    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_lin_encap_set
 * Purpose
 *    Set the lin encap record
 */
int
cbxi_lin_encap_set(cbx_portid_t portid,
                   int dliet_idx, int is_mcast,
                   cbxi_encap_record_t *record, uint32_t flags)
{
    int need_ert = 0;
    int ert_idx = 0;
    cbxi_lin_encap_t linencap;
    cbxi_lin_encap_index_info_t *linencap_idxinfo;
    uint32 sts = 0;

    if (!record) {
        return CBX_E_PARAM;
    }
    if ((record->len < CBXI_EGR_ENCAP_RECORD_LEN_MAX) &&
        (record->data[record->len-1] != CBXI_EGR_ENCAP_EOR)) {
        return CBX_E_PARAM;
    }
    if ((record->len > 8) && (record->data[8] != CBXI_EGR_ENCAP_EOR)) {
        need_ert++;
    }
    if (record->len > 20)  {
        need_ert++;
    }

    /* Overwrite any existing entry */
    linencap_idxinfo = GET_LIN_ENCAP_ENTRY(dliet_idx);
    if (linencap_idxinfo == NULL) {
        linencap_idxinfo = sal_alloc(sizeof(cbxi_lin_encap_index_info_t), "EPP Lin Encap");
        if (linencap_idxinfo == NULL) {
            return CBX_E_MEMORY;
        }
    }

    sal_memset(&linencap, 0, sizeof(cbxi_lin_encap_t));
    linencap.idx_info = linencap_idxinfo;
    CBX_IF_ERROR_RETURN(cbxi_lin_encap_pack(&linencap, record));

    if (need_ert > 0) {
        ert_idx = cbxi_alloc_ert(need_ert);
        if (ert_idx < 0) {
            return CBX_E_MEMORY;
        }

        if ((flags & CBXI_ENCAP_AVG_PRI) || (flags & CBXI_ENCAP_AVG_SYS)) {
            CBX_IF_ERROR_RETURN(soc_robo2_ert_set(CBX_AVENGER_PRIMARY,
                        ert_idx, &linencap.ert_entry[0], &sts));
            if (need_ert > 1) {
                CBX_IF_ERROR_RETURN(soc_robo2_ert_set(CBX_AVENGER_PRIMARY,
                                ert_idx + 1, &linencap.ert_entry[1], &sts));
            }
        }
        if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
            ((flags & CBXI_ENCAP_AVG_SEC) || (flags & CBXI_ENCAP_AVG_SYS))) {
            CBX_IF_ERROR_RETURN(soc_robo2_ert_set(CBX_AVENGER_SECONDARY,
                                    ert_idx, &linencap.ert_entry[0], &sts));
            if (need_ert > 1) {
                CBX_IF_ERROR_RETURN(soc_robo2_ert_set(CBX_AVENGER_SECONDARY,
                                    ert_idx + 1, &linencap.ert_entry[1], &sts));
            }
        }

        if (is_mcast == 0) {
            /* Configure only UC encap_ptr to ert_idx */
            linencap.dliet_entry.uc_encap_ptr = ert_idx;
            linencap.dliet_entry.mc_encap_ptr = 0;
        } else if (is_mcast == 1) {
            /* Configure only MC encap_ptr to ert_idx */
            linencap.dliet_entry.uc_encap_ptr = 0;
            linencap.dliet_entry.mc_encap_ptr = ert_idx;
        } else {
            /* Configure both UC and MC encap_ptr to ert_idx */
            linencap.dliet_entry.uc_encap_ptr = ert_idx;
            linencap.dliet_entry.mc_encap_ptr = ert_idx;
        }
    }
    if ((flags & CBXI_ENCAP_AVG_PRI) || (flags & CBXI_ENCAP_AVG_SYS)) {
        CBX_IF_ERROR_RETURN(soc_robo2_dliet_set(CBX_AVENGER_PRIMARY,
                                    dliet_idx, &linencap.dliet_entry, &sts));
    }
    if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
            ((flags & CBXI_ENCAP_AVG_SEC) || (flags & CBXI_ENCAP_AVG_SYS))) {
        CBX_IF_ERROR_RETURN(
            soc_robo2_dliet_set(CBX_AVENGER_SECONDARY, dliet_idx, &linencap.dliet_entry, &sts));
    }

    linencap.idx_info->portid = portid;
    linencap.idx_info->is_mcast = is_mcast;
    linencap.idx_info->dliet_index = dliet_idx;
    linencap.idx_info->ert_index = ert_idx;
    linencap.idx_info->n_ert_index = need_ert;

    SET_LIN_ENCAP_ENTRY(dliet_idx, linencap_idxinfo);
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_populate_lin_encap_table_entries
 * Purpose
 *    Retrieve the dliet and ert entries and populate it into lin_encap_t structure
 */
int cbxi_populate_lin_encap_table_entries(
        cbxi_lin_encap_t *linencap,
        cbxi_lin_encap_index_info_t *linencap_idxinfo
    )
{
    linencap->idx_info = linencap_idxinfo;

    if (linencap_idxinfo->n_ert_index > 0) {
        CBX_IF_ERROR_RETURN(
            soc_robo2_ert_get(CBX_AVENGER_PRIMARY, linencap_idxinfo->ert_index, &linencap->ert_entry[0]));
        if (linencap_idxinfo->n_ert_index > 1) {
            CBX_IF_ERROR_RETURN(
                soc_robo2_ert_get(CBX_AVENGER_PRIMARY, linencap_idxinfo->ert_index + 1, &linencap->ert_entry[1]));
        }
    }
    CBX_IF_ERROR_RETURN(
        soc_robo2_dliet_get(CBX_AVENGER_PRIMARY, linencap_idxinfo->dliet_index, &linencap->dliet_entry));

    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_lin_encap_get
 * Purpose
 *    Get the lin encap record
 */
int
cbxi_lin_encap_get(cbx_portid_t port,
                   int *dliet_index, int *is_mcast,
                   cbxi_encap_record_t *record)
{
    cbxi_lin_encap_t linencap;
    cbxi_lin_encap_index_info_t *linencap_idxinfo;
    int found = FALSE;
    int i = 0;

    if (!record) {
        return CBX_E_PARAM;
    }
    if (*dliet_index < 0) {
        for (i = 0; i < CBXI_EGR_NUM_DLIET_MAX; i++) {
            linencap_idxinfo = GET_LIN_ENCAP_ENTRY(i);
            if (linencap_idxinfo->portid == port) {
                found = TRUE;
                break;
            }
        }
    } else {
        if (*dliet_index < CBXI_EGR_NUM_DLIET_MAX) {
            linencap_idxinfo = GET_LIN_ENCAP_ENTRY(*dliet_index);
            found = TRUE;
        } else {
            return CBX_E_PARAM;
        }
    }
    if (found) {
        if (is_mcast) *is_mcast = linencap_idxinfo->is_mcast;
        CBX_IF_ERROR_RETURN(
            cbxi_populate_lin_encap_table_entries(&linencap, linencap_idxinfo));
        CBX_IF_ERROR_RETURN(
            cbxi_lin_encap_unpack(&linencap, record));
        return CBX_E_NONE;
    }
    return CBX_E_NOT_FOUND;
}
#endif /* INCLUDE_LIN_ENCAP */

/*
 * Function
 *    cbxi_port_encap_set
 * Purpose
 *    Set the port encap record
 */
int
cbxi_port_encap_set(int unit, int port, cbxi_encap_record_t *record)
{
    uint32 sts = 0;
    cbxi_port_encap_t *portencap;

    if (!record) {
        return CBX_E_PARAM;
    }
    if ((record->len < CBXI_EGR_ENCAP_PET_ENCAP_LEN) &&
        (record->data[record->len-1] != CBXI_EGR_ENCAP_EOR)) {
        return CBX_E_PARAM;
    }
    /* Overwrite any existing entry */
    portencap = GET_PORT_ENCAP_ENTRY(unit, port);
    if (portencap == NULL) {
        portencap = sal_alloc(sizeof(cbxi_port_encap_t), "EPP Port Encap");
        if (portencap == NULL) {
            return CBX_E_MEMORY;
        }
    }

    /* Coverity: Move the pointer assignment before encap pack as portencap
     * pointer will be lost and ther will be no means to do sal_free in
     * cbxi_port_encap_clear if any of the following 2 function call fails */
    SET_PORT_ENCAP_ENTRY(unit, port, portencap);
    CBX_IF_ERROR_RETURN(cbxi_port_encap_pack(portencap, record));
    CBX_IF_ERROR_RETURN(soc_robo2_pet_set(unit, port, &portencap->pet_entry, &sts));
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_port_encap_get
 * Purpose
 *    Get the port encap record
 */
int
cbxi_port_encap_get(int unit, int port, cbxi_encap_record_t *record)
{
    cbxi_port_encap_t *portencap;

    if (!record) {
        return CBX_E_PARAM;
    }
    portencap = GET_PORT_ENCAP_ENTRY(unit, port);
    if (portencap == NULL) {
        return CBX_E_NOT_FOUND;
    }
    CBX_IF_ERROR_RETURN(cbxi_port_encap_unpack(portencap, record));
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_port_encap_clear
 * Purpose
 *    Clear the port encap record
 */
int
cbxi_port_encap_clear(int unit, int port)
{
    cbxi_port_encap_t *portencap;
    pet_t pet_entry;
    uint32 sts = 0;

    sal_memset(&pet_entry, 0, sizeof(pet_t));
    CBX_IF_ERROR_RETURN(
        soc_robo2_pet_set(unit, port, &pet_entry, &sts));

    portencap = GET_PORT_ENCAP_ENTRY(unit, port);
    if (portencap != NULL) {
        sal_free(portencap);
    }
    SET_PORT_ENCAP_ENTRY(unit, port, NULL);

    return CBX_E_NONE;
}
/*
 * Function
 *    cbxi_encap_init
 * Purpose
 *    Initialize the encap module
 */
int
cbxi_encap_init(void)
{
#ifdef INCLUDE_LIN_ENCAP
    int i;
#endif
    sal_memset(&cbxi_encaps, 0, sizeof(cbxi_encap_cb_t));
#ifdef INCLUDE_LIN_ENCAP
    for (i = 0; i < CBXI_EGR_NUM_ERT_MAX; i++) {
        cbxi_encaps.ert_indexes[i] = CBXI_ERT_IDX_INVALID;
    }
#endif
    return CBX_E_NONE;
}

/*
 * Function
 *    cbxi_encap_uninit
 * Purpose
 *    Cleanup the encap module
 */
int
cbxi_encap_uninit(void)
{
#ifdef INCLUDE_LIN_ENCAP
    cbxi_lin_encap_index_info_t *linencap_idxinfo;
#endif
    cbxi_port_encap_t *portencap;
    int i, j;

#ifdef INCLUDE_LIN_ENCAP
    for (i = 0; i < CBXI_EGR_NUM_DLIET_MAX; i++) {
        linencap_idxinfo = GET_LIN_ENCAP_ENTRY(i);
        if (linencap_idxinfo != NULL) {
            cbxi_free_ert(linencap_idxinfo->ert_index, linencap_idxinfo->n_ert_index);
            cbxi_lin_encap_clear(linencap_idxinfo->dliet_index);
            sal_free(linencap_idxinfo);
            SET_LIN_ENCAP_ENTRY(i, NULL);
        }
    }
#endif
    for (i = 0; i < CBX_MAX_UNITS; i++) {
        for (j = 0; j < CBX_MAX_PORT_PER_UNIT; j++) {
            portencap = GET_PORT_ENCAP_ENTRY(i, j);
            if (portencap != NULL) {
                cbxi_port_encap_clear(i, j);
                sal_free(portencap);
                SET_PORT_ENCAP_ENTRY(i, j, NULL);
            }
        }
    }
    return CBX_E_NONE;
}

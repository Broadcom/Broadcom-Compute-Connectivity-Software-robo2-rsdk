/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/* *
 * * File:
 * *     imp.h
 * * Purpose:
 * *     This file contains definitions for BCM53158 IMP Header Defs.
 */

#ifndef __IMP_H
#define __IMP_H


/* Forwarding based on CBXI Header */
#define CBXI_FWD_OPCODE_UCAST_DIRECTED      0
#define CBXI_FWD_OPCODE_MCAST_DIRECTED      1
#define CBXI_FWD_OPCODE_EXCEPTION           2 /* Not Applicable for CP -> Switch */




/* Forwarding based on BRCM Header */
#define BRCM_FWD_OPCODE_SWITCH_DIRECTED 0   /* Switch to forward */
#define BRCM_FWD_OPCODE_CP_DIRECTED     1   /* Control plane directed */

#define BRCM_FWD_TE_UCAST_DPP_DIRECTED    0
#define BRCM_FWD_TE_UCAST_DLI_DIRECTED    1
#define BRCM_FWD_TE_MCAST_DLI_DIRECTED    2

/*
 * Header Types
 */
typedef enum {
    imp_cb_ing,
    imp_cb_egr,
    imp_br_ing,
    imp_br_egr,
    imp_cb_ts,
} imp_header_type_t;


/*
 * BRCM Classic Egress IMP Header
 * Egress Header (Switch to CP)
 */
typedef struct {
    uint32 sport;
    uint32 tc;
    uint32 fwdop;
    uint32 exception;
    uint32 opcode;
} brcm_egress_imp_header_t;

#define CBXI_BRCM_EGR_SPORT_POS        0
#define CBXI_BRCM_EGR_SPORT_MSK        0xF
#define CBXI_BRCM_EGR_TC_POS           6
#define CBXI_BRCM_EGR_TC_MSK           0x7
#define CBXI_BRCM_EGR_FWDOP_POS        8
#define CBXI_BRCM_EGR_FWDOP_MSK        0x7
#define CBXI_BRCM_EGR_EXCEPTION_POS    16
#define CBXI_BRCM_EGR_EXCEPTION_MSK    0x1FFF
#define CBXI_BRCM_EGR_OPCODE_POS       29
#define CBXI_BRCM_EGR_OPCODE_MSK       0x7

/*
 * BRCM Classic Ingress IMP Header
 * Ingress Header (CP to Switch)
 */
typedef struct {
    uint32 dlli;
    uint32 timestamp;
    uint32 mode;
    uint32 tc;
    uint32 opcode;
} brcm_ingress_imp_header_t;

#define CBXI_BRCM_ING_DLLI_POS           0x0
#define CBXI_BRCM_ING_DLLI_MSK           0x3FFFF
#define CBXI_BRCM_ING_TIMESTAMP_POS      23
#define CBXI_BRCM_ING_TIMESTAMP_MSK      0x1
#define CBXI_BRCM_ING_MODE_POS           24
#define CBXI_BRCM_ING_MODE_MSK           0x3
#define CBXI_BRCM_ING_TC_POS             26
#define CBXI_BRCM_ING_TC_MSK             0x7
#define CBXI_BRCM_ING_OPCODE_POS         29
#define CBXI_BRCM_ING_OPCODE_MSK         0x7

/*
 * CBX Packet IMP Header
 */
typedef struct {
    uint32 ethertype;
    uint32 tc;
    uint32 dp;
    uint32 fwdop;
    uint32 unit;
    uint32 trapgrp_lbh_dpp;
    uint32 dest;
    uint32 spg;
    uint32 t;
    uint32 vsi;
} cbxi_imp_header_t;

#define CBXI_IMP_ETHERTYPE_POS       16
#define CBXI_IMP_ETHERTYPE_MSK       0xFFFF
#define CBXI_IMP_TC_POS              13
#define CBXI_IMP_TC_MSK              0x7
#define CBXI_IMP_DP_POS              11
#define CBXI_IMP_DP_MSK              0x3
#define CBXI_IMP_FWDOP_POS           8
#define CBXI_IMP_FWDOP_MSK           0x7
#define CBXI_IMP_UNIT_POS            7
#define CBXI_IMP_UNIT_MSK            0x1
#define CBXI_IMP_TGLHDPP_POS         0
#define CBXI_IMP_TGLHDPP_MSK         0x3F
#define CBXI_IMP_DEST_POS            20
#define CBXI_IMP_DEST_MSK            0xFFF
#define CBXI_IMP_SPG_POS             13
#define CBXI_IMP_SPG_MSK             0x1F
#define CBXI_IMP_T_POS               12
#define CBXI_IMP_T_MSK               0x1
#define CBXI_IMP_VSI_POS             0
#define CBXI_IMP_VSI_MSK             0xFFF

#define CBXI_IMP_PACK_VSI(h)    ((((h)->vsi) & CBXI_IMP_VSI_MSK) << CBXI_IMP_VSI_POS)
#define CBXI_IMP_PACK_T(h)      ((((h)->t) & CBXI_IMP_T_MSK) << CBXI_IMP_T_POS)
#define CBXI_IMP_PACK_SPG(h)    ((((h)->spg) & CBXI_IMP_SPG_MSK) << CBXI_IMP_SPG_POS)
#define CBXI_IMP_PACK_DEST(h)   ((((h)->dest) & CBXI_IMP_DEST_MSK) << CBXI_IMP_DEST_POS)
#define CBXI_IMP_PACK_UNIT(h)   ((((h)->unit) & CBXI_IMP_UNIT_MSK) << CBXI_IMP_UNIT_POS)
#define CBXI_IMP_PACK_FWDOP(h)  ((((h)->fwdop) & CBXI_IMP_FWDOP_MSK) << CBXI_IMP_FWDOP_POS)
#define CBXI_IMP_PACK_DP(h)     ((((h)->dp) & CBXI_IMP_DP_MSK) << CBXI_IMP_DP_POS)
#define CBXI_IMP_PACK_TC(h)     ((((h)->tc) & CBXI_IMP_TC_MSK) << CBXI_IMP_TC_POS)
#define CBXI_IMP_PACK_TGLHDPP(h)   ((((h)->trapgrp_lbh_dpp) & CBXI_IMP_TGLHDPP_MSK) \
                                                     << CBXI_IMP_TGLHDPP_POS)
#define CBXI_IMP_PACK_ETHERTYPE(h)   ((((h)->ethertype) & CBXI_IMP_ETHERTYPE_MSK) \
                                                     << CBXI_IMP_ETHERTYPE_POS)

#define CBXI_IMP_UNPACK_VSI(w, h)   ((h)->vsi = ((w >> CBXI_IMP_VSI_POS) & CBXI_IMP_VSI_MSK))
#define CBXI_IMP_UNPACK_T(w, h)     ((h)->t = ((w >> CBXI_IMP_T_POS) & CBXI_IMP_T_MSK))
#define CBXI_IMP_UNPACK_SPG(w, h)   ((h)->spg = ((w >> CBXI_IMP_SPG_POS) & CBXI_IMP_SPG_MSK))
#define CBXI_IMP_UNPACK_DEST(w, h)  ((h)->dest = ((w >> CBXI_IMP_DEST_POS) & CBXI_IMP_DEST_MSK))
#define CBXI_IMP_UNPACK_UNIT(w, h)  ((h)->unit = ((w >> CBXI_IMP_UNIT_POS) & CBXI_IMP_UNIT_MSK))
#define CBXI_IMP_UNPACK_FWDOP(w, h) ((h)->fwdop = ((w >> CBXI_IMP_FWDOP_POS) & CBXI_IMP_FWDOP_MSK))
#define CBXI_IMP_UNPACK_DP(w, h)    ((h)->dp = ((w >> CBXI_IMP_DP_POS) & CBXI_IMP_DP_MSK))
#define CBXI_IMP_UNPACK_TC(w, h)    ((h)->tc = ((w >> CBXI_IMP_TC_POS) & CBXI_IMP_TC_MSK))
#define CBXI_IMP_UNPACK_TGLHDPP(w, h)   ((h)->trapgrp_lbh_dpp = \
                                          ((w >> CBXI_IMP_TGLHDPP_POS) & CBXI_IMP_TGLHDPP_MSK))
#define CBXI_IMP_UNPACK_ETHERTYPE(w, h) ((h)->ethertype = \
                                          ((w >> CBXI_IMP_ETHERTYPE_POS) & CBXI_IMP_ETHERTYPE_MSK))

#define CBXI_IMP_PACK_W0(cbh)                                     \
           CBXI_IMP_PACK_ETHERTYPE(cbh) | CBXI_IMP_PACK_TC(cbh) | \
           CBXI_IMP_PACK_DP(cbh) | CBXI_IMP_PACK_FWDOP(cbh)     | \
           CBXI_IMP_PACK_UNIT(cbh) | CBXI_IMP_PACK_TGLHDPP(cbh)

#define CBXI_IMP_PACK_W1(cbh)                                \
           CBXI_IMP_PACK_DEST(cbh) | CBXI_IMP_PACK_T(cbh)  | \
           CBXI_IMP_PACK_VSI(cbh)  | CBXI_IMP_PACK_SPG(cbh)

#define CBXI_IMP_UNPACK_W0(w0, cbh)                                         \
           CBXI_IMP_UNPACK_ETHERTYPE(w0, cbh); CBXI_IMP_UNPACK_TC(w0, cbh); \
           CBXI_IMP_UNPACK_DP(w0, cbh); CBXI_IMP_UNPACK_FWDOP(w0, cbh);     \
           CBXI_IMP_UNPACK_UNIT(w0, cbh); CBXI_IMP_UNPACK_TGLHDPP(w0, cbh)

#define CBXI_IMP_UNPACK_W1(w1, cbh)                                   \
           CBXI_IMP_UNPACK_DEST(w1, cbh); CBXI_IMP_UNPACK_T(w1, cbh); \
           CBXI_IMP_UNPACK_VSI(w1, cbh);  CBXI_IMP_UNPACK_SPG(w1, cbh)


/*
 * CBX Packet Ingress IMP Header
 * Ingress Header (CP to Switch)
 */
typedef cbxi_imp_header_t cbxi_ingress_imp_header_t;

/*
 * CBX Packet Egress IMP Header
 * Egress Header (Switch to CP)
 */
typedef cbxi_imp_header_t cbxi_egress_imp_header_t;

/*
 * Timestamp header
 */
typedef struct {
    uint32 ethertype;
    uint32 timestamp_h;
    uint32 timestamp_l;
} cbxi_timestamp_header_t;

#define CBXI_IMP_TS_ETHERTYPE_POS       16
#define CBXI_IMP_TS_ETHERTYPE_MSK       0xFFFF
#define CBXI_IMP_TS_TIMESTAMP_H_POS     0
#define CBXI_IMP_TS_TIMESTAMP_H_MSK     0xFFFF
#define CBXI_IMP_TS_TIMESTAMP_L_POS     0
#define CBXI_IMP_TS_TIMESTAMP_L_MSK     0xFFFFFFFF

#define CBXI_IMP_TS_UNPACK_ETHERTYPE(w, h)    \
               ((h)->ethertype = ((w >> CBXI_IMP_TS_ETHERTYPE_POS) & CBXI_IMP_ETHERTYPE_MSK))
#define CBXI_IMP_TS_UNPACK_TIMESTAMP_H(w, h)  \
               ((h)->timestamp_h = ((w >> CBXI_IMP_TS_TIMESTAMP_H_POS) & CBXI_IMP_TS_TIMESTAMP_H_MSK))
#define CBXI_IMP_TS_UNPACK_TIMESTAMP_L(w, h)  \
               ((h)->timestamp_l = ((w >> CBXI_IMP_TS_TIMESTAMP_L_POS) & CBXI_IMP_TS_TIMESTAMP_L_MSK))


#define CBXI_IMP_TS_UNPACK_W0(w0, tsh)   \
           CBXI_IMP_TS_UNPACK_ETHERTYPE(w0, tsh); CBXI_IMP_TS_UNPACK_TIMESTAMP_H(w0, tsh)

#define CBXI_IMP_TS_UNPACK_W1(w1, tsh)   CBXI_IMP_TS_UNPACK_TIMESTAMP_L(w1, tsh)


int
cbxi_pack_imp_header(uint8 *buffer, cbxi_ingress_imp_header_t *header);
int
cbxi_unpack_imp_header(uint8 *buffer, cbxi_egress_imp_header_t *header);
int
cbxi_pack_brcm_header(uint8 *buffer, brcm_ingress_imp_header_t *header);
int
cbxi_unpack_brcm_header(uint8 *buffer, brcm_egress_imp_header_t *header);
int
cbxi_unpack_timestamp_header(uint8 *buffer, cbxi_timestamp_header_t *header);
int
cbxi_get_imp_header_type(imp_header_type_t type);



#endif  /* IMP_H */

/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     stg.h
 * * Purpose:
 * *     This file contains definitions internal to STG module.
 */

#ifndef CBX_INT_STG_H
#define CBX_INT_STG_H

#include <shared/types.h>
#include <shared/bitop.h>
#include <fsal/vlan.h>
#include <fsal/stg.h>
#include <fsal_int/vlan.h>

/* STG number 0 is the default and
 * all ports are in forwarding state on this STG */
#define CBX_STG_DEFAULT         (cbx_stgid_t)0
#define CHECK_STG_INIT(csi)         \
    if (!csi->init)             \
        return CBX_E_INIT

#define CHECK_STG(csi, stg)     \
    if ((stg) < csi->stg_min || (stg) > csi->stg_max)         \
        return CBX_E_BADID

/*
 * Allocation bitmap macros
 */
#define STG_BITMAP_TST(csi, stg)                SHR_BITGET(csi->stg_bitmap, stg)
#define STG_BITMAP_SET(csi, stg)                SHR_BITSET(csi->stg_bitmap, stg)
#define STG_BITMAP_CLR(csi, stg)                SHR_BITCLR(csi->stg_bitmap, stg)

typedef struct cbx_stg_sw_info_s {
    int           init;        /* TRUE if STG module has been initialized */
    cbx_stgid_t   stg_min;     /* STG table min index */
    cbx_stgid_t   stg_max;     /* STG table max index */
    cbx_stgid_t   stg_defl;    /* Default STG */
    SHR_BITDCL    *stg_bitmap; /* Bitmap of allocated STGs */
    int           stg_count;   /* Number STGs allocated */
#ifdef CBX_MANAGED_MODE
    /* STG reverse map - keep a linked list of VLANs in each STG */
    cbx_vlan_t  *vlan_first;    /* Indexed by STG (also links free list) */
    cbx_vlan_t  *vlan_next;     /* Indexed by VLAN ID */
#endif /* CBX_MANAGED_MODE */
} cbx_stg_sw_info_t;


extern int cbxi_stg_init(void);
extern int cbxi_stg_default_get(cbx_stgid_t *stgid);
extern int cbxi_stg_default_set(cbx_stgid_t stgid);
extern int cbxi_stg_valid_check(cbx_stgid_t stgid);
extern int cbxi_stg_port_state_set(int unit, cbx_stgid_t stgid,
                        cbxi_pgid_t pgid, cbx_stg_stp_state_t stp_state);
extern int cbxi_stg_vlan_remove(cbx_stgid_t stgid, cbx_vlanid_t vlanid);
extern int cbxi_stg_vlan_destroy(cbx_stgid_t stgid, cbx_vlanid_t vlanid);
extern int cbxi_stg_port_set_all(cbx_portid_t portid, cbx_stg_stp_state_t stp_state);
extern int cbxi_stg_stp_set(cbx_stgid_t stgid, cbx_portid_t portid,
                            cbx_stg_stp_state_t stp_state);

#endif


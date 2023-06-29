/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     vlan.h
 * * Purpose:
 * *     This file contains definitions internal to VLAN module.
 */

#ifndef CBX_INT_VLAN_H
#define CBX_INT_VLAN_H

#include <shared/types.h>
#include <shared/bitop.h>
#include <fsal_int/types.h>
#include <fsal/switch.h>

#define CBX_VID_VALID_MIN        1
#define CBX_VID_VALID_MAX        4095
#define CBX_VID_INVALID          0x1000
#define CBX_VLAN_DEFAULT         1

/* TPID values */
#define CBX_TPID_CTAG           0x8100
#define CBX_TPID_STAG           0x88A8
/* Port update flags */
#define CBX_VLAN_PORT_REMOVE     0x1
#define CBX_VLAN_PORT_ADD        0x2
#define CBX_VLAN_PORT_LIF        0x10

/* pv2li_op : NOVID:0, CTAG:1, STAG:2 */
#define CBX_VLAN_PV2LI_NOVID     0
#define CBX_VLAN_PV2LI_CTAG      0x1
#define CBX_VLAN_PV2LI_STAG      0x2

#define CHECK_VLAN_INIT(cvi)                    \
        if (!cvi->init)             \
        return CBX_E_INIT

#define CHECK_VID_RANGE(vid)    \
        if ((vid) < CBX_VID_VALID_MIN || (vid) > CBX_VID_VALID_MAX) \
        return (CBX_E_PARAM);

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
/************* Support both VLAN ID and VSIID type *****************  */
#define CBXI_VSI_MODE           0x80000000
#define CBXI_VSI_MODE_CHECK(vsiid)      \
                ((vsiid & CBXI_VSI_MODE) == CBXI_VSI_MODE)
#define CBXI_VSI_MODE_SET(vsiid, vsi)      \
                (vsiid = vsi | CBXI_VSI_MODE)

#define CHECK_VID(vid)                    \
        if (CBXI_VSI_MODE_CHECK(vid)) {\
            CHECK_VID_RANGE(vid & ~CBXI_VSI_MODE);}\
        else {\
            CHECK_VID_RANGE(vid);}
/******************************************************************  */

#else /* CONFIG_VIRTUAL_PORT_SUPPORT */
#define CHECK_VID(vid)                    \
         if ((vid) < CBX_VID_VALID_MIN || (vid) > CBX_VID_VALID_MAX) \
        return (CBX_E_PARAM)
#endif /* !CONFIG_VIRTUAL_PORT_SUPPORT */

#define PV_SLI_2_VSI(sli)               \
        ((sli & 0x20000) ? (sli & 0xFFF) : CBX_VID_INVALID)

#define VLAN_NULL               0


#ifndef CBX_MANAGED_MODE /* UM+ mode */
/*
 * Allocation bitmap macros for vlan_bitmap in UM+
 */
#define VLAN_BITMAP_TST(cvi, vid)            SHR_BITGET(cvi->vlan_bitmap, vid)
#define VLAN_BITMAP_SET(cvi, vid)            SHR_BITSET(cvi->vlan_bitmap, vid)
#define VLAN_BITMAP_CLR(cvi, vid)            SHR_BITCLR(cvi->vlan_bitmap, vid)
#define VLAN_BITMAP_ITER(cvi, vid)           SHR_BIT_ITER(cvi->vlan_bitmap, CBX_VID_VALID_MAX, vid)
#endif /* !CBX_MANAGED_MODE */

/*
 * Allocation bitmap macros for vlan_ubmp in UM+
 */
#define VLAN_UBMP_TST(cvi, lpg)            SHR_BITGET(cvi->vlan_ubmp, lpg)
#define VLAN_UBMP_SET(cvi, lpg)            SHR_BITSET(cvi->vlan_ubmp, lpg)
#define VLAN_UBMP_CLR(cvi, lpg)            SHR_BITCLR(cvi->vlan_ubmp, lpg)

/*
 * VLAN internal struct used for book keeping
 */
#ifdef CBX_MANAGED_MODE
typedef struct vlist_s {
    cbx_vlan_t          vid;
    cbx_vlan_t          vsi;
    uint16_t            pp_map[CBX_MAX_UNITS]; /* Physical port map */
    pbmp_t              lag_map; /* A bit map of LAGs that are part of VLAN */
    struct vlist_s      *next;
} vlist_t;
#endif /* CBX_MANAGED_MODE */

/* SW VLAN database */
typedef struct cbx_vlan_info_s {
    int         init;       /* TRUE if VLAN module has been inited */
    cbx_vlan_t  defl_vid;    /* Default VLAN */
    cbx_stgid_t defl_stg;   /* Default STG */
#ifdef CBX_MANAGED_MODE
    vlist_t     *list;      /* vlist of existing VLANs */
#else
    SHR_BITDCL  *vlan_bitmap; /* Bitmap of VLANs created */
#endif /* CBX_MANAGED_MODE */
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    SHR_BITDCL  *vsi_bitmap; /* Bitmap of VSIs alloted */
#endif
    SHR_BITDCL  *vlan_ubmp;   /* Bitmap of untagged ports added to VLANs */
    int         count;      /* Number of existing VLANs */
} cbx_vlan_sw_info_t;

/* Egress tag edit opcode enum */
typedef enum cbxi_vlan_tag_edit_e {
    cbxiTagCtpid0    = 0, /* INSERT_CTPID0 */
    cbxiTagCtpid1    = 1, /* INSERT_CTPID1 */
    cbxiTagStpid0    = 2, /* INSERT_STPID0 */
    cbxiTagStpid1    = 3, /* INSERT_STPID1 */
    cbxiTagReserved1 = 4, /* INSERT_ETPID  */
    cbxiTagReserved2 = 5, /* Reserved */
    cbxiTagReserved3 = 6, /* Reserved */
    cbxiTagReserved4 = 7, /* Reserved */
} cbxi_vlan_tag_edit_t;

extern cbx_vlan_sw_info_t cbx_vlan_info;

extern int cbxi_vlan_vid_check(cbx_vlanid_t vlanid);
extern int cbxi_robo2_vlan_port_resolve(cbx_portid_t portid,
                                cbxi_pgid_t *lpg, cbxi_pgid_t *rlpg);
extern int cbxi_robo2_pg_map_resolve(
                int unit, pbmp_t lpg_map, pbmp_t *pp_map, pbmp_t *lag_map);
extern int cbxi_vlan_vsit_index_get(cbx_vlanid_t vid, cbx_vlan_t *vsi);
extern int cbxi_vlan_vsit_to_vid(cbx_vlan_t vsi, cbx_vlan_t *vid);
extern int cbxi_vlan_learn_mode_get(cbx_switch_vlan_mode_t *mode);
extern int cbxi_vlan_init(void);
extern int cbxi_vlan_stgid_get(cbx_vlanid_t vlanid, cbx_stgid_t *stgid);
extern int cbxi_vlan_stgid_set(cbx_vlanid_t vlanid, cbx_stgid_t stgid);
extern int cbxi_vlan_all_cascade_ports_add(int unit);


#ifndef CBX_MANAGED_MODE /* UM+ mode */
extern int cbxi_vlan_stg_remove_all(cbx_stgid_t stgid);
#endif /* !CBX_MANAGED_MODE */

#endif

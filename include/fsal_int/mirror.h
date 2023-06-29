/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     mirror.h
 * * Purpose:
 * *     This file contains definitions internal to Mirror module.
 */

#ifndef CBX_INT_MIRROR_H
#define CBX_INT_MIRROR_H

#include <fsal_int/types.h>
#include <shared/bitop.h>

#define CBXI_VMT_MAX             4
#define CBXI_MIRROR_PORT_ADD        0x00000001
#define CBXI_MIRROR_PORT_REMOVE     0x00000002
#define CBXI_MIRROR_VLAN_ADD        0x00000004
#define CBXI_MIRROR_VLAN_REMOVE     0x00000008

typedef struct cbx_mirror_vmt_info_s {
    char                  in_use;/* IVMT/EVMT entry use/free flag */
    cbx_vlanid_t         vlanid;/* Mirror vlan ID */
} cbx_mirror_vmt_info_t;

typedef struct cbx_mirror_info_s {
    char                 in_use;/* Mirror entry use/free flag */
    uint32_t             flags; /**< Flags:  CBX_MIRROR_XX */
    cbx_mirrorid_t       mirrorid; /* Mirror ID for the group */
    cbx_mirror_destid_t  mirror_destid;/* Mirror destination ID */
    pbmp_t               ingport_pbmp[CBX_MAX_UNITS];/* Ingress source ports bitmap for mirroring  */
    pbmp_t               egrport_pbmp[CBX_MAX_UNITS];/* Egress source ports bitmap for mirroring  */
#ifdef MANAGED_MODE
    SHR_BITDCL           *ingvlan_bitmap;/* Ingress vlan ID bitmap for mirroring */
    SHR_BITDCL           *egrvlan_bitmap;/* Egress vlan ID bitmap for mirroring */
#endif /* MANAGED_MODE */
} cbx_mirror_info_t;

typedef struct cbx_mirror_dest_info_s {
    char                  in_use;/* Mirror entry use/free flag */
    int                   dlinid;     /* DLI.N index to access DL2LDPG table */
    int                   ref_count;  /* Reference count for mirror destination usage */
    cbx_mirror_destid_t   mirror_destid;/* Mirror destination ID */
    cbx_mirror_dest_t     mirror_dest;/* Mirror destination info */
} cbx_mirror_dest_info_t;

typedef struct cbx_mirror_cntl_s {
    char         init;       /* TRUE if Mirror module has been inited */
    SHR_BITDCL   *mirror_bitmap; /* Bitmap of allocated mirror groups  */
    SHR_BITDCL   *mirror_dest_bitmap; /* Bitmap of allocated mirror destination IDs */
    cbx_mirror_info_t  *mirror_info;/* Mirror group info */
    cbx_mirror_dest_info_t  *mirror_dest;/* Mirror destination infor */
} cbx_mirror_cntl_t;

extern int cbx_mirror_init(void);
int cbxi_robo2_dlin_init( int unit );
int cbxi_robo2_dlin_slot_get( int unit, int  *dlinid );

#endif /* !CBX_INT_MIRROR_H  */

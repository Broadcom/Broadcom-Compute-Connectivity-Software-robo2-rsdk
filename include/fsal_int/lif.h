/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     lif.h
 * * Purpose:
 * *     This file contains definitions internal to LIF module.
 */

#ifndef CBX_INT_LIF_H
#define CBX_INT_LIF_H

#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <fsal_int/types.h>
#include <fsal/lif.h>

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
/* M2TK indeces for different LIM keys */
#define CBXI_LIM_M2TK_PV               1
#define CBXI_LIM_M2TK_PV_DECAP         2
#define CBXI_LIM_M2TK_PVV              3
#define CBXI_LIM_M2TK_PVV_DECAP        4
#define CBXI_LIM_M2TK_MIM              5
#define CBXI_LIM_M2TK_MPLS             6

/* Concept is to divide the ~8k GHT into 4 equal sized tables of 2K size each */

/* GHT table ids */
#define CBXI_GHT_IDX_PV                0
#define CBXI_GHT_IDX_PV_DECAP          1
#define CBXI_GHT_IDX_PVV               2
#define CBXI_GHT_IDX_PVV_DECAP         3
#define CBXI_GHT_IDX_MIM               4
#define CBXI_GHT_IDX_MPLS              5

/* LIM GHT table sizes : Actual table size is 2^num_entries mentioned here */
#define CBXI_GHT_SIZE_SHIFT_PV           8 /* 2^8 = 0.25K entries */
#define CBXI_GHT_SIZE_SHIFT_PV_DECAP     8 /* 2^8 = 0.25K entries */
#define CBXI_GHT_SIZE_SHIFT_PVV          8 /* 2^8 = 0.25K entries */
#define CBXI_GHT_SIZE_SHIFT_PVV_DECAP    8 /* 2^8 = 0.25K entries */
#define CBXI_GHT_SIZE_SHIFT_MIM          9 /* 2^9 = 0.5K entries */
#define CBXI_GHT_SIZE_SHIFT_MPLS         9 /* 2^9 = 0.5K entries */

#define CBXI_GHT_SIZE_PV                 (1 << CBXI_GHT_SIZE_SHIFT_PV)
#define CBXI_GHT_SIZE_PV_DECAP           (1 << CBXI_GHT_SIZE_SHIFT_PV_DECAP)
#define CBXI_GHT_SIZE_PVV                (1 << CBXI_GHT_SIZE_SHIFT_PVV)
#define CBXI_GHT_SIZE_PVV_DECAP          (1 << CBXI_GHT_SIZE_SHIFT_PVV_DECAP)
#define CBXI_GHT_SIZE_MIM                (1 << CBXI_GHT_SIZE_SHIFT_MIM)
#define CBXI_GHT_SIZE_MPLS               (1 << CBXI_GHT_SIZE_SHIFT_MPLS)

#define CHECK_LIF_INIT(cli)                    \
        if (!cli.init)             \
        return CBX_E_INIT


typedef enum cbxi_vlan_port_match_e {
    cbxiVlanLifMatchInvalid = 0,         /* Illegal */
    cbxiVlanLifMatchNone = 1,            /* No match criteria */
    cbxiVlanLifMatchPortVlan = 2,        /* Port - VID */
    cbxiVlanLifMatchPortVlanStacked = 3, /* Port - VID - VID*/
    cbxiVlanLifMatchCount = 4            /* Must be last. */
} cbxi_vlan_port_match_t;


#ifdef CBX_MANAGED_MODE
typedef struct liflist_s {
    int                    lin;
    uint16_t               vsi;
    uint16_t               mode;
    cbx_vlan_lif_params_t  *params;
    struct liflist_s       *next;
} liflist_t;
#endif /* CBX_MANAGED_MODE */

/* SW VLAN database */
typedef struct cbxi_lif_info_s {
    int         init;       /* TRUE if LIF module has been initiated */
#ifdef CBX_MANAGED_MODE
    liflist_t   *list;      /* list of existing LIFs */
#endif
} cbxi_lif_sw_info_t;



extern int cbxi_lif_init( void );
extern int cbxi_lif_vsi_get(int lin, cbx_vlan_t *vsi, uint16_t *mode);
extern int cbxi_lif_vsi_set(int lin, cbx_vlan_t vsi, uint16_t mode);
extern int cbxi_lif_vsi_port_lookup(cbx_vlan_t vsi, int array_max,
        cbx_portid_t *portid_array, uint32_t *mode_array, int *array_size);
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */

#endif /* CBX_INT_LIF_H */

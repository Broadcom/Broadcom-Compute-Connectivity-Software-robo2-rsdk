/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     vlan.c
 * * Purpose:
 * *     Robo2 vlan module
 * *
 * * Note:
 * *  Unit number is kept in internal functions for future enhancements.
 * * FSAL APIs which are exposed don't get any unit no. and value 0 is passed to
 * * internal functions.
 * *
 * */


#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal/vlan.h>
#include <fsal/stg.h>
#include <fsal/l2.h>
#include <fsal/switch.h>
#include <fsal_int/vlan.h>
#include <fsal_int/port.h>
#include <fsal_int/stg.h>
#include <fsal_int/types.h>
#include <fsal_int/lag.h>
#include <fsal_int/slic.h>
#include <fsal_int/lif.h>
#include <fsal_int/trap.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif /* CONFIG_PORT_EXTENDER */
#include <soc/robo2/common/tables.h>
#include <sal_alloc.h>

cbx_vlan_sw_info_t cbx_vlan_info;

STATIC int
cbxi_vlan_create(cbx_vlan_params_t *vlan_params, cbx_vlanid_t *vsiid);
STATIC int
cbxi_vlan_port_update(cbx_vlanid_t vlanid,
                    cbx_portid_t portid, uint32_t  mode, uint32_t flags);

/*
 * Function:
 *  cbxi_robo2_vlan_port_resolve
 * Purpose:
 *  Obtain local pg and remote pg for a given vlan port
 */
int
cbxi_robo2_vlan_port_resolve(cbx_portid_t portid,
                                cbxi_pgid_t *lpg, cbxi_pgid_t *rlpg)
{
    dli2ldpg_t ent_dli2ldpg;
    int unit = 0;
    int n_val;


    n_val = CBX_PORTID_VLAN_PORT_ID_GET(portid);
    if (n_val < 0) {
        return CBX_E_PORT;
    }

    /* Read entry from DLI2LDPG */
    CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(unit, n_val, &ent_dli2ldpg));
    *lpg = ent_dli2ldpg.ldpgid;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(
                                        unit, n_val, &ent_dli2ldpg));
        *rlpg = ent_dli2ldpg.ldpgid;
    } else {
        *rlpg = CBXI_PGID_INVALID;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_pg_map_resolve
 * Purpose:
 *  Obtain physical port bit map and lag bit map from given bitmap of Local port
 *  groups.
 */

int
cbxi_robo2_pg_map_resolve(
                int unit, pbmp_t lpg_map, pbmp_t *pp_map, pbmp_t *lag_map)
{
    lpg2ppfov_t  ent_ppfov;
    cbx_portid_t lagid;
    int          idx, port, rv;

    CBX_PBMP_CLEAR(*pp_map);

    CBX_PBMP_ITER(lpg_map, idx) {
        /* Retrive LPG2PPFOV table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit, idx, &ent_ppfov));
        *pp_map |= ent_ppfov.ppfov;
    }


    CBX_PBMP_ITER(*pp_map, port) {
        rv = cbxi_robo2_lag_member_check(CBXI_GLOBAL_PORT(port, unit), &lagid);
        if (CBX_SUCCESS(rv)) {
            rv = CBX_PORTID_TRUNK_GET(lagid);
            if (rv < 0) {
                return CBX_E_PORT;
            } else {
                idx = rv;
            }
            CBX_PBMP_PORT_REMOVE(*pp_map, port);
            CBX_PBMP_PORT_ADD(*lag_map, idx);
        }
    }

    return CBX_E_NONE;
}

#ifndef CBX_MANAGED_MODE /* UM+ mode */
/*
 * Function:
 *  cbxi_vlan_vid_pg_map_resolve
 * Purpose:
 *  Obtain physical port bit map and lag bit map for given VLAN
 */

STATIC int
cbxi_vlan_vid_pg_map_resolve(int unit, cbx_vlan_t vsi,
                                pbmp_t *pp_map, pbmp_t *lag_map)
{
    vsit_t       ent_vsit;
    pbmp_t       lpg_map;

    /* Retrive PV2LI table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, (uint32)vsi, &ent_vsit));
    /* Obtain LPGs that are part of given VLAN in given unit */
    lpg_map = ent_vsit.pg_map;

    CBX_IF_ERROR_RETURN(cbxi_robo2_pg_map_resolve(
                                        unit, lpg_map, pp_map, lag_map));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_vid_pp_map_get
 * Purpose:
 *  Obtain physical port bit map for given VLAN
 */

STATIC int
cbxi_vlan_vid_pp_map_get(int unit, cbx_vlan_t vsi, pbmp_t *pp_map)
{
    pbmp_t lag_map;

    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_pg_map_resolve(
                                        unit, vsi, pp_map, &lag_map));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_vid_lag_map_get
 * Purpose:
 *  Obtain lag bit map for given VLAN
 */

STATIC int
cbxi_vlan_vid_lag_map_get(cbx_vlan_t vsi, pbmp_t *lag_map)
{
    int unit;
    uint8 num_units = 1;
    pbmp_t pp_map;

    CBX_PBMP_CLEAR(*lag_map);

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        /* Same pointer is taken and updated for secondary avenger case */
        CBX_IF_ERROR_RETURN(cbxi_vlan_vid_pg_map_resolve(
                                        unit, vsi, &pp_map, lag_map));
    }
    return CBX_E_NONE;
}
#endif /* !CBX_MANAGED_MODE */

/*
 * Function:
 *  cbxi_vlan_vid_insert
 * Purpose:
 *  Add a VLAN ID to S/W list
 */

STATIC int
cbxi_vlan_vid_insert(cbx_vlan_t vid, cbx_vlan_t vsi)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cvi->list;
    vlist_t *v;

    if ((v = sal_alloc(sizeof (vlist_t), "vlist")) == NULL) {
        return CBX_E_MEMORY;
    }

    v->vid = vid;
    v->vsi = vsi;

    CBX_PBMP_CLEAR(v->lag_map);
    CBX_PBMP_CLEAR(v->pp_map[CBX_AVENGER_PRIMARY]);
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_PBMP_CLEAR(v->pp_map[CBX_AVENGER_SECONDARY]);
    }

    v->next = *list;
    *list = v;
#else /* UM+ mode */
    if (vid != vsi) {
        /* UM+ always has 1:1 mapping */
        return CBX_E_PARAM;
    }

    VLAN_BITMAP_SET(cvi, vid);
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_list_vid_remove
 * Purpose:
 *  Delete a VLAN ID from a S/W list
 */

STATIC int
cbxi_vlan_list_vid_remove(cbx_vlan_t vid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cvi->list;
    vlist_t     *v;

    while (*list != NULL) {
        if ((*list)->vid == vid) {
            v = *list;
            *list = v->next;
            sal_free(v);
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    if (!VLAN_BITMAP_TST(cvi, vid)) {
        return CBX_E_NOT_FOUND;
    }
    VLAN_BITMAP_CLR(cvi, vid);
    return CBX_E_NONE;
#endif /* CBX_MANAGED_MODE */
}

/*
 * Function:
 *  cbxi_vlan_vid_lookup
 * Purpose:
 *  Return TRUE if a VLAN ID exists in a vlist, FALSE otherwise
 */

STATIC int
cbxi_vlan_vid_lookup(cbx_vlanid_t vid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cvi->list;

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    if (CBXI_VSI_MODE_CHECK(vid)) {
        if (SHR_BITGET(cvi->vsi_bitmap, (vid & 0xFFF))) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

#endif
    while (*list != NULL) {
        if ((*list)->vid == vid) {
            return TRUE;
        }
        list = &(*list)->next;
    }
#else /* UM+ mode */
    if (VLAN_BITMAP_TST(cvi, vid)) {
        return TRUE;
    }
#endif /* CBX_MANAGED_MODE */
    return FALSE;
}

/*
 * Function:
 *  cbxi_vlan_list_destroy
 * Purpose:
 *  Free all memory used by a VLAN list
 */

STATIC int
cbxi_vlan_list_destroy(void)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
#ifndef CBX_MANAGED_MODE /* UM+ mode */
    cbx_vlan_t vid;
    int count = 0;
#endif /* !CBX_MANAGED_MODE */

#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cvi->list;
    while (*list != NULL) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_list_vid_remove((*list)->vid));
    }
#else /* UM+ mode */
    count = cvi->count;
    VLAN_BITMAP_ITER(cvi, vid) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_list_vid_remove(vid));
        count--;
        if (!count) {
            break;
        }
    }
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_port_bit_map_set
 * Purpose:
 *  Set bit map of physical ports part of vid
 */

STATIC int
cbxi_vlan_port_bit_map_set(int unit, cbx_vlan_t vsi, pbmp_t pp_map)
{
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cbx_vlan_info.list;

    while (*list != NULL) {
        if ((*list)->vsi == vsi) {
            CBX_PBMP_CLEAR((*list)->pp_map[unit]);
            CBX_PBMP_OR((*list)->pp_map[unit], pp_map);
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    return CBX_E_NONE;
#endif /* CBX_MANAGED_MODE */
}

/*
 * Function:
 *  cbxi_vlan_port_bit_map_get
 * Purpose:
 *  Get bit map of physical ports part of vid
 */

STATIC int
cbxi_vlan_port_bit_map_get(
                int unit, cbx_vlan_t vsi, pbmp_t *pp_map)
{
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cbx_vlan_info.list;

    SAL_ASSERT(pp_map != NULL);

    while (*list != NULL) {
        if ((*list)->vsi == vsi) {
            CBX_PBMP_CLEAR(*pp_map);
            CBX_PBMP_OR(*pp_map, (*list)->pp_map[unit]);
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_pp_map_get(unit, vsi, pp_map));
    return CBX_E_NONE;
#endif /* CBX_MANAGED_MODE */
}

/*
 * Function:
 *  cbxi_vlan_lag_bit_map_set
 * Purpose:
 *  Set bit map of LAGs part of vid
 */

STATIC int
cbxi_vlan_lag_bit_map_set(cbx_vlan_t vsi, pbmp_t lag_map)
{
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cbx_vlan_info.list;

    while (*list != NULL) {
        if ((*list)->vsi == vsi) {
            CBX_PBMP_CLEAR((*list)->lag_map);
            CBX_PBMP_OR((*list)->lag_map, lag_map);
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    return CBX_E_NONE;
#endif /* CBX_MANAGED_MODE */
}

/*
 * Function:
 *  cbxi_vlan_lag_bit_map_get
 * Purpose:
 *  Get bit map of LAGs part of vid
 */

STATIC int
cbxi_vlan_lag_bit_map_get(cbx_vlan_t vsi, pbmp_t *lag_map)
{
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cbx_vlan_info.list;

    SAL_ASSERT(lag_map != NULL);

    while (*list != NULL) {
        if ((*list)->vsi == vsi) {
            CBX_PBMP_CLEAR(*lag_map);
            CBX_PBMP_OR(*lag_map, (*list)->lag_map);
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_lag_map_get(vsi, lag_map));
    return CBX_E_NONE;
#endif /* CBX_MANAGED_MODE */
}

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
/*
 * Function:
 *  cbxi_vlan_vsit_index_allocate
 * Purpose:
 *  Allocate free VSIT table index
 */

int
cbxi_vlan_vsit_index_allocate(cbx_vlan_t *vsi)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    int idx = 0;

    for (idx = 1; idx < CBX_VLAN_MAX; idx++) {
        if (!SHR_BITGET(cvi->vsi_bitmap, idx)) {
            *vsi = (cbx_vlan_t)idx;
            return CBX_E_NONE;
        }
    }
    return CBX_E_FULL;
}
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */

/*
 * Function:
 *  cbxi_vlan_vsit_index_get
 * Purpose:
 *  Return VSIT table index for given VLAN ID
 *  For UM+ this has 1:1 mapping.
 *  But this may not be the case for MM
 */

int
cbxi_vlan_vsit_index_get(cbx_vlanid_t vid, cbx_vlan_t *vsi)
{
#ifdef CBX_MANAGED_MODE
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    vlist_t **list = &cbx_vlan_info.list;

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    if (CBXI_VSI_MODE_CHECK(vid)) {
        if (SHR_BITGET(cvi->vsi_bitmap, (vid & 0xFFF))) {
            *vsi = (vid & 0xFFF);
            return CBX_E_NONE;
        } else {
            return CBX_E_NOT_FOUND;
        }
    }
#endif
    while (*list != NULL) {
        if ((*list)->vid == vid) {
            *vsi = (*list)->vsi;
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    *vsi = vid;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_vsit_to_vid
 * Purpose:
 *  Return VLAN ID associated with VSIT index given.
 *  For UM+ this has 1:1 mapping.
 *  But this may not be the case for MM
 */

int
cbxi_vlan_vsit_to_vid(cbx_vlan_t vsi, cbx_vlan_t *vid)
{
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cbx_vlan_info.list;

    while (*list != NULL) {
        if ((*list)->vsi == vsi) {
            *vid = (*list)->vid;
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#else /* UM+ mode */
    *vid = vsi;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_vlan_learn_mode_get
 * Purpose:
 *    Obtain the global vlan learning mode : IVL / SVL
 * Parameters:
 *    mode - (OUT) vlan learn mode of type cbx_switch_vlan_mode_t
 */
int
cbxi_vlan_learn_mode_get(cbx_switch_vlan_mode_t *mode)
{
    cbx_switch_params_t switch_params;

    /* TO DO : Define a function in switch module to obtain switch params */
    switch_params.vlan_mode = 1;
#if 0
    CBX_IF_ERROR_RETURN(cbx_switch_params_get(&switch_params));
#endif

    *mode = switch_params.vlan_mode;

    return CBX_E_NONE;
}

#ifdef CBX_MANAGED_MODE
/*
 * Function:
 *  cbxi_vlist_vlan_1st
 * Purpose:
 *  Return the first defined VLAN ID that is not the default VLAN.
 */

STATIC cbx_vlanid_t
cbxi_vlist_vlan_1st(void)
{
    vlist_t *vl;

    for (vl = cbx_vlan_info.list; vl != NULL; vl = vl->next) {
        if (vl->vid != cbx_vlan_info.defl_vid) {
            return vl->vid;
        }
    }

    return CBX_VLAN_INVALID;
}
#endif /* CBX_MANAGED_MODE */

#ifdef CONFIG_CASCADED_MODE
/**
 * Add Cascade Ports to a existing VLAN
 *
 * @return return code
 */
int
cbxi_vlan_all_cascade_ports_add(int unit)
{
    int rv = CBX_E_NONE;
    cbx_portid_t portid;
    uint32 mode = CBX_VLAN_PORT_MODE_TAGGED;
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
#ifdef CBX_MANAGED_MODE
    vlist_t **list;
#else
    int count;
    cbx_vlan_t vid;
#endif

    if ((unit > 0) && !(SOC_IS_CASCADED(CBX_AVENGER_PRIMARY))) {
        return CBX_E_INIT;
    }
    rv = cbxi_cascade_port_get(unit, &portid);

    if (rv == CBX_E_NONE) {
#ifdef CBX_MANAGED_MODE
        list = &cvi->list;
        while (*list != NULL) {
                cbxi_vlan_port_update((*list)->vid, portid,
                                      mode, CBX_VLAN_PORT_ADD);
        }
#else /* UM+ mode */
        count = cvi->count;
        VLAN_BITMAP_ITER(cvi, vid) {
                cbxi_vlan_port_update(vid, portid, mode, CBX_VLAN_PORT_ADD);
            count--;
            if (!count) {
                break;
            }
        }
#endif /* CBX_MANAGED_MODE */
    }
    return rv;
}

/**
 * Remove Cascade Ports from a VLAN
 *
 * @param vlanid        VLAN Identifier
 *
 * @return return code
 */
int
cbxi_vlan_cascade_ports_remove(cbx_vlanid_t vlanid)
{
    int rv = CBX_E_NONE;
    int unit;
    cbx_portid_t portid;
    uint32 mode = CBX_VLAN_PORT_MODE_TAGGED;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        for (unit = 0; unit < 2; unit++) {
            CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(unit, &portid));
            CBX_IF_ERROR_RETURN(
                cbxi_vlan_port_update(vlanid, portid, mode, CBX_VLAN_PORT_REMOVE));
        }
    }
    return rv;
}
#endif /* CONFIG_CASCADED_MODE */


/*
 * Function:
 *  cbxi_vlan_egress_tag_init
 * Purpose:
 *  Initialize TPIDs to be used for CTAG and STAG at egress
 *
 */

STATIC int
cbxi_vlan_egress_tag_init(void)
{
    tet_t ent_tet;
    uint32_t status = 0;
    uint8_t num_units = 1;
    uint8_t i;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (i = 0; i < num_units; i++) {
        CBX_IF_ERROR_RETURN(soc_robo2_tet_get(i, cbxiTagCtpid0, &ent_tet));
        ent_tet.tag = CBX_TPID_CTAG; /* CTAG */
        CBX_IF_ERROR_RETURN(soc_robo2_tet_set(
                        i, cbxiTagCtpid0, &ent_tet, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_tet_get(i, cbxiTagStpid0, &ent_tet));
        ent_tet.tag = CBX_TPID_STAG; /* STAG */
        CBX_IF_ERROR_RETURN(soc_robo2_tet_set(
                        i, cbxiTagStpid0, &ent_tet, &status));

    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_init
 * Purpose:
 *  Global Initialization for VLAN related table/ regs.
 *
 */

STATIC int
cbxi_vlan_table_init(void)
{
    vsit_t ent_vsit;
    pv2li_t ent_pv2li;
    int reserved_vlan = 0;
    uint32 status = 0;
    int max_index = 0;
    uint8 num_units, i;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (i = 0; i < num_units; i++) {
        /* Enable VSIT */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_enable(i));
        /* Reset VSIT*/
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_reset(i));
        max_index =  soc_robo2_vsit_max_index(i);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, 0, &ent_vsit));
        ent_vsit.pg_map = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_init(i, 1, max_index,0, &ent_vsit));

        /* Enable PV2LI */
        CBX_IF_ERROR_RETURN(soc_robo2_pv2li_enable(i));
        /* Reset PV2LI*/
        CBX_IF_ERROR_RETURN(soc_robo2_pv2li_reset(i));

        /* Enable VTCT */
        CBX_IF_ERROR_RETURN(soc_robo2_vtct_enable(i));
        /* Reset VTCT*/
        CBX_IF_ERROR_RETURN(soc_robo2_vtct_reset(i));

        /* Enable ETCT */
        CBX_IF_ERROR_RETURN(soc_robo2_etct_enable(i));
        /* Reset ETCT*/
        CBX_IF_ERROR_RETURN(soc_robo2_etct_reset(i));

        /* Enable TET */
        CBX_IF_ERROR_RETURN(soc_robo2_tet_enable(i));
        /* Reset TET*/
        CBX_IF_ERROR_RETURN(soc_robo2_tet_reset(i));

        /* Use reserved VLAN id 0, for the purpose of sending cpu traffic
         * using CBX_PACKET_TX_FWD_UCAST */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, reserved_vlan, &ent_vsit));
        ent_vsit.pg_map = 0x3FFF;
        ent_vsit.learn = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i, reserved_vlan, &ent_vsit, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_pv2li_get(i, reserved_vlan, &ent_pv2li));
        ent_pv2li.pg_map = 0x3FFF;
        CBX_IF_ERROR_RETURN(soc_robo2_pv2li_set(i, reserved_vlan, &ent_pv2li, &status));

    }

    return CBX_E_NONE;
}

/**
 * Initialize VLAN module
 *
 * @param void N/A
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_vlan_init(void)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    int               pp, port;
    cbx_stgid_t       defl_stg;
    cbx_vlanid_t      vsiid;
    cbx_vlan_params_t vlan_params;
#if 0
    cbxi_trap_ctrl_t  trap_ctl;
#endif
    cbx_port_params_t port_params;
    uint8_t           num_units = 1;
    uint8_t           unit = 0;

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_init()..\n")));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Init VLAN related tables */
    CBX_IF_ERROR_RETURN(cbxi_vlan_table_init());

    /* Initialize software structure */
    cvi->defl_vid = CBX_VLAN_DEFAULT;
    CBX_IF_ERROR_RETURN(
        cbxi_stg_default_get(&defl_stg));
    cvi->defl_stg = defl_stg;
    cvi->count = 0;

#ifndef CBX_MANAGED_MODE /* UM+ mode */
    if (cvi->vlan_bitmap == NULL) {
        cvi->vlan_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(CBX_VLAN_MAX), "VLAN-BITMAP");
    }
    if (cvi->vlan_bitmap == NULL) {
        return CBX_E_MEMORY;
    }
#endif /* !CBX_MANAGED_MODE */
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    if (cvi->vsi_bitmap == NULL) {
        cvi->vsi_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(CBX_VLAN_MAX), "VSI-BITMAP");
    }
    if (cvi->vsi_bitmap == NULL) {
        return CBX_E_MEMORY;
    }
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */
    if (cvi->vlan_ubmp == NULL) {
        cvi->vlan_ubmp =
                sal_alloc(SHR_BITALLOCSIZE(CBX_PBMP_PORT_MAX), "VLAN-UBMP");
        if (cvi->vlan_ubmp) {
            sal_memset(cvi->vlan_ubmp, 0, SHR_BITALLOCSIZE(CBX_PBMP_PORT_MAX));
        }
    }
    if (cvi->vlan_ubmp == NULL) {
        return CBX_E_MEMORY;
    }

    /* Initialize TPIDs to be used for CTAG and STAG */
    CBX_IF_ERROR_RETURN(cbxi_vlan_egress_tag_init());

#if 0 /* Remove LEARN_FRAME trap control setting */
    for (unit = 0; unit < num_units; unit++) {
        /* Enable LEARN_FRAME trap to send to CP */
        CBX_IF_ERROR_RETURN(cbxi_trap_control_get(
                                    unit, LEARN_FRAME, &trap_ctl));
        trap_ctl.trap_flags = 0;
        trap_ctl.drop = 0;
        trap_ctl.term = 0;
        CBX_IF_ERROR_RETURN(cbxi_trap_control_set(
                                    unit, LEARN_FRAME, &trap_ctl));
    }
#endif
    /* In case cbx_vlan_init is called more than once */
    CBX_IF_ERROR_RETURN(cbxi_vlan_list_destroy());
    cvi->init = TRUE;
    /* Create default VLAN and add all ports to it */
    cbx_vlan_params_t_init(&vlan_params);
    vlan_params.vlan = cvi->defl_vid;
    CBX_IF_ERROR_RETURN(cbxi_vlan_create(&vlan_params, &vsiid));

    for (unit = 0; unit < num_units; unit++) {
        CBX_PBMP_ITER(PBMP_ALL(unit), pp) {
            port =  pp + (unit * CBX_MAX_PORT_PER_UNIT);
            if ((port == CBX_PORT_ECPU) || (port == CBX_PORT_ICPU)) {
                continue;
            }
            /* cbxi_vlan_port_update function expects portid in 0 - 31 port range */
            CBX_IF_ERROR_RETURN(
                    cbxi_vlan_port_update(cvi->defl_vid, (cbx_portid_t)port,
                            CBX_VLAN_PORT_MODE_UNTAGGED, CBX_VLAN_PORT_ADD));
            /* Configure PGT default_vid in order to provide VID = 1
             * for PV2LI operation for untagged packets. */
            CBX_IF_ERROR_RETURN(cbx_port_info_get(port, &port_params));
            port_params.default_vid = cvi->defl_vid;
            CBX_IF_ERROR_RETURN(cbx_port_info_set(port, &port_params));
        }
    }

    return CBX_E_NONE;
}

STATIC uint8
cbxi_vlan_pack_isomap(cbx_vlan_params_t *vlan_params)
{
    uint8 iso_map = 0;
    uint8 i;

    for (i = 0; i < CBX_VLAN_ISOLATION_MAX; i++) {
        iso_map |= vlan_params->isolation[i] << i;
    }

    return iso_map;
}

STATIC void
cbxi_vlan_unpack_isomap(uint8 iso_map, cbx_vlan_params_t *vlan_params)
{
    uint8 i;

    for (i = 0; i < CBX_VLAN_ISOLATION_MAX; i++) {
        vlan_params->isolation[i] = (iso_map >> i) & 1;
    }
}

/*
 * Function:
 *  cbxi_vlan_vid_check
 * Purpose:
 *   Validate a VID value
 *
 */

int
cbxi_vlan_vid_check(cbx_vlanid_t vlanid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_NONE;
    }
#endif /* CONFIG_PORT_EXTENDER */

    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    /* Check if the given VLAN exists */
    if (!cbxi_vlan_vid_lookup(vlanid)) {
         LOG_ERROR(BSL_LS_FSAL_VLAN,
                (BSL_META("VLAN %d doesn't exist\n"), vlanid));
        return CBX_E_NOT_FOUND;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_stgid_get
 * Purpose:
 *   Retrive stgid from VSIT associated with specified vlanid
 *
 */

int
cbxi_vlan_stgid_get(cbx_vlanid_t vlanid, cbx_stgid_t *stgid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    cbx_vlan_t vsi;
    vsit_t ent_vsit;

    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    /* Check if the given VLAN exists */
    if (!cbxi_vlan_vid_lookup(vlanid)) {
        return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));

    /* Retrive VSIT table entry */
    CBX_IF_ERROR_RETURN(
        soc_robo2_vsit_get(CBX_AVENGER_PRIMARY, (uint32)vsi, &ent_vsit));

    /* Same stgid is configured for a specific VSIT entry on both the Avengers
     * in Cascaded mode */

    *stgid = (cbx_stgid_t)ent_vsit.stgid;

    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_vlan_stgid_set
 * Purpose:
 *   Put specified VLAN in specified STG
 *
 */

int
cbxi_vlan_stgid_set(cbx_vlanid_t vlanid, cbx_stgid_t stgid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    uint8 num_units, i;
    uint32 status = 0;
    cbx_vlan_t vsi;
    vsit_t ent_vsit;

    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    /* Check if the given VLAN exists */
    if (!cbxi_vlan_vid_lookup(vlanid)) {
        return CBX_E_NOT_FOUND;
    }

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /* Check if STG exists */
    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    /* Obtain VSIT table id corresponding to given vlanid */
    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));

    for (i = 0; i < num_units; i++) {
        /* Retrive VSIT table entry */
        CBX_IF_ERROR_RETURN(
            soc_robo2_vsit_get(i, (uint32)vsi, &ent_vsit));

        ent_vsit.stgid = (uint8)stgid;

        /* Write updated entry to VSIT table  */
        CBX_IF_ERROR_RETURN(
            soc_robo2_vsit_set(i, (uint32)vsi, &ent_vsit, &status));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_vlan_create
 * Purpose:
 *  Main body of cbx_vlan_create;
 */

STATIC int
cbxi_vlan_create(cbx_vlan_params_t *vlan_params, cbx_vlanid_t *vsiid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    cbx_switch_vlan_mode_t mode;
    uint8 num_units, i;
    uint32 idx;
    uint32 status = 0;
    vsit_t ent_vsit;
    vtct_t ent_vtct;
    pv2li_t ent_pv2li;
    cbx_vlan_t  vsi;

    /* Check if vlan alreay exists */
    if (!(vlan_params->flags & CBX_VLAN_ALLOCATE_VSI)) {
        if(cbxi_vlan_vid_lookup(vlan_params->vlan)) {
            return CBX_E_EXISTS;
        }
    } else {
        vlan_params->vlan = CBX_VID_INVALID;
    }

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_allocate(&vsi));
    idx = (uint32)(vsi & 0xFFF);
#else
    /* Assuming 1:1 mapping between VID and VSI in UM+*/
    idx = vlan_params->vlan;
    vsi = idx;
#endif /* !CONFIG_VIRTUAL_PORT_SUPPORT */

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /* Retrive VSIT table entry */
    CBX_IF_ERROR_RETURN(
                soc_robo2_vsit_get(CBX_AVENGER_PRIMARY, idx, &ent_vsit));

    /* Update required fields in VSIT entry */
    if (CBX_VLAN_DROP_ULF & vlan_params->flags) {
            ent_vsit.drop_ulf = 1;
    }
    if (CBX_VLAN_DROP_SLF & vlan_params->flags) {
            ent_vsit.drop_slf = 1;
    }
    if (CBX_VLAN_DROP_DLF & vlan_params->flags) {
            ent_vsit.drop_mlf = 1;
    }
    if (CBX_VLAN_DROP_BCAST & vlan_params->flags) {
            ent_vsit.drop_bcast = 1;
    }
    ent_vsit.learn = (uint8)vlan_params->learn_mode;
    /* Add default stg to VSIT. cbx_stg_vlan_add function call at the end
     * adds the stgid given by user */
    ent_vsit.stgid = cbx_vlan_info.defl_stg;
    if (CBX_VLAN_WRED_ENABLE & vlan_params->flags) {
            ent_vsit.wred_ena = 1;
    }
    ent_vsit.iso_map = cbxi_vlan_pack_isomap(vlan_params);

    if (vlan_params->mcastid != 0) {
        ent_vsit.default_is_multicast = 1;
        ent_vsit.dlliid_dgid = (uint16)vlan_params->mcastid;
    } else if (vlan_params->ucastid != 0) {
        ent_vsit.default_is_multicast = 0;
        ent_vsit.dlliid_dgid = (uint16)vlan_params->ucastid;
    } else if (vlan_params->flags & CBX_VLAN_ALLOCATE_VSI) {
        /*No default flooding if MCASTID is not mentioned */
        ent_vsit.default_is_multicast = 0;
    }

    /* Add FID value based on vlan mode : IVL/ SVL  */
    CBX_IF_ERROR_RETURN(cbxi_vlan_learn_mode_get(&mode));
    if (mode == cbxSwitchVlanModeIndependent) {
        ent_vsit.fid = (uint16)idx;
    } else {
        ent_vsit.fid = 0;
    }

    /* Clear pg_map in VSIT entry */
    ent_vsit.pg_map = 0;

    for (i = 0; i < num_units; i++) {
        /* Write updated entry to VSIT table  */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i, idx, &ent_vsit, &status));
    }

    if (vlan_params->vlan != CBX_VID_INVALID) {
        /* Retrive VTCT table entry */
        CBX_IF_ERROR_RETURN(
                    soc_robo2_vtct_get(CBX_AVENGER_PRIMARY, idx, &ent_vtct));

        ent_vtct.vid = (uint16)vlan_params->vlan;

        /* Retrive PV2LI table entry */
        CBX_IF_ERROR_RETURN(
                soc_robo2_pv2li_get(
                        CBX_AVENGER_PRIMARY, vlan_params->vlan, &ent_pv2li));

        /* Update required fields in entry */
        ent_pv2li.vsi = vsi;

        /* Clear pg_map in PV2LI entry */
        ent_pv2li.pg_map = 0;

        for (i = 0; i < num_units; i++) {
            /* Write updated entry to VTCT table */
            CBX_IF_ERROR_RETURN(soc_robo2_vtct_set(i, idx, &ent_vtct, &status));

            /* Write updated entry to PV2LI table at index = VLAN */
            CBX_IF_ERROR_RETURN(soc_robo2_pv2li_set(
                                i, vlan_params->vlan, &ent_pv2li, &status));
        }
    }

    /* Update software structures */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_insert(vlan_params->vlan, vsi));
    cvi->count++;

    /*  VSIT index of created VLAN */
    *vsiid = (cbx_vlanid_t)vsi;
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    CBXI_VSI_MODE_SET(*vsiid, vsi);
    SHR_BITSET(cvi->vsi_bitmap, (int)vsi);
#endif

    /* Register vlanid in stg module for given stgid*/
    CBX_IF_ERROR_RETURN(cbx_stg_vlan_add(
                                vlan_params->stgid, *vsiid));

    return CBX_E_NONE;
}

/**
 * Function:
 * cbxi_vlan_destroy
 * Purpose:
 *  Main body of cbx_vlan_create;
 */

STATIC int
cbxi_vlan_destroy(cbx_vlanid_t vlanid )
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    cbx_switch_vlan_mode_t mode;
    uint8 num_units, i;
    uint32 reg_val, lpg;
    uint32 status = 0;
    cbx_vlan_t vsi;
    cbx_vlan_t vid;
    cbx_stgid_t stgid;
    vsit_t ent_vsit;
    vtct_t ent_vtct;
    pv2li_t ent_pv2li;


    /* Cannot destroy default VLAN */
    if (vlanid == cvi->defl_vid) {
        return CBX_E_BADID;
    }

    if (!cbxi_vlan_vid_lookup(vlanid)) {
        return CBX_E_NOT_FOUND;
    }

#ifdef CONFIG_CASCADE_MODE
    /* Delete cascade ports if applicable */
    CBX_IF_ERROR_RETURN(cbxi_vlan_cascade_ports_remove(vlanid));
#endif

    /* Obtain the stg that the given vlan is part of and delete it from stg
     * database */
    CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vlanid, &stgid));
    CBX_IF_ERROR_RETURN(cbxi_stg_vlan_destroy(stgid, vlanid));

    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));
    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_to_vid(vsi, &vid));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /* Retrive VTCT table entry */
    CBX_IF_ERROR_RETURN(
        soc_robo2_vtct_get(CBX_AVENGER_PRIMARY, (uint32)vsi, &ent_vtct));
    CBX_IF_ERROR_RETURN(REG_READ_CB_ETM_VTCT_IA_RDATA_PART1r(
                                                CBX_AVENGER_PRIMARY, &reg_val));

    reg_val = 0;
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_ETM_VTCT_IA_RDATA_PART1r(
                                                CBX_AVENGER_PRIMARY, &reg_val));

    ent_vsit.pg_map = 0;
    sal_memset(&ent_pv2li, 0, sizeof(pv2li_t));
    sal_memset(&ent_vtct, 0, sizeof(vtct_t));

    for (i = 0; i < num_units; i++) {
        /* Retrive VSIT table entry */
        CBX_IF_ERROR_RETURN(
            soc_robo2_vsit_get(i, (uint32)vsi, &ent_vsit));
        /* Remove any untagged ports from VLAN_UBMP */
        CBX_PBMP_ITER(ent_vsit.pg_map, lpg) {
            /* test if the LPG is added as Untagged in VTCT */
            if (((reg_val >> (lpg * 2)) & 0x3) == 0) {
                VLAN_UBMP_CLR(cvi, (lpg + (i * CBX_MAX_PORT_PER_UNIT)));
            }
        }
        ent_vsit.pg_map = 0;

        /* Write updated entry to VSIT table  */
        CBX_IF_ERROR_RETURN(
                soc_robo2_vsit_set(i, (uint32)vsi, &ent_vsit, &status));

        if (vid != CBX_VID_INVALID) {
            /* Write updated entry to PV2LI table  */
            CBX_IF_ERROR_RETURN(
                soc_robo2_pv2li_set(i, vid, &ent_pv2li, &status));
        }

        /* Write updated entry to VTCT table */
        CBX_IF_ERROR_RETURN(
                soc_robo2_vtct_set(i, (uint32)vsi, &ent_vtct, &status));
    }

    /* Flush all L2 entries in the given vlan if  IVL mode */
    CBX_IF_ERROR_RETURN(cbxi_vlan_learn_mode_get(&mode));
    if (mode == cbxSwitchVlanModeIndependent) {
        CBX_IF_ERROR_RETURN(cbx_l2_flush(vlanid));
    }

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    SHR_BITCLR(cvi->vsi_bitmap, (int)vsi);
#endif

    /* Update software structures */
    CBX_IF_ERROR_RETURN(cbxi_vlan_list_vid_remove(vid));
    cvi->count--;

    return CBX_E_NONE;
}

/**
 * Function:
 * cbxi_vlan_port_mode_set
 * Purpose:
 *  VLAN Port Modes: tagged, untagged modes
 */

STATIC int
cbxi_vlan_port_mode_set(int unit, uint32_t vsi,
                        cbxi_pgid_t local_pg, uint32_t mode)
{
    uint8 tca;
    pgt_t  ent_pgt;
    vtct_t ent_vtct;
    etct_t ent_etct;
    uint32 status = 0;
    pbmp_t pg_map = 0;
    cbx_port_type_t port_type = cbxPortTypeCustomer;
    cbxi_slic_rule_id_t rule = SLIC_DEFAULT;

    /* Obtain pepper field of local pg */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, local_pg, &ent_pgt));

    /* Add the port to proper SLICTCAM rule */
    port_type = ((ent_pgt.pepper & CBX_PORT_PEPPER_PORT_TYPE_MASK)
                >> CBX_PORT_PEPPER_PORT_TYPE_SHIFT);
    if (ent_pgt.pepper & CBX_PORT_PEPPER_TRUST_PORT_TCDP) {
        if (ent_pgt.pepper & CBX_PORT_PEPPER_EN_VIRTUAL_PORT) {
            if (port_type == cbxPortTypeProvider) {
                rule = SLIC_TRUST_PROVIDER_PORT_LIN;
            } else {
                rule = SLIC_TRUST_CUSTOMER_PORT_LIN;
            }
        } else {
            if (port_type == cbxPortTypeCustomer) {
                rule = SLIC_TRUST_CUSTOMER_PORT_VLAN;
            } else if (port_type == cbxPortTypeProvider) {
                rule = SLIC_TRUST_PROVIDER_PORT_VLAN;
            } else {
                rule = SLIC_TRUST_PORT_BASED_VLAN;
            }
        }
    } else {
        if (ent_pgt.pepper & CBX_PORT_PEPPER_EN_VIRTUAL_PORT) {
            if (port_type == cbxPortTypeProvider) {
                rule = SLIC_PROVIDER_PORT_LIN;
            } else {
                rule = SLIC_CUSTOMER_PORT_LIN;
            }
        } else {
            if ((port_type == cbxPortTypeCustomer) ||
                (port_type == cbxPortTypePortVLAN)) {
                rule = SLIC_CUSTOMER_PORT_VLAN;
            } else if (port_type == cbxPortTypeProvider) {
                rule = SLIC_PROVIDER_PORT_VLAN;
            } else {
                rule = SLIC_PORT_BASED_VLAN;
            }
        }
    }

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, rule, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, local_pg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, rule, pg_map));

    /* Configure egress edit directives */
    if (CBX_VLAN_PORT_MODE_UNTAGGED == mode) {
        tca = 0;
    } else {
        tca = 1;
    }

    /* Retrive VTCT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_vtct_get(unit, vsi, &ent_vtct));

    switch (local_pg) {
        case 0 :
            ent_vtct.tca0 = tca;
            break;
        case 1 :
            ent_vtct.tca1 = tca;
            break;
        case 2 :
            ent_vtct.tca2 = tca;
            break;
        case 3 :
            ent_vtct.tca3 = tca;
            break;
        case 4 :
            ent_vtct.tca4 = tca;
            break;
        case 5 :
            ent_vtct.tca5 = tca;
            break;
        case 6 :
            ent_vtct.tca6 = tca;
            break;
        case 7 :
            ent_vtct.tca7 = tca;
            break;
        case 8 :
            ent_vtct.tca8 = tca;
            break;
        case 9 :
            ent_vtct.tca9 = tca;
            break;
        case 10 :
            ent_vtct.tca10 = tca;
            break;
        case 11 :
            ent_vtct.tca11 = tca;
            break;
        case 12 :
            ent_vtct.tca12 = tca;
            break;
        case 13 :
            ent_vtct.tca13 = tca;
            break;
        case 14 :
            ent_vtct.tca14 = tca;
            break;
        case 15 :
            ent_vtct.tca15 = tca;
            break;
        default :
            return CBX_E_INTERNAL;
    }

    /* Write updated entry to VTCT table */
    CBX_IF_ERROR_RETURN(soc_robo2_vtct_set(unit, vsi, &ent_vtct, &status));


    CBX_IF_ERROR_RETURN(soc_robo2_etct_get(unit, local_pg, &ent_etct));
    if (port_type == cbxPortTypeCustomer) {
        ent_etct.opcode = cbxiTagCtpid0;
    } else if (port_type == cbxPortTypeProvider) {
        ent_etct.opcode = cbxiTagStpid0;
    }
    CBX_IF_ERROR_RETURN(soc_robo2_etct_set(unit, local_pg, &ent_etct, &status));

    return CBX_E_NONE;
}

/**
 * Function:
 * cbxi_vlan_port_mode_get
 * Purpose:
 *  Retrieve VLAN Port Modes: tagged, untagged modes
 */

STATIC int
cbxi_vlan_port_mode_get(int unit, uint32_t vsi,
                        cbxi_pgid_t local_pg, uint32_t *mode)
{
    uint8  tca;
    vtct_t ent_vtct;

    /* Retrive VTCT table entry to get port mode */
    CBX_IF_ERROR_RETURN(soc_robo2_vtct_get(unit, vsi, &ent_vtct));
    switch (local_pg) {
        case 0 :
            tca = ent_vtct.tca0;
            break;
        case 1 :
            tca = ent_vtct.tca1;
            break;
        case 2 :
            tca = ent_vtct.tca2;
            break;
        case 3 :
            tca = ent_vtct.tca3;
            break;
        case 4 :
            tca = ent_vtct.tca4;
            break;
        case 5 :
            tca = ent_vtct.tca5;
            break;
        case 6 :
            tca = ent_vtct.tca6;
            break;
        case 7 :
            tca = ent_vtct.tca7;
            break;
        case 8 :
            tca = ent_vtct.tca8;
            break;
        case 9 :
            tca = ent_vtct.tca9;
            break;
        case 10 :
            tca = ent_vtct.tca10;
            break;
        case 11 :
            tca = ent_vtct.tca11;
            break;
        case 12 :
            tca = ent_vtct.tca12;
            break;
        case 13 :
            tca = ent_vtct.tca13;
            break;
        case 14 :
            tca = ent_vtct.tca14;
            break;
        case 15 :
            tca = ent_vtct.tca15;
            break;
        default :
           return CBX_E_INTERNAL;
    }
    if (tca == 0 ) {
        *mode = CBX_VLAN_PORT_MODE_UNTAGGED;
    } else {
        *mode = CBX_VLAN_PORT_MODE_TAGGED;
    }

    return CBX_E_NONE;
}

/**
 * Function:
 * cbxi_vlan_pg_map_set
 * Purpose:
 *  Configure pg_map in VSIT and VTCT
 *  Assumes check is already done to handle conflicting configuration
 */

STATIC int
cbxi_vlan_pg_map_set(int unit, cbx_vlanid_t vlanid,
                    cbxi_pgid_t local_pg, uint32_t  mode, uint32_t flags)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    vsit_t ent_vsit;
    pv2li_t ent_pv2li;
    uint32 idx;
    uint32 status = 0;
    cbx_vlan_t vsi;
    cbx_vlan_t vid;
    pbmp_t pg_map;
#ifndef CONFIG_VLAN_PVID
    cbx_port_t port;
#endif
    cbxi_pgid_t csd_lpg;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Get Cascade port LPG if in Cascaded mode */
        CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &status));
        soc_CB_PQM_CASCADEr_field_get(unit, &status, LSPGIDf, &csd_lpg);
        status = 0;
    }

    CBX_IF_ERROR_RETURN(
        cbxi_vlan_vsit_index_get(vlanid, &vsi));
    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_to_vid(vsi, &vid));

    idx = (uint32_t)vsi;

    /* Retrive VSIT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, idx, &ent_vsit));

    pg_map = (pbmp_t)ent_vsit.pg_map;

#ifndef CONFIG_VLAN_PVID
    port = local_pg + (unit * CBX_MAX_PORT_PER_UNIT);
#endif
    /* Update pg_map according to flags */
    if (flags & CBX_VLAN_PORT_ADD) {
#ifndef CONFIG_VLAN_PVID
        if (!((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && (csd_lpg == local_pg)) ||
                                (flags & CBX_VLAN_PORT_LIF))) {
            /* Skip the check if cascade port */
            if ((CBX_VLAN_PORT_MODE_UNTAGGED == mode) &&
                        (VLAN_UBMP_TST(cvi, port))){
                 LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("VLAN port update API: port %d is already present in\
                                another VLAN in untagged mode\n"), port));
                return CBX_E_PARAM;
            }
        }
#endif
        CBX_PBMP_PORT_ADD(pg_map, local_pg);
    } else if (flags & CBX_VLAN_PORT_REMOVE) {
        CBX_PBMP_PORT_REMOVE(pg_map, local_pg);
    }

    /* Update required fields in VSIT entry */
    ent_vsit.pg_map = (uint16)pg_map;

    /* Write updated entry to VSIT table  */
    CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(unit, idx, &ent_vsit, &status));

    if (!(flags & CBX_VLAN_PORT_LIF) && (vid != CBX_VID_INVALID)) {
        /* Retrive PV2LI table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_pv2li_get(unit, vid, &ent_pv2li));
        /* Update required fields in entry */
        ent_pv2li.pg_map = (uint16)pg_map;
        /* Write updated entry to PV2LI table  */
        CBX_IF_ERROR_RETURN(soc_robo2_pv2li_set(
                                        unit, vid, &ent_pv2li, &status));
    }

    if (!((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && (csd_lpg == local_pg)) ||
                                        (flags & CBX_VLAN_PORT_LIF))) {
        /* Update only for non cascade ports */
        /* Update untagged port bit map */
        if (flags & CBX_VLAN_PORT_REMOVE) {
            CBX_IF_ERROR_RETURN(cbxi_vlan_port_mode_get(
                                            unit, idx, local_pg, &mode));
            if (CBX_VLAN_PORT_MODE_UNTAGGED == mode) {
                VLAN_UBMP_CLR(cvi, (local_pg + (unit * CBX_MAX_PORT_PER_UNIT)));
            }
            mode = CBX_VLAN_PORT_MODE_UNTAGGED;
        } else if (CBX_VLAN_PORT_MODE_UNTAGGED == mode) {
            VLAN_UBMP_SET(cvi, (local_pg + (unit * CBX_MAX_PORT_PER_UNIT)));
        }
    }

    /* Update VLAN port mode */
    CBX_IF_ERROR_RETURN(cbxi_vlan_port_mode_set(unit, idx, local_pg, mode));

    /* Add cascade port only when ports are present on both Avengers */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Read partner Avg's pgmap */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(
                        CBXI_CASC_PARTNER_UNIT(unit), idx, &ent_vsit));

        if ((flags & CBX_VLAN_PORT_ADD) && (ent_vsit.pg_map != 0)) {
            if (!CBX_PBMP_MEMBER(pg_map, csd_lpg)) {
                /* Add cascade ports */
                for (unit = 0; unit < 2; unit++) {
                    CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &status));
                    soc_CB_PQM_CASCADEr_field_get(unit, &status, LSPGIDf, &csd_lpg);
                    status = 0;

                    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, idx, &ent_vsit));
                    CBX_PBMP_PORT_ADD(ent_vsit.pg_map, csd_lpg);
                    /* Write updated entry to VSIT table  */
                    CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(unit, idx, &ent_vsit, &status));

                    CBX_IF_ERROR_RETURN(soc_robo2_pv2li_get(unit, idx, &ent_pv2li));
                    CBX_PBMP_PORT_ADD(ent_pv2li.pg_map, csd_lpg);
                    CBX_IF_ERROR_RETURN(soc_robo2_pv2li_set(unit, idx, &ent_pv2li, &status));
                }
            }
        } else if ((flags & CBX_VLAN_PORT_REMOVE)) {
            CBX_PBMP_PORT_REMOVE(pg_map, csd_lpg);
            if (pg_map == 0) {
                /* Remove cascade ports */
                for (unit = 0; unit < 2; unit++) {
                    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, idx, &ent_vsit));
                    CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &status));
                    soc_CB_PQM_CASCADEr_field_get(unit, &status, LSPGIDf, &csd_lpg);
                    status = 0;

                    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, idx, &ent_vsit));
                    CBX_PBMP_PORT_REMOVE(ent_vsit.pg_map, csd_lpg);
                    /* Write updated entry to VSIT table  */
                    CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(unit, idx, &ent_vsit, &status));

                    CBX_IF_ERROR_RETURN(soc_robo2_pv2li_get(unit, idx, &ent_pv2li));
                    CBX_PBMP_PORT_REMOVE(ent_pv2li.pg_map, csd_lpg);
                    CBX_IF_ERROR_RETURN(soc_robo2_pv2li_set(unit, idx, &ent_pv2li, &status));
                }
            }
        }
    }

    return CBX_E_NONE;
}


#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
#define CBXI_VSI_INVALID 0xFFF
/**
 * Function:
 * cbxi_lif_port_update
 * Purpose:
 *  Main body of Logical interface port add/remove
 *  Assumes that VLAN and port check are already done
 */

STATIC int
cbxi_lif_port_update(cbx_vlanid_t vlanid,
                    cbx_portid_t portid, uint32_t  mode, uint32_t flags)
{
    uint8 num_units, unit;
    lin2vsi_t ent_lin2vsi;
    dli2ldpg_t ent_dli2ldpg;
    int linid;
    cbx_vlan_t vsi;
    cbxi_pgid_t local_pg = CBXI_PGID_INVALID;
    uint32_t status = 0;

    linid = CBX_PORTID_VLAN_PORT_ID_GET(portid);
    if (linid < 0) {
        return CBX_E_PARAM;
    }

    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    flags |= CBX_VLAN_PORT_LIF;
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_get(unit, linid, &ent_lin2vsi));
        if (flags & CBX_VLAN_PORT_ADD) {
            ent_lin2vsi.vsi = vsi;
        } else if (flags & CBX_VLAN_PORT_REMOVE) {
            ent_lin2vsi.vsi = CBXI_VSI_INVALID;
        }
        CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_set(
                                        unit, linid, &ent_lin2vsi, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(unit, linid, &ent_dli2ldpg));
        local_pg = ent_dli2ldpg.ldpgid;
        CBX_IF_ERROR_RETURN(cbxi_vlan_pg_map_set(
                                        unit, vlanid, local_pg, mode, flags));

    }
#ifdef CBX_MANAGED_MODE
    CBX_IF_ERROR_RETURN(cbxi_lif_vsi_set(linid, vsi, mode));
#endif

    flags &= (~CBX_VLAN_PORT_LIF);

    return CBX_E_NONE;
}
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */

/**
 * Function:
 * cbxi_vlan_port_update
 * Purpose:
 *  Main body of cbx_vlan_port_add and cbx_vlan_port_remove
 */

STATIC int
cbxi_vlan_port_update(cbx_vlanid_t vlanid,
                    cbx_portid_t portid, uint32_t  mode, uint32_t flags)
{
    int unit = CBX_AVENGER_PRIMARY;
    cbx_portid_t lagid;
    cbx_port_t port;
    pbmp_t lag_map;
    pbmp_t pp_map;
    cbx_vlan_t vsi;
    cbxi_pgid_t local_pg = CBXI_PGID_INVALID;
    cbxi_pgid_t remote_pg = CBXI_PGID_INVALID;
    int rv = CBX_E_NONE;
    cbx_lag_info_t lag_info;
#ifndef CONFIG_VLAN_PVID
    cbx_port_params_t port_params;
#endif

   /* Check if VLAN exists */
    if(!cbxi_vlan_vid_lookup(vlanid)) {
         return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));

    /* Check if the portid is of type LAG */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        /* Obtain LPG values on Primary and Secondary Avengers corresponding
         * to LAG id indicated by portid passed*/

        CBX_IF_ERROR_RETURN(cbxi_vlan_lag_bit_map_get(vsi, &lag_map));
        rv = CBX_PORTID_TRUNK_GET(portid);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_FSAL_VLAN,
                (BSL_META("VLAN port update API:Invalid LAG id \n")));
            return CBX_E_PORT;
        } else {
            lagid = rv;
        }
        if (flags & CBX_VLAN_PORT_ADD) {
            if (CBX_PBMP_MEMBER(lag_map, lagid)) {
                LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("VLAN port update API: LAG id %d is \
                        already present in vlan %d\n"), portid, vlanid));
                return CBX_E_EXISTS;
            }
        } else if (flags & CBX_VLAN_PORT_REMOVE) {
            if (!CBX_PBMP_MEMBER(lag_map, lagid)) {
                LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("VLAN port update API: LAG id %d is \
                        not present in vlan %d\n"), portid, vlanid));
                return CBX_E_NOT_FOUND;
            }
        }

        /* Obtain LPG values on Primary and Secondary Avengers corresponding
         * to LAG id indicated by portid passed*/

        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        local_pg = lag_info.lpgid;
        remote_pg = lag_info.rlpgid;
        if ((local_pg == CBXI_PGID_INVALID) &&
                (remote_pg == CBXI_PGID_INVALID)) {
            return CBX_E_PORT;
        }
        if (local_pg != CBXI_PGID_INVALID) {
            unit = CBX_AVENGER_PRIMARY;
            CBX_IF_ERROR_RETURN(cbxi_vlan_pg_map_set(
                                        unit, vlanid, local_pg, mode, flags));
        }
        if (remote_pg != CBXI_PGID_INVALID) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                unit = CBX_AVENGER_SECONDARY;
                CBX_IF_ERROR_RETURN(cbxi_vlan_pg_map_set(
                                unit, vlanid, remote_pg, mode, flags));
            } else {
                return CBX_E_PARAM;
            }
        }
        /* Update LAG id in lag_map of SW database */
        if (flags & CBX_VLAN_PORT_ADD) {
            CBX_PBMP_PORT_ADD(lag_map, lagid);
        } else if (flags & CBX_VLAN_PORT_REMOVE) {
            CBX_PBMP_PORT_REMOVE(lag_map, lagid);
        }
        CBX_IF_ERROR_RETURN(cbxi_vlan_lag_bit_map_set(vsi, lag_map));
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    /* Check if the portid is VLAN port */
    } else if (CBX_PORTID_IS_VLAN_PORT(portid)) {
        CBX_IF_ERROR_RETURN(cbxi_lif_port_update(vlanid, portid, mode, flags));
        return CBX_E_NONE;
#endif

    } else {
        /* portid is type other than LAG:
         * physical port no., local port (physical port in portid format) */
        /* vlan_port (MM feature) needs to be handled separately */

    /* If port is a member of the trunk then we need to return error */
        rv = cbxi_robo2_lag_member_check(portid, &lagid);
        if (CBX_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_FSAL_VLAN,
                (BSL_META("VLAN port update API: Port %d is member of a LAG %d.\
                Cannot add port separately to the VLAN\n"), portid, lagid));
            return CBX_E_PARAM;
        }
        /* Validate the port number passed, and obtain physical port
         * corresponding to given portid */
        rv = cbxi_robo2_port_validate(portid, &port, &local_pg, &unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("VLAN port update API:Invalid port \n")));
            return rv;
        }

        CBX_IF_ERROR_RETURN(cbxi_vlan_port_bit_map_get(unit, vsi, &pp_map));
        if (flags & CBX_VLAN_PORT_ADD) {
            if (CBX_PBMP_MEMBER(pp_map, port)) {
                LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("VLAN port update API: port %d is \
                        already present in vlan %d\n"), portid, vlanid));
                return CBX_E_EXISTS;
            }
        } else if (flags & CBX_VLAN_PORT_REMOVE) {
            if (!CBX_PBMP_MEMBER(pp_map, port)) {
                LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("VLAN port update API: port %d is \
                        not present in vlan %d\n"), portid, vlanid));
                return CBX_E_NOT_FOUND;
            }
        }

        CBX_IF_ERROR_RETURN(cbxi_vlan_pg_map_set(
                                        unit, vlanid, local_pg, mode, flags));

        /* Update port in pp_map of SW database */
        if (flags & CBX_VLAN_PORT_ADD) {
            CBX_PBMP_PORT_ADD(pp_map, port);
        } else if (flags & CBX_VLAN_PORT_REMOVE) {
            CBX_PBMP_PORT_REMOVE(pp_map, port);
        }
        CBX_IF_ERROR_RETURN(cbxi_vlan_port_bit_map_set(unit, vsi, pp_map));
    }


#ifndef CONFIG_VLAN_PVID
    if ((flags & CBX_VLAN_PORT_ADD) &&
                        (CBX_VLAN_PORT_MODE_UNTAGGED == mode)) {
        CBX_IF_ERROR_RETURN(cbx_port_info_get(portid, &port_params));
        if (port_params.port_type != cbxPortTypeCascade) {
            port_params.default_vid = vlanid;
            CBX_IF_ERROR_RETURN(cbx_port_info_set(portid, &port_params));
        }
    }
#endif

    return CBX_E_NONE;
}

/**
 * Function:
 * cbxi_vlan_params_get
 * Purpose:
 *  Main body of cbx_vlan_get
 */

STATIC int
cbxi_vlan_params_get(cbx_vlanid_t vlanid, cbx_vlan_params_t *vlan_params)
{
    vsit_t ent_vsit;
    cbx_vlan_t vsi;

    if (vlan_params == NULL) {
        return CBX_E_EMPTY;
    }

    sal_memset(vlan_params, 0, sizeof(cbx_vlan_params_t));

    /* Check if VLAN exists */
    if(!cbxi_vlan_vid_lookup(vlanid)) {
         return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));

    /* Retrive VSIT table entry */
    CBX_IF_ERROR_RETURN(
        soc_robo2_vsit_get(CBX_AVENGER_PRIMARY, (uint32)vsi, &ent_vsit));

    if (ent_vsit.drop_ulf == 1) {
        vlan_params->flags |= CBX_VLAN_DROP_ULF;
    }
    if (ent_vsit.drop_slf == 1) {
        vlan_params->flags |= CBX_VLAN_DROP_SLF;
    }
    if (ent_vsit.drop_mlf == 1) {
        vlan_params->flags |= CBX_VLAN_DROP_DLF;
    }
    if (ent_vsit.drop_bcast == 1) {
        vlan_params->flags |= CBX_VLAN_DROP_BCAST;
    }
    if (ent_vsit.wred_ena == 1) {
        vlan_params->flags |= CBX_VLAN_WRED_ENABLE;
    }

    vlan_params->vlan = vlanid;
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_to_vid(vsi, &vlan_params->vlan));
    if (vlan_params->vlan == CBX_VID_INVALID) {
        vlan_params->flags = CBX_VLAN_ALLOCATE_VSI;
        vlan_params->vlan = 0;
    }
#endif

    vlan_params->learn_mode = ent_vsit.learn;
    vlan_params->stgid = ent_vsit.stgid;

    if (ent_vsit.default_is_multicast == 1) {
        vlan_params->mcastid = ent_vsit.dlliid_dgid;
    } else {
        vlan_params->ucastid = ent_vsit.dlliid_dgid;
    }

    cbxi_vlan_unpack_isomap(ent_vsit.iso_map, vlan_params);

    return CBX_E_NONE;
}

/**
 * Function:
 * cbxi_vlan_params_set
 * Purpose:
 *  Main body of cbx_vlan_set
 */

STATIC int
cbxi_vlan_params_set(cbx_vlanid_t vlanid, cbx_vlan_params_t vlan_params)
{
    uint8 num_units, i;
    uint32 status = 0;
    vsit_t ent_vsit;
    cbx_stgid_t cur_stgid;
    cbx_vlan_t vsi;

    /* Check if VLAN exists */
    if(!cbxi_vlan_vid_lookup(vlanid)) {
         return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (i = 0; i < num_units; i++) {
        /* Retrive VSIT table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, (uint32)vsi, &ent_vsit));

        /* Update required fields in VSIT entry */
        if (CBX_VLAN_DROP_ULF & vlan_params.flags) {
                ent_vsit.drop_ulf = 1;
        } else {
                ent_vsit.drop_ulf = 0;
        }
        if (CBX_VLAN_DROP_SLF & vlan_params.flags) {
                ent_vsit.drop_slf = 1;
        } else {
                ent_vsit.drop_slf = 0;
        }
        if (CBX_VLAN_DROP_DLF & vlan_params.flags) {
                ent_vsit.drop_mlf = 1;
        } else {
                ent_vsit.drop_mlf = 0;
        }
        if (CBX_VLAN_DROP_BCAST & vlan_params.flags) {
                ent_vsit.drop_bcast = 1;
        } else {
                ent_vsit.drop_bcast = 0;
        }
        if (CBX_VLAN_WRED_ENABLE & vlan_params.flags) {
                ent_vsit.wred_ena = 1;
        } else {
                ent_vsit.wred_ena = 0;
        }
        ent_vsit.learn = (uint8)vlan_params.learn_mode;
        ent_vsit.iso_map = cbxi_vlan_pack_isomap(&vlan_params);

        if (vlan_params.mcastid != 0) {
            ent_vsit.default_is_multicast = 1;
            ent_vsit.dlliid_dgid = (uint16)vlan_params.mcastid;
        } else if (vlan_params.ucastid != 0) {
            ent_vsit.default_is_multicast = 0;
            ent_vsit.dlliid_dgid = (uint16)vlan_params.ucastid;
        } else if (vlan_params.flags & CBX_VLAN_ALLOCATE_VSI) {
            /*No default flooding if MCASTID is not mentioned */
            ent_vsit.default_is_multicast = 0;
            ent_vsit.dlliid_dgid = 0;
        } else {
            ent_vsit.default_is_multicast = 1;
            ent_vsit.dlliid_dgid = 0;
        }

        /* Write updated entry to VSIT table  */
        CBX_IF_ERROR_RETURN(
                soc_robo2_vsit_set(i, (uint32)vsi, &ent_vsit, &status));
    }

    /* Call cbx_stg_vlan_add function to update stg if it is different from
     * cur_stgid*/
    CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vlanid, &cur_stgid));
    if (vlan_params.stgid != cur_stgid) {
        /* Register vlanid in stg module for given vlan_params->stgid */
    CBX_IF_ERROR_RETURN(cbx_stg_vlan_add(
                                vlan_params.stgid, vlanid));

    }
    return CBX_E_NONE;
}


/**************************************************************************
 *                 VLAN FSAL API IMPLEMENTATION                           *
 **************************************************************************/


/**
 * Initialize vlan entry parameters.
 *
 * @param  vlan_params  parameter object
 */

void cbx_vlan_params_t_init(cbx_vlan_params_t *vlan_params)
{
    uint8 i;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return;
    }
#endif /* CONFIG_PORT_EXTENDER */

    if (vlan_params != NULL) {
        sal_memset(vlan_params, 0, sizeof(cbx_vlan_params_t));
        /*
         * Do not enable Wred by default till atleast WRED feature is complete
         *
         * vlan_params->flags = CBX_VLAN_WRED_ENABLE;
         */
        vlan_params->vlan = CBX_VID_INVALID;
        vlan_params->learn_mode = cbxVlanLearnModeMarkReady;
        vlan_params->stgid = cbx_vlan_info.defl_stg;
        for (i = 0; i < CBX_VLAN_ISOLATION_MAX; i++) {
            vlan_params->isolation[i] = 0;
        }
    }
    return;
}

/**
 * Create a vlan
 * This routine is used to create a vlan.
 *
 * @param vlan_params   (IN)  Vlan parameters.
 * @param vlanid        (OUT) Handle of the vlan created
 *                            CBX_VLAN_INVALID: Vlan could not be created
 *
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_vlan_create(cbx_vlan_params_t *vlan_params, cbx_vlanid_t *vsiid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    int rv;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */

    CHECK_VLAN_INIT(cvi);
    if (!(vlan_params->flags & CBX_VLAN_ALLOCATE_VSI)) {
        CHECK_VID(vlan_params->vlan);
    }

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_create()..\n")));

    if ((rv = cbxi_vlan_create(vlan_params, vsiid)) < 0) {
        *vsiid = CBX_VLAN_INVALID;
        return rv;
    }

    return CBX_E_NONE;
}

/**
 * Destroy a vlan previously created by cbx_vlan_create()
 *
 * @param vlanid    Handle of the vlan to be destroyed
 *
 * @retval 0    : Success CBX_E_NONE
 * @retval < 0  : Failure
 *
 * @note This routine destroys a previously created vlan.
 */

int cbx_vlan_destroy(cbx_vlanid_t vlanid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    int rv;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_destroy()..\n")));

    rv = cbxi_vlan_destroy(vlanid);

    return rv;

}

/**
 * Vlan Destroy All
 * This routine is used to destroy all Vlans that have been created.
 * The special Vlan vlanid=1 is not affected.
 *
 * @param void          n/a
 *
 * @return return code
 * @retval 0 success CBX_E_NONE
 */

int cbx_vlan_destroy_all(void)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    cbx_vlanid_t vlanid;
#ifndef CBX_MANAGED_MODE /* UM+ mode */
    int count = 0;
#endif /* !CBX_MANAGED_MODE */

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_destroy()..\n")));
#ifdef CBX_MANAGED_MODE
    if ((vlanid = cbxi_vlist_vlan_1st()) != CBX_VLAN_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_destroy(vlanid));
    }
#else /* UM+ mode */
    count = cvi->count;
    VLAN_BITMAP_ITER(cvi, vlanid) {
        if (vlanid != CBX_VLAN_DEFAULT) {
            CBX_IF_ERROR_RETURN(cbxi_vlan_destroy(vlanid));
        }
        count--;
        if (!count) {
            break;
        }
    }
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/**
 * VLAN Information get
 * This routine is used to get the parameters of a Vlan.
 *
 * @param vlanid   VLAN Identifier
 * @parm  vlan_params Vlan parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_get(cbx_vlanid_t vlanid, cbx_vlan_params_t *vlan_params)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_get()..\n")));

    CBX_IF_ERROR_RETURN(cbxi_vlan_params_get(vlanid, vlan_params));

    return CBX_E_NONE;
}

/**
 * VLAN Information set
 * This routine is used to set the parameters of a Vlan.
 *
 * @param vlanid   VLAN Identifier
 * @parm  vlan_params Vlan parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_set(cbx_vlanid_t vlanid, cbx_vlan_params_t vlan_params)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_set()..\n")));

    CBX_IF_ERROR_RETURN(cbxi_vlan_params_set(vlanid, vlan_params));

    return CBX_E_NONE;
}



/**
 * Add a Port to a VLAN
 * This routine is used to add a port (cbx_portid) to a VLAN.
 * The port can be a Physical Port or a Port Group (LAG)
 *
 * @param vlanid        VLAN Identifier
 * @param portid        Port Identifier
 * @param mode          tagged, untagged modes
 *
 * @return return code
 * @retval 0 success : CBX_E_NONE
 */

int cbx_vlan_port_add(cbx_vlanid_t vlanid, cbx_portid_t portid, uint32_t mode )
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    int rv;
    uint32_t flags;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }

#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_port_add()..\n")));
#ifdef CONFIG_CASCADED_MODE
    rv = cbxi_cascade_port_check(portid);
    if (CBX_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_FSAL_VLAN,
                (BSL_META("FSAL API : cbx_vlan_port_add() \
                        cannot add cascade port %d\n"), portid));
        return CBX_E_PORT;
    }
#endif /* CONFIG_CASCADED_MODE */

    flags = CBX_VLAN_PORT_ADD;

    rv = cbxi_vlan_port_update(vlanid, portid, mode, flags);

    return rv;
}


/**
 * Delete a Port from a VLAN
 * This routine is used to remove a port (cbx_portid) from a VLAN.
 * The port can be a Physical Port or a Port Group (LAG)
 *
 * @param vlanid        VLAN Identifier
 * @param portid        Port Identifier
 *
 * @return return code
 * @retval 0 success : CBX_E_NONE
 */

int cbx_vlan_port_remove(cbx_vlanid_t vlanid, cbx_portid_t portid )
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    int rv;
    uint32_t mode;
    uint32_t flags;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);
    CHECK_VID(vlanid);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_port_remove()..\n")));
#ifdef CONFIG_CASCADED_MODE
    rv = cbxi_cascade_port_check(portid);
    if (CBX_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_FSAL_VLAN,
                (BSL_META("FSAL API : cbx_vlan_port_remove() \
                        cannot remove cascade port %d\n"), portid));
        return CBX_E_PORT;
    }
#endif /* CONFIG_CASCADED_MODE */
    flags = CBX_VLAN_PORT_REMOVE;
    mode = CBX_VLAN_PORT_MODE_UNTAGGED;

    rv = cbxi_vlan_port_update(vlanid, portid, mode, flags);

    return rv;
}

/**
 * Get an array of Ports on a VLAN
 * This routine is used to get an array of ports (cbx_portid) on a VLAN.
 * The ports can be a Physical Port or a Port Group (LAG)
 *
 * @param vlanid        VLAN Identifier
 * @param array_max     Maximum number of elements in the array
 * @param portid_array  (OUT) Array of portids
 * @param array_size    (OUT) Actual number of elements in the array
 *
 * @return return code
 * @retval 0 success : CBX_E_NONE
 */

int cbx_vlan_port_get(cbx_vlanid_t vlanid, int array_max,
                      cbx_portid_t *portid_array, uint32_t *mode_array,
                      int *array_size)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    uint8 num_units = 1;
    uint8 unit;
    pbmp_t port_map = 0;
    pbmp_t lag_map = 0;
    cbx_portid_t portid;
    cbx_vlan_t vsi;
    cbx_port_t port;
    int count = 0;
    cbx_lag_info_t lag_info;
    uint32_t pgid;

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        return CBX_E_UNAVAIL;
    }
#endif /* CONFIG_PORT_EXTENDER */
    CHECK_VLAN_INIT(cvi);

    LOG_INFO(BSL_LS_FSAL_VLAN,
            (BSL_META("FSAL API : cbx_vlan_port_get()..\n")));

    /* Check if VLAN exists */
    if(!cbxi_vlan_vid_lookup(vlanid)) {
         return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));

    if (array_max < 1) {
        return CBX_E_PARAM;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    sal_memset(portid_array, 0, ((sizeof(cbx_portid_t) * array_max)));
    sal_memset(mode_array, 0, ((sizeof(uint32_t) * array_max)));

#ifdef CBX_MANAGED_MODE
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    *array_size = 0;
    count = 0;
    CBX_IF_ERROR_RETURN(cbxi_lif_vsi_port_lookup(vsi, array_max, portid_array,
                                                    mode_array, array_size));
    while (count < *array_size) {
        portid_array++;
        mode_array++;
        count++;
    }

    if (count) {
        /* In current implementation,
         * VSIs cannot have a combination of LIF and normal ports */
        return CBX_E_NONE;
    }
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */
#endif /* CBX_MANAGED_MODE */

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_port_bit_map_get(
                                        unit, vsi, &port_map));
        CBX_PBMP_ITER(port_map, port) {
            cbx_port_lookup_t lu;
            lu.port_index = port + (unit * CBX_MAX_PORT_PER_UNIT);
#ifdef CONFIG_CASCADED_MODE
            if (CBX_SUCCESS(cbxi_cascade_port_check(lu.port_index))) {
                /* Skip cascade ports */
                continue;
            }
#endif /* CONFIG_CASCADED_MODE */
            if (CBX_E_NONE == cbx_port_lookup(&lu, portid_array)) {
                cbxi_pgid_t _local_pg = CBXI_PGID_INVALID;

                portid_array++;

                CBX_IF_ERROR_RETURN(cbxi_robo2_port_to_lpg(unit, port, &_local_pg));
                CBX_IF_ERROR_RETURN(cbxi_vlan_port_mode_get(
                                            unit, vsi, _local_pg, mode_array));
                mode_array++;

                count++;
                if (count >= array_max) {
                    *array_size = count;
                    return CBX_E_NONE;
                }
            }
        }
    }

    CBX_IF_ERROR_RETURN(cbxi_vlan_lag_bit_map_get(vsi, &lag_map));
    CBX_PBMP_ITER(lag_map, port) {
        CBX_PORTID_TRUNK_SET(portid, port);
#ifdef CONFIG_CASCADED_MODE
        if (CBX_SUCCESS(cbxi_cascade_port_check(portid))) {
            /* Skip cascade LAG */
            continue;
        }
#endif /* CONFIG_CASCADED_MODE */

        /* Obtain lpgid and hence mode for given LAG */
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));

        if (lag_info.lpgid != CBXI_PGID_INVALID) {
            unit = CBX_AVENGER_PRIMARY;
            pgid = lag_info.lpgid;
        } else if (lag_info.rlpgid != CBXI_PGID_INVALID) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                unit = CBX_AVENGER_SECONDARY;
                pgid = lag_info.rlpgid;
            } else {
                return CBX_E_PARAM;
            }
        } else {
            return CBX_E_PARAM;
        }
        CBX_IF_ERROR_RETURN(cbxi_vlan_port_mode_get(
                                   unit , vsi, pgid, mode_array));

        *portid_array = portid;
        portid_array++;
        mode_array++;

        count++;
        if (count >= array_max) {
            *array_size = count;
            return CBX_E_NONE;
        }
    }

    *array_size = count;

    return CBX_E_NONE;

}

#ifdef CONFIG_VLAN_PVID
int cbx_vlan_port_pvid_set( cbx_vlanid_t vlanid,
                            cbx_portid_t portid)
{
    int unit = CBX_AVENGER_PRIMARY;
    cbx_port_params_t port_params;
    cbx_port_t port;
    cbxi_pgid_t local_pg = CBXI_PGID_INVALID;
    int rv;

    /* Check if VLAN exists */
    if(!cbxi_vlan_vid_lookup(vlanid)) {
        return CBX_E_NOT_FOUND;
    }

    /* Validate the port number passed, and obtain physical port
     * corresponding to given portid */
    rv = cbxi_robo2_port_validate(portid, &port, &local_pg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_VLAN,
            (BSL_META("VLAN port pvid set API:Invalid port \n")));
        return rv;
    }

    CBX_IF_ERROR_RETURN(cbx_port_info_get(portid, &port_params));
    if (port_params.port_type != cbxPortTypeCascade) {
        port_params.default_vid = vlanid;
        CBX_IF_ERROR_RETURN(cbx_port_info_set(portid, &port_params));
    }

    return CBX_E_NONE;
}
#endif

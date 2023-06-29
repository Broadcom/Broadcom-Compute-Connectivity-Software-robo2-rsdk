/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     stg.c
 * * Purpose:
 * *     Robo2 stg module
 * *
 * * Note:
 * *  Implementation of RSTP/MSTP.
 * *
 * */


#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal/stg.h>
#include <fsal/switch.h>
#include <fsal_int/stg.h>
#include <fsal_int/types.h>
#include <fsal_int/vlan.h>
#include <fsal_int/port.h>
#include <fsal_int/lag.h>
#include <soc/robo2/common/tables.h>
#include <sal_alloc.h>

static cbx_stg_sw_info_t cbx_stg_sw_info;

#ifdef CBX_MANAGED_MODE
/*
 * Function:
 *      cbxi_stg_rmap_add (internal)
 * Purpose:
 *      Add VLAN to STG linked list
 *      rmap is a reverse mapto get all vlans part of a stg.
 */

STATIC void
cbxi_stg_rmap_add(cbx_stgid_t stgid, cbx_vlan_t vsi)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;

    csi->vlan_next[vsi] = csi->vlan_first[stgid];
    csi->vlan_first[stgid] = vsi;
}

/*
 * Function:
 *      cbxi_stg_rmap_delete (internal)
 * Purpose:
 *      Remove VLAN from STG linked list. rmap is a reverse mapto get all vlans
 *      part of a stg.
 *      Return CBX_E_NOT_FOUND if VLAN is not in given STG
 */

STATIC void
cbxi_stg_rmap_delete(cbx_stgid_t stgid, cbx_vlan_t vsi)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_vlan_t  *vp;

    SAL_ASSERT(vsi != VLAN_NULL);

    vp = &csi->vlan_first[stgid];

    while (*vp != VLAN_NULL) {
        if (*vp == vsi) {
            *vp = csi->vlan_next[*vp];
        } else {
            vp = &csi->vlan_next[*vp];
        }
    }
}
#endif /* CBX_MANAGED_MODE */

/*
 * Function:
 *  cbxi_stg_stp_init
 * Purpose:
 *  Put all the ports in DISCARD spanning tree state for given stgid
 *
 */

STATIC int
cbxi_stg_stp_init(cbx_stgid_t stgid)
{
    int unit = 0;
    uint8 num_units = 1;
    cbx_port_t port;
    cbxi_pgid_t lpg;
    uint32_t link_state = 0;

#ifdef CONFIG_CASCADED_MODE
    int rv = 0;
    int lagunit = 0;
    cbx_portid_t lagid;
    cbx_port_t lagport;
#endif

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (unit = 0; unit < num_units; unit++) {
        CBX_PBMP_ITER(PBMP_ALL(unit), port) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_to_lpg(unit, port, &lpg));

            if ((CBXI_GLOBAL_PORT(port, unit) == CBX_PORT_ICPU) ||
                        (CBXI_GLOBAL_PORT(port, unit) == CBX_PORT_ECPU)) {
            /* CPU port on Primary Avenger should be in FORWARD state */
                CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                unit, stgid, lpg, cbxStgStpStateForward));
#ifdef CONFIG_PORT_EXTENDER
            } else if (port == CBX_PE_LB_PORT) {
                /* Loopback port in PE mode */
                CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                unit, stgid, lpg, cbxStgStpStateForward));
                if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    /* LB port maynot be a valid port on Secondary AVG
                     * Configure on Partner Avenger also */
                    CBX_IF_ERROR_RETURN(cbxi_robo2_port_to_lpg(
                                        CBX_AVENGER_SECONDARY, port, &lpg));
                    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                    CBX_AVENGER_SECONDARY, stgid, lpg, cbxStgStpStateForward));
                }
#endif
            } else if (stgid == CBX_STG_DEFAULT) {
                /* Check for Link status and if link is not up, put to discard
                 * state */
                CBX_IF_ERROR_RETURN(cbxi_port_link_status_get(
                                        unit, port, &link_state));
                if (link_state) {
                    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                unit, stgid, lpg, cbxStgStpStateForward));
                } else {
                    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                unit, stgid, lpg, cbxStgStpStateDiscard));
                }
            } else {
                    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                unit, stgid, lpg, cbxStgStpStateDiscard));
            }
#ifdef CONFIG_CASCADED_MODE
            if (CBX_SUCCESS(cbxi_cascade_port_check(
                        port + (unit * CBX_MAX_PORT_PER_UNIT)))) {

                rv = cbxi_robo2_lag_member_check(
                                        (port + (unit * CBX_MAX_PORT_PER_UNIT)),
                                         &lagid);
                if (CBX_SUCCESS(rv)) {
                    rv = cbxi_robo2_port_validate(lagid, &lagport,
                                                  &lpg, &lagunit);
                    if (CBX_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_FSAL_VLAN,
                            (BSL_META("FSAL API : cbxi_stg_stp_init()..  \
                                       Invalid port \n")));
                        return rv;
                   }
                }
                /* Cascading port of both Primary and secondary
                 * should be in FORWARD state*/
                    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(unit, stgid,
                                               lpg, cbxStgStpStateForward));

            }
#endif /* CONFIG_CASCADED_MODE */
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_create_id
 * Purpose:
 *  Create a stg with given stgid
 *
 */

STATIC int
cbxi_stg_create_id(cbx_stgid_t stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbxi_stg_create_id()..\n")));

    if ((cbxi_stg_valid_check(stgid)) == CBX_E_NONE) {
        /* Given stgid exists already */
        return CBX_E_EXISTS;
    }

    /*Put all ports to DISABLED state for given stgid */
    CBX_IF_ERROR_RETURN(cbxi_stg_stp_init(stgid));

    STG_BITMAP_SET(csi, stgid);
    csi->stg_count++;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_table_init
 * Purpose:
 *  Global Initialization for STG related table/ regs.
 *
 */

STATIC int
cbxi_stg_table_init(int unit)
{
    /* Enable ISTGET */
    CBX_IF_ERROR_RETURN(soc_robo2_istget_enable(unit));
    /* Reset ISTGET */
    CBX_IF_ERROR_RETURN(soc_robo2_istget_reset(unit));

    /* Enable ESTGET */
    CBX_IF_ERROR_RETURN(soc_robo2_estget_enable(unit));
    /* Reset ESTGET */
    CBX_IF_ERROR_RETURN(soc_robo2_estget_reset(unit));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_bpdu_rule_set
 * Purpose:
 *  Set rule in SLIC TCAM for BPDU identification and appropriate traps to
 *  forward them to CPU
 *
 */

STATIC int
cbxi_stg_bpdu_slic_rule_set()
{
    /* TO DO: SLIC TCAM programming functions needs to be defined */

    return CBX_E_NONE;
}

#ifndef CBX_MANAGED_MODE
/*
 * Function:
 *  cbxi_stg_vlan_remove_all
 * Purpose:
 *   Remove all vlans from given stg: stgid
 *   This functions walks throuh the list of created VLANs in VLAN S/W database
 *   and removes those VLANs which are part of given STG
 *Note:
 *   Assumes that stgid validity is already checked
 */

int
cbxi_stg_vlan_remove_all(cbx_stgid_t stgid)
{
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    cbx_vlan_t vid;
    cbx_stgid_t cur_stg;
    int count = 0;

    CHECK_VLAN_INIT(cvi);

    count = cvi->count;
    VLAN_BITMAP_ITER(cvi, vid) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vid, &cur_stg));
        if (cur_stg == stgid) {
            CHECK_VID(vid);
            /* vlan is in given stg. Remove vlan */
            CBX_IF_ERROR_RETURN(cbxi_stg_vlan_remove(stgid, vid));
        }
        count--;
        if (!count) {
            break;
        }
    }

    return CBX_E_NONE;
}
#endif /* !CBX_MANAGED_MODE */

/**
 * Detach STG module
 *
 * @param void N/A
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_stg_detach(void)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_stgid_t  stgid;
    int rv = CBX_E_NONE;

    if (!csi->init) {
        return CBX_E_NONE;
    }

    /* Destroy all STGs present */
    for (stgid = csi->stg_min; stgid <= csi->stg_max; stgid++) {
        if (STG_BITMAP_TST(csi, stgid)) {
            if (stgid != CBX_STG_DEFAULT) {
                rv = cbx_stg_destroy(stgid);
            }
        }
    }

    /* Manualy remove Default STG entry */
    STG_BITMAP_CLR(csi, CBX_STG_DEFAULT);
    csi->stg_count--;

    /* If stg_bitmap and vlan lists are empty, free the memory */
    if (CBX_SUCCESS(rv))
    {
#ifdef CBX_MANAGED_MODE
        sal_free(csi->vlan_next);
        csi->vlan_next = NULL;
        sal_free(csi->vlan_first);
        csi->vlan_first = NULL;
#endif /* CBX_MANAGED_MODE */
        sal_free(csi->stg_bitmap);
        csi->stg_bitmap = NULL;
    }

    csi->init = 0;

    return rv;
}

/**
 * Initialize STG module
 *
 * @param void N/A
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_stg_init(void)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    uint32 size;

    LOG_INFO(BSL_LS_FSAL_STG,
            (BSL_META("FSAL API : cbxi_stg_init()..\n")));

    /* Init STG related tables */
    CBX_IF_ERROR_RETURN(cbxi_stg_table_init(CBX_AVENGER_PRIMARY));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(cbxi_stg_table_init(CBX_AVENGER_SECONDARY));
    }

    /* Set BPDU SLIC TCAM rule */
    CBX_IF_ERROR_RETURN(cbxi_stg_bpdu_slic_rule_set());

    /* Destroy previously created database if any */
    if (csi->init == TRUE) {
        CBX_IF_ERROR_RETURN(cbxi_stg_detach());
    }

    /* Initialize software structure */
    csi->stg_min = 0;                   /* stgid range 0 - 127 */
    csi->stg_max = CBX_STG_MAX - 1;
    csi->stg_count = 0;
    csi->stg_defl = CBX_STG_DEFAULT;
    CHECK_STG(csi, csi->stg_defl);

    size = SHR_BITALLOCSIZE(csi->stg_max + 1);
    if (csi->stg_bitmap == NULL) {
        csi->stg_bitmap = sal_alloc(size, "STG-bitmap");
    }

#ifdef CBX_MANAGED_MODE
    if (csi->vlan_first == NULL) {
        csi->vlan_first = sal_alloc(
                        (csi->stg_max + 1) * sizeof (cbx_vlan_t), "STG-vfirst");
    }

    if (csi->vlan_next == NULL) {
        csi->vlan_next = sal_alloc(
                        CBX_VLAN_MAX * sizeof (cbx_vlan_t), "STG-vnext");
    }

    if (csi->stg_bitmap == NULL || csi->vlan_first == NULL ||
                                        csi->vlan_next == NULL) {
        if (csi->stg_bitmap != NULL) {
            sal_free(csi->stg_bitmap);
            csi->stg_bitmap = NULL;
        }

        if (csi->vlan_first != NULL) {
            sal_free(csi->vlan_first);
            csi->vlan_first = NULL;
        }

        if (csi->vlan_next != NULL) {
            sal_free(csi->vlan_next);
            csi->vlan_next = NULL;
        }

        return CBX_E_MEMORY;
    }
#else
    if (csi->stg_bitmap == NULL) {
        return CBX_E_MEMORY;
    }
#endif /* CBX_MANAGED_MODE */

    /* Memory buffer clear to default. */
    sal_memset(csi->stg_bitmap, 0, size);
#ifdef CBX_MANAGED_MODE
    sal_memset(csi->vlan_first, 0, (csi->stg_max + 1) * sizeof (cbx_vlan_t));
    sal_memset(csi->vlan_next, 0, CBX_VLAN_MAX * sizeof (cbx_vlan_t));
#endif /* CBX_MANAGED_MODE */


   /* Create default STG*/
    CBX_IF_ERROR_RETURN(cbxi_stg_create_id(csi->stg_defl));

    csi->init = TRUE;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_valid_check
 * Purpose:
 *  Check if given stgid is valid.
 *  Return CBX_E_NONE if stg already exists
 *  Return CBX_E_NOT_FOUND if stg is not created
 */

int
cbxi_stg_valid_check(cbx_stgid_t stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;

    CHECK_STG_INIT(csi);
    CHECK_STG(csi, stgid);

    if (STG_BITMAP_TST(csi, stgid)) {
        return CBX_E_NONE;
    } else {
        return CBX_E_NOT_FOUND;
    }
}

/*
 * Function:
 *  cbxi_stg_port_state_set
 * Purpose:
 *  Main body of cbx_stg_stp_set
 *  Assumes stgid and port are valid
 */

int
cbxi_stg_port_state_set(int unit, cbx_stgid_t stgid, cbxi_pgid_t pgid,
                                cbx_stg_stp_state_t stp_state)
{
    uint32 idx;
    uint32 status = 0;
    istget_t ent_istget;
    estget_t ent_estget;
    pbmp_t rx_map;
    pbmp_t tx_map;

    idx = (uint32)stgid;

    /* Retrive ISTGET table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_istget_get(unit, idx, &ent_istget));
    /* Retrive ESTGET table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_estget_get(unit, idx, &ent_estget));

    rx_map = ent_istget.rx_ena;
    tx_map = ent_estget.tx_ena;

    switch (stp_state) {
        case cbxStgStpStateDiscard:
            CBX_PBMP_PORT_REMOVE(rx_map, pgid);
            CBX_PBMP_PORT_REMOVE(tx_map, pgid);
            break;
        case cbxStgStpStateLearn:
            CBX_PBMP_PORT_ADD(rx_map, pgid);
            CBX_PBMP_PORT_REMOVE(tx_map, pgid);
            break;
        case cbxStgStpStateForward:
            CBX_PBMP_PORT_ADD(rx_map, pgid);
            CBX_PBMP_PORT_ADD(tx_map, pgid);
            break;
        default:
            return CBX_E_PARAM;
    }

    /* Update fields for corresponding pgid */
    ent_istget.rx_ena = (uint16)rx_map;
    ent_estget.tx_ena = (uint16)tx_map;

    /* Write updated entry to ISTGET table  */
    CBX_IF_ERROR_RETURN(soc_robo2_istget_set(unit, idx, &ent_istget, &status));
    /* Write updated entry to ESTGET table  */
    CBX_IF_ERROR_RETURN(soc_robo2_estget_set(unit, idx, &ent_estget, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_stp_set
 * Purpose:
 *  Main body of cbx_stg_stp_set
 *  Assumes stgid and port are valid
 */
int
cbxi_stg_stp_set(cbx_stgid_t stgid, cbx_portid_t portid,
                                cbx_stg_stp_state_t stp_state)
{
    int unit = CBX_AVENGER_PRIMARY;
    cbx_portid_t lagid;
    cbx_port_t port;
    cbxi_pgid_t local_pg = CBXI_PGID_INVALID;
    cbxi_pgid_t remote_pg = CBXI_PGID_INVALID;
    int rv = CBX_E_NONE;
    cbx_lag_info_t lag_info;

    /* Check if the portid is of type LAG */
    if (CBX_PORTID_IS_TRUNK(portid)) {
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
            CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                        unit, stgid, local_pg, stp_state));
        }
        if (remote_pg != CBXI_PGID_INVALID) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                unit = CBX_AVENGER_SECONDARY;
                CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                        unit, stgid, remote_pg, stp_state));
            } else {
                return CBX_E_PARAM;
            }
        }
    } else {
        /* portid is type other than LAG:
         * physical port no., local port (physical port in portid format) */

    /* If port is a member of the trunk then we need to return error
     * It is not allowed to set stp_state for a single port if the port is on
     * LAG*/
        rv = cbxi_robo2_lag_member_check(portid, &lagid);
        if (CBX_SUCCESS(rv)) {
            return CBX_E_PARAM;
        }
        /* Validate the port number passed, and obtain physical port
         * corresponding to given portid */
        rv = cbxi_robo2_port_validate(portid, &port, &local_pg, &unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("FSAL API : cbx_port_group_info_get()..Invalid  \
                                  port \n")));
            return rv;
        }

        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                                        unit, stgid, local_pg, stp_state));
    }

    return CBX_E_NONE;
}



/*
 * Function:
 *  cbxi_stg_stp_get
 * Purpose:
 *  Main body of cbx_stg_stp_get
 *  Assumes stgid and port are valid
 */

STATIC int
cbxi_stg_stp_get(int unit, cbx_stgid_t stgid, cbxi_pgid_t pgid,
                                cbx_stg_stp_state_t *stp_state)
{
    uint32 idx;
    istget_t ent_istget;
    estget_t ent_estget;
    pbmp_t rx_map;
    pbmp_t tx_map;

    idx = (uint32)stgid;

    /* Retrive ISTGET table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_istget_get(unit, idx, &ent_istget));
    /* Retrive ESTGET table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_estget_get(unit, idx, &ent_estget));

    rx_map = ent_istget.rx_ena;
    tx_map = ent_estget.tx_ena;

    if (CBX_PBMP_MEMBER(rx_map, pgid)) {
        if (CBX_PBMP_MEMBER(tx_map, pgid)) {
            *stp_state = cbxStgStpStateForward;
        } else {
            *stp_state = cbxStgStpStateLearn;
        }
    } else {
        if (CBX_PBMP_MEMBER(tx_map, pgid)) {
        /* STP pgid rx disabled and tx enabled is not a valid case */
            return CBX_E_INTERNAL;
        } else {
            *stp_state = cbxStgStpStateDiscard;
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_vlan_add
 * Purpose:
 *  Main body of cbx_stg_vlan_add
 *  Assumes stgid and vlanid are valid, and stg exists
 */

STATIC int
cbxi_stg_vlan_add(cbx_stgid_t stgid, cbx_vlanid_t vlanid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_stgid_t cur_stgid;
    cbx_vlan_t  vsi;

    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));

    /* Obtain the current STG that the given VID is part of */
    CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vlanid, &cur_stgid));

    /* Both cur_stgid and the stgid to which VLAN needs to be added are same
     * only when both of them are default stg. This case arises while creating a
     * new VLAN and adding it to default stg.
     * Otherwise cur_stgid and stgid should be different */
    if ((cur_stgid == csi->stg_defl && stgid == csi->stg_defl) ||
                                                (cur_stgid != stgid)) {
#ifdef CBX_MANAGED_MODE
        cbxi_stg_rmap_delete(cur_stgid, vsi);
#endif /* CBX_MANAGED_MODE */

        CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_set(vlanid, stgid));

#ifdef CBX_MANAGED_MODE
        cbxi_stg_rmap_add(stgid, vsi);
#endif /* CBX_MANAGED_MODE */
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_vlan_remove
 * Purpose:
 *  Main body of cbx_stg_vlan_remove
 *  Assumes stgid and vlanid are valid, and stg exists
 */

int
cbxi_stg_vlan_remove(cbx_stgid_t stgid, cbx_vlanid_t vlanid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_stgid_t cur_stgid;
    cbx_vlan_t  vsi;

    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));

    /* Obtain the current STG that the given VID is part of */
    CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vlanid, &cur_stgid));

    if (cur_stgid != stgid) {
        /* VLAN ID is not a part of given stgid */
        return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_set(vlanid, csi->stg_defl));

#ifdef CBX_MANAGED_MODE
    cbxi_stg_rmap_delete(stgid, vsi);
    cbxi_stg_rmap_add(csi->stg_defl, vsi);
#endif /* CBX_MANAGED_MODE */

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_vlan_destroy
 * Purpose:
 *  The vlan is removed from given stg and is not placed in default stg
 *  This is called from VLAN module when VLAN itself is destroyed
 *
 */

int
cbxi_stg_vlan_destroy(cbx_stgid_t stgid, cbx_vlanid_t vlanid)
{
#ifdef CBX_MANAGED_MODE
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
#endif /* CBX_MANAGED_MODE */
    cbx_vlan_t  vsi;

    CBX_IF_ERROR_RETURN(
            cbxi_vlan_vsit_index_get(vlanid, &vsi));

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("STG: cbxi_stg_vlan_destroy()..\n")));

    CHECK_VID(vlanid);
    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    CBX_IF_ERROR_RETURN(cbxi_stg_vlan_remove(stgid, vlanid));
#ifdef CBX_MANAGED_MODE
    cbxi_stg_rmap_delete(csi->stg_defl, vsi);
#endif /* CBX_MANAGED_MODE */

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_stg_default_get
 * Purpose:
 *  Return Default STG.
 */

int
cbxi_stg_default_get(cbx_stgid_t *stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;

    CHECK_STG_INIT(csi);

    if (STG_BITMAP_TST(csi, csi->stg_defl)) {
        *stgid = csi->stg_defl;
        return CBX_E_NONE;
    } else {
        return CBX_E_INIT; /* Default STG not created */
    }
}

/*
 * Function:
 *  cbxi_stg_default_set
 * Purpose:
 *  Set Default STG.
 */

int
cbxi_stg_default_set(cbx_stgid_t stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_vlan_sw_info_t *cvi = &cbx_vlan_info;
    cbx_vlan_t vid;
    cbx_stgid_t cur_stg, cur_defl;
#ifdef CBX_MANAGED_MODE
    vlist_t **list = &cvi->list;
#else /* UM+ mode */
    int count = 0;
#endif /* CBX_MANAGED_MODE */

    CHECK_STG_INIT(csi);

    if (!STG_BITMAP_TST(csi, stgid)) {
        return CBX_E_PARAM;
    }

    CBX_IF_ERROR_RETURN(cbxi_stg_default_get(&cur_defl));

    if (stgid == cur_defl) {
        return CBX_E_NONE;
    }

    csi->stg_defl = stgid;

    if (!cvi->init) {
        return CBX_E_NONE;
    }

    /* Set default stg parameter in VLAN module */
    cvi->defl_stg = stgid;

    /* Change STG of VLANs that are in previous default STG */
#ifdef CBX_MANAGED_MODE
    while (*list != NULL) {
        vid = (*list)->vid;
        CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vid, &cur_stg));
        if (cur_stg == cur_defl) {
            CHECK_VID(vid);
            /* vlan is in previous default stg.
             * Add it to new default stg */
            CBX_IF_ERROR_RETURN(cbxi_stg_vlan_add(stgid, vid));
        }
        list = &(*list)->next;
    }
#else /* UM+ mode */
    count = cvi->count;
    VLAN_BITMAP_ITER(cvi, vid) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_stgid_get(vid, &cur_stg));
        if (cur_stg == cur_defl) {
            CHECK_VID(vid);
            /* vlan is in previous default stg.
             * Add it to new default stg */
            CBX_IF_ERROR_RETURN(cbxi_stg_vlan_add(stgid, vid));
        }
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
 *  cbxi_stg_port_set_all
 * Purpose:
 *  Set STP state of a port across all STGs created
 */

int
cbxi_stg_port_set_all(cbx_portid_t portid, cbx_stg_stp_state_t stp_state)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_stgid_t stgid;

    CHECK_STG_INIT(csi);

    SHR_BIT_ITER(csi->stg_bitmap, CBX_STG_MAX, stgid) {
        CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(stgid, portid, stp_state));
    }

    return CBX_E_NONE;
}


/**************************************************************************
 *                 STG FSAL API IMPLEMENTATION                           *
 **************************************************************************/


/**
 * Initialize STG parameters.
 *
 * @param  stg_params  parameter object
 */
void cbx_stg_params_t_init(cbx_stg_params_t *stg_params)
{
    if (stg_params != NULL) {
        sal_memset(stg_params, 0, sizeof(cbx_stg_params_t));
        stg_params->stg_mode = cbxStgModeNormal;
    }
    return;
}

/**
 * Create a STG
 * This routine is used to create a Spanning Tree Group.
 *
 * @param stg_params   (IN)  STG parameters.
 * @param stgid        (OUT) Handle of the STG created
 *                           CBX_STG_INVALID: STG could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stg_create(cbx_stg_params_t *stg_params, cbx_stgid_t *stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
    cbx_stgid_t stg;

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_create()..\n")));

    CHECK_STG_INIT(csi);

    /* get unused ID */
    for (stg = csi->stg_min; stg <= csi->stg_max; stg++) {
        if (!STG_BITMAP_TST(csi, stg)) {
            break;
        }
    }

    if (stg > csi->stg_max) {
        return CBX_E_FULL;
    }

    CBX_IF_ERROR_RETURN(cbxi_stg_create_id(stg));

    *stgid = stg;

    /* Configure the first STG created by user as default and
     * add all existing VLANs to new STG */
    if (csi->stg_defl == CBX_STG_DEFAULT) {
         CBX_IF_ERROR_RETURN(cbxi_stg_default_set(stg));
    }

    return CBX_E_NONE;
}

/**
 * Destroy a stg previously created by cbx_stg_create()
 *
 * @param stgid    (IN)  Handle of the STG to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created Spanning Tree Group.
 */

int cbx_stg_destroy(cbx_stgid_t stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_destroy()..\n")));

    /* Check if STG exists */
    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    /* Cannot destroy system default STG 0 */
    if (stgid == CBX_STG_DEFAULT) {
        return CBX_E_NOT_FOUND;
    }else if (stgid == csi->stg_defl) {
        /* If first STG created by user is getting destroyed,
         * configure system default STG 0 as default STG */
        CBX_IF_ERROR_RETURN(cbxi_stg_default_set(CBX_STG_DEFAULT));
    }

    CBX_IF_ERROR_RETURN(cbx_stg_vlan_remove_all(stgid));

    STG_BITMAP_CLR(csi, stgid);
    csi->stg_count--;

    return CBX_E_NONE;
}

/**
 * STG set
 * This routine sets the Spanning Tree Protocol state
 * of a port in the specified STG.
 *
 * @param stgid      STG Identifier
 * @param portid     Port Identifier
 * @param stp_state  Spanning Tree Protocol port state
 *
 * @return return code
 * @retval 0 success
 *
 * @note STG must be defined and Port added to STG
 */

int cbx_stg_stp_set(cbx_stgid_t stgid, cbx_portid_t portid,
                                cbx_stg_stp_state_t stp_state)
{
#ifdef CONFIG_CASCADED_MODE
    int rv = 0;
#endif /* CONFIG_CASCADED_MODE */

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_stp_set()..\n")));

    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

#ifdef CONFIG_CASCADED_MODE
    rv = cbxi_cascade_port_check(portid);
    if (CBX_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_stp_set() \
                        cannot set STP state for  cascade port %d\n"), portid));
        return CBX_E_PORT;
    }
#endif /* CONFIG_CASCADED_MODE */

    CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(stgid, portid, stp_state));

    return CBX_E_NONE;
}


/**
 * STG STP port state get
 * This routine returns the Spanning Tree Protocol state
 *   of a port in the specified STG.
 *
 * @param stgid      STG Identifier
 * @param portid     Port Identifier
 * @param stp_state  (OUT) Spanning Tree Protocol port state
 *
 * @return return code
 * @retval 0 success
 *
 * @note STG must be defined and Port added to STG
 */

int cbx_stg_stp_get(cbx_stgid_t stgid, cbx_portid_t portid,
                                cbx_stg_stp_state_t *stp_state)
{
    int unit = CBX_AVENGER_PRIMARY;
    cbx_portid_t lagid;
    cbx_port_t port;
    cbxi_pgid_t local_pg = CBXI_PGID_INVALID;
    cbxi_pgid_t remote_pg = CBXI_PGID_INVALID;
    int rv = CBX_E_NONE;
    cbx_lag_info_t lag_info;

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_stp_get()..\n")));

    SAL_ASSERT(stp_state != NULL);

    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    /* Check if the portid is of type LAG */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        /* Obtain LPG values on Primary and Secondary Avengers corresponding
         * to LAG id indicated by portid passed*/
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        local_pg = lag_info.lpgid;
        remote_pg = lag_info.rlpgid;

        if ((local_pg == CBXI_PGID_INVALID) &&
                        (remote_pg == CBXI_PGID_INVALID)) {
            return CBX_E_PORT;
        }
        /* Assuming local_pg and remote_pg will be configured same stp_state */
        if (local_pg != CBXI_PGID_INVALID) {
            unit = CBX_AVENGER_PRIMARY;
            CBX_IF_ERROR_RETURN(cbxi_stg_stp_get(
                                        unit, stgid, local_pg, stp_state));
        } else if (remote_pg != CBXI_PGID_INVALID) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                unit = CBX_AVENGER_SECONDARY;
                CBX_IF_ERROR_RETURN(cbxi_stg_stp_get(
                                        unit, stgid, remote_pg, stp_state));
            } else {
                return CBX_E_PARAM;
            }
        }
    } else {
        /* portid is type other than LAG:
         * physical port no., local port (physical port in portid format) */

    /* If port is a member of the trunk then we need to return error */
        rv = cbxi_robo2_lag_member_check(portid, &lagid);
        if (CBX_SUCCESS(rv)) {
            return CBX_E_PARAM;
        }
        /* Validate the port number passed, and obtain physical port
         * corresponding to given portid */
        rv = cbxi_robo2_port_validate(portid, &port, &local_pg, &unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_VLAN,
                    (BSL_META("FSAL API : cbx_port_group_info_get()..Invalid  \
                                  port \n")));
            return rv;
        }

        CBX_IF_ERROR_RETURN(cbxi_stg_stp_get(
                                        unit, stgid, local_pg, stp_state));
    }

    return CBX_E_NONE;
}

/**
 * STG vlan add
 * This routine is used to add a vlan to a STG
 *
 * @param stgid   STG Identifier
 * @param portid  Vlan Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure

 */

int cbx_stg_vlan_add(cbx_stgid_t stgid, cbx_vlanid_t vlanid)
{
    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_vlan_add()..\n")));

    CHECK_VID(vlanid);
    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    CBX_IF_ERROR_RETURN(cbxi_stg_vlan_add(stgid, vlanid));

    return CBX_E_NONE;
}

/**
 * STG vlan remove
 * This routine is used to remove a vlan from a STG
 *
 * @param stgid       (IN)  STG Identifier
 * @param vlanid      (IN)  Vlan Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stg_vlan_remove(cbx_stgid_t stgid, cbx_vlanid_t vlanid)
{
    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_vlan_remove()..\n")));

    CHECK_VID(vlanid);
    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    CBX_IF_ERROR_RETURN(cbxi_stg_vlan_remove(stgid, vlanid));


    return CBX_E_NONE;
}

/**
 * STG vlan remove all
 * This routine is used to remove all the vlans from a STG.
 * Place removed VLANs in default STG
 *
 * @param stgid       (IN)  STG Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stg_vlan_remove_all(cbx_stgid_t stgid)
{
    cbx_stg_sw_info_t *csi = &cbx_stg_sw_info;
#ifdef CBX_MANAGED_MODE
    cbx_vlanid_t vlanid;
#endif /* CBX_MANAGED_MODE */

    LOG_INFO(BSL_LS_FSAL_STG,
                (BSL_META("FSAL API : cbx_stg_vlan_remove_all()..\n")));

    CBX_IF_ERROR_RETURN(cbxi_stg_valid_check(stgid));

    if (stgid == csi->stg_defl) {
        /* Null operation */
        return CBX_E_NONE;
    }

#ifdef CBX_MANAGED_MODE
    while ((vlanid = csi->vlan_first[stgid]) != VLAN_NULL) {
        /* cbxi_stg_vlan_remove fuction puts vlanid to default stg */
        CBX_IF_ERROR_RETURN(cbxi_stg_vlan_remove(stgid, vlanid));
    }
#else
    CBX_IF_ERROR_RETURN(cbxi_stg_vlan_remove_all(stgid));
#endif /* CBX_MANAGED_MODE */

    return CBX_E_NONE;
}



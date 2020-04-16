/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     mcast.c
 * * Purpose:
 * *     Robo2 L2 multicast module
 * *
 * *
 * */


#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal/mcast.h>
#include <fsal_int/mcast.h>
#include <fsal_int/port.h>
#include <fsal_int/lag.h>
#include <fsal_int/vlan.h>
#include <fsal_int/lin.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif /* CONFIG_PORT_EXTENDER */
#include <fsal_int/trap.h>
#include <soc/robo2/common/tables.h>
#include <sal_alloc.h>


static cbx_mcast_sw_info_t cbx_mcast_sw_info;

STATIC int
cbxi_mcast_member_update(uint32 mcast, cbx_portid_t  portid, int flags);

/*
 * Function:
 *  cbxi_mcast_table_init
 * Purpose:
 *  Global Initialization for L2 multicst related table/ regs.
 *  Tables : DGT, LILT
 *
 */

STATIC int
cbxi_mcast_table_init(int unit)
{
    /* Enable DGT */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_enable(unit));
    /* Reset DGT */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_reset(unit));

    /* Enable LILT */
    CBX_IF_ERROR_RETURN(soc_robo2_lilt_enable(unit));
    /* Reset LILT */
    CBX_IF_ERROR_RETURN(soc_robo2_lilt_reset(unit));

    return CBX_E_NONE;
}

/**
 * Detach MCAST module
 *
 * @param void N/A
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_mcast_detach(void)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    cbx_mcastid_t mcastid;
    uint32_t  id;
    int rv = CBX_E_NONE;

    if (!cmi->init) {
        return CBX_E_NONE;
    }

    /* Destroy all MCAST ids present */
    SHR_BIT_ITER(cmi->mcast_bitmap, CBX_MCAST_MAX, id) {
        if (SHR_BITGET(cmi->mcast_bitmap, id)) {
        /* Form portid from mcast id value */
            CBX_PORTID_MCAST_ID_SET(mcastid, id);
            rv = cbx_mcast_destroy(mcastid);
        }
    }
    /* free the memory */
    if (CBX_SUCCESS(rv))
    {
        sal_free(cmi->mcast_bitmap);
        cmi->mcast_bitmap = NULL;

        sal_free(cmi->lilt_bitmap);
        cmi->lilt_bitmap = NULL;
    }
    cmi->init = 0;

    return rv;
}

/**
 * Initialize mcast module
 *
 * @param void N/A
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_mcast_init(void)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    uint32 size;

    LOG_INFO(BSL_LS_FSAL_MCAST,
            (BSL_META("FSAL API : cbxi_mcast_init()..\n")));

    /* Init mcast related tables */
    CBX_IF_ERROR_RETURN(cbxi_mcast_table_init(CBX_AVENGER_PRIMARY));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(cbxi_mcast_table_init(CBX_AVENGER_SECONDARY));
    }

    /* Destroy previously created database if any */
    if (cmi->init == TRUE) {
        CBX_IF_ERROR_RETURN(cbxi_mcast_detach());
    }

    /* Initialize software structure */
    /* DGT index 0 is not used as lilt_ndx that we may have to use becomes 0  */
    cmi->mcast_min = 1;
    cmi->mcast_max = CBX_MCAST_MAX - 1;
    cmi->mcast_count = 0;

    size = SHR_BITALLOCSIZE(cmi->mcast_max + 1);
    if (cmi->mcast_bitmap == NULL) {
        cmi->mcast_bitmap = sal_alloc(size, "MCAST-bitmap");
    }

    if (cmi->lilt_bitmap == NULL) {
        cmi->lilt_bitmap = sal_alloc(CBXI_LILT_ARRAY_MAX, "LILT-array-map");
    }

    if (cmi->mcast_bitmap == NULL || cmi->lilt_bitmap == NULL) {
        if (cmi->mcast_bitmap != NULL) {
            sal_free(cmi->mcast_bitmap);
            cmi->mcast_bitmap = NULL;
        }
        if (cmi->lilt_bitmap != NULL) {
            sal_free(cmi->lilt_bitmap);
            cmi->lilt_bitmap = NULL;
        }
        return CBX_E_MEMORY;
    }

    /* Memory buffer clear to default. */
    sal_memset(cmi->mcast_bitmap, 0, size);

    cmi->init = TRUE;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_id_check
 * Purpose:
 *   Validate a Multicat id value
 *
 */

int
cbxi_mcast_id_check(cbx_mcastid_t mcastid)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    uint32_t mcast;

    CHECK_MCAST_INIT(cmi);
    if (CBX_PORTID_MCAST_ID_GET(mcastid) < 0) {
        return CBX_E_PARAM;
    }
    mcast = CBX_PORTID_MCAST_ID_GET(mcastid);
    CHECK_MCAST(cmi, mcast);

    if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
        return CBX_E_NOT_FOUND;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_lilt_mode_get
 * Purpose:
 *  For a given DGT index (mcast group) determine the lilt mode
 */

STATIC int
cbxi_mcast_lilt_mode_get(uint32_t mcast_idx, cbx_mcast_lilt_mode_t *lilt_mode)
{
    dgt_t ent_dgt;
    int unit = 0;

    /* Retrive DGT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast_idx, &ent_dgt));

    if (ent_dgt.lilt_type == 1) {
        *lilt_mode = cbxMcastLiltModeList;
    } else if (ent_dgt.lilt_ndx == 0) {
        *lilt_mode = cbxMcastLiltModeNone;
    } else {
        *lilt_mode = cbxMcastLiltModeArray;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_lilt_ndx_get
 * Purpose:
 *  Obtain next available slot for LILT array;
 */

STATIC int
cbxi_mcast_lilt_ndx_get(cbx_mcast_params_t *mcast_params, uint16_t *lilt_ndx)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    lilt_t ent_lilt;
    int num_units = 1;
    int unit = 0;
    uint32_t ndx, end_ndx;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Get next available slot for LILT array for packed DLIIDs */
    /* LILT index 0 is not used as it has a special meaning in DGT that no
     * DLIIDs are present on that mcast group */
    for (ndx = 1; ndx < CBXI_LILT_ARRAY_MAX; ndx++) {
        if (!SHR_BITGET(cmi->lilt_bitmap, ndx)) {
            if (mcast_params->lilt_mode != cbxMcastLiltModeList) {
                /* One 8 entry slot is sufficient */
                break;
            } else if (((ndx + 1) < CBXI_LILT_ARRAY_MAX) &&
                        (!SHR_BITGET(cmi->lilt_bitmap, ndx + 1))) {
                /* Check for 2 consecutive free slots for LILT list mode
                 * (Two 8 entry slots = 16 entry slot) */
                break;
            }
        }
    }
    if (ndx == CBXI_LILT_ARRAY_MAX) {
        return CBX_E_FULL;
    }
    /* Each slot in bitmap corresponds to 8 entries of 32 bit size  */
    *lilt_ndx = ndx * 8;

    if (mcast_params->lilt_mode == cbxMcastLiltModeArray) {
        SHR_BITSET(cmi->lilt_bitmap, ndx);
        /* Clear 8 entries in LILT starting from lilt_ndx */
        end_ndx = *lilt_ndx + 7;
    } else if (mcast_params->lilt_mode == cbxMcastLiltModeList) {
        /* Reserve two consecutive 8 entry array (16 entries in total) */
        SHR_BITSET(cmi->lilt_bitmap, ndx);
        /* Allocate 2 consecutive free entries */
        SHR_BITSET(cmi->lilt_bitmap, (ndx + 1));
        /* Clear 16 entries in LILT starting from lilt_ndx */
        end_ndx = *lilt_ndx + 15;
    } else {
        return CBX_E_PARAM;
    }

    ent_lilt.lilt_entry = 0;
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(soc_robo2_lilt_init(
                        unit, *lilt_ndx, end_ndx, 0, &ent_lilt));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_create
 * Purpose:
 *  Main body of cbx_mcast_create;
 */

STATIC int
cbxi_mcast_create(cbx_mcast_params_t *mcast_params, uint32_t mcast)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    dgt_t ent_dgt;
    uint32 status = 0;
    int num_units = 1;
    int unit = 0;
#ifdef CONFIG_CASCADED_MODE
    cbx_portid_t portid;
#endif

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Retrive DGT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));

    ent_dgt.pgfov = 0;

    if (mcast_params->lilt_mode == cbxMcastLiltModeNone) {
        ent_dgt.lilt_type = 0;
        ent_dgt.lilt_ndx = 0;
    } else if (mcast_params->lilt_mode == cbxMcastLiltModeArray) {
        ent_dgt.lilt_type = 0;
        /* Get next available slot for LILT array for packed DLIIDs */
        CBX_IF_ERROR_RETURN(
                cbxi_mcast_lilt_ndx_get(mcast_params, &ent_dgt.lilt_ndx));
    } else if (mcast_params->lilt_mode == cbxMcastLiltModeList) {
        /***  No support for this in FSAL v1  ***/
        ent_dgt.lilt_type = 1;
        /* Get next available slot for LILT array for DLIIDs */
        CBX_IF_ERROR_RETURN(
                cbxi_mcast_lilt_ndx_get(mcast_params, &ent_dgt.lilt_ndx));
    }

    /* Write updated entry to DGT table */
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(soc_robo2_dgt_set(unit, mcast, &ent_dgt, &status));
    }

    /* Reserve alloted mcast id */
    SHR_BITSET(cmi->mcast_bitmap, mcast);

#ifdef CONFIG_CASCADED_MODE
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        for (unit = 0; unit < 2; unit++) {
            CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(unit, &portid));
            CBX_IF_ERROR_RETURN(
                cbxi_mcast_member_update(mcast, portid, CBXI_MCAST_MEMBER_ADD));
        }
    }
#endif

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_destroy
 * Purpose:
 *  Main body of cbx_mcast_destroy;
 */

STATIC int
cbxi_mcast_destroy(uint32_t mcast)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    lilt_t ent_lilt;
    dgt_t ent_dgt;
    cbx_mcast_lilt_mode_t lilt_mode;
    int end_idx = 0;
    uint32 status = 0;
    int num_units = 1;
    int unit = 0;
#ifdef CONFIG_CASCADED_MODE
    cbx_portid_t portid;
#endif

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
#ifdef CONFIG_CASCADED_MODE
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        for (unit = 0; unit < num_units; unit++) {
            CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(unit, &portid));
            CBX_IF_ERROR_RETURN(
                cbxi_mcast_member_update(mcast, portid,
                                         CBXI_MCAST_MEMBER_REMOVE));
        }
    }
#endif

    unit = CBX_AVENGER_PRIMARY;
    /* Retrive DGT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));

    ent_dgt.pgfov = 0;
    ent_dgt.lilt_type = 0;


    ent_lilt.lilt_entry = 0;
    if (ent_dgt.lilt_ndx) {
        /* Clear LILT entries */
        CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_mode_get(mcast, &lilt_mode));
        if (lilt_mode == cbxMcastLiltModeArray) {
            end_idx = ent_dgt.lilt_ndx + (CBX_MAX_PORT_PER_UNIT/2);
            SHR_BITCLR(cmi->lilt_bitmap, (ent_dgt.lilt_ndx / (CBX_MAX_PORT_PER_UNIT/2)));
        } else if (lilt_mode == cbxMcastLiltModeList) {
            end_idx = ent_dgt.lilt_ndx + CBX_MAX_PORT_PER_UNIT;
            SHR_BITCLR(cmi->lilt_bitmap, (ent_dgt.lilt_ndx /CBX_MAX_PORT_PER_UNIT));
        } else {
            end_idx = ent_dgt.lilt_ndx;
        }
    }

    /* Write updated entry to DGT table */
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(soc_robo2_lilt_init(
                unit, ent_dgt.lilt_ndx, (end_idx - 1), 0, &ent_lilt));
    }


    ent_dgt.lilt_ndx = 0;
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(soc_robo2_dgt_set(unit, mcast, &ent_dgt, &status));
    }

    /* Release alloted mcast id */
    SHR_BITCLR(cmi->mcast_bitmap, mcast);

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_lilt_entry_get
 * Purpose:
 *  Get count number of DLIs in LILT starting from lilt_ndx
 */
STATIC int
cbxi_mcast_lilt_entry_get(
        int unit, uint32_t mcast, uint32_t lilt_ndx, uint32_t *list, int count)
{
    lilt_t ent_lilt;
    cbx_mcast_lilt_mode_t lilt_mode;
    int idx, end_idx;
    int arr_idx = 0;
    uint32_t entry;

    CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_mode_get(mcast, &lilt_mode));
    if (lilt_mode == cbxMcastLiltModeArray) {
        end_idx = lilt_ndx + ((count + 1) / 2);
    } else if (lilt_mode == cbxMcastLiltModeList) {
        end_idx = lilt_ndx + count;
    } else {
        return CBX_E_PARAM;
    }

    for (idx = lilt_ndx; idx < end_idx; idx++) {
        /* Retrive LILT table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_lilt_get(unit, idx, &ent_lilt));
        entry = ent_lilt.lilt_entry;
        if (lilt_mode == cbxMcastLiltModeArray) {
            /* Bits 16 thru 31 are dliid0 and bits 0 thru 15 are dliid1*/
            list[arr_idx++] = (entry >> CBXI_MCAST_SHIFT_16) &
                                                CBXI_MCAST_LOWER_16_MASK;
            list[arr_idx++] = entry & CBXI_MCAST_LOWER_16_MASK;
        } else {
            list[arr_idx++] = entry;
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_lilt_entry_set
 * Purpose:
 *  Set count number of DLIs in LILT starting from lilt_ndx
 */
STATIC int
cbxi_mcast_lilt_entry_set(
        int unit, uint32_t mcast, uint32_t lilt_ndx, uint32_t *list, int count)
{
    lilt_t ent_lilt;
    cbx_mcast_lilt_mode_t lilt_mode;
    uint32 status = 0;
    int idx, end_idx;
    int arr_idx = 0;
    uint32_t entry;

    CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_mode_get(mcast, &lilt_mode));
    /* To make sure LILT entries are configured to zeros for unused LPGs,
     * configure LILT entries upto LPG 13 */
    count = 14;
    if (lilt_mode == cbxMcastLiltModeArray) {
        end_idx = lilt_ndx + ((count + 1) / 2);
    } else if (lilt_mode == cbxMcastLiltModeList) {
        end_idx = lilt_ndx + count;
    } else {
        return CBX_E_PARAM;
    }

    for (idx = lilt_ndx; idx < end_idx; idx++) {
        /* Retrive LILT table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_lilt_get(unit, idx, &ent_lilt));
        entry = ent_lilt.lilt_entry;
        if (lilt_mode == cbxMcastLiltModeArray) {
            /* Bits 16 thru 31 are dliid0 and bits 0 thru 15 are dliid1*/
            entry = (list[arr_idx++] & CBXI_MCAST_LOWER_16_MASK)
                                                << CBXI_MCAST_SHIFT_16;
            if (arr_idx < count) {
                entry |= (list[arr_idx++] & CBXI_MCAST_LOWER_16_MASK);
            }
        } else {
            entry = list[arr_idx++];
        }
        ent_lilt.lilt_entry = entry;
        /* Write updated entry to LILT table */
        CBX_IF_ERROR_RETURN(soc_robo2_lilt_set(unit, idx, &ent_lilt, &status));

    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_entry_set
 * Purpose:
 *  Set pgfov field in DGT for given LPG value and corresponding DLI in LILT
 */

int
cbxi_mcast_entry_set(int unit, uint32_t  mcast,
                        cbxi_pgid_t lpg_new, uint32_t lilt_ent, int flags)
{
    dgt_t ent_dgt;
    cbx_mcast_lilt_mode_t lilt_mode;
    uint32 status = 0;
    uint32_t old_list[CBX_MAX_PORT_PER_UNIT];
    uint32_t new_list[CBX_MAX_PORT_PER_UNIT];
    uint16_t index = 0;
    uint16_t index_new = 0;
    uint16_t lilt_ndx;
    cbxi_pgid_t lpg;
    pbmp_t pgfov;
    int count = 0;

    sal_memset(old_list, 0, sizeof(old_list));
    sal_memset(new_list, 0, sizeof(new_list));

    CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_mode_get(mcast, &lilt_mode));

    /* Retrive DGT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));

    pgfov = (pbmp_t)ent_dgt.pgfov;
    CBX_PBMP_COUNT(pgfov, count);
    if (flags & CBXI_MCAST_MEMBER_ADD) {
        CBX_PBMP_PORT_ADD(pgfov, lpg_new);
    }

    if (lilt_mode == cbxMcastLiltModeNone) { /* LILT table is not used */
        if (flags & CBXI_MCAST_MEMBER_REMOVE) {
            CBX_PBMP_PORT_REMOVE(pgfov, lpg_new);
        }
    } else { /* LILT table is used */
        lilt_ndx = ent_dgt.lilt_ndx;

        CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_entry_get(
                        unit, mcast, lilt_ndx, old_list, count));

        CBX_PBMP_ITER(pgfov, lpg) {
            if (lpg < lpg_new) {
                new_list[index_new] = old_list[index];
                index++;
                index_new++;
            } else if (lpg == lpg_new) {
                if (flags & CBXI_MCAST_MEMBER_ADD) {
                    new_list[index_new] = lilt_ent;
                    index_new++;
                } else if (flags & CBXI_MCAST_MEMBER_UPDATE) {
                    new_list[index_new] = lilt_ent;
                    index_new++;
                    index++;
                } else {
                    if (flags & CBXI_MCAST_MEMBER_REMOVE) {
#ifdef CONFIG_PORT_EXTENDER
                        if ((cbxi_check_port_extender_mode() == CBX_E_NONE)  &&
                                (flags & CBXI_MCAST_MEMBER_LILT_REL)) {
                /* Release dlinid for cascade port */
                            cbxi_lin_slot_release(old_list[index]);
                        }
#endif /* CONFIG_PORT_EXTENDER */
                    }
                    index++;
                }
            } else {
                new_list[index_new] = old_list[index];
                index_new++;
                index++;
            }
            if (index_new >= CBX_MAX_PORT_PER_UNIT ||
                            index >= CBX_MAX_PORT_PER_UNIT) {
                break;
            }
        }

        if (flags & CBXI_MCAST_MEMBER_REMOVE) {
            CBX_PBMP_PORT_REMOVE(pgfov, lpg_new);
            if (index_new >= CBX_MAX_PORT_PER_UNIT) {
                return CBX_E_PARAM;
            }
            new_list[index_new] = 0;
        }
        CBX_PBMP_COUNT(pgfov, count);

        CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_entry_set(
                        unit, mcast, lilt_ndx, new_list, count));

    } /* LILT table is used */

    ent_dgt.pgfov = (uint16_t)pgfov;

    /* Write updated entry to DGT table */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_set(unit, mcast, &ent_dgt, &status));

    /* FIXME: Observing issue in PE mode while adding a port as PE cascade.
     * During 2nd call to update LILT, the ent_dgt.pgfov is not reflecting the
     * LPG added by the previous call.
     * A DGT table get call here seems to have resolved the issue */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_pe_shared_lag_update
 * Purpose:
 *  Update any changes to shared LAG lpgid in all mcast groups
 */
int
cbxi_mcast_pe_shared_lag_update(
                cbx_portid_t lagid, cbx_portid_t portid)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    cbx_mcastid_t mcastid;
    uint32_t mcast;
    int rv;

    SHR_BIT_ITER(cmi->mcast_bitmap, cmi->mcast_max, mcast) {
        CBX_PORTID_MCAST_ID_SET(mcastid, mcast);
        /* Try to remove previous LAG port entry from MCAST */
        rv = cbxi_mcast_member_update(mcast, portid, CBXI_MCAST_MEMBER_REMOVE);
        /* If the entry was present, add the shared LAG back  */
        if ((rv == CBX_E_NONE) && (lagid != CBX_LAG_INVALID)) {
            /* Remove and add back the LAG entry to Update lpgid and rlpgid info */
            rv = cbxi_mcast_member_update(mcast, lagid, CBXI_MCAST_MEMBER_REMOVE);
            CBX_IF_ERROR_RETURN(cbx_mcast_member_add(mcastid, lagid));
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_mcast_member_update
 * Purpose:
 *  Main body of cbx_mcast_member_add/remove;
 */

STATIC int
cbxi_mcast_member_update(uint32 mcast, cbx_portid_t  portid, int flags)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    cbx_mcast_lilt_mode_t lilt_mode;
    cbx_port_params_t port_params;
#ifdef CONFIG_PORT_EXTENDER
    mtgt_t ent_mtgt;
#endif /* CONFIG_PORT_EXTENDER */
    int unit = CBX_AVENGER_PRIMARY;
    dgt_t ent_dgt;
    cbx_portid_t lagid;
    cbx_port_t port;
    pbmp_t lpg_map = 0;
    pbmp_t lag_map = 0;
    pbmp_t pp_map = 0;
    cbxi_pgid_t local_pg = CBXI_PGID_INVALID;
    cbxi_pgid_t remote_pg = CBXI_PGID_INVALID;
    cbxi_pgid_t global_pg = CBXI_PGID_INVALID;
    int rv = CBX_E_NONE;
    int num_units = 1;
    int id;
    uint32_t lilt_ent;
    cbx_lag_info_t lag_info;
    uint32_t csd_lpg = CBXI_PGID_INVALID;
#ifdef CONFIG_CASCADED_MODE
    uint32_t regval = 0;
    uint32_t fval   = 0;
#endif


    CHECK_MCAST(cmi, mcast);
    if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
        return CBX_E_NOT_FOUND;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    if (CBX_PORTID_IS_VLAN_PORT(portid)) {
        /* Portid is a vlan port.  Not valid in Basic multicast mode */
        CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_mode_get(mcast, &lilt_mode));
        if (lilt_mode == cbxMcastLiltModeNone) {
            return CBX_E_PARAM;
        }

        /* Obtain local_pg and remote_pg associated with given vlan_port */
        CBX_IF_ERROR_RETURN(cbxi_robo2_vlan_port_resolve(
                                portid, &local_pg, &remote_pg));
        id = CBX_PORTID_VLAN_PORT_ID_GET(portid);
        /* Form LLIID to fill LILT table */
        lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);

        /* Update DGT and LILT */
        if (local_pg != CBXI_PGID_INVALID) {
            unit = CBX_AVENGER_PRIMARY;
            csd_lpg = CBXI_PGID_INVALID;
#ifdef CONFIG_CASCADED_MODE
            /* Do not edit if cascade port */
            CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
            soc_CB_PQM_CASCADEr_field_get(unit, &regval, ENABLEf, &fval);
            if (fval != 0) {
                soc_CB_PQM_CASCADEr_field_get(unit, &regval, LSPGIDf, &csd_lpg);
            }
#endif
            if (local_pg != csd_lpg) {
                CBX_IF_ERROR_RETURN(cbxi_mcast_entry_set(
                                unit, mcast, local_pg, lilt_ent, flags));
            }
        }
        if (remote_pg != CBXI_PGID_INVALID) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                unit = CBX_AVENGER_SECONDARY;
                csd_lpg = CBXI_PGID_INVALID;
#ifdef CONFIG_CASCADED_MODE
                /* Do not edit if cascade port */
                CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
                soc_CB_PQM_CASCADEr_field_get(unit, &regval, ENABLEf, &fval);
                if (fval != 0) {
                    soc_CB_PQM_CASCADEr_field_get(unit, &regval, LSPGIDf, &csd_lpg);
                }
#endif
                if (remote_pg != csd_lpg) {
                    CBX_IF_ERROR_RETURN(cbxi_mcast_entry_set(
                                unit, mcast, remote_pg, lilt_ent, flags));
                }
            } else {
                return CBX_E_PARAM;
            }
        }


    } else if (CBX_PORTID_IS_TRUNK(portid)) {
        CBX_IF_ERROR_RETURN(cbx_port_info_get(portid, &port_params));
        /* Check if the portid is of type LAG */

        /* Obtain bit map of LAGs that are already members of given mcastid */
        for (unit = 0; unit < num_units; unit ++) {
            /* Retrive DGT table entry */
            CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));
            lpg_map = ent_dgt.pgfov;
            CBX_IF_ERROR_RETURN(
                cbxi_robo2_pg_map_resolve(unit, lpg_map, &pp_map, &lag_map));
        }

        /* Check for current membership of LAG in given mcastid */
        rv = CBX_PORTID_TRUNK_GET(portid);
        if (rv < 0) {
            return CBX_E_PORT;
        } else {
            lagid = rv;
        }
        if (flags & CBXI_MCAST_MEMBER_ADD) {
            if (CBX_PBMP_MEMBER(lag_map, lagid)) {
                return CBX_E_EXISTS;
            }
        } else if (flags & CBXI_MCAST_MEMBER_REMOVE) {
            if (!CBX_PBMP_MEMBER(lag_map, lagid)) {
                return CBX_E_NOT_FOUND;
            }
        }

        /* Obtain LPG values on Primary and Secondary Avengers corresponding
         * to LAG id indicated by portid passed*/

        rv = cbxi_robo2_lag_get(portid, &lag_info);
        if (rv == CBX_E_NOT_FOUND) {
            LOG_ERROR(BSL_LS_FSAL_MCAST, (BSL_META("Invalid lagid\n")));
            rv = CBX_E_PORT;
        }
        if (CBX_FAILURE(rv)) {
            return rv;
        }
        local_pg = lag_info.lpgid;
        remote_pg = lag_info.rlpgid;
        global_pg = lag_info.gpgid;
        if ((local_pg == CBXI_PGID_INVALID) &&
                (remote_pg == CBXI_PGID_INVALID)) {
            return CBX_E_PORT;
        }

        /* Form LLIID to fill LILT table */
        lilt_ent = CBXI_MCAST_PACKED_DLI_PV_GET(global_pg);

        /* Update DGT and LILT */
        if (local_pg != CBXI_PGID_INVALID) {
            unit = CBX_AVENGER_PRIMARY;
#ifdef CONFIG_PORT_EXTENDER
            if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
                if (port_params.port_type != cbxPortTypeCascade){
#if 1/* PE loopback solution: Trunk members are added as DLIs */
                    CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(portid, &id));
                    lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);
#endif
                } else {
                    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                        unit, (TRAP_GROUP_TRAP2CASC + 64), &ent_mtgt));
                    id = ent_mtgt.dli_n;
                    lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);
                }
            }
#endif /* CONFIG_PORT_EXTENDER */
            CBX_IF_ERROR_RETURN(cbxi_mcast_entry_set(
                                unit, mcast, local_pg, lilt_ent, flags));
        }

        if (remote_pg != CBXI_PGID_INVALID) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                unit = CBX_AVENGER_SECONDARY;
#ifdef CONFIG_PORT_EXTENDER
                if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
                    if (port_params.port_type != cbxPortTypeCascade){
#if 1/* PE loopback solution: Trunk members are added as DLIs */
                        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(portid, &id));
                        lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);
#endif
                    } else {
                        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                            unit, (TRAP_GROUP_TRAP2CASC + 64), &ent_mtgt));
                        id = ent_mtgt.dli_n;
                        lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);
                    }
                }
#endif /* CONFIG_PORT_EXTENDER */

                CBX_IF_ERROR_RETURN(cbxi_mcast_entry_set(
                                unit, mcast, remote_pg, lilt_ent, flags));
            } else {
                return CBX_E_PARAM;
            }
        }
    } else {
        /* portid is type other than LAG:
         * physical port no., local port (physical port in portid format) */
        /* vlan_port (MM feature) needs to be handled separately */

        /* If port is a member of the trunk then we need to return error */
        rv = cbxi_robo2_lag_member_check(portid, &lagid);
        if (CBX_SUCCESS(rv)) {
            return CBX_E_PARAM;
        }


        CBX_IF_ERROR_RETURN(cbx_port_info_get(portid, &port_params));
        /* Validate the port number passed, and obtain physical port
         * corresponding to given portid */
        rv = cbxi_robo2_port_validate(portid, &port, &local_pg, &unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_MCAST, (BSL_META("Invalid port \n")));
            return rv;
        }

        /* Obtain bit map of physical ports that are members of given mcast */
        /* Retrive DGT table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));
        lpg_map = ent_dgt.pgfov;

        /* Check for current membership of port in given mcastid */
        CBX_IF_ERROR_RETURN(
                cbxi_robo2_pg_map_resolve(unit, lpg_map, &pp_map, &lag_map));
        if (flags & CBXI_MCAST_MEMBER_ADD) {
            if (CBX_PBMP_MEMBER(pp_map, port)) {
                return CBX_E_EXISTS;
            }
        } else if (flags & CBXI_MCAST_MEMBER_REMOVE) {
            if (!CBX_PBMP_MEMBER(pp_map, port)) {
                return CBX_E_NOT_FOUND;
            }
        }

        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(
                                        unit, local_pg, &global_pg));
        /* Form LLIID to fill LILT table */
        lilt_ent = CBXI_MCAST_PACKED_DLI_PV_GET(global_pg);
#ifdef CONFIG_PORT_EXTENDER
        if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
            if (port_params.port_type != cbxPortTypeCascade){
#if 1/* PE loopback solution: DGT members are added as PV type for non PE extender ports */
                CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(portid, &id));
                lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);
#endif
            } else {
                CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                    unit, (TRAP_GROUP_TRAP2CASC + 64), &ent_mtgt));
                id = ent_mtgt.dli_n;
                lilt_ent = CBXI_MCAST_PACKED_DLI_LIN_GET(id);
            }
        }
#endif /* CONFIG_PORT_EXTENDER */


        /* Update DGT and LILT */
        CBX_IF_ERROR_RETURN(cbxi_mcast_entry_set(
                                unit, mcast, local_pg, lilt_ent, flags));
    }

    return CBX_E_NONE;
}



/**************************************************************************
 *                 L2 MULTICAST FSAL API IMPLEMENTATION                   *
 **************************************************************************/

/**
 * Initialize mcast entry parameters.
 *
 * @param  mcast_params parameter object
 */
void cbx_mcast_params_t_init(cbx_mcast_params_t *mcast_params)
{
    if (mcast_params != NULL) {
        mcast_params->lilt_mode = cbxMcastLiltModeNone;
    }
    return;
}


/**
 * Multicast Create
 * This routine is used to create a mcast.
 *
 * @param mcast_params   (IN)  Multicast parameters.
 * @param mcastid        (OUT) Handle of the mcast created
 *                             CBX_MCAST_INVALID: Multicast could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_mcast_create(cbx_mcast_params_t *mcast_params, cbx_mcastid_t *mcastid )
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    uint32_t mcast;

    LOG_INFO(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_create()..\n")));

    CHECK_MCAST_INIT(cmi);

    /* get unused ID */
    for (mcast = cmi->mcast_min; mcast <= cmi->mcast_max; mcast++) {
        if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
            break;
        }
    }

    if (mcast > cmi->mcast_max) {
        return CBX_E_FULL;
    }

#ifdef CONFIG_PORT_EXTENDER
    if ((cbxi_check_port_extender_mode() == CBX_E_NONE)) {
        /* Always array type in Port extender to accomodate Cascade ports */
        mcast_params->lilt_mode = cbxMcastLiltModeArray;
    }
#endif /* CONFIG_PORT_EXTENDER */

    CBX_IF_ERROR_RETURN(cbxi_mcast_create(mcast_params, mcast));

    CBX_PORTID_MCAST_ID_SET(*mcastid, mcast);


    return CBX_E_NONE;
}

/**
 * Multicast destroy
 * This routine destroys a mcast previously created by cbx_mcast_create()
 *
 * @param mcastid  (IN)  Handle of the mcast to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mcast.
 */

int cbx_mcast_destroy(cbx_mcastid_t mcastid)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    uint32_t mcast;
    int id;

    LOG_INFO(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_destroy()..\n")));

    CHECK_MCAST_INIT(cmi);

    if ((id = CBX_PORTID_MCAST_ID_GET(mcastid)) < 0) {
        return CBX_E_PARAM;
    }

    mcast = id;

    CHECK_MCAST(cmi, mcast);

    if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
        return CBX_E_NOT_FOUND;
    }

    CBX_IF_ERROR_RETURN(cbxi_mcast_destroy(mcast));

    return CBX_E_NONE;
}

/**
 * Multicast member add
 * This routine is used to add a port to a mcast.
 *
 * @param mcastid   (IN)  MCAST Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mcast_member_add(cbx_mcastid_t mcastid, cbx_portid_t  portid)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
#ifdef CONFIG_PORT_EXTENDER
    cbxi_extender_mode_t pe_mode;
    int unit = 0;
    uint8_t num_units = 1;
    uint32_t flags_u0 = 0;
#endif
#if 0
    cbx_port_params_t port_params;
    cbx_l2_entry_t    l2entry;
    uint32_t e_cid = 0;
#endif
    uint32_t mcast, flags;
    int rv = 0;

    LOG_INFO(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_member_add()..\n")));

    CHECK_MCAST_INIT(cmi);

    if ((rv = CBX_PORTID_MCAST_ID_GET(mcastid)) < 0) {
        return CBX_E_PARAM;
    }
    mcast = rv;
    CHECK_MCAST(cmi, mcast);

    if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
        return CBX_E_NOT_FOUND;
    }

#ifdef CONFIG_CASCADED_MODE
    rv = cbxi_cascade_port_check(portid);
    if (CBX_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_member_add() \
                        cannot add cascade port %d\n"), portid));
        return CBX_E_PORT;
    }
#endif /* CONFIG_CASCADED_MODE */

    flags = CBXI_MCAST_MEMBER_ADD;
    CBX_IF_ERROR_RETURN(cbxi_mcast_member_update(mcast, portid, flags));

#ifdef CONFIG_PORT_EXTENDER
    /* PE loopback solution: Add loopback ports to MCAST groups */
    /* Add Port 14 for transit PE */
    /*FIXME:
     * Access PE : LB port not required. Do not add
     * Transit PE: Add LB port only when an PE Extender port is present */

    CBX_IF_ERROR_RETURN(cbxi_pe_mode_get(&pe_mode));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    if (pe_mode == cbxiPEModeTransit) {
        for (unit = 0; unit < num_units; unit ++) {
            flags = 0;
            CBX_IF_ERROR_RETURN(cbxi_pe_mcast_lb_check(unit, mcast, &flags));
            if (!unit) {
                flags_u0 = flags;
            }
        }
        if ((flags ==  CBXI_MCAST_MEMBER_ADD) ||
                    (flags_u0 ==  CBXI_MCAST_MEMBER_ADD)) {
            /* Add LB port on both Avengers even if one of the Avenger units has
             * an extender port*/
            for (unit = 0; unit < num_units; unit ++) {
                CBX_IF_ERROR_RETURN(cbxi_mcast_member_update(mcast,
                    (CBX_PE_LB_PORT + (unit * CBX_MAX_PORT_PER_UNIT)),
                                                    CBXI_MCAST_MEMBER_ADD));
            }
        }
    }
#endif

#if 0 /* PE loopback solution: Separate DLIs not required */
    CBX_IF_ERROR_RETURN(cbx_port_info_get(portid, &port_params));
    if (port_params.port_type == cbxPortTypeExtenderCascade) {
        CBX_IF_ERROR_RETURN(cbxi_pe_mcast_ecid_get(mcastid, &e_cid));
        if (CHECK_ECID_IS_MCAST(e_cid)) {
            /* MCAST group L2 entry is present already
             * Create LIN port to encap MCAST E_CID*/
            /* L2 entry is not used as it is setup already */
            CBX_IF_ERROR_RETURN(cbxi_pe_cascade_port_set(
                                                portid, e_cid, &l2entry));
        }
    }
#endif
    return CBX_E_NONE;
}

/**
 * Multicast remove
 * This routine is used to remove a port from a mcast.
 *
 * @param mcastid   (IN)  MCAST Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mcast_member_remove(cbx_mcastid_t mcastid, cbx_portid_t  portid)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
#ifdef CONFIG_PORT_EXTENDER
    cbxi_extender_mode_t pe_mode;
    int unit = 0;
    uint8_t num_units = 1;
    uint32_t flags_u0 = 0;
#endif
    uint32_t mcast, flags;
    int rv = 0;

    LOG_INFO(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_member_remove()..\n")));

    CHECK_MCAST_INIT(cmi);

    if ((rv = CBX_PORTID_MCAST_ID_GET(mcastid)) < 0) {
        return CBX_E_PARAM;
    }
    mcast = rv;
    CHECK_MCAST(cmi, mcast);
    if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
        return CBX_E_NOT_FOUND;
    }

#ifdef CONFIG_CASCADED_MODE
    rv = cbxi_cascade_port_check(portid);
    if (CBX_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_member_remove() \
                        cannot remove cascade port %d\n"), portid));
        return CBX_E_PORT;
    }
#endif /* CONFIG_CASCADED_MODE */


    flags = CBXI_MCAST_MEMBER_REMOVE;
    CBX_IF_ERROR_RETURN(cbxi_mcast_member_update(mcast, portid, flags));

#ifdef CONFIG_PORT_EXTENDER
    /* PE loopback solution: Remove loopback ports if not required */
    /*FIXME:
     * Access PE : LB port not required.
     * Transit PE: Add LB port only when an PE Extender port is present */

    CBX_IF_ERROR_RETURN(cbxi_pe_mode_get(&pe_mode));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    if (pe_mode == cbxiPEModeTransit) {
        for (unit = 0; unit < num_units; unit ++) {
            flags = 0;
            CBX_IF_ERROR_RETURN(cbxi_pe_mcast_lb_check(unit, mcast, &flags));
            if (!unit) {
                flags_u0 = flags;
            }
        }
        if ((flags == CBXI_MCAST_MEMBER_REMOVE) &&
                        (flags_u0 == CBXI_MCAST_MEMBER_REMOVE)) {
            /* Remove LB port on both units only when there are no extender
             * ports present in both the units */
            for (unit = 0; unit < num_units; unit ++) {
                CBX_IF_ERROR_RETURN(cbxi_mcast_member_update(mcast,
                    (CBX_PE_LB_PORT + (unit * CBX_MAX_PORT_PER_UNIT)),
                                                    CBXI_MCAST_MEMBER_REMOVE));
            }
        }
    }
#endif

    return CBX_E_NONE;
}

/**
 * Multicast Member get
 * This routine is used to get information including array of ports on a mcast.
 *
 * @param mcastid            (IN)  MCAST Identifier
 * @param mcast_member_info  (IN/OUT) MCAST Information structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mcast_member_info_get(cbx_mcastid_t mcastid,
                                cbx_mcast_member_info_t *mcast_member_info)
{
    cbx_mcast_sw_info_t *cmi = &cbx_mcast_sw_info;
    cbx_mcast_lilt_mode_t lilt_mode;
    uint32_t lilt_list[CBX_MAX_PORT_PER_UNIT];
    uint16_t lilt_ndx;
    dgt_t ent_dgt;
    lpg2ppfov_t  ent_ppfov;
    cbx_portid_t lagid;
    uint8 num_units = 1;
    int unit = 0;
    int count = 0;
    pbmp_t lpg_map = 0;
    pbmp_t pp_map = 0;
    pbmp_t lag_map = 0;
    cbx_portid_t portid;
    cbx_portid_t* portid_array;
    cbx_port_t port, lpg;
    uint32_t mcast;
    uint32_t regval = 0;
    uint8_t  pe_en = 0;
    int rv, id, skip;

    sal_memset(lilt_list, 0, sizeof(lilt_list));

    LOG_INFO(BSL_LS_FSAL_MCAST,
                (BSL_META("FSAL API : cbx_mcast_member_info_get()..\n")));

    CHECK_MCAST_INIT(cmi);

    if ((id = CBX_PORTID_MCAST_ID_GET(mcastid)) < 0) {
        return CBX_E_PARAM;
    }

    mcast = id;
    CHECK_MCAST(cmi, mcast);
    if (!SHR_BITGET(cmi->mcast_bitmap, mcast)) {
        return CBX_E_NOT_FOUND;
    }

    if ((mcast_member_info->max_portid  < 1) ||
                (mcast_member_info->portid_array == NULL)) {
        return CBX_E_PARAM;
    }

    portid_array = mcast_member_info->portid_array;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_mode_get(mcast, &lilt_mode));

    for (unit = 0; unit < num_units; unit++) {
        /* Retrive DGT table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));

        lpg_map = ent_dgt.pgfov;

        /* lag_map will get updated for both Primary and socondary avenger */
        CBX_IF_ERROR_RETURN(
                cbxi_robo2_pg_map_resolve(unit, lpg_map, &pp_map, &lag_map));
    }


    for (unit = 0; unit < num_units; unit++) {
        /* Retrive DGT table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));

        lpg_map = ent_dgt.pgfov;

        lilt_ndx = ent_dgt.lilt_ndx;

        if (lilt_mode != cbxMcastLiltModeNone) {
            /* LILT table is not used in cbxMcastLiltModeNone*/
            id = 0;
            CBX_IF_ERROR_RETURN(cbxi_mcast_lilt_entry_get(
                unit, mcast, lilt_ndx, lilt_list, CBX_MAX_PORT_PER_UNIT));

        }

        /* Get PPFOV of Cascade ports */
        CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
        soc_CB_PQM_CASCADEr_field_get(unit, &regval, PPFOVf, &pp_map);

        CBX_PBMP_ITER(lpg_map, lpg) {
            skip = 0;
            port = 0;
            rv = 0;
            CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(
                                        unit, lpg, &ent_ppfov));
            /* Remove cascade ports if present */
            CBX_PBMP_REMOVE(ent_ppfov.ppfov, pp_map);

            if (ent_ppfov.ppfov == 0) {
                /* Skip cascade ports */
                skip = 1;
            }

            CBX_PBMP_ITER(ent_ppfov.ppfov, rv) {
                /* Get one of the port members */
                port = rv;
            }
            rv = 0;
            CBX_PORTID_LOCAL_SET(portid, CBXI_GLOBAL_PORT(port, unit));
            /* Check if port belongs to LAG */

            rv = cbxi_robo2_lag_member_check(
                                    CBXI_GLOBAL_PORT(port, unit), &lagid);
            if (CBX_SUCCESS(rv)) {
                rv = CBX_PORTID_TRUNK_GET(lagid);
                if (rv < 0) {
                    return CBX_E_PORT;
                } else if (CBX_PBMP_MEMBER(lag_map, rv)) {
                    /* Add lagid to portid list and remove from lag_map */
                    portid = lagid;
                    CBX_PBMP_PORT_REMOVE(lag_map, rv);
                } else {
                    /* !CBX_PBMP_MEMBER(lag_map, rv) */
                    /* LAG is already in portid list. Move to next LPG */
                    skip = 1;
                }
            }
#ifdef CONFIG_PORT_EXTENDER
            if ((cbxi_check_port_extender_mode() == CBX_E_NONE)) {
                pe_en = 1;
            }
#endif /* CONFIG_PORT_EXTENDER */

            if (pe_en == 1) {
                if (port == CBX_PE_LB_PORT) {
                    /* Skip Loopback port */
                    skip = 1;
                }
                /* All ports in PE mode are of type PP or LAG */
            } else {
                if (((lilt_mode == cbxMcastLiltModeArray) &&
                (CHECK_PACKED_DLI_LIN(lilt_list[id]))) ||
                    (((lilt_mode == cbxMcastLiltModeList) &&
                        (CHECK_NORMAL_DLI_LIN(lilt_list[id]))))) {
                /* Check if the DLIIDs in LILT belongs to a vlan_port */
                /* Construct vlan_port  */
                    CBX_PORTID_VLAN_PORT_ID_SET(portid, lilt_list[id]);
                }
            }
            id++;

            if (!skip) {
                *portid_array = portid;
                portid_array++;
                count++;
                if (count >= mcast_member_info->max_portid) {
                    mcast_member_info->valid_portid = count;
                    return CBX_E_NONE;
                }
            }
        }
    }

    mcast_member_info->valid_portid = count;

    return CBX_E_NONE;
}

/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     lif.c
 * * Purpose:
 * *     Defines internal functions to handle Logincal Interfaces.
 * *
 * */

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT

#include <fsal/error.h>
#include <fsal_int/lin.h>
#include <fsal_int/lif.h>
#include <fsal_int/slic.h>
#include <fsal_int/cfp.h>
#include <fsal_int/l2.h>
#include <fsal_int/encap.h>
#include <fsal/vlan.h>
#include <fsal_int/vlan.h>
#include <fsal/lif.h>
#include <bsl_log/bsl.h>
#include <soc/drv.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <shared/bitop.h>

cbxi_lif_sw_info_t lif_info;

/*
 * Function:
 *  cbxi_lif_vsi_get
 * Purpose:
 *  Get VSI and mode associated with LIF
 */

int
cbxi_lif_vsi_get(int lin, cbx_vlan_t *vsi, uint16_t *mode)
{
#ifdef CBX_MANAGED_MODE
    liflist_t **list = &lif_info.list;

    while (*list != NULL) {
        if ((*list)->lin == lin) {
            *vsi  = (*list)->vsi;
            *mode = (*list)->mode;
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lif_vsi_set
 * Purpose:
 *  Set VSI and mode associated with LIF
 */

int
cbxi_lif_vsi_set(int lin, cbx_vlan_t vsi, uint16_t mode)
{
#ifdef CBX_MANAGED_MODE
    liflist_t **list = &lif_info.list;

    while (*list != NULL) {
        if ((*list)->lin == lin) {
            (*list)->vsi  = vsi;
            (*list)->mode = mode;
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lif_vsi_port_lookup
 * Purpose:
 *  Provide array of all LIFs associated with given VSI
 */

int
cbxi_lif_vsi_port_lookup(cbx_vlan_t vsi, int array_max,
        cbx_portid_t *portid_array, uint32_t *mode_array, int *array_size)
{
#ifdef CBX_MANAGED_MODE
    liflist_t **list = &lif_info.list;

    while (*list != NULL) {
        if ((*list)->vsi == vsi) {
            CBX_PORTID_VLAN_PORT_ID_SET(*portid_array, (*list)->lin);
            *mode_array = (*list)->mode;

            portid_array++;
            mode_array++;
            *array_size += 1;

            if (*array_size >= array_max) {
                return CBX_E_NONE;
            }
        }
        list = &(*list)->next;
    }
    return CBX_E_NONE;
#endif /* CBX_MANAGED_MODE */
}

/*
 * Function:
 *  cbxi_lif_list_insert
 * Purpose:
 *  Add a LIF SW list
 */

STATIC int
cbxi_lif_list_insert(cbx_vlan_lif_params_t *params, int lin)
{
#ifdef CBX_MANAGED_MODE
    liflist_t **list = &lif_info.list;
    liflist_t *lif_i;

    if ((lif_i = sal_alloc(sizeof (liflist_t), "liflist")) == NULL) {
        return CBX_E_MEMORY;
    }

    if ((lif_i->params = sal_alloc(sizeof (cbx_vlan_lif_params_t), "lifparam")) == NULL) {
        return CBX_E_MEMORY;
    }

    lif_i->lin = lin;
    sal_memcpy(lif_i->params, params, sizeof(cbx_vlan_lif_params_t));

    /* Initialize vsi invalid. VLAN port add will set it up */
    lif_i->vsi = CBX_VID_INVALID;

    lif_i->next = *list;
    *list = lif_i;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_lif_list_remove
 * Purpose:
 *  Remove a LIF SW list
 */

STATIC int
cbxi_lif_list_remove(int lin)
{
#ifdef CBX_MANAGED_MODE
    liflist_t **list = &lif_info.list;
    liflist_t *lif_i;

    while (*list != NULL) {
        if ((*list)->lin == lin) {
            lif_i = *list;
            *list = lif_i->next;
            sal_free(lif_i->params);
            sal_free(lif_i);
            return CBX_E_NONE;
        }
        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lif_list_lookup
 * Purpose:
 *  Lookup a LIF in SW list
 */

STATIC int
cbxi_lif_list_lookup(cbx_vlan_lif_params_t *params, int *linid)
{
#ifdef CBX_MANAGED_MODE
    liflist_t **list = &lif_info.list;
    cbx_vlan_lif_params_t *tmp_par;
    cbxi_pgid_t tmp_spgid = CBXI_PGID_INVALID;
    cbxi_pgid_t lif_spgid = CBXI_PGID_INVALID;

    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_resolve(params->portid, &lif_spgid));
    while (*list != NULL) {
        tmp_par = (*list)->params;
        /* Match Portid */
        CBX_IF_ERROR_RETURN(cbxi_robo2_portid_resolve(
                                        tmp_par->portid, &tmp_spgid));

        if (lif_spgid == tmp_spgid) {
            /* Match Criteria */
            if ((tmp_par->criteria == params->criteria) &&
                (tmp_par->match_vlan == params->match_vlan) &&
                (tmp_par->match_inner_vlan == params->match_inner_vlan)) {
                /* Match Action */
                if ((tmp_par->vlan_action == params->vlan_action) &&
                    (tmp_par->egress_vlan == params->egress_vlan) &&
                    (tmp_par->inner_vlan_action == params->inner_vlan_action) &&
                    (tmp_par->egress_inner_vlan == params->egress_inner_vlan)) {
                        *linid = (*list)->lin;
                        return CBX_E_NONE;
                }
            }
        }

        list = &(*list)->next;
    }
    return CBX_E_NOT_FOUND;
#endif /* CBX_MANAGED_MODE */
    return CBX_E_NONE;
}


int cbxi_seed_shift_r128(ghst_t* seed, int shift)
{
    if (shift > 128) {
        return CBX_E_PARAM;
    }

    if (shift >= 32) {
        seed->seed_31_0   = seed->seed_63_32;
        seed->seed_63_32  = seed->seed_95_64;
        seed->seed_95_64  = seed->seed_113_96;
        seed->seed_113_96 = 0;
        cbxi_seed_shift_r128(seed, shift - 32);
    } else {
        seed->seed_31_0   = (seed->seed_63_32 << (32 - shift))  |
                                                (seed->seed_31_0 >> shift);
        seed->seed_63_32  = (seed->seed_95_64 << (32 - shift))  |
                                                (seed->seed_63_32 >> shift);
        seed->seed_95_64  = (seed->seed_113_96 << (32 - shift)) |
                                                (seed->seed_95_64 >> shift);
        seed->seed_113_96 = (seed->seed_113_96 >> shift);

    }
    return CBX_E_NONE;
}

int cbxi_lim_hash_key(uint32_t *key, ghst_t *seed, int len, uint32_t *index) {
    int ii;
    ghst_t seed_shift;

    *index = 0;

    for (ii = 113; ii >= (113 - len); ii--) {
        if (SHR_BITGET(key, ii)) {
            sal_memcpy(&seed_shift, seed, sizeof(ghst_t));
            cbxi_seed_shift_r128(&seed_shift, ii);
            *index ^= seed_shift.seed_31_0;
        }
    }


    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_lim_table_init
 * Purpose:
 *   Initialize LIM tables
 *
 */
STATIC int
cbxi_lim_table_init( void )
{
    uint32        num_units = 1;
    int           unit = 0;

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        /* --------------- LIM configurations ----------- */
        /* Enable and reset M2TK */
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_reset(unit));

        /* Enable and reset GHTDT */
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_reset(unit));

        /* Enable and reset GHST */
        CBX_IF_ERROR_RETURN(soc_robo2_ghst_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_ghst_reset(unit));

        /* Enable and reset GHT */
        CBX_IF_ERROR_RETURN(soc_robo2_ght_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_ght_reset(unit));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lim_init
 * Purpose:
 *  Initialize LIM.
 *
 */
int cbxi_lif_init( void )
{
    m2tk_t  ent_m2tk;
    ghtdt_t ent_ghtdt;
    ghst_t  ent_ghst;
    ght_t   ent_ght;
    uint32_t status = 0;
    uint32   num_units = 1;
    int      unit = 0;
    uint32_t start, end;

    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
        return CBX_E_NONE;
    }

    /* Reset SW structure */
    sal_memset(&lif_info, 0, sizeof (cbxi_lif_sw_info_t));
    lif_info.list = NULL;

    CBX_IF_ERROR_RETURN(cbxi_lim_table_init());

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        /* Initialize seed values */
        CBX_IF_ERROR_RETURN(soc_robo2_ghst_get(unit, 0, &ent_ghst));
        ent_ghst.seed_31_0   = 0x11221122;
        ent_ghst.seed_63_32  = 0x33443344;
        ent_ghst.seed_95_64  = 0x55665566;
        ent_ghst.seed_113_96 = 0x77887788;
        CBX_IF_ERROR_RETURN(soc_robo2_ghst_set(unit, 0, &ent_ghst, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_ghst_get(unit, 1, &ent_ghst));
        ent_ghst.seed_31_0   = 0x00110011;
        ent_ghst.seed_63_32  = 0x00220022;
        ent_ghst.seed_95_64  = 0x00330033;
        ent_ghst.seed_113_96 = 0x00440044;
        CBX_IF_ERROR_RETURN(soc_robo2_ghst_set(unit, 1, &ent_ghst, &status));

        /* Configure M2TK table */
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_get(unit, CBXI_LIM_M2TK_PV, &ent_m2tk));
        ent_m2tk.table_id = CBXI_GHT_IDX_PV;
        ent_m2tk.key_id = CBXI_IKFT_LIM_PV_KEY_ID;
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_set(unit, CBXI_LIM_M2TK_PV, &ent_m2tk, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_get(unit, CBXI_LIM_M2TK_PV_DECAP, &ent_m2tk));
        ent_m2tk.table_id = CBXI_GHT_IDX_PV_DECAP;
        ent_m2tk.key_id = CBXI_IKFT_LIM_PV_KEY_ID;
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_set(unit, CBXI_LIM_M2TK_PV_DECAP, &ent_m2tk, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_get(unit, CBXI_LIM_M2TK_PVV, &ent_m2tk));
        ent_m2tk.table_id = CBXI_GHT_IDX_PVV;
        ent_m2tk.key_id = CBXI_IKFT_LIM_PVV_KEY_ID;
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_set(unit, CBXI_LIM_M2TK_PVV, &ent_m2tk, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_get(unit, CBXI_LIM_M2TK_PVV_DECAP, &ent_m2tk));
        ent_m2tk.table_id = CBXI_GHT_IDX_PVV_DECAP;
        ent_m2tk.key_id = CBXI_IKFT_LIM_PVV_KEY_ID;
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_set(unit, CBXI_LIM_M2TK_PVV_DECAP, &ent_m2tk, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_get(unit, CBXI_LIM_M2TK_MIM, &ent_m2tk));
        ent_m2tk.table_id = CBXI_GHT_IDX_MIM;
        ent_m2tk.key_id = CBXI_IKFT_LIM_MIM_KEY_ID;
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_set(unit, CBXI_LIM_M2TK_MIM, &ent_m2tk, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_get(unit, CBXI_LIM_M2TK_MPLS, &ent_m2tk));
        ent_m2tk.table_id = CBXI_GHT_IDX_MPLS;
        ent_m2tk.key_id = CBXI_IKFT_LIM_MPLS_KEY_ID;
        CBX_IF_ERROR_RETURN(soc_robo2_m2tk_set(unit, CBXI_LIM_M2TK_MPLS, &ent_m2tk, &status));



        /* Configure GHT tables using GHTDT table */
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(unit, CBXI_GHT_IDX_PV, &ent_ghtdt));
        ent_ghtdt.num_entries = CBXI_GHT_SIZE_SHIFT_PV;
        /* 2^8 entries. Since auto_base_disable is not set, base will be auto
         * calculated*/
        ent_ghtdt.seed0 = 0;
        ent_ghtdt.seed1 = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_set(unit, CBXI_GHT_IDX_PV, &ent_ghtdt, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(unit, CBXI_GHT_IDX_PV_DECAP, &ent_ghtdt));
        ent_ghtdt.num_entries = CBXI_GHT_SIZE_SHIFT_PV_DECAP;
        /* 2^8 entries. Since auto_base_disable is not set, base will be auto
         * calculated*/
        ent_ghtdt.seed0 = 0;
        ent_ghtdt.seed1 = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_set(unit, CBXI_GHT_IDX_PV_DECAP, &ent_ghtdt, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(unit, CBXI_GHT_IDX_PVV, &ent_ghtdt));
        ent_ghtdt.num_entries = CBXI_GHT_SIZE_SHIFT_PVV;
        /* 2^8 entries. Since auto_base_disable is not set, base will be auto
         * calculated*/
        ent_ghtdt.seed0 = 0;
        ent_ghtdt.seed1 = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_set(unit, CBXI_GHT_IDX_PVV, &ent_ghtdt, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(unit, CBXI_GHT_IDX_PVV_DECAP, &ent_ghtdt));
        ent_ghtdt.num_entries = CBXI_GHT_SIZE_SHIFT_PVV_DECAP;
        /* 2^8 entries. Since auto_base_disable is not set, base will be auto
         * calculated*/
        ent_ghtdt.seed0 = 0;
        ent_ghtdt.seed1 = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_set(unit, CBXI_GHT_IDX_PVV_DECAP, &ent_ghtdt, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(unit, CBXI_GHT_IDX_MIM, &ent_ghtdt));
        ent_ghtdt.num_entries = CBXI_GHT_SIZE_SHIFT_MIM;
        /* 2^11 entries. Since auto_base_disable is not set, base will be auto
         * calculated*/
        ent_ghtdt.seed0 = 0;
        ent_ghtdt.seed1 = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_set(unit, CBXI_GHT_IDX_MIM, &ent_ghtdt, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(unit, CBXI_GHT_IDX_MPLS, &ent_ghtdt));
        ent_ghtdt.num_entries = CBXI_GHT_SIZE_SHIFT_MPLS;
        /* 2^11 entries. Since auto_base_disable is not set, base will be auto
         * calculated*/
        ent_ghtdt.seed0 = 0;
        ent_ghtdt.seed1 = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_set(unit, CBXI_GHT_IDX_MPLS, &ent_ghtdt, &status));


        /* Reset all GHT entries (all 4 tables)*/
        CBX_IF_ERROR_RETURN(soc_robo2_ght_get(unit, 0, &ent_ght));
        sal_memset(&ent_ght, 0, sizeof(ght_t));
        /* FIXME : Clear all ght entries in all GHT tables */
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(
                        unit, 0, CBXI_GHT_SIZE_PV -1 , 0, &ent_ght));

        /* For GHT, table_id needs to be passed as part of index
         * index[20:23] = table_id */
        start = (CBXI_GHT_IDX_PV << 20);
        end = (CBXI_GHT_IDX_PV << 20) | (CBXI_GHT_SIZE_PV -1);
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(unit, start, end, 0, &ent_ght));

        start = (CBXI_GHT_IDX_PV_DECAP << 20);
        end = (CBXI_GHT_IDX_PV_DECAP << 20) | (CBXI_GHT_SIZE_PV_DECAP -1);
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(unit, start, end, 0, &ent_ght));

        start = (CBXI_GHT_IDX_PVV << 20);
        end = (CBXI_GHT_IDX_PVV << 20) | (CBXI_GHT_SIZE_PVV -1);
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(unit, start, end, 0, &ent_ght));

        start = (CBXI_GHT_IDX_PVV_DECAP << 20);
        end = (CBXI_GHT_IDX_PVV_DECAP << 20) | (CBXI_GHT_SIZE_PVV -1);
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(unit, start, end, 0, &ent_ght));

        start = (CBXI_GHT_IDX_MIM << 20);
        end = (CBXI_GHT_IDX_MIM << 20) | (CBXI_GHT_SIZE_MIM -1);
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(unit, start, end, 0, &ent_ght));

        start = (CBXI_GHT_IDX_MPLS << 20);
        end = (CBXI_GHT_IDX_MPLS << 20) | (CBXI_GHT_SIZE_MPLS -1);
        CBX_IF_ERROR_RETURN(soc_robo2_ght_init(unit, start, end, 0, &ent_ght));
    }

    lif_info.init = TRUE;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lim_ght_key_set
 * Purpose:
 *  test LIM.
 *
 */
int cbxi_lim_ght_key_set(cbxi_vlan_port_match_t type,
                        uint32_t *lim_key, int linid, uint32_t flags)
{
    ghtdt_t  ent_ghtdt;
    ght_t    ent_ght;
    ghst_t   seed_n;
    uint32_t idx_even = 0, idx_odd = 0;
    uint32_t status = 0;
    uint32   num_units = 1;
    int      unit = 0;
    uint32_t table_idx = 0;
    uint32_t key_len = 0;


    switch (type) {
    case cbxiVlanLifMatchPortVlan:
        key_len = 17; /* SPG + VID = 5 + 12 = 17 */
        if (flags & CBX_VLAN_LIF_INNER_VLAN_PRESERVE) {
            /* Use LIM1 to preserve Inner VLAN */
            table_idx = CBXI_GHT_IDX_PV;
        } else {
            /* Use LIM0 to remove Inner VLAN */
            table_idx = CBXI_GHT_IDX_PV_DECAP;
        }
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(
                        CBX_AVENGER_PRIMARY, table_idx, &ent_ghtdt));
        break;
    case cbxiVlanLifMatchPortVlanStacked:
        key_len = 29; /* SPG + VID + VID= 5 + 12 +12 = 29 */
        if (flags & CBX_VLAN_LIF_INNER_VLAN_PRESERVE) {
            /* Use LIM1 to preserve Inner VLAN */
            table_idx = CBXI_GHT_IDX_PVV;
        } else {
            /* Use LIM0 to remove Inner VLAN */
            table_idx = CBXI_GHT_IDX_PVV_DECAP;
        }
        CBX_IF_ERROR_RETURN(soc_robo2_ghtdt_get(
                        CBX_AVENGER_PRIMARY, table_idx, &ent_ghtdt));
        break;
    default:
        return CBX_E_PARAM;
        break;
    };

    /****************    Calculate Hash ***************/
    status = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_ghst_get(
                CBX_AVENGER_PRIMARY, ent_ghtdt.seed0, &seed_n));
    /* If a GHT has size (2^n) then the low order n-1 bits returned from the hash
     * function called with the 'even' seed are multiplied by 2 and the
     * result is used to extract the even entry while the low order n-1 bits
     * returned from the hash function called with the 'odd' seed are
     * multiplied by 2 and summed with 1 and the result is used to extract the
     * odd entry */
    cbxi_lim_hash_key(&lim_key[0], &seed_n, key_len, &status);
    idx_even = ((status & ((1 << (ent_ghtdt.num_entries - 1)) -1)) * 2);

    status = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_ghst_get(
                CBX_AVENGER_PRIMARY, ent_ghtdt.seed1, &seed_n));
    cbxi_lim_hash_key(&lim_key[0], &seed_n, key_len, &status);
    idx_odd = ((status & ((1 << (ent_ghtdt.num_entries - 1)) - 1)) *2) + 1;


    /**************************************************/

    /* Check if HASH0 idx is free else configure in HASH1 idx */
    /* index[20:23] = table_id */
    idx_even |= (table_idx << 20);
    idx_odd  |= (table_idx << 20);
    CBX_IF_ERROR_RETURN(soc_robo2_ght_get(CBX_AVENGER_PRIMARY, idx_even, &ent_ght));
    if (!ent_ght.valid) {
        table_idx = idx_even;
    } else {
        CBX_IF_ERROR_RETURN(soc_robo2_ght_get(
                CBX_AVENGER_PRIMARY, idx_odd, &ent_ght));
        if (!ent_ght.valid) {
            table_idx = idx_odd;
        } else {
            return CBX_E_FULL;
        }
    }

    ent_ght.key_31_0    = lim_key[0];
    ent_ght.key_63_32   = lim_key[1];
    ent_ght.key_95_64   = lim_key[2];
    ent_ght.key_113_96  = lim_key[3] & 0x3FFFF;
    ent_ght.n_or_vsi    = linid;
    ent_ght.li_type     = 0; /* N type SLI */
    ent_ght.valid       = 1;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        CBX_IF_ERROR_RETURN(soc_robo2_ght_set(unit, table_idx, &ent_ght, &status));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lif_dest_set
 * Purpose:
 *  test LIM.
 *
 */
int cbxi_lif_dest_set(cbx_portid_t portid, int linid)
{
    int         unit = 0;
    uint32      num_units = 1;
    pg2lpg_t    ent_pg2lpg;
    cbxi_pgid_t spgid;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_resolve(portid, &spgid));

    for ( unit = 0; unit < num_units; unit++ ) {
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(unit, spgid, &ent_pg2lpg));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, linid, ent_pg2lpg.lpg));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lif_lim_idx_get
 * Purpose:
 *  Get LIM entry idx for given LIN id..
 *
 */
int cbxi_lif_lim_idx_get(int linid, int *idx)
{
    ght_t    ent_ght;
    int      i = 0;

    /* FIXME: Need to calculate the max idx */
    for (i = 0; i < CBXI_GHT_SIZE_PV; i++) {
        CBX_IF_ERROR_RETURN(soc_robo2_ght_get(
                                CBX_AVENGER_PRIMARY, i, &ent_ght));
        if ((ent_ght.valid == 1) && (ent_ght.li_type == 0)) {
            if (ent_ght.n_or_vsi == linid) {
                *idx = i;
                return CBX_E_NONE;
            }
        }
    }
    *idx = -1;
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lif_tc_dp_set
 * Purpose:
 *  Set default TC, DP for LIF
 *
 */
int cbxi_lif_tc_dp_set(int tc, int dp, cbx_portid_t portid)
{
    lin2vsi_t ent_lin2vsi;
    uint32_t  status = 0;
    int       linid;
    int       unit = 0;
    uint8_t   num_units = 1;

    linid = CBX_PORTID_VLAN_PORT_ID_GET(portid);
    if (linid < 0) {
        return CBX_E_PARAM;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_get(unit, linid, &ent_lin2vsi));
        ent_lin2vsi.tc = tc;
        ent_lin2vsi.use_tc = 1;
        ent_lin2vsi.dp = dp;
        ent_lin2vsi.use_dp = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_set(
                                        unit, linid, &ent_lin2vsi, &status));
    }

    return CBX_E_NONE;
}


/**************************************************************************
 *        Logical interface  FSAL API IMPLEMENTATION                      *
 **************************************************************************/

/**
 * Initialize vlan logical interface entry parameters.
 *
 * @param  vlan_lif_params  parameter object
 */
void cbx_vlan_lif_params_t_init( cbx_vlan_lif_params_t *vlan_lif_params )
{
    sal_memset(vlan_lif_params, 0, sizeof(cbx_vlan_lif_params_t));
    vlan_lif_params->criteria = cbxVlanLifMatchNone;
    vlan_lif_params->match_vlan = CBX_VID_INVALID;
    vlan_lif_params->match_inner_vlan = CBX_VID_INVALID;

    return;
}


/**
 * Create a vlan_lif
 * This routine is used to create a VLAN Logical Interface (vlan_lif)
 *
 * @param vlan_lif_params  (IN)  vlan_lif parameters.
 * @param vlan_lifid       (OUT) Handle of the vlan_lif created
 *                            CBX_VLAN_LIF_INVALID  vlan_lif could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_lif_create ( cbx_vlan_lif_params_t *vlan_lif_params,
                          cbx_portid_t          *vlan_lifid )
{
    cbxi_vlan_port_match_t type;
    cbxi_encap_record_t    encap_record;
    uint32_t    lim_key[4];
    cbxi_pgid_t spgid;
    int         linid;
    cbx_port_t  port;
    cbxi_pgid_t lpgid;
    int         unit = 0;
    uint32_t    status = 0;

    CHECK_LIF_INIT(lif_info);

    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_resolve(
                                vlan_lif_params->portid, &spgid));

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
        vlan_lif_params->portid, &port, &lpgid, &unit));

    sal_memset(&lim_key, 0, (sizeof(uint32_t) * 4));
    sal_memset(&encap_record, 0, sizeof(cbxi_encap_record_t));

    /* Match the FSAL criteria to internal criteria type */
    switch (vlan_lif_params->criteria) {
        case cbxVlanLifMatchInvalid:
            return CBX_E_PARAM;
        case cbxVlanLifMatchNone:
            type = cbxiVlanLifMatchNone;
            break;
        case cbxVlanLifMatchPortVlan:
            if (vlan_lif_params->match_inner_vlan != CBX_VID_INVALID) {
                type = cbxiVlanLifMatchPortVlan;
                /* key[113:109] = SPG
                 * key[109:97] = match_inner_vid */
                lim_key[3] |= ((spgid & 0x1F) << 13);
                lim_key[3] |= (vlan_lif_params->match_inner_vlan << 1);
            } else {
                /* Parameters doesn't match criteria */
                return CBX_E_PARAM;
            }
            break;
        case cbxVlanLifMatchPortVlanStacked:
            if ((vlan_lif_params->match_vlan != CBX_VID_INVALID) &&
                    (vlan_lif_params->match_inner_vlan != CBX_VID_INVALID)) {
                type = cbxiVlanLifMatchPortVlanStacked;
                /* key[113:109] = SPG
                 * key[109:97] = match_outer_vid
                 * key[96:84] = match_inner_vid*/
                lim_key[3] |= ((spgid & 0x1F) << 13);
                lim_key[3] |= (vlan_lif_params->match_vlan << 1);

                lim_key[3] |= (vlan_lif_params->match_inner_vlan >> 11) & 0x1;
                lim_key[2] |= ((vlan_lif_params->match_inner_vlan & 0x7FF) << 21);
            } else {
                /* Parameters doesn't match criteria */
                return CBX_E_PARAM;
            }
            break;
        default:
            return CBX_E_PARAM;
    }

    CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&linid));
    if (vlan_lif_params->criteria != cbxVlanLifMatchNone) {
        CBX_IF_ERROR_RETURN(cbxi_lim_ght_key_set(
                        type, &lim_key[0], linid, vlan_lif_params->flags));
    }

    /* ********* Egress configuration ********* */
    /* Configure Encap on the LIN port */

    uint8 encap_data[3];

    /* FIXME: Configure EPMT index */

    /* Add encaps for CTAG */
    encap_data[0] = ((cbxiTagCtpid0 << 5)|(lpgid + 1)); /* TPID of CTAG | primap idx*/
    encap_data[1] = (0x0 << 4) ; /* Hardcoded PCP, DEI */
    encap_data[1] = (vlan_lif_params->egress_inner_vlan >> 8) & 0xF; /* VID [11:8] */
    encap_data[2] = (vlan_lif_params->egress_inner_vlan & 0xFF); /* VID [7:0] */
    if (vlan_lif_params->inner_vlan_action == cbxVlanLifTagActionAdd) {
        CBX_EPP_INSERT_4B_TAG(&encap_record, encap_data);
    } else if (vlan_lif_params->inner_vlan_action == cbxVlanLifTagActionReplace) {
        CBX_EPP_INSERT_CVLAN_EDIT(&encap_record, encap_data);
    }

    /* Add encaps for STAG */
    encap_data[0] = ((cbxiTagStpid0 << 5)|(lpgid + 1)); /* TPID of CTAG | primap idx*/
    encap_data[1] = (0x0 << 4) ; /* Hardcoded PCP, DEI */
    encap_data[1] = (vlan_lif_params->egress_vlan >> 8) & 0xF; /* VID [11:8] */
    encap_data[2] = (vlan_lif_params->egress_vlan & 0xFF); /* VID [7:0] */
    if (vlan_lif_params->vlan_action == cbxVlanLifTagActionAdd) {
        CBX_EPP_INSERT_4B_TAG(&encap_record, encap_data);
    } else if (vlan_lif_params->vlan_action == cbxVlanLifTagActionReplace) {
        CBX_EPP_INSERT_SVLAN_EDIT(&encap_record, encap_data);
    }

    CBX_EPP_INSERT_EOR(&encap_record);

    /* The behavior depends on CB_ETM_Cascade_Port_Enable reg value.
     * If Cascade port is set in the above reg, then put same encap directive in
     * both Avengers, otherwise configure DLIET of only one Avenger
     */

    REG_READ_CB_ETM_CASCADE_PORT_ENABLEr(unit, &status);
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && (status == 0)) {
        if (unit) {
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(vlan_lif_params->portid,
                        linid, 0, &encap_record, CBXI_ENCAP_AVG_SEC));
        } else {
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(vlan_lif_params->portid,
                        linid, 0, &encap_record, CBXI_ENCAP_AVG_PRI));
        }
    } else {
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(vlan_lif_params->portid,
                        linid, 0, &encap_record, CBXI_ENCAP_AVG_SYS));
    }

    /* Set LIN destination */
    CBX_IF_ERROR_RETURN(cbxi_lif_dest_set(vlan_lif_params->portid, linid));


    /* Return Portid */
    CBX_PORTID_VLAN_PORT_ID_SET(*vlan_lifid, linid);

    /* Assign default TC and DP if non zero */
    if ((vlan_lif_params->default_tc != 0) ||
                (vlan_lif_params->default_dp != 0)) {
        CBX_IF_ERROR_RETURN(cbxi_lif_tc_dp_set(vlan_lif_params->default_tc,
                                vlan_lif_params->default_dp, *vlan_lifid));
    }

#ifdef CBX_MANAGED_MODE
    /* Update LIF software list */
    CBX_IF_ERROR_RETURN(cbxi_lif_list_insert(vlan_lif_params, linid));
#endif

    return CBX_E_NONE;
}

/**
 * Destroy a vlan logical interface previously created by cbx_vlan_lif_create()
 *
 * @param vlan_lifid    Handle of the vlan_lif to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created vlan logical interface.
 */

int cbx_vlan_lif_destroy ( cbx_portid_t vlan_lifid )
{
    int      idx = -1;
    int      linid;
    ght_t    ent_ght;
    uint32_t sts = 0;

    CHECK_LIF_INIT(lif_info);

    linid = CBX_PORTID_VLAN_PORT_ID_GET(vlan_lifid);
    if (linid < 1) {
        return CBX_E_PARAM;
    }

    /* Check if LIM table is used for the given LIN id and clear LIM table */
    CBX_IF_ERROR_RETURN(cbxi_lif_lim_idx_get(linid, &idx));

    if (idx >= 0) {
        CBX_IF_ERROR_RETURN(soc_robo2_ght_set(
                                CBX_AVENGER_PRIMARY, idx, &ent_ght, &sts));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(soc_robo2_ght_set(
                        CBX_AVENGER_SECONDARY, idx, &ent_ght, &sts));
        }

    }

    /* Clear all encap for LIN */
    CBX_IF_ERROR_RETURN(cbxi_lin_encap_release(linid));

    /* Release LIN slot */
    CBX_IF_ERROR_RETURN(cbxi_lin_slot_release(linid));

#ifdef CBX_MANAGED_MODE
    /* Update LIF software list */
    CBX_IF_ERROR_RETURN(cbxi_lif_list_remove(linid));
#endif

    return CBX_E_NONE;
}

/**
 * Find a vlan logical interface previously created by cbx_vlan_lif_create()
 *  based on vlan_lif parameter match criteria
 *
 * @param vlan_lif_params    (IN)  vlan_lif parameters to match.
 * @param vlan_lifid         (OUT) Handle of the vlan_lif found
 *                               CBX_VLAN_LIF_INVALID if not found
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 */

int cbx_vlan_lif_find ( cbx_vlan_lif_params_t *vlan_lif_params,
                        cbx_portid_t          *vlan_lifid )
{
    int linid;

    CHECK_LIF_INIT(lif_info);

#ifdef CBX_MANAGED_MODE
    CBX_IF_ERROR_RETURN(cbxi_lif_list_lookup(vlan_lif_params, &linid));
    CBX_PORTID_VLAN_PORT_ID_SET(*vlan_lifid, linid);
    return CBX_E_NONE;
#else
    return CBX_E_UNAVAIL;
#endif
}
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */

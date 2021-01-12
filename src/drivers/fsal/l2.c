/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     l2.c
 * * Purpose:
 * *     Robo2 l2 module
 * *
 * */

#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal/l2.h>
#include <fsal/mcast.h>
#include <fsal/vlan.h>
#include <fsal_int/l2.h>
#include <fsal_int/port.h>
#include <fsal_int/vlan.h>
#include <fsal_int/lag.h>
#include <fsal_int/types.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif /* CONFIG_PORT_EXTENDER */
#include <sal_alloc.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/arl_scan.h>
#include <soc/robo2/common/regacc.h>

cbx_l2_info_t cbx_l2_info;

typedef struct arl_seed_s {
    /**
     * Bits 0 thru 31 of a seed.
     */
    uint32  seed_31_0;

    /**
     * Bits 32 thru 59 of a seed.
     */
    uint32  seed_59_32;

} arl_seed_t;



int cbxi_seed_shift_r64(arl_seed_t* seed, int shift)
{
    if (shift > 64) {
        return CBX_E_PARAM;
    }

    if (shift >= 32) {
        seed->seed_31_0   = seed->seed_59_32;
        seed->seed_59_32  = 0;
        cbxi_seed_shift_r64(seed, shift - 32);
    } else if (shift != 0) {
        seed->seed_31_0   = (seed->seed_59_32 << (32 - shift))  |
                                                (seed->seed_31_0 >> shift);
        seed->seed_59_32  = (seed->seed_59_32 >> shift);
    }
    return CBX_E_NONE;
}

int cbxi_arl_hash_key(uint32_t *key, arl_seed_t *seed, int len, uint32_t *index) {
    int ii;
    arl_seed_t seed_shift;

    *index = 0;

    for (ii = 59; ii >= (59 - len); ii--) {
        if (SHR_BITGET(key, ii)) {
            sal_memcpy(&seed_shift, seed, sizeof(arl_seed_t));
            cbxi_seed_shift_r64(&seed_shift, ii);
            *index ^= seed_shift.seed_31_0;
        }
    }

    return CBX_E_NONE;
}


int cbxi_arl_learn_suspend(int suspend)
{
    int unit = 0;
    uint8 num_units = 1;
    uint32_t regval = 0;
    uint32_t val = 0;


    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(unit, &regval));
        if (suspend) {
            /* Disable learn */
            val = 1;
        } else {
            /* Enable learn */
            val = 0;
        }
        soc_CB_ITM_ARL_CONFIGr_field_set(unit, &regval, DISABLE_LEARNINGf, &val);
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_CONFIGr(unit, &regval));
    }

    return CBX_E_NONE;
}

int cbxi_arl_aging_suspend(int suspend)
{
    int unit = 0;
    uint8 num_units = 1;
    uint32_t regval = 0;
    uint32_t val = 0;


    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_AGING_CONFIGr(unit, &regval));
        if (suspend) {
            /* Suspend aging */
            val = 1;
        } else {
            /* Enable aging */
            val = 0;
        }
        soc_CB_ITM_ARL_AGING_CONFIGr_field_set(
                                unit, &regval, SUSPENDf, &val);
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_AGING_CONFIGr(unit, &regval));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_l2_mask_create
 * Purpose:
 *    Generate ARLFM mask depending on the flags passed
 * Parameters:
 *    arl_entry - ROBO ARL entry in which mask fields needs to be updated
 *    flags - Mask flags
 */
STATIC int
cbxi_l2_mask_create(soc_robo2_arl_scan_entry_t *arl_entry, uint32 flags)
{
    if (!(flags & CBXI_L2_MASK_ALL)) {
        return CBX_E_PARAM;
    }

    if (flags & CBXI_L2_MAC_MASK) {
        arl_entry->mac_hi_mask = ARL_MAC_HI_MASK;
        arl_entry->mac_lo_mask = ARL_MAC_LO_MASK;
    } else {
        arl_entry->mac_hi_mask = 0;
        arl_entry->mac_lo_mask = 0;
    }

    if (flags & CBXI_L2_VLAN_MASK) {
        arl_entry->fid_mask = ARL_FID_MASK;
    } else {
        arl_entry->fid_mask = 0;
    }

    if (flags & CBXI_L2_DEST_MASK) {
        arl_entry->dst_type_mask = ARL_DST_TYPE_MASK;
        arl_entry->dst_mask = ARL_DST_MASK;
    } else {
        arl_entry->dst_type_mask = 0;
        arl_entry->dst_mask = 0;
    }

    if (flags & CBXI_L2_UPD_CTRL_MASK) {
        arl_entry->upd_ctrl_mask = ARL_UPD_CTRL_MASK;
    } else {
        arl_entry->upd_ctrl_mask = 0;
    }

    if (flags & CBXI_L2_FWD_CTRL_MASK) {
        arl_entry->fwd_ctrl_mask = ARL_FWD_CTRL_MASK;
    } else {
        arl_entry->fwd_ctrl_mask = 0;
    }

    if (flags & CBXI_L2_VALID_MASK) {
        arl_entry->valid_mask = ARL_VALID_MASK;
    } else {
        arl_entry->valid_mask = 0;
    }

    if (flags & CBXI_L2_VALID_FIELD_MASK) {
        arl_entry->valid_mask = ARL_VALID_FIELD_MASK;
    }
    if (flags & CBXI_L2_HIT_MASK) {
        /* S/W cannot assure the if the hit bit is set or not
         * It is subjected to hardware aging behavior*/
        arl_entry->hit_mask = ARL_HIT_MASK;
    } else {
        arl_entry->hit_mask = 0;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_robo2_portid_resolve
 * Purpose:
 *    Obtain Global port group number for given portid
 *    portid  - (IN)  Portid parameter
 *    gpg     - (OUT) Global Port Group
 */
int
cbxi_robo2_portid_resolve(cbx_portid_t portid, cbxi_pgid_t *gpg)
{
    cbx_lag_info_t lag_info;
    lpg2pg_t lpg2pg_entry;
    cbx_port_t port;
    cbxi_pgid_t lpg;
    int rv, unit;

    if (CBX_PORTID_IS_VLAN_PORT(portid)) {
        return CBX_E_NONE;
    } else if (CBX_PORTID_IS_TRUNK(portid)) {
        rv = CBX_PORTID_TRUNK_GET(portid);
        if (rv < 0) {
           return CBX_E_PORT;
        }
        /* Obtain GPG value of given lag*/
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        if (lag_info.gpgid != CBXI_PGID_INVALID) {
            *gpg = lag_info.gpgid;
        } else {
            *gpg = lag_info.rgpgid;
        }
    } else {
        /* Obtain GPG corresponding to given physical port */
        rv = cbxi_robo2_port_validate(portid, &port, &lpg, &unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_STG,
                (BSL_META("Invalid portid\n")));
            return rv;
        }
        if (CBX_PORTID_IS_MCAST(portid)) {
            *gpg = CBXI_PGID_INVALID;
            return CBX_E_NONE;
        }
        /* Read entry from LPG2PG */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, lpg, &lpg2pg_entry));
        *gpg = lpg2pg_entry.pg;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_robo2_map_gpg2lpg
 * Purpose:
 *    Determine the LPG and the unit no. corresponding to given GPG
 *    gpg     - (IN) Global Port Group
 *    portid  - (OUT)  Local Port Group
 *    unit    - (OUT) Unit no.
 */
STATIC int
cbxi_robo2_map_gpg2lpg(cbxi_pgid_t gpg, cbxi_pgid_t *lpg, int *unit)
{
    lpg2pg_t lpg2pg_entry;
    pg2lpg_t pg2lpg_entry;
    int i, j, num_units;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /* Walking through lpg2pg table ensures that given GPG is valid and in use.
     * This may not be the case if we directly index pg2lpg as it may contain
     * stale value */
    /* Cross check with reverse mapping PG2LPG to make sure LPG and GPG are valid */
    for (i = 0; i < num_units; i++) {
        /* Read entry from PG2LPG from unit 'i' */
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(i, gpg, &pg2lpg_entry));
        for (j = 0; j < CBX_MAX_PORT_PER_UNIT; j++) {
            /* Read entry from LPG2PG */
            CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(i, j, &lpg2pg_entry));
            if ((lpg2pg_entry.pg == gpg) && (j == pg2lpg_entry.lpg)) {
                *lpg = j;
                *unit = i;
                return CBX_E_NONE;
            }
        }
    }

    return CBX_E_NOT_FOUND;
}

/*
 * Function:
 *    cbxi_robo2_portid_construct
 * Purpose:
 *    Obtain Global port group number for given portid
 *    gpg     - (IN) Global Port Group
 *    portid  - (OUT)  Portid parameter
 */
STATIC int
cbxi_robo2_portid_construct(cbxi_pgid_t gpg, cbx_portid_t *portid)
{
    lpg2ppfov_t ppfov_entry;
    cbx_portid_t lagid;
    cbxi_pgid_t lpg;
    pbmp_t ppfov = 0;
    int unit, port, rv;

    /* Obtain LPG and unit no. for given GPG */
    CBX_IF_ERROR_RETURN(cbxi_robo2_map_gpg2lpg(gpg, &lpg, &unit));

    /* Read entry from LPP2PGFOV */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit, lpg, &ppfov_entry));
    ppfov = ppfov_entry.ppfov;
    if (CBX_PBMP_IS_NULL(ppfov)) {
        return CBX_E_PARAM;
    }

    CBX_PBMP_ITER(ppfov, port) {
#ifdef CONFIG_CASCADED_MODE
        /* Skip cascade ports as none of the L2 entry will be learnt on casc
         * port*/
        if (CBX_SUCCESS(cbxi_cascade_port_check(port))) {
            continue;
        }
#endif
        rv = cbxi_robo2_lag_member_check((unit*16) + port, &lagid);
        if (CBX_SUCCESS(rv)) {
        /* Given DPG represents a LAG. Form portid from obtained lagid */
            CBX_PORTID_TRUNK_SET(*portid, lagid);
        } else {
        /* Given DPG represents a physical port. Return physical port number */
            if (unit == CBX_AVENGER_SECONDARY) {
                CBX_PORTID_LOCAL_SET(*portid, (port + CBX_MAX_PORT_PER_UNIT));
            } else {
                CBX_PORTID_LOCAL_SET(*portid, port);
            }
        }
        return CBX_E_NONE;
    }
    return CBX_E_NOT_FOUND;
}

/*
 * Function:
 *    cbxi_l2_l2addr_to_arl
 * Purpose:
 *    Translate hardware-independent L2 entry to hardware-specific ARL entry
 * Parameters:
 *    l2_entry  - (IN) Hardware-independent L2 entry
 *    arl_entry - (OUT) ROBO ARL entry
 */
STATIC int
cbxi_l2_l2addr_to_arl(cbx_l2_entry_t *l2_entry,
                                soc_robo2_arl_scan_entry_t *arl_entry)
{
    uint32_t mac_uint32[2];
    cbx_portid_t portid;
    cbxi_pgid_t dpg;
    int key;
#ifndef CONFIG_PORT_EXTENDER
    cbx_switch_vlan_mode_t mode;
    cbx_vlan_t vsi;
#endif

    sal_memset(arl_entry, 0, sizeof(soc_robo2_arl_scan_entry_t));

    if (l2_entry->params.flags & CBX_L2_ADDR_STATIC) {
        arl_entry->valid_key = cbxL2EntryStatic;
    } else {
        arl_entry->valid_key = cbxL2EntryValid;
        arl_entry->hit_key   = 1;
    }

    if (l2_entry->params.flags & CBX_L2_ADDR_ECID) {
        if (l2_entry->params.e_cid < 1) {
            return CBX_E_PARAM;
        }
        arl_entry->mac_lo_key = (l2_entry->params.e_cid & 0x3FFF);
        arl_entry->valid_key = cbxL2EntryStatic;
    } else {
        CBX_MAC_ADDR_TO_UINT32(l2_entry->src_mac, mac_uint32);
        arl_entry->mac_lo_key = mac_uint32[0];
        arl_entry->mac_hi_key = (uint16)mac_uint32[1];
    }

    portid = l2_entry->params.portid;
    /* Validate portid and Map given portid to DPG value */
    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_resolve(portid, &dpg));

    if (l2_entry->params.dest_type == cbxL2AddrDestDrop) {
        arl_entry->dst_type_key = cbxL2DstInvalid;
    } else if (l2_entry->params.dest_type == cbxL2AddrDestMcast) {
        arl_entry->dst_type_key = cbxL2DstMulticast;
        /*  extract DGT index (mcast id) from portid passed */
        if ((key = CBX_PORTID_MCAST_ID_GET(portid)) < 0) {
            return CBX_E_PARAM;
        }
        arl_entry->dst_key = key; /* Mcast id (DGT index) */
    } else if (l2_entry->params.dest_type == cbxL2AddrDestDLI) {
        arl_entry->dst_type_key = cbxL2DstLIN;
        /*  extract N value from portid passed */
        if ((key = CBX_PORTID_VLAN_PORT_ID_GET(portid)) < 0) {
            return CBX_E_PARAM;
        }
        arl_entry->dst_key = key; /* N field of N-type LI */
    } else if (l2_entry->params.dest_type == cbxL2AddrDestDPG) {
        arl_entry->dst_type_key = cbxL2DstDPG;
        /* Use mapped DPG value */
        arl_entry->dst_key = (uint16)dpg;
#ifdef CONFIG_PORT_EXTENDER
        /* All ARL entries to Physical port/LAG are added as LIN
         * Get the LIN value */
        if ((cbxi_pe_port_lin_get(portid, &key)) == CBX_E_NONE) {
            arl_entry->dst_key      = key;
            arl_entry->dst_type_key = cbxL2DstLIN;
        }
#endif
    }

    arl_entry->upd_ctrl_key = l2_entry->params.upd_ctrl;
    arl_entry->fwd_ctrl_key = l2_entry->params.fwd_ctrl;

#ifdef CONFIG_PORT_EXTENDER
    /* No need for VSI mapping */
    arl_entry->fid_key = (uint16)l2_entry->vlanid;
#else
    /* Add FID value based on vlan mode : IVL/ SVL  */
    CBX_IF_ERROR_RETURN(cbxi_vlan_learn_mode_get(&mode));
    if (mode == cbxSwitchVlanModeIndependent) {
        CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(l2_entry->vlanid, &vsi));
        arl_entry->fid_key = (uint16)vsi;
    } else {
        arl_entry->fid_key = 0;
    }
#endif

    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_l2_arl_entry_form
 * Purpose:
 *    Form the arl entry by reading appropriate ARLFM0 and GHT table entries
 * Parameters:
 *    index    - (IN) ARL table index
 *    arl_entry - (OUT) Hardware-dependent arl entry
 *
 * Notes:
 *    ARLFM0 is a separate table with 16k entries which can be indexed directly
 *    using given index value.
 *
 *    ARLFM1 is 64bit wide and it shares 128b wide GHT table with LIM.
 *    Also there are 2 GHT entries H0 and H1.
 *    H0 maps to lower half of total number of ARL entries, and each GHT entry
 *    contains 2 ARLFM1 entries.  (First 8K entries in case of 16K ARL)
 *    H1 maps to upper half of total number of ARL entries, and each GHT entry
 *    contains 2 ARLFM1 entries.  (Second 8K entries in case of 16K ARL)
 *
 *    If index is less than half the ARL size (ARL entries / 2),
 *              then use GHT_H0 at entry  (index/2).
 *    If index is greater than (ARL entries / 2),
 *              then use GHT_H1 at entry ((index - (ARL entries / 2)) / 2)
 *    index[0] gives whether to use higher 64 bits or lower 64 bits
 */
STATIC int
cbxi_l2_arl_entry_form(uint32 index, soc_robo2_arl_scan_entry_t *arl_entry)
{
    cbx_l2_info_t *cl2i = &cbx_l2_info;
    arlfm_t ent_arlfm;
    arlfm0_t ent_arlfm0;
    arlfm1_ght_h0_t ent_ght;
    arlfm1_ght_h1_t ent_ght_h1;
    uint32 index_ght;
    int unit = CBX_AVENGER_PRIMARY;

    /* Retrive appropriate GHT table entry */
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        /* Retrive ARLFM table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_arlfm_get(unit, index, &ent_arlfm));

        arl_entry->valid_key = ent_arlfm.valid;
        arl_entry->dst_key = ent_arlfm.dst;
        arl_entry->dst_type_key = ent_arlfm.dst_type;
        arl_entry->hit_key = ent_arlfm.hit;
        arl_entry->mac_lo_key = ent_arlfm.mac_lo;
        arl_entry->mac_hi_key = ent_arlfm.mac_hi;
        arl_entry->fid_key = ent_arlfm.fid;
        arl_entry->fwd_ctrl_key = ent_arlfm.fwd_ctrl;
        arl_entry->upd_ctrl_key = ent_arlfm.upd_ctrl;
    } else {
        /* Retrive ARLFM0 table entry */
        CBX_IF_ERROR_RETURN(soc_robo2_arlfm0_get(unit, index, &ent_arlfm0));

        arl_entry->valid_key = ent_arlfm0.valid;
        arl_entry->dst_key = ent_arlfm0.dst;
        arl_entry->dst_type_key = ent_arlfm0.dst_type;
        arl_entry->hit_key = ent_arlfm0.hit;

        if (index < (cl2i->num_entries / 2)) {
            /* Use GHT H0 */
            index_ght = index >> 1;
            CBX_IF_ERROR_RETURN(
                    soc_robo2_arlfm1_ght_h0_get(unit, index_ght, &ent_ght));
        } else {
            /* Use GHT H1 */
            index_ght = ((index - (cl2i->num_entries / 2)) >> 1);
            CBX_IF_ERROR_RETURN(
                    soc_robo2_arlfm1_ght_h1_get(unit, index_ght, &ent_ght_h1));
            /* H0 and H1 tables are identical */
            sal_memcpy(&ent_ght, &ent_ght_h1, sizeof(arlfm1_ght_h0_t));
        }

        if (index & 1) {
            /* Extract higher 64 bits of GHT entry : ent_ght */
            arl_entry->mac_lo_key = ent_ght.ght_key_95_64_arlfm1_mac_lo;
            arl_entry->mac_hi_key = ent_ght.ght_key_111_96_arlfm1_mac_hi;
            arl_entry->fid_key = ent_ght.ght_n_vsi_9_0_key_113_112_arlfm1_fid;
            arl_entry->fwd_ctrl_key = ent_ght.ght_n_vsi_11_10_arlfm1_fwd_ctrl_1_0;
            arl_entry->upd_ctrl_key = ent_ght.ght_valid_litype_arlfm1_upd_ctrl_1_0;

        } else {
            /* Extract lower 64 bits of GHT entry : ent_ght */
            arl_entry->mac_lo_key = ent_ght.ght_key_31_0_arlfm1_mac_lo;
            arl_entry->mac_hi_key = ent_ght.ght_key_47_32_arlfm1_mac_hi_15_0;
            arl_entry->fid_key = ent_ght.ght_key_59_48_arlfm1_fid_11_0;
            arl_entry->fwd_ctrl_key = ent_ght.ght_key_61_60_arlfm1_fwd_ctrl_1_0;
            arl_entry->upd_ctrl_key = ent_ght.ght_key_63_62_arlfm1_upd_ctrl_1_0;
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_l2_arl_entry_get
 * Purpose:
 *    Obtain ARL entry value at index passed, in hardware independent l2 entry
 *    format
 * Parameters:
 *    index    - (IN) ARL table index
 *    l2_entry - (OUT) Hardware-independent L2 entry
 */
STATIC int
cbxi_l2_arl_entry_get(uint32 index, cbx_l2_entry_t *l2_entry)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    cbx_switch_vlan_mode_t mode;
    cbx_vlan_t vid;
    uint32_t mac_uint32[2];

    sal_memset(l2_entry, 0, sizeof(cbx_l2_entry_t));

    /* Obtain harware-dependent arl entry at given index */
    CBX_IF_ERROR_RETURN(cbxi_l2_arl_entry_form(index, &arl_entry));

    mac_uint32[0] = arl_entry.mac_lo_key;
    mac_uint32[1] = arl_entry.mac_hi_key;

    CBX_MAC_ADDR_FROM_UINT32(l2_entry->src_mac, mac_uint32);

    if (arl_entry.valid_key == cbxL2EntryStatic) {
        l2_entry->params.flags = CBX_L2_ADDR_STATIC;
    }
    if (arl_entry.dst_type_key == cbxL2DstInvalid) {
        l2_entry->params.dest_type = cbxL2AddrDestDrop;
    } else if (arl_entry.dst_type_key == cbxL2DstMulticast) {
        l2_entry->params.dest_type = cbxL2AddrDestMcast;
        /*  Form portid (mcast id) from DGT index in dst_key*/
        CBX_PORTID_MCAST_ID_SET(l2_entry->params.portid, arl_entry.dst_key);
    } else if (arl_entry.dst_type_key == cbxL2DstLIN) {
        l2_entry->params.dest_type = cbxL2AddrDestDLI;
        /*  Form portid from N value in dst_key */
        CBX_PORTID_VLAN_PORT_ID_SET(l2_entry->params.portid, arl_entry.dst_key);
#ifdef CONFIG_PORT_EXTENDER
        /* All ARL entries to Physical port/LAG are added as LIN
         * Get back the port/LAG value if applicable */
        if ((cbxi_pe_lin_port_resolve(
                arl_entry.dst_key, &l2_entry->params.portid)) == CBX_E_NONE) {
            l2_entry->params.dest_type = cbxL2AddrDestDPG;
        }
#endif
    } else if (arl_entry.dst_type_key == cbxL2DstDPG) {
        l2_entry->params.dest_type = cbxL2AddrDestDPG;
        /* Obtain portid (lag/physical port) associated with DPG in dst_key */
        CBX_IF_ERROR_RETURN(cbxi_robo2_portid_construct(
                        arl_entry.dst_key, &l2_entry->params.portid));
    }

#ifdef CONFIG_PORT_EXTENDER
    /* In PE mode, ARL hardware contains ECID value in mac_lo_key field */
    l2_entry->params.e_cid = (mac_uint32[0] & 0x3FFF);
    l2_entry->params.flags |= CBX_L2_ADDR_ECID;
#endif
    l2_entry->params.upd_ctrl = arl_entry.upd_ctrl_key;
    l2_entry->params.fwd_ctrl = arl_entry.fwd_ctrl_key;

    /* Obtain VLAN id based on fid key */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_to_vid(arl_entry.fid_key, &vid));
    CBX_IF_ERROR_RETURN(cbxi_vlan_learn_mode_get(&mode));
    if (mode == cbxSwitchVlanModeIndependent) {
        l2_entry->vlanid = vid;
    } else {
        l2_entry->vlanid = CBX_VLAN_INVALID;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_robo2_rand_next
 * Purpose:
 *    Generate a random number
 * Parameters:
 *    val - (IN) Input unsigned int value
 * Return:
 *    Generated new value
 */
STATIC uint32
cbxi_robo2_rand_next(uint32 *val)
{
    *val = (*val >> 8) + 0x4321;
    return (*val);
}

/*
 * Function:
 *    cbxi_robo2_random_seed_gen
 * Purpose:
 *    Return a random seed
 * Parameters:
 *    seed_old - (IN) Present 32 bit value of hash seed
 * Return:
 *    Generated new seed value
 */
STATIC uint32
cbxi_robo2_random_seed_gen(uint32 seed_old)
{
    uint32 seed_new;

    seed_new =  ((cbxi_robo2_rand_next(&seed_old)%192)<<24)+
                ((cbxi_robo2_rand_next(&seed_old)%192)<<16)+
                ((cbxi_robo2_rand_next(&seed_old)%192)<<8)+
                (cbxi_robo2_rand_next(&seed_old)%192);

    return (seed_new);
}

/*
 * Function:
 *    cbxi_robo2_nearest_power2
 * Purpose:
 *    Find nearest value which is greater than or equal to given number which
 *    is a power of 2
 * Parameters:
 *    val - (IN) Value passed
 * Return:
 *    Nearest vlaue which is a power of 2
 */
STATIC uint32
cbxi_robo2_nearest_power2(uint32 val)
{
    val--;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    val++;

    return val;
}

/*
 * Function:
 *    cbxi_robo2_find_exponent
 * Purpose:
 *    Determine the exponent of base 2 for the value passed
 * Parameters:
 *    val - (IN) Value passed
 * Return:
 *    Exponent value
 * Note:
 *    This functions assumes that the value passed is already a power of 2.
 *    Hence determining the position of only bit set will give the exponint.
 *    If the given value is not a power of 2, go for log2 implementation.
 */
STATIC uint32
cbxi_robo2_find_exponent(uint32 val)
{
    uint32 exp = 0;

    /* Find the bit position using PBMP_ITER */
    CBX_PBMP_ITER(val, exp) {
        return exp;
    }
    return exp;
}

/*
 * Function:
 *    cbxi_robo2_reset_arl_data
 * Purpose:
 *    Configure the scan data and mask register to find an non existing entry
 *    (like all zeos or all 0xFFFFs). This help scanner fail to find next match entry
 *    and quickly back to ready status
  */
STATIC int cbxi_robo2_reset_arl_data(int unit)
{
    uint32 regval = 0;
    uint32 field = 0;

    field = 0x3;
    SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART2r_ROBO2, &regval,
                        VALIDf, &field);

    REG_WRITE_CB_ITM_ARLFM_SCAN_DATA_PART2r(unit, &regval);
    regval = 0;
    REG_WRITE_CB_ITM_ARLFM_SCAN_DATA_PART1r(unit, &regval);
    REG_WRITE_CB_ITM_ARLFM_SCAN_DATA_PART0r(unit, &regval);

    regval = 0xFFFFFFFF;
    REG_WRITE_CB_ITM_ARLFM_SCAN_MASK_PART2r(unit, &regval);
    REG_WRITE_CB_ITM_ARLFM_SCAN_MASK_PART1r(unit, &regval);
    REG_WRITE_CB_ITM_ARLFM_SCAN_MASK_PART0r(unit, &regval);

    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_robo2_arl_scanner_release
 * Purpose:
 *    Bring back the ARL scanner to ready state in case of scanner pause
 *    due to read command issued
 */
STATIC int
cbxi_robo2_arl_scanner_release(int unit)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    uint32 status = 0;
    uint32 index = 0;

    cbxi_robo2_reset_arl_data(unit);
    sal_memset(&arl_entry, 0, sizeof(soc_robo2_arl_scan_entry_t));

    /* Read arl_scan_status using arl scan function with 0 delay */
    CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(unit, 0, &status));

    if ((status & SCANNER_STATUS_READY) &&
                (status & SCANNER_STATUS_SEARCH_VALID)) {
        /* Scanner has found a valid entry at last index on last call
         * Clear scanner using count operation */
        CBX_IF_ERROR_RETURN(
            soc_robo2_arl_count_entry(unit, &arl_entry, &index));
        return CBX_E_NONE;
    } else if (status & SCANNER_STATUS_READY) {
        /* Scanner is already in ready state. No need to resume again */
        return CBX_E_NONE;
    }

    /* Resume scan until scanner reaches end of ARL and scanner moves to ready
     * state*/
    do {
        status = 0;
        CBX_IF_ERROR_RETURN(
            soc_robo2_arl_search_next_entry(unit, &arl_entry, &index));
        /* Read arl_scan_status using arl scan function with 0 delay */
        CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(unit, 0, &status));
    } while(!(status & SCANNER_STATUS_READY));

    if ((status & SCANNER_STATUS_READY) &&
                (status & SCANNER_STATUS_SEARCH_VALID)) {
        /* Scanner has found a valid entry at last index on last call
         * Clear scanner using count operation */
        CBX_IF_ERROR_RETURN(
            soc_robo2_arl_count_entry(unit, &arl_entry, &index));
    }

    return CBX_E_NONE;
}

#ifdef CONFIG_PORT_EXTENDER
/*
 * Function:
 *  cbxi_l2_extender_set
 * Purpose:
 *  Configure extender port.
 *
 */

STATIC int
cbxi_l2_extender_set(cbx_portid_t portid, cbx_l2_entry_t *l2entry)
{
    cbx_port_params_t port_params;

    if (CBX_PORTID_IS_MCAST(portid)) {
        CBX_IF_ERROR_RETURN(cbxi_pe_extender_port_set(
                portid, l2entry->params.e_cid, l2entry));
        return CBX_E_NONE;
    }

    CBX_IF_ERROR_RETURN(cbx_port_info_get(portid, &port_params));

    if (port_params.port_type == cbxPortTypeExtenderCascade) {
        CBX_IF_ERROR_RETURN(cbxi_pe_cascade_port_set(
            portid, l2entry->params.e_cid, l2entry));
    } else if (port_params.port_type == cbxPortTypeExtender) {
        if (CHECK_ECID_IS_MCAST(l2entry->params.e_cid)) {
            return CBX_E_NONE;
        } else {
            CBX_IF_ERROR_RETURN(cbxi_pe_extender_port_set(
                portid, l2entry->params.e_cid, l2entry));
        }
    } else {
        LOG_ERROR(BSL_LS_FSAL_STG,
             (BSL_META("FSAL API : Port 0x%0xis not a extender type\n"), portid));
        return CBX_E_PARAM;
    }

    return CBX_E_NONE;
}
#endif /* CONFIG_PORT_EXTENDER */

/*
 * Function:
 *  cbxi_l2_arl_counter_update
 * Purpose:
 *  Update ARL counters
 *  Index values 0 - 29 corresponds to GPG 0  -29
 *  Index 30 corresponds to all N type destinations
 *
 */

STATIC int
cbxi_l2_arl_counter_update(int unit, uint16_t dpg, int32_t count, uint32_t flags)
{
    uint32_t reg_val;
    int      age_time = 0;

    /* Disable aging during counter update */
    cbx_l2_age_timer_get(&age_time);
    cbx_l2_age_timer_set(0);

    REG_READ_CB_ITM_ARL_ENTRY_COUNT_Ar(unit, dpg, &reg_val);
    if (flags == CBXI_ARL_ENTRY_ADD) {
        /* Write negative value of count to increment ARL counter */
        reg_val = (uint32_t)(count * -1);
    } else if (flags & CBXI_ARL_ENTRY_REMOVE) {
        /* Write positive value of count to decrement ARL counter */
        if (reg_val > count) {
            reg_val = (uint32_t)count;
        } else {
            /* Clear the counter */
            reg_val = reg_val;
        }
    }
    REG_WRITE_CB_ITM_ARL_ENTRY_COUNT_Ar(unit, dpg, &reg_val);

    /* Enable aging */
    cbx_l2_age_timer_set(age_time);

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_l2_arl_table_init
 * Purpose:
 *  Initialization for L2 ARL table.
 *
 */

STATIC int
cbxi_l2_arl_table_init(void)
{
    uint8 num_units, i;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (i = 0; i < num_units; i++) {
        if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
            /* Enable ARLFM */
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm_enable(i));
            /* Reset ARLFM*/
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm_reset(i));
        } else {
            /* Enable ARLFM0 */
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm0_enable(i));
            /* Reset ARLFM0*/
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm0_reset(i));

            /* Enable ARLFM1_GHT_H0 */
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm1_ght_h0_enable(i));
            /* Reset ARLFM1_GHT_H0 */
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm1_ght_h0_reset(i));

            /* Enable ARLFM1_GHT_H1 */
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm1_ght_h1_enable(i));
            /* Reset ARLFM1_GHT_H1 */
            CBX_IF_ERROR_RETURN(soc_robo2_arlfm1_ght_h1_reset(i));
        }
    }
    return CBX_E_NONE;
}



/**
 * Initialize L2 module
 *
 * @param void N/A
 *
 * @return return code
 * @retval 0 success.
 */
int cbxi_l2_init(void)
{
    cbx_l2_info_t *cl2i = &cbx_l2_info;
    uint32 regval = 0;
    uint8 num_units, i;
    uint32 value = 0;
    uint32 temp, num_entries;


    LOG_INFO(BSL_LS_FSAL_STG,
            (BSL_META("FSAL API : cbxi_stg_init()..\n")));

    /* Init L2 ARLFM tables */
    CBX_IF_ERROR_RETURN(cbxi_l2_arl_table_init());

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /* Determine the nearest value which is greater than or
     * equal to the given number of entries and is a power of 2 */
    if (otp_flags & SOC_OTP_FLAG_ARL_SIZE_8K) {
        num_entries = cbxi_robo2_nearest_power2(CBXI_L2_ARL_NUM_OF_ENTRIES_8K);
    } else {
        num_entries = cbxi_robo2_nearest_power2(CBXI_L2_ARL_NUM_OF_ENTRIES_16K);
    }

    /* Determine the exponent 'value' for which 2 power 'value' gives
     * num_entries */
    value = cbxi_robo2_find_exponent(num_entries);

    /* Set appropriate bit in CB_ITM_arl_config reg */
    if (value < 2 || value > 14) {
        /* Allowed value is [2,14] */
        return CBX_E_PARAM;
    }
    temp = 0;
    for (i = 0; i < num_units; i++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(i, &regval));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_set(
                i, &regval, NUM_ENTRIESf, &value));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_set(
                i, &regval, DISABLE_LEARNINGf, &temp));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_CONFIGr(i, &regval));
    }
    cl2i->num_entries = num_entries;

    cl2i->init = 1;

    CBX_IF_ERROR_RETURN(cbx_l2_age_timer_set(CBXI_L2_ARL_AGE_TIME_DEFAULT));

    return CBX_E_NONE;
}

/**************************************************************************
 *                 L2 FSAL API IMPLEMENTATION                             *
 **************************************************************************/

/**
 * L2 Address add
 * This routine is used to add a L2 Address to the forwarding database.
 *
 * @param l2addr   (IN)  L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_entry_add(cbx_l2_entry_t *l2entry)
{
    soc_robo2_arl_scan_entry_t arl_entry;
#if 0
    cbx_mcast_member_info_t mcast_info;
#endif
    uint8 num_units, i;
    uint32 index, flags;
    soc_robo2_arl_scan_entry_t arl_old_even, arl_old_odd, arl_old;;
    uint8 arl_cnt_upd = 0;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
            (BSL_META("FSAL API cbx_l2_entry_add()..\n")));

    /* Check for valid vlanid */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_check(l2entry->vlanid));

#ifdef CONFIG_PORT_EXTENDER
    if (l2entry->params.flags & CBX_L2_ADDR_ECID) {
        if (l2entry->params.e_cid < 1) {
            return CBX_E_PARAM;
        }

        if (CBX_PORTID_IS_MCAST(l2entry->params.portid)) {
            if (!CHECK_ECID_IS_MCAST(l2entry->params.e_cid)) {
                return CBX_E_PARAM;
            }
        }

        CBX_IF_ERROR_RETURN(cbxi_l2_extender_set(
                        l2entry->params.portid, l2entry));
    }
#endif /* CONFIG_PORT_EXTENDER */

    /* CBXI_L2_MASK_ALL excludes HIT_MASK */
    flags = CBXI_L2_MASK_ALL | CBXI_L2_HIT_MASK;
    CBX_IF_ERROR_RETURN(cbxi_l2_l2addr_to_arl(l2entry, &arl_entry));
    CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /****************    Calculate Hash ***************/
    uint32_t val = 0;
    uint32_t idx_even, idx_odd, num_ent;
    arl_seed_t seed_n;
    uint32_t arl_key[2];
    int key_len = 59;
    int unit = 0;

    /* Form ARL key */
    sal_memset(&arl_key, 0, (2 * sizeof(uint32_t)));
    arl_key[1] = (arl_entry.fid_key << 16) | (arl_entry.mac_hi_key & 0xFFFF);
    arl_key[0] = (arl_entry.mac_lo_key);

    /* No. of entries */
    CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(unit, &val));
    CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_get(
                                unit, &val, NUM_ENTRIESf, &num_ent));

    unit = 0;
    CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_HASH0_PART0r(unit, &seed_n.seed_31_0));
    CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_HASH0_PART1r(unit, &seed_n.seed_59_32));


    /* If ARL has size (2^n) then the low order n-2 bits returned from the hash
     * function called with the 'even' seed are multiplied by 2 * 2 = 4
     * (each index has 2 ARL entries : 2 * 2 = 4)and the
     * result is used to extract the even entry while the low order n-2 bits
     * returned from the hash function called with the 'odd' seed are
     * multiplied by 2 and summed with 1 and again multiplied by 2 and
     * the result is used to extract the odd entry */

    /* ARL has 2 entries per Hardware table  index */
    cbxi_arl_hash_key(&arl_key[0], &seed_n, key_len, &val);
    idx_even = ((val & ((1 << (num_ent - 2)) -1)) * 4);

    val = 0;
    unit = 0;
    CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_HASH1_PART0r(unit, &seed_n.seed_31_0));
    CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_HASH1_PART1r(unit, &seed_n.seed_59_32));

    /* ARL has 2 entries per index */
    cbxi_arl_hash_key(&arl_key[0], &seed_n, key_len, &val);
    idx_odd = ((((val & ((1 << (num_ent - 2)) - 1)) * 2) + 1) *2);

#if 1
    /* Disable aging & learning while add operation */
    CBX_IF_ERROR_RETURN(cbxi_arl_aging_suspend(TRUE));
    CBX_IF_ERROR_RETURN(cbxi_arl_learn_suspend(TRUE));
#endif

    /* Retrive entries on HASH0 and HASH1 indices */
    CBX_IF_ERROR_RETURN(cbxi_l2_arl_entry_form(idx_even, &arl_old_even));
    CBX_IF_ERROR_RETURN(cbxi_l2_arl_entry_form(idx_odd, &arl_old_odd));


    /**************************************************/

    for (i = 0; i < num_units; i++) {
        /* Make sure scanner is ready */
        CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(i));

        CBX_IF_ERROR_RETURN(soc_robo2_arl_insert_entry(i, &arl_entry, &index));

        /* Check if needs to update ARL counter for previously present entries */
        if (index == idx_even) {
            if (arl_old_even.valid_key & 0x2) {
                /* Valid entry was present before */
                sal_memcpy(&arl_old, &arl_old_even, sizeof(soc_robo2_arl_scan_entry_t));
                arl_cnt_upd = 1;
            }
        } else if (index == idx_odd) {
            if (arl_old_odd.valid_key & 0x2) {
                /* Valid entry was present before */
                sal_memcpy(&arl_old, &arl_old_odd, sizeof(soc_robo2_arl_scan_entry_t));
                arl_cnt_upd = 1;
            }
        }

        if (arl_cnt_upd == 1) {
            /* Decrement removed entries if applicable */
            if (arl_old.dst_type_key == cbxL2DstDPG) {
                CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                            i, arl_old.dst_key, 1, CBXI_ARL_ENTRY_REMOVE));
            } else if (arl_old.dst_type_key == cbxL2DstLIN) {
                CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                                            i, 30, 1, CBXI_ARL_ENTRY_REMOVE));
            }

        }


        /* Update ARL counters */
        if (arl_entry.dst_type_key == cbxL2DstDPG) {
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                        i, arl_entry.dst_key, 1, CBXI_ARL_ENTRY_ADD));
        } else if (arl_entry.dst_type_key == cbxL2DstLIN) {
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                                        i, 30, 1, CBXI_ARL_ENTRY_ADD));
        }
    }

#ifndef CONFIG_PORT_EXTENDER /* Keep learning disabled in PE mode */
    /* Enable aging & learning  */
    CBX_IF_ERROR_RETURN(cbxi_arl_aging_suspend(FALSE));
    CBX_IF_ERROR_RETURN(cbxi_arl_learn_suspend(FALSE));
#endif

    return CBX_E_NONE;
}

/**
 * L2 Address remove
 * This routine is used to remove a L2 Address to the forwarding database.
 *
 * @param l2addr   (IN)  L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_addr_remove(cbx_l2_entry_t *l2entry)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    uint32 count = 0;
    uint32 cnt_unit = 0;
    uint8 num_units, i;
    uint32 flags;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
            (BSL_META("FSAL API cbx_l2_entry_remove()..\n")));

    /* Check for valid vlanid */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_check(l2entry->vlanid));

    /* Match valid entries on MAC + VLAN + Port */
    flags = (CBXI_L2_MAC_MASK | CBXI_L2_VLAN_MASK |
                        CBXI_L2_DEST_MASK | CBXI_L2_VALID_MASK);
    if (l2entry->params.flags & CBX_L2_ADDR_ECID) {
        if (l2entry->params.e_cid < 1) {
            return CBX_E_PARAM;
        }
#if 0 /* PE loopback solution implements unique LIN dstination for each port
         No need to release LIN slots as only limited LINs are allocated*/
        CBX_IF_ERROR_RETURN(cbxi_pe_release_lin_slot(l2entry));
#endif

        flags = CBXI_L2_MAC_MASK | CBXI_L2_VALID_MASK; /*Check for only E-CID */

#ifdef CONFIG_PORT_EXTENDER
        if (!CHECK_ECID_IS_MCAST(l2entry->params.e_cid)) {
            CBX_IF_ERROR_RETURN(cbxi_pe_cfp_rule_remove(l2entry->params.e_cid));
        }
#endif
    }

    CBX_IF_ERROR_RETURN(cbxi_l2_l2addr_to_arl(l2entry, &arl_entry));
    CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (i = 0; i < num_units; i++) {
        cnt_unit = 0;
        /* Make sure scanner is ready */
        CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(i));

        CBX_IF_ERROR_RETURN(soc_robo2_arl_delete_entry(
                                        i, &arl_entry, &cnt_unit));
        /* Update ARL counters */
        if (arl_entry.dst_type_key == cbxL2DstDPG) {
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                        i, arl_entry.dst_key, cnt_unit, CBXI_ARL_ENTRY_REMOVE));
        } else if (arl_entry.dst_type_key == cbxL2DstLIN) {
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                                    i, 30, cnt_unit, CBXI_ARL_ENTRY_REMOVE));
        }

        count += cnt_unit;
    }
    LOG_INFO(BSL_LS_FSAL_L2,
        (BSL_META("FSAL API cbx_l2_entry_remove: Number of entries deleted:"
        " %d\n"),count));

    if (!count) {
        /* Valid entry doesn't exist */
        return CBX_E_NOT_FOUND;
    }

    return CBX_E_NONE;
}

/**
 * L2 Address lookup
 * This routine is used to find a L2 Address entry that matches MAC and VLAN and
 * update the l2 entry structure passed.
 *
 * @param l2addr   (IN/OUT)  L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE      Success
 * @retval CBX_E_NOT_FOUND Not Found
 * @retval CBX_E_XXXX      Failure
 */

int cbx_l2_lookup(cbx_l2_entry_t *l2entry)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    uint32 index = 0;
    uint32 status, flags;
    uint32 e_cid = 0;
    int unit = CBX_AVENGER_PRIMARY;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
            (BSL_META("FSAL API cbx_l2_lookup()..\n")));

    if (l2entry == NULL) {
        return CBX_E_PARAM;
    }

    /* Check for valid vlanid */
    CBX_IF_ERROR_RETURN(cbxi_vlan_vid_check(l2entry->vlanid));

    /* Make sure scanner is ready */
    CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(unit));

    flags = CBXI_L2_MAC_MASK | CBXI_L2_VLAN_MASK | CBXI_L2_VALID_MASK;
    if (l2entry->params.flags & CBX_L2_ADDR_ECID) {
        if (l2entry->params.e_cid < 1) {
            return CBX_E_PARAM;
        }
        e_cid = l2entry->params.e_cid;
        flags = CBXI_L2_MAC_MASK | CBXI_L2_VALID_MASK; /*Check for only E-CID */
    }

    /* Lookup only valid and static entries */
    CBX_IF_ERROR_RETURN(cbxi_l2_l2addr_to_arl(l2entry, &arl_entry));
    CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

    /* Lookup only VALID (2) and STATIC (3) entries
     * Match only bit 1 for this purpose*/
    arl_entry.valid_key = cbxL2EntryValid;

    CBX_IF_ERROR_RETURN(
        soc_robo2_arl_search_first_entry(unit, &arl_entry, &index));

    /* Read arl_scan_status using arl scan function with 0 delay */
    CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(unit, 0, &status));

    if (!(status & SCANNER_STATUS_SEARCH_VALID)) {
        return CBX_E_NOT_FOUND;
    }

    LOG_INFO(BSL_LS_FSAL_L2,
            ("cbx_l2_lookup: Entry found at index = %d\n", index));

    /* Update l2addr parameters from ARL entry at index */
    CBX_IF_ERROR_RETURN(cbxi_l2_arl_entry_get(index, l2entry));
    l2entry->params.e_cid = e_cid;

    /* Make sure scanner is ready */
    CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(unit));

    return CBX_E_NONE;
}


/**
 * L2 Address lookup next
 * This routine is used to find the next L2 Address entry in the database.
 * This provides the ability to walk the database.
 * If the first parameter is NULL,
 * then the first entry in the database is returned.
 *
 * @param l2addr   (IN)  L2 Address structure
 * @param l2addr   (OUT) L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE      Success
 * @retval CBX_E_PARAM     If first paramenter is not NULL, but is not in ARL
 * @retval CBX_E_NOT_FOUND Not Found
 * @retval CBX_E_XXXX      Failure
 */

int cbx_l2_lookup_next(cbx_l2_entry_t *l2entry, cbx_l2_entry_t *l2entry_next)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    uint32 index, status;
    uint32 flags = 0;
    int unit = CBX_AVENGER_PRIMARY;
    uint8 resume = 0;
    eth_mac_t null_mac = {0 , 0, 0, 0, 0, 0};

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
            (BSL_META("FSAL API cbx_l2_lookup_next()..\n")));

    if (l2entry_next == NULL) {
        return CBX_E_PARAM;
    }

    sal_memset(&arl_entry, 0, sizeof(soc_robo2_arl_scan_entry_t));

    /* Read arl_scan_status using arl scan function with 0 delay */
    CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(unit, 0, &status));

    if ((status & SCANNER_STATUS_READY) &&
                (status & SCANNER_STATUS_SEARCH_VALID)) {
        /* Scanner has found a valid entry at last index on last call
         * Clear scanner and return CBX_E_NOT_FOUND error */
        CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(unit));
        return CBX_E_NOT_FOUND;
    } else if (status & SCANNER_STATUS_READY) {
        /* Scanner in ready state. Check for the first entry */
        if (l2entry != NULL) {
            if (l2entry->vlanid > 0) {
                /* Check for valid vlanid */
                CBX_IF_ERROR_RETURN(cbxi_vlan_vid_check(l2entry->vlanid));
                flags |= CBXI_L2_VLAN_MASK;
            }
            if (!CBX_MAC_ADDR_EQUAL(l2entry->src_mac, null_mac)) {
                /* Use SRC MAC value if it is non zero */
                flags |= CBXI_L2_MAC_MASK;
            }
            if ((l2entry->params.portid != 0) ||
                                (l2entry->params.dest_type != 0)) {
                 /* Use SRC port value if it is non zero */
                flags |= CBXI_L2_DEST_MASK;
            }
            CBX_IF_ERROR_RETURN(cbxi_l2_l2addr_to_arl(l2entry, &arl_entry));
            CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

            if (flags & CBXI_L2_MAC_MASK) {
                /* Search 1st entry only if there is valid non zero MAC */
                resume = 1; /* search for next matching entry */
                CBX_IF_ERROR_RETURN(
                    soc_robo2_arl_search_first_entry(unit, &arl_entry, &index));

                /* Read arl_scan_status using arl scan function with 0 delay */
                CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(
                                                        unit, 0, &status));

                if (!(status & SCANNER_STATUS_SEARCH_VALID)) {
                    /* First parameter is not NULL, but is not present in ARL */
                    LOG_INFO(BSL_LS_FSAL_L2,("cbx_l2_lookup_next: \
                        Unable to search the entry passed as first parameter\n"));
                    return CBX_E_PARAM;
                }
            }
        }

        /* Search next valid entry in ARL to fill l2entry_next */
        flags |= CBXI_L2_VALID_MASK;
        flags &= ~CBXI_L2_MAC_MASK;

        /* FIXME : Retain the VLAN , port mask once the mechanism of passing the
         * information on wheteher to do walkthrough on VLAN and/or port is
         * finalized */
        flags &= ~CBXI_L2_VLAN_MASK;
        flags &= ~CBXI_L2_DEST_MASK;

        CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

        /* Lookup both VALID (2) and STATIC (3) entries
         * Match only bit 1 for this purpose*/
        arl_entry.valid_key = cbxL2EntryValid;

        /* Update scanner data registers to match only on valid bits and
         * VLAN (fid) bits if set previously*/
        CBX_IF_ERROR_RETURN(soc_robo2_arl_update_scan_data_reg(
                                                        unit, &arl_entry));
    } else if (status & SCANNER_STATUS_SEARCH_VALID) {
        /* Scanner in already in search valid state. Scanner is paused and
         * issuing only resume command is sufficient */
        /* First entry is ignored */
        resume = 1;
    } else {
        return CBX_E_INIT;
    }

    if (resume) {
        /* Obtain the index of next entry in the ARL */
        CBX_IF_ERROR_RETURN(
            soc_robo2_arl_search_next_entry(unit, &arl_entry, &index));
        /* Read arl_scan_status using arl scan function with 0 delay */
        CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(unit, 0, &status));

        if (!(status & SCANNER_STATUS_SEARCH_VALID)) {
            LOG_INFO(BSL_LS_FSAL_L2,
            ("cbx_l2_lookup_next:No entry found. Lookup reached end of ARL\n"));

            return CBX_E_NOT_FOUND;
        }
    } else {
        /* Obtaing the index of first valid entry */
        CBX_IF_ERROR_RETURN(
                soc_robo2_arl_search_first_entry(unit, &arl_entry, &index));

        /* Read arl_scan_status using arl scan function with 0 delay */
        CBX_IF_ERROR_RETURN(soc_robo2_arl_scanner_wait_for(unit, 0, &status));

        if (!(status & SCANNER_STATUS_SEARCH_VALID)) {
        /* First parameter is NULL and no other ARL entry is present */
            LOG_INFO(BSL_LS_FSAL_L2,
            ("cbx_l2_lookup_next: ARL is empty. No valid entry found\n"));
            return CBX_E_EMPTY;
        }
    }

    /* Fill in l2addr_next value from ARL entry at index */
    CBX_IF_ERROR_RETURN(cbxi_l2_arl_entry_get(index, l2entry_next));

    LOG_INFO(BSL_LS_FSAL_L2,
            ("cbx_l2_lookup_next: Next entry found at index = %d\n", index));


    return CBX_E_NONE;
}

/**
 * ARL aging timer set
 * ARL aging is automatic.
 *  The hardware will automatically delete non-static entries that have
 *  been aged out. The aging process of the ARL is controlled by the
 *  ARL Aging Process Configuration register.
 *
 * - value of zero disables aging
 *
 * @param age_seconds  (IN)  age time in seconds
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_age_timer_set(int age_seconds)
{
    int unit = CBX_AVENGER_PRIMARY;
    uint8 num_units, i;
    uint32 regval = 0;
    uint32 accel, period;
    uint64 age_value_us;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
            (BSL_META("FSAL API cbx_l2_age_timer_set()..\n")));

    if (!age_seconds) {
        /* age_seconds of zero indicates disabling aging */
        period = 0;
        LOG_INFO(BSL_LS_FSAL_L2,
                (BSL_META("Disabling aging\n")));
    } else if ((age_seconds < 1) || (age_seconds > 1000000)) {
        return CBX_E_PARAM;
    } else {
        age_value_us = ((uint64)age_seconds * CBXI_SEC_TO_MICROSEC) /
                                                cbx_l2_info.num_entries;

        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
            /* Normal aging not working in A0.
             * Enable fast aging (329.67 times faster)*/
            age_value_us = (age_value_us * 32967) / 100;
        }

        if (age_value_us >= (1 << CBXI_AGING_PERIOD_BITS)) {
            /* Period field is 26 bit wide. For higher values of age time and
             * lower arl entry count, the value may overflow beyond 26 bits */

            LOG_ERROR(BSL_LS_FSAL_L2,(BSL_META(
                "Period value overflowed for given age time : %d seconds, "
                "and for no. of arl entries : %d\n"),
                age_seconds, cbx_l2_info.num_entries));
            return CBX_E_CONFIG;
        }
        period = age_value_us;
    }

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    for (i = 0; i < num_units; i++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_AGING_CONFIGr(i, &regval));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_AGING_CONFIGr_field_set(
                                                i, &regval, PERIODf, &period));

        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
            accel = 0;
        } else {
            /* Normal aging not working in A0.
             * Enable fast aging (329.67 times faster)*/
            accel = 1;
        }

#ifdef EMULATION_BUILD /* Aging is too slow on emulation. Accelerate aging */
        accel = 1;
#endif
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_AGING_CONFIGr_field_set(
                                        i, &regval, ACCELERATEf, &accel));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_AGING_CONFIGr(i, &regval));
    }
    return CBX_E_NONE;
}


/**
 * ARL aging timer get
 *
 * @param age_seconds  (OUT) age time in seconds
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_age_timer_get(int *age_seconds)
{
    int unit = CBX_AVENGER_PRIMARY;
    uint32 regval = 0;
    uint32 period;
    uint64 age_time;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
            (BSL_META("FSAL API cbx_l2_age_timer_get()..\n")));

    CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_AGING_CONFIGr(unit, &regval));
    CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_AGING_CONFIGr_field_get(
                                        unit, &regval, PERIODf, &period));
    if (!period) {
        /* Aging is disabled */
        *age_seconds = 0;
        return CBX_E_NONE;
    }

    age_time = ((uint64)(cbx_l2_info.num_entries) * period) /
                        CBXI_SEC_TO_MICROSEC;

    age_time &= ((1 << CBXI_AGING_PERIOD_BITS) - 1);


    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
        /* Normal aging not working in A0.
         * Fast aging is enabled (329.67 times faster)*/
        age_time = (age_time * 100) / 32967;
    }

    *age_seconds = (int) age_time;
    /* Add 1 secinds: Truncation error */
    *age_seconds += 1;

    return CBX_E_NONE;
}

/**
 * Reseed of ARL hash
 * Application control to reseed the ARL hash
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_l2_reseed (void)
{
    uint32 seed, value;
    uint32 hash_reg = 0;
    uint32 hash_field = 0;
    uint32 regval = 0;
    uint8 num_units, hash_part;
    int unit = CBX_AVENGER_PRIMARY;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
        (BSL_META("FSAL API cbx_l2_reseed()..\n")));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    value = 1;
    /* Disable hardware learning */
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(unit, &regval));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_set(
                unit, &regval, DISABLE_LEARNINGf, &value));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_CONFIGr(unit, &regval));
    }

    /* Flush all ARL entries */
    CBX_IF_ERROR_RETURN(cbx_l2_flush(0));

    for (hash_part = 0; hash_part < 4 ; hash_part++) {
        switch (hash_part) {
        case 0:
            hash_reg = CB_ITM_HASH0_PART0r;
            hash_field = SEED_LSWf;
            break;
        case 1:
            hash_reg = CB_ITM_HASH0_PART1r;
            hash_field = SEED_MSWf;
            break;
        case 2:
            hash_reg = CB_ITM_HASH1_PART0r;
            hash_field = SEED_LSWf;
            break;
        case 3:
            hash_reg = CB_ITM_HASH1_PART1r;
            hash_field = SEED_MSWf;
            break;
        }

        for (unit = 0; unit < num_units; unit++) {
            CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(
                                unit, hash_reg, REG_PORT_ANY, 0, &regval));
            CBX_IF_ERROR_RETURN(DRV_REG_FIELD_GET(
                                unit, hash_reg, &regval, hash_field, &seed));
            seed = cbxi_robo2_random_seed_gen(seed);
            CBX_IF_ERROR_RETURN(DRV_REG_FIELD_SET(
                                unit, hash_reg, &regval, hash_field, &seed));
            CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(
                                unit, hash_reg, REG_PORT_ANY, 0, &regval));
        }
    }
    /* Enable Hardware learning */
    value = 0;
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(unit, &regval));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_set(
                unit, &regval, DISABLE_LEARNINGf, &value));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_CONFIGr(unit, &regval));
    }
    return CBX_E_NONE;
}

/**
 * Flush ARL entries
 * - For Independent VLAN Learing, flush all entires on the VLAN
 * - For Single VLAN Learining, all ARL entries are flush
 *
 * @param vlanid       (IN)  Vlan Identifier
 *                           when vlanid=0, then flush all ARL entries
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_flush(cbx_vlanid_t vlanid)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    cbx_switch_vlan_mode_t mode;
    cbx_vlan_t vsi;
    uint32 count = 0;
    uint8 num_units, i;
    uint32 flags;
    uint16_t dpg = 0;
    uint32_t cnt = 0;

    CHECK_L2_INIT(cbx_l2_info);

    LOG_INFO(BSL_LS_FSAL_L2,
        (BSL_META("FSAL API cbx_l2_flush()..\n")));

    sal_memset(&arl_entry, 0, sizeof(soc_robo2_arl_scan_entry_t));

    if (vlanid) {
        /* Check for valid vlanid */
        CBX_IF_ERROR_RETURN(cbxi_vlan_vid_check(vlanid));
    }
    /* vlan mode : IVL/ SVL  */
    CBX_IF_ERROR_RETURN(cbxi_vlan_learn_mode_get(&mode));
    if ((mode == cbxSwitchVlanModeShared) || (vlanid == 0)) {
        /* Remove all valid ARL entries */
        flags = CBXI_L2_VALID_MASK;
        arl_entry.valid_key = cbxL2EntryValid;
    } else {
        /* Remove ARL entries associated with vlanid passed */
        flags = CBXI_L2_VALID_MASK | CBXI_L2_VLAN_MASK;
        arl_entry.valid_key = cbxL2EntryValid;
        CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));
        arl_entry.fid_key = (uint16)vsi;
    }

    CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }

    /* Issue delete command so that the count of deleted entries per port can be
     * updated in ARL counters */

    /* GPG 0 - 29 */
    arl_entry.dst_type_key  = cbxL2DstDPG;
    arl_entry.dst_type_mask = ARL_DST_TYPE_MASK;
    for (dpg = 0; dpg < 30; dpg++) {
        for (i = 0; i < num_units; i++) {
            cnt = 0;
            arl_entry.dst_key  = dpg;
            arl_entry.dst_mask = ARL_DST_MASK;
            /* Make sure scanner is ready */
            CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(i));

            /* Single delete command to H/W deletes all the matching entries */
            CBX_IF_ERROR_RETURN(soc_robo2_arl_delete_entry(
                                                i, &arl_entry, &cnt));

            /* Update ARL counters */
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                        i, arl_entry.dst_key, cnt, CBXI_ARL_ENTRY_REMOVE));
            count += cnt;
        }
    }

    /* ALL Dest type = LIN */
    arl_entry.dst_type_key  = cbxL2DstLIN;
    arl_entry.dst_type_mask = ARL_DST_TYPE_MASK;
    arl_entry.dst_key  = 0;
    arl_entry.dst_mask = 0;
    for (i = 0; i < num_units; i++) {
        cnt = 0;
        /* Make sure scanner is ready */
        CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(i));

        /* Single delete command to H/W deletes all the matching entries */
        CBX_IF_ERROR_RETURN(soc_robo2_arl_delete_entry(i, &arl_entry, &cnt));

        /* Update ARL counters */
        CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                        i, 30, cnt, CBXI_ARL_ENTRY_REMOVE));
        count += cnt;
    }

    /* All remaining entries */
    /* Remove both cbxL2AddrDestDrop (0b00) and cbxL2AddrDestMcast (0b01):
     * Configure MASK to match only BIT 1 for zero */
    arl_entry.dst_type_key  = cbxL2AddrDestDrop; /* BIT 1 = 0b0 */
    arl_entry.dst_type_mask = 0x2; /* Match only BIT 1 */
    for (i = 0; i < num_units; i++) {
        cnt = 0;
        /* Make sure scanner is ready */
        CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(i));

        /* Single delete command to H/W deletes all the matching entries */
        CBX_IF_ERROR_RETURN(soc_robo2_arl_delete_entry(i, &arl_entry, &cnt));

        count += cnt;
    }

    LOG_INFO(BSL_LS_FSAL_L2,
        (BSL_META("FSAL API cbx_l2_flush: Number of entries deleted:"
        " %d\n"),count));

    return CBX_E_NONE;
}

/**
 * Flush ARL entries on a given portid
 * - Flush all entries learned on the given portid.
 * - To flush all the entries learned on all the ports,
 *   use cbx_l2_flush API with vlanid = 0
 *
 * @param portid       (IN)  portid on which ARL entries to be deleted
 *                           This may represent a LAG or a physical port.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_port_flush ( cbx_portid_t portid)
{
    soc_robo2_arl_scan_entry_t arl_entry;
    cbxi_pgid_t dpg;
    uint32 count = 0;
    uint8 num_units, i;
    uint32 flags;
    int key;

    CHECK_L2_INIT(cbx_l2_info);

    sal_memset(&arl_entry, 0, sizeof(soc_robo2_arl_scan_entry_t));

    flags = CBXI_L2_VALID_MASK | CBXI_L2_DEST_MASK;
    arl_entry.valid_key = cbxL2EntryValid;

    /* Validate portid and Map given portid to DPG value */
    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_resolve(portid, &dpg));

    if (CBX_PORTID_IS_MCAST(portid)) {
        arl_entry.dst_type_key = cbxL2AddrDestMcast;
        /*  extract DGT index (mcast id) from portid passed */
        if ((key = CBX_PORTID_MCAST_ID_GET(portid)) < 0) {
            return CBX_E_PARAM;
        }
        arl_entry.dst_key = key; /* Mcast id (DGT index) */
    } else if (CBX_PORTID_IS_VLAN_PORT(portid)) {
         arl_entry.dst_type_key = cbxL2DstLIN;
        /*  extract N value from portid passed */
        if ((key = CBX_PORTID_VLAN_PORT_ID_GET(portid)) < 0) {
            return CBX_E_PARAM;
        }
        arl_entry.dst_key = key; /* N field of N-type LI */
    } else {
        arl_entry.dst_type_key = cbxL2DstDPG;
        arl_entry.dst_key = (uint16)dpg;
#ifdef CONFIG_PORT_EXTENDER
        /* All ARL entries to Physical port/LAG are added as LIN
         * Get the LIN value */
        if ((cbxi_pe_port_lin_get(portid, &key)) == CBX_E_NONE) {
            arl_entry.dst_key      = key;
            arl_entry.dst_type_key = cbxL2DstLIN;
        }
#endif
    }

    CBX_IF_ERROR_RETURN(cbxi_l2_mask_create(&arl_entry, flags));

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        num_units = 1;
    }
    for (i = 0; i < num_units; i++) {
        /* Make sure scanner is ready */
        CBX_IF_ERROR_RETURN(cbxi_robo2_arl_scanner_release(i));

        /* Single delete command to H/W deletes all the matching entries */
        CBX_IF_ERROR_RETURN(soc_robo2_arl_delete_entry(i, &arl_entry, &count));

        /* Update ARL counters */
        if (arl_entry.dst_type_key == cbxL2DstDPG) {
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                        i, arl_entry.dst_key, count, CBXI_ARL_ENTRY_REMOVE));
        } else if (arl_entry.dst_type_key == cbxL2DstLIN) {
            CBX_IF_ERROR_RETURN(cbxi_l2_arl_counter_update(
                                    i, 30, count, CBXI_ARL_ENTRY_REMOVE));
        }
    }
    LOG_INFO(BSL_LS_FSAL_L2,
        (BSL_META("FSAL API cbx_l2_port_flush: Number of entries deleted:"
        " %d\n"),count));

    return CBX_E_NONE;
}

/**
 * Resets the ARL count values for all the ports.
 *
 * @param  Unit number.
 *
 * @return None
 * @retval 0 success.
 */
void cbxi_arl_reset_entrycount(int unit)
{
    uint32_t idx, reg_val;
    int      age_time = 0;

    /* Disable aging during counter update */
    cbx_l2_age_timer_get(&age_time);
    cbx_l2_age_timer_set(0);

    for(idx = 0; idx < 31; idx++)
    {
        REG_READ_CB_ITM_ARL_ENTRY_COUNT_Ar(unit, idx, &reg_val);
        if(reg_val) {
            /* Write the same value of count to decrement ARL counter */
            REG_WRITE_CB_ITM_ARL_ENTRY_COUNT_Ar(unit, idx, &reg_val);
        }
    }
    /* Enable aging */
    cbx_l2_age_timer_set(age_time);
}


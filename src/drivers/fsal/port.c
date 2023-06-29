/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     port.c
 * * Purpose:
 * *     Robo2 port module. This tracks and manages physical ports.
 * *     MAC/PHY interfaces are managed through respective drivers.
 * *
 * */
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/robo2/common/tables.h>
#include <fsal/error.h>
#include <fsal/port.h>
#include <fsal/lag.h>
#include <fsal/switch.h>
#ifdef CONFIG_PTP
#include <fsal/ptp.h>
#endif
#include <fsal_int/port.h>
#include <fsal_int/types.h>
#include <fsal_int/lag.h>
#include <fsal_int/l2.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif/* CONFIG_PORT_EXTENDER */
#include <fsal_int/native_types.h>
#include <fsal_int/encap.h>
#include "fsal_int/vlan.h"
#include "fsal_int/slic.h"
#include "fsal_int/mcast.h"
#include "fsal_int/vlan.h"
#include <fsal_int/trap.h>
#include <avng_phy.h>

#define CBXI_PORT_TABLE_ENTRY_ADD    0
#define CBXI_PORT_TABLE_ENTRY_REMOVE 1

extern cbx_l2_info_t cbx_l2_info;

cbxi_port_info_t port_info[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT + 1];
pbmp_t pp_created_pbmp[2];

#ifdef CONFIG_CASCADED_MODE
cbx_portid_t cascade_ports[CBX_MAX_UNITS];
#endif

#ifdef CONFIG_CASCADED_MODE
int
cbxi_cascade_port_check(cbx_portid_t portid)
{
    int unit = 0;
    cbx_port_t port;
    cbxi_pgid_t csd_lpg, port_lpg;

    if (CBX_PORTID_IS_TRUNK(portid) || CBX_PORTID_IS_LOCAL(portid) ||
                        (!CBX_PORTID_IS_SET(portid))) {
        /* Compare only if portid is LAG or physical port */
        if (SOC_IS_CASCADED(0)) {
            if (cascade_ports[unit] == CBXI_PGID_INVALID) {
                return CBX_E_INIT;
            }
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                    portid, &port, &port_lpg, &unit));
            /* Check if portid matches */
            if (portid == cascade_ports[unit]) {
                return CBX_E_NONE;
            }
            /* Else check if Local PGs are same */
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                            cascade_ports[unit], &port, &csd_lpg, &unit));
            if (csd_lpg == port_lpg) {
                return CBX_E_NONE;
            } else {
                return CBX_E_PORT;
            }
        } else {
            return CBX_E_INIT;
        }
    } else {
        return CBX_E_PORT;
    }
}


int
cbxi_cascade_port_get(int unit, cbx_portid_t *portid)
{
    if (portid == NULL) {
        return CBX_E_PARAM;
    }
    if (SOC_IS_CASCADED(0)) {
        if (cascade_ports[unit] == CBXI_PGID_INVALID) {
            return CBX_E_INIT;
        }
        *portid = cascade_ports[unit];
        return CBX_E_NONE;
    } else {
        return CBX_E_INIT;
    }
}

int
cbxi_cascade_port_delete(cbx_portid_t portid)
{
    int rv = 0;
    int unit = CBX_AVENGER_PRIMARY;
    cbx_port_t port_out;
    cbxi_pgid_t oldlpg, lpg;
    uint32 sts, regval, fval;
    int start, end, max;
    pg2lpg_t       pg2lpg_entry;

    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API:Port validation failed for %d \n"), portid));
        return CBX_E_PARAM;
    }

    CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
    soc_CB_PQM_CASCADEr_field_get(unit, &regval, LSPGIDf, &oldlpg);
    if (lpg == oldlpg) {
       fval = 0;
       lpg = 0;
       soc_CB_PQM_CASCADEr_field_set(unit, &regval, ENABLEf, &fval);
       soc_CB_PQM_CASCADEr_field_set(unit, &regval, LSPGIDf, &lpg);
       soc_CB_PQM_CASCADEr_field_set(unit, &regval, PPFOVf, &fval);
       CBX_IF_ERROR_RETURN(REG_WRITE_CB_PQM_CASCADEr(unit, &regval));

       REG_READ_CB_ETM_CASCADE_PORT_ENABLEr(unit, &regval);
       regval = 0;
       CBX_IF_ERROR_RETURN(REG_WRITE_CB_ETM_CASCADE_PORT_ENABLEr(unit, &regval));

       /* Revert the Cascade setup */
       CBX_PBMP_COUNT(pp_created_pbmp[0], max);
       end = soc_robo2_pg2lpg_max_index(unit);
       start = (unit == CBX_AVENGER_PRIMARY) ? max : 0;
       end   = (unit == CBX_AVENGER_PRIMARY) ? end : max - 1;
       while (start <= end) {
           CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(unit, start, &pg2lpg_entry));
           pg2lpg_entry.lpg = start;
           CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(unit, start, &pg2lpg_entry, &sts));
           start++;
       }
    }
#ifdef CONFIG_PORT_EXTENDER
    CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
    soc_CB_PQM_CASCADEr_field_get(unit, &regval, PPFOVf, &fval);
    /* Do not retain decaped headers at egress */
    REG_READ_CB_PQM_PDr(unit, &regval);
    regval &= ~((uint32_t)fval);
    REG_WRITE_CB_PQM_PDr(unit, &regval);
#endif

    return CBX_E_NONE;
}

int
cbxi_pqm_cascade_ppfov_modify(int flags )
{
    int rv = 0;
    int unit = CBX_AVENGER_PRIMARY;
    uint32  regval, fval = 0;
    uint32 ppfov = 0;

    CBX_PBMP_ASSIGN(ppfov,PBMP_CASCADE(unit));

    CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
    soc_CB_PQM_CASCADEr_field_get(unit, &regval, PPFOVf, &fval);

    if(flags == CBXI_PQM_CASCADE_PPFOV_ADD) {
        fval |= ppfov;
    } else if(flags == CBXI_PQM_CASCADE_PPFOV_DELETE) {
        fval &= ~ppfov;
    } else {
        return CBX_E_PARAM;
    }

    soc_CB_PQM_CASCADEr_field_set(unit, &regval, PPFOVf, &fval);
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_PQM_CASCADEr(unit, &regval));
    return rv;
}

int
cbxi_cascade_port_add(cbx_portid_t portid)
{
    int rv = 0;
    int unit = CBX_AVENGER_PRIMARY;
    cbx_port_t port_out;
    cbxi_pgid_t lpg;
    uint32 sts, regval, fval;
    int start, end, max;
    cbx_lag_info_t lag_info;
    pg2lpg_t       pg2lpg_entry;
    pbmp_t       pg_map = 0;

    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);

    if (unit) {
    /* WAR: Drop CSD header packets with devid 1 on secondary,
     * and forward devid 0 packets (as before) */
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(
                                        unit, SLIC_CSD_DEV1_DROP, &pg_map));
        CBX_PBMP_PORT_ADD(pg_map, lpg);
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(
                                        unit, SLIC_CSD_DEV1_DROP, pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(
                                        unit, SLIC_CSD_DEV0_FWD, pg_map));
    }

#ifdef CONFIG_PORT_EXTENDER
    /* Enbale CSD header alternatively */
    if (!unit) {
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(
                                        unit, SLIC_CSD_DEV1_DROP, &pg_map));
        CBX_PBMP_PORT_ADD(pg_map, lpg);
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(
                                        unit, SLIC_CSD_DEV1_DROP, pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(
                                        unit, SLIC_CSD_DEV0_FWD, pg_map));
    }
#endif /* CONFIG_PORT_EXTENDER */

    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API:Port validation failed for %d \n"), portid));
        return CBX_E_PARAM;
    }
    sal_printf("Cascade port set: Portid: %d Portout %d Lpg %d unit %d\n",
                 portid, port_out, lpg, unit);
    fval = 1;
    if (CBX_PORTID_IS_TRUNK(portid)) {
        CBX_PBMP_CLEAR(lag_info.lag_pbmp[CBX_AVENGER_PRIMARY]);
        CBX_PBMP_CLEAR(lag_info.lag_pbmp[CBX_AVENGER_SECONDARY]);
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        soc_CB_PQM_CASCADEr_field_set(unit, &regval, ENABLEf, &fval);
        soc_CB_PQM_CASCADEr_field_set(unit, &regval, LSPGIDf, &lpg);
        fval = 0;

        /* Adding cascade port to PQM cascade PPFOV,
         * to avoid traffic to loopback on cascade port.
         */
        CBX_PBMP_OR(fval, lag_info.lag_pbmp[unit]);

        soc_CB_PQM_CASCADEr_field_set(unit, &regval, PPFOVf, &fval);
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_PQM_CASCADEr(unit, &regval));
        CBX_PBMP_OR(PBMP_CASCADE(unit), lag_info.lag_pbmp[unit]);

#if 0 /* With this setup LBH index is always 0 for forwarding and
       * load distribution across LAG in cascade mode is not working
       */
        /* Setup LBH Override for Cascade */
        REG_READ_CB_IPP_KBU_CONFIGr(unit, &regval);
        fval = 4;
        soc_CB_IPP_KBU_CONFIGr_field_set(unit, &regval, SPP2LBHf, &fval);
        REG_WRITE_CB_IPP_KBU_CONFIGr(unit, &regval);
#endif
    } else {
        soc_CB_PQM_CASCADEr_field_set(unit, &regval, ENABLEf, &fval);
        soc_CB_PQM_CASCADEr_field_set(unit, &regval, LSPGIDf, &lpg);
        fval = 0;

        /* Adding cascade port to PQM cascade PPFOV
         * to avoid traffic to loopback on cascade port.
         */
        CBX_PBMP_PORT_ADD(fval, port_out);

        soc_CB_PQM_CASCADEr_field_set(unit, &regval, PPFOVf, &fval);
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_PQM_CASCADEr(unit, &regval));
        CBX_PBMP_PORT_ADD(PBMP_CASCADE(unit), port_out);
    }

#ifndef CONFIG_BCM_TAG
    REG_READ_CB_ETM_CASCADE_PORT_ENABLEr(unit, &regval);
    regval = 1 << lpg;
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_ETM_CASCADE_PORT_ENABLEr(unit, &regval));
#endif
    /* Setup non local PGs to pass through cascade port */
    CBX_PBMP_COUNT(pp_created_pbmp[0], max);
    end = soc_robo2_pg2lpg_max_index(unit);
    start = (unit == CBX_AVENGER_PRIMARY) ? max : 0;
    end   = (unit == CBX_AVENGER_PRIMARY) ? end: max - 1;
    while (start <= end) {
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(unit, start, &pg2lpg_entry));
        pg2lpg_entry.lpg = lpg;
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(unit, start, &pg2lpg_entry, &sts));
        start++;
    }
    cascade_ports[unit] = portid;

    /* Enable distributed lag pruning */
    REG_READ_CB_PQM_DLAGENr(unit, &regval);
    fval = 0x1;
    soc_CB_PQM_DLAGENr_field_set(unit, &regval, DLAG_PRUN_ENf, &fval);
    REG_WRITE_CB_PQM_DLAGENr(unit, &regval);

    /* Configure trap destination for secondary avenger */
    /* Add cascade ports to all existing vlans
     * For all new vlans its added when vlan is created
     *| Fixme
    cbxi_vlan_all_cascade_ports_add(unit);
    */
    /* Configure trap destination for secondary avenger */
    if(unit == CBX_AVENGER_SECONDARY) {
        cbxi_trap_dest_update(unit, portid);
    }

    CBX_IF_ERROR_RETURN(cbxi_dest_cascade_set(unit, portid));

#ifdef CONFIG_PORT_EXTENDER
    CBX_IF_ERROR_RETURN(cbxi_pe_avg_csd_config());

    CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
    soc_CB_PQM_CASCADEr_field_get(unit, &regval, PPFOVf, &fval);
    /* Retain Decaped headers at egress */
    REG_READ_CB_PQM_PDr(unit, &regval);
    regval |= ((uint32_t)fval);
    REG_WRITE_CB_PQM_PDr(unit, &regval);

#endif
    return rv;
}
#endif

void
cbxi_pgt_default_umplus_value_set(pgt_t *pgt_entry) {

    pgt_entry->pepper = 0;
    pgt_entry->pppoe_en = 0;
    pgt_entry->itag_en = 0;
    pgt_entry->tag_parse_en = 1;
    pgt_entry->tlv_parse_en = 1;
    pgt_entry->dfp_en = 0;

    pgt_entry->ipv4_en = 1;
    pgt_entry->ipv6_en = 1;
    pgt_entry->slic_map_en = 1;
    pgt_entry->ctag_en = 1;
    pgt_entry->soft_tag_en = 0;
    pgt_entry->mcmpls_en = 0;

    pgt_entry->llc_en = 0;
    pgt_entry->ptp_en = 0;
    pgt_entry->etag_en = 0;
    pgt_entry->stag_en = 1;
    pgt_entry->arp_en = 1;
    pgt_entry->ucmpls_en = 0;

    pgt_entry->metadata_len = 0;
    pgt_entry->hdr_words_m1 = 0;

    pgt_entry->len0 = 12;
    pgt_entry->len1 = 0;
    pgt_entry->proto0 = CBXI_NT_MAC;
    pgt_entry->proto1 = 0;

    pgt_entry->arb0 = 3;
    pgt_entry->arb1 = 0;
    pgt_entry->arb2 = 1;
    pgt_entry->arb3 = 0;
    pgt_entry->arb4 = 2;
    pgt_entry->arb5 = 0;
    pgt_entry->arb6 = 1;
    pgt_entry->arb7 = 0;

    pgt_entry->src0 = 1;  /* CTAG 802.1P */
    pgt_entry->src1 = 7;  /* IPv4 DSCP */
    pgt_entry->src2 = 8;  /* IPv6 DSCP */

    pgt_entry->pri_base0 = 8;
    pgt_entry->pri_base1 = 64;
    pgt_entry->pri_base2 = 64;

    pgt_entry->default_tc = 0;
    pgt_entry->default_dp = 0;
    pgt_entry->default_vid = 1;

    pgt_entry->default_sli =  0;
    pgt_entry->default_slicid = 0;

    pgt_entry->fcd0 = 0;
    pgt_entry->fcd1 = 1;
    pgt_entry->fcd2 = 2;
    pgt_entry->fcd3 = 3;
    pgt_entry->fcd4 = 4;
    pgt_entry->fcd5 = 5;
    pgt_entry->fcd6 = 6;
    pgt_entry->fcd7 = 7;

    pgt_entry->spgid = 0;
    pgt_entry->mirror = 0;

    pgt_entry->drop_ulf = 1;
    pgt_entry->drop_slf = 1;
    pgt_entry->drop_mlf = 1;
    pgt_entry->drop_bcast = 1;
    pgt_entry->rp_extra = 0;

}

/*
 * Function:
 *  cbxi_port_table_init
 * Purpose:
 *  Global Initialization for all port related tables.
 *
 */
int
cbxi_port_table_init() {

    uint32        num_units = 1;
    int           i = 0;
    int           max_index = 0;
    lpg2ppfov_t   lpg2ppfov_entry;
#ifndef EMULATION_BUILD
    pg2lpg_t      pg2lpg_entry;
    lpg2pg_t      lpg2pg_entry;
    lpg2pg_entry.pg = 0;
    pg2lpg_entry.lpg = 0;
    int           index = 0;
    uint32        status  = 0;
#endif /* !EMULATION_BUILD */


    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    for ( i = 0; i < num_units; i++ ) {

        /* Enable and reset PGT */
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_reset(i));

        /* Enable and reset PET */
        CBX_IF_ERROR_RETURN(soc_robo2_pet_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pet_reset(i));

        /* Enable and reset DLIET */
        CBX_IF_ERROR_RETURN(soc_robo2_dliet_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_dliet_reset(i));

        /* Enable and reset ERT */
        CBX_IF_ERROR_RETURN(soc_robo2_ert_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ert_reset(i));

        /* Enable, reset PP2LPG and set 1:1 PP to PG MAP */
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_reset(i));

        CBX_IF_ERROR_RETURN(soc_robo2_pqm_pp2lpg_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_pp2lpg_reset(i));

        /* Enable and reset EPGT */
        CBX_IF_ERROR_RETURN(soc_robo2_epgt_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_epgt_reset(i));

        /* Enable and reset PG2IG */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ig_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ig_reset(i));

        /* Enable and reset GSPG2IG */
        CBX_IF_ERROR_RETURN(soc_robo2_gspg2ig_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_gspg2ig_reset(i));

        /* Enable and reset LPG2PPFOV */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_reset(i));

        max_index =  soc_robo2_lpg2ppfov_max_index(i);
        lpg2ppfov_entry.ppfov = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_init(
                     i, 0, max_index, 0, &lpg2ppfov_entry));
        /* Enable and reset SVT */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_svt_reset(i));

        /* Enable and reset PG2AP */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ap_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2ap_reset(i));

        /* Enable and reset PGLCT */
        CBX_IF_ERROR_RETURN(soc_robo2_pglct_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pglct_reset(i));

       /* Enable and reset PG2LPG, set 1:1 PG to LPG MAP  */
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_reset(i));
#ifndef EMULATION_BUILD
        max_index =  soc_robo2_pg2lpg_max_index(i);
        /* Global PG is in the range 16-29  for Secondary Avenger */
        if (i == CBX_AVENGER_SECONDARY) {
            index = CBX_MAX_PORT_PER_UNIT;
            while (index <= max_index) {
                pg2lpg_entry.lpg = index - CBX_MAX_PORT_PER_UNIT;
                CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(i, index,
                                                  &pg2lpg_entry, &status));
                index++;
            }
        }
#endif /* !EMULATION_BUILD  */
       /* Enable and reset LPG2PG, set LPG to PG MAP  */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_reset(i));
#ifndef EMULATION_BUILD
        max_index =  soc_robo2_lpg2pg_max_index(i);
        /* Global PG starts from 16 for Secondary Avenger */
        if (i == CBX_AVENGER_SECONDARY) {
            index = 0;
            while (index <= max_index) {
                lpg2pg_entry.pg = index + CBX_MAX_PORT_PER_UNIT;
                CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(i, index,
                                                &lpg2pg_entry, &status));
                index++;
            }
        }
#endif /* !EMULATION_BUILD  */
       /* Enable and reset spg_pp_bmp_config */
        CBX_IF_ERROR_RETURN(soc_robo2_spg_pp_bmp_config_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_spg_pp_bmp_config_reset(i));

        CBX_IF_ERROR_RETURN(soc_robo2_eeecfg_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_eeecfg_reset(i));
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *      cbxi_robo2_portid_validate
 * Description:
 *      Helper funtion to validate portid parameter
 * Parameters:
 *      port_in  - (IN) PortId to validate
 *      port_out - (OUT) Physical Port number if valid.
 * Return Value:
 *      CBX_E_NONE Success
 *      CBX_E_PORT Port Invalid
 */
int
cbxi_robo2_portid_validate(cbx_portid_t port_in, cbx_port_t *port_out) {
    if (CBX_PORTID_IS_SET(port_in)) {
        if (CBX_PORTID_IS_LOCAL(port_in)) {
            *port_out = CBX_PORTID_LOCAL_GET(port_in);
            if (*port_out >= CBX_PORT_MAX) {
                return CBX_E_PORT;
            }
        } else if (CBX_PORTID_IS_VLAN_PORT(port_in)) {
            *port_out = CBX_PORTID_VLAN_PORT_ID_GET(port_in);
            if (*port_out >= CBX_PORT_MAX) {
                return CBX_E_PORT;
            }
        } else if (CBX_PORTID_IS_TRUNK(port_in)) {
            *port_out = CBX_PORTID_TRUNK_GET(port_in);
            if (*port_out >= CBX_LAG_MAX) {
                return CBX_E_PORT;
            }
        } else if (CBX_PORTID_IS_MCAST(port_in)) {
            CBX_IF_ERROR_RETURN(cbxi_mcast_id_check(port_in));
            *port_out = CBX_PORTID_MCAST_ID_GET(port_in);
            return CBX_E_NONE;
        }
    } else if (port_in < CBX_PORT_MAX) {
        *port_out = port_in;
    } else {
        return CBX_E_PORT;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_robo2_lpg_to_gpg
 * Description:
 *      Obtain Global PG associated with given Local PG
 * Parameters:
 *      unit     - (IN unit number(master or slave))
 *      lpg      - (IN) Local Port group associated with port.
 *      gpg      - (OUT) Global port group
 * Return Value:
 *      CBX_E_NONE - Port OK
 *      CBX_E_PORT - Port Invalid
 */

int
cbxi_robo2_lpg_to_gpg(int unit, cbxi_pgid_t lpg, cbxi_pgid_t *gpg)
{
    lpg2pg_t      lpg2pg_entry;

    /* Read entry from LPG2PG */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, lpg, &lpg2pg_entry));
    *gpg = lpg2pg_entry.pg;

    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_robo2_port_to_lpg
 * Description:
 *      Obtain Local PG associated with given physical port
 * Parameters:
 *      unit     - (IN unit number(master or slave))
 *      port     - (IN) Physical Port to validate
 *      lpg      - (OUT) Port group associated with port no.
 * Return Value:
 *      CBX_E_NONE - Port OK
 *      CBX_E_PORT - Port Invalid
 */

int
cbxi_robo2_port_to_lpg(int unit, cbx_port_t port, cbxi_pgid_t *lpg)
{
    ipp_pp2lpg_t   pp2lpg_entry;

    CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(unit, port, &pp2lpg_entry));

    *lpg = pp2lpg_entry.pgid;

    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_robo2_port_validate
 * Description:
 *      Helper funtion to validate port parameter
 * Parameters:
 *      port_in  - (IN) Port to validate
 *      port_out - (OUT) Port number if valid.
 *      local_pg - (OUT) Port group associated with port no.
 *      unit     - (OUT) unit number(master or slave)
 * Return Value:
 *      CBX_E_NONE - Port OK
 *      CBX_E_PORT - Port Invalid
 */
int
cbxi_robo2_port_validate(cbx_portid_t  portid, cbx_port_t *port_out,
                         cbxi_pgid_t *local_pg, int *unit) {
    int rv = CBX_E_NONE;
    int max_index = 0;
    pbmp_t p_map;
    pbmp_t p_map_all;
    cbx_lag_info_t lag_info;
    cbx_port_t      port;

    *unit = CBX_AVENGER_PRIMARY;

    if (portid == 2) {
        sal_printf("Don't check port 2.\n");
        return CBX_E_NONE;
    }

    /* Validate the portid passed */
    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_validate(portid, port_out));
    if (CBX_PORTID_IS_MCAST(portid)) {
        *unit = CBX_AVENGER_PRIMARY;
        *local_pg = CBXI_PGID_INVALID;
        return CBX_E_NONE;
    }
    max_index = soc_robo2_ipp_pp2lpg_max_index(CBX_AVENGER_PRIMARY);
    // sal_printf("max_index is %d\n", max_index);

    if (max_index < 0) {
        sal_printf("Internal error\n");
        return CBX_E_INTERNAL;
    }
    // sal_printf("*port_out is %d\n", *port_out);
    if ((*port_out > max_index)) {
        if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY))) {
            *port_out = *port_out - max_index - 1;
            *unit = CBX_AVENGER_SECONDARY;
        } else {
            sal_printf("Invalid port non-cascade\n");
            return CBX_E_PORT;
        }
    }
    CBX_PBMP_CLEAR(lag_info.lag_pbmp[CBX_AVENGER_PRIMARY]);
    CBX_PBMP_CLEAR(lag_info.lag_pbmp[CBX_AVENGER_SECONDARY]);
    CBX_PBMP_CLEAR(p_map);

    /* Obtain sku based valid pbm defined in switch init and check if port is a
     * valid physical port*/
    p_map_all = PBMP_ALL(*unit);

    if (CBX_PORTID_IS_TRUNK(portid)) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        if (CBX_PBMP_NOT_NULL(lag_info.lag_pbmp[CBX_AVENGER_PRIMARY])) {
            *local_pg = lag_info.lpgid;
            CBX_PBMP_ITER(lag_info.lag_pbmp[CBX_AVENGER_PRIMARY], port) {
                *port_out = port;
                *unit = CBX_AVENGER_PRIMARY;
                break;
            }
        } else if (CBX_PBMP_NOT_NULL(lag_info.lag_pbmp[CBX_AVENGER_SECONDARY])) {
            *local_pg = lag_info.rlpgid;
            CBX_PBMP_ITER(lag_info.lag_pbmp[CBX_AVENGER_SECONDARY], port) {
                *port_out = port;
                *unit = CBX_AVENGER_SECONDARY;
                break;
            }
        } else {
            return CBX_E_PORT;
        }
    } else {
        CBX_PBMP_PORT_SET(p_map, *port_out);
        CBX_PBMP_AND(p_map, p_map_all);
        if (0 == p_map) {
            sal_printf("Not valid port %d\n", *port_out);
            return CBX_E_PORT;
        }
        /* Obtain Local port group associated with physical port */
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_to_lpg(*unit, *port_out, local_pg));
    }
    /* Check if port  is already created */
    if (CBX_PBMP_MEMBER(pp_created_pbmp[*unit], *port_out)) {
        return rv;
    } else {
        return CBX_E_NOT_FOUND;
    }
}

/**
 * Function:
 *   cbxi_port_egress_info_set
 * Purpose :
 *   This routine is used to set the egress parameters of a Port
 *
 * @param unit     device Identifier - PRIMARY OR SECONDARY
 * @param portid   PORT Identifier
 * @param lpgid    Local PGID
 * @parm  port_params Port parameters
 * @return return code - CBX_E_XX
 * @retval success - CBX_E_NONE
 */

int
cbxi_port_egress_info_set( int unit,
                           int port,
                           int lpgid,
                           cbx_port_params_t *port_params )
{
    uint32 regval = 0;
    cbxi_encap_record_t  encap_record;
#ifdef CONFIG_PTP
    cbx_ptp_clk_mode_t mode;
    uint8 ptp_admin;
#endif

    sal_memset(&encap_record, 0, sizeof(cbxi_encap_record_t));
    if ((((port == CBX_PORT_ICPU) || (port == CBX_PORT_ECPU)) &&
        (unit == CBX_AVENGER_PRIMARY)) ||
        port_params->port_type == cbxPortTypeHost) {
#ifdef CONFIG_PTP
        CBX_EPP_PREPEND_PTP_TIMESTAMP_TLV(&encap_record, 0x00);

        regval = 0xBBCC; /* Custom Ethertype to be used for Timestamp */
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_ETHERTYPE_CONFIG_TSr(unit, &regval));
#endif

#ifndef CONFIG_BC_MODE1
        /* Insert CB tag directive */
        CBX_EPP_INSERT_CB_IMP_TAG(&encap_record);
#endif
    } else if (port_params->port_type == cbxPortTypeCascade) {
#ifndef CONFIG_PORT_EXTENDER
        /* Insert Cascade Header, No Timestamping */
        CBX_EPP_INSERT_CB_CASCADE_TAG(&encap_record);
#endif
        CBX_IF_ERROR_RETURN(REG_READ_NPA_NPA_TLV_CONTROLr(unit, &regval));
        regval |= (0x1 << (uint32)port);
        CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_TLV_CONTROLr(unit, &regval));
    } else {
        /* Enable Timestamping */
        CBX_IF_ERROR_RETURN(REG_READ_NPA_NPA_TLV_CONTROLr(unit, &regval));
        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
            regval |= (0x1 << (uint32)port);
#ifdef CONFIG_PTP
            regval &= ~(0x1 << (uint32)port);
#endif
        } else {
            regval &= ~(0x1 << (uint32)port);
        }
        CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_TLV_CONTROLr(unit, &regval));

#ifdef CONFIG_PTP
        cbx_ptp_port_admin_get(port, &ptp_admin);
        if (ptp_admin == 1) {
            cbx_ptp_clock_mode_get(&mode);
            if (mode == cbxPtpClkTwoStep) {
                encap_record.data[encap_record.len++] = 0x10;
                encap_record.data[encap_record.len++] = 0x80;
            }
        }
        CBX_EPP_PREPEND_PTP_TIMESTAMP_TLV(&encap_record, 0x00);
#else
        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
            /* Set prepend TLV encap directive to add DMAC back */
            CBX_EPP_PREPEND_PTP_TIMESTAMP_TLV(&encap_record, 0x00);
        }
#endif
#ifdef DSCP_REMARK
        /* Need to uncomment this for enabling DSCP remark and TTL
           decrement. Once it is enabled, all non IPv4 packets
           will get dropped becuase EPP doesn't find the IP header
           to update DSCP/TTL.*/
        /* EPMT index is from where DSCP value needs to be got
           = base(from CB_EPP_Egress_Port_Map_Base) +
           (EPMT index - 1) * 6 + offset computed based on TC and DP */
        CBX_EPP_INSERT_DSCP_RE_WRITE(&encap_record, 0x1);
#endif
#ifdef TTL_DECREMENT
        CBX_EPP_INSERT_TTL_DECREMENT(&encap_record);
#endif
    }

    /* Insert EOR directive at end and configure encap_record */
    CBX_EPP_INSERT_EOR(&encap_record);
    CBX_IF_ERROR_RETURN(cbxi_port_encap_set(unit, lpgid, &encap_record));
    return CBX_E_NONE;
}

/**
 * Function:
 *   cbxi_port_info_set
 * Purpose :
 *   This routine is used to set the parameters of a Port in PGT.
 *
 * @param unit     device Identifier - PRIMARY OR SECONDARY
 * @param portid   PORT Identifier
 * @parm  port_params Port parameters, including:
 * - port type
 * - drop tagged packets
 * - drop untagged packets
 *
 * @return return code - CBX_E_XX
 * @retval success - CBX_E_NONE
 */

int
cbxi_port_info_set( int unit,
                    int port,
                    int lpgid,
                    cbx_port_params_t *port_params ) {

    pgt_t          pgt_entry;
    epgt_t         epgt_entry;
    lpg2ap_t       lpg2ap_entry;
    pglct_t        pglct_entry;
    uint32         status  = 0;
    cbxi_pgid_t    pgid = 0;
    cbx_vlan_t     vsi;
    int            rv = CBX_E_NONE;
    uint32_t       regval = 0;
    uint8_t        pe_en = 0;


    /* Read entry from PGT */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get( unit, lpgid, &pgt_entry));

    pgt_entry.drop_bcast = 0;
    pgt_entry.drop_ulf = 0;
    pgt_entry.drop_mlf = 0;
    pgt_entry.drop_slf = 0;
    pgt_entry.pepper = 0;
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpgid, &pgid));
    pgt_entry.spgid = pgid;

    /* Enable tag parsing */
    pgt_entry.tag_parse_en = 1;
    pgt_entry.slic_map_en = 1;
    pgt_entry.ctag_en = 1;
    pgt_entry.metadata_len = 0x2;
    pgt_entry.tlv_parse_en = 0;
    pgt_entry.ipv4_en = 1;
    if (SOC_CONTROL(unit)->info.driver_type == SOC_CHIP_BCM53158_B1) {
        pgt_entry.ipv6_en = 1;
        pgt_entry.llc_en = 1;
    } else {
        pgt_entry.ipv6_en = 0;
        pgt_entry.llc_en = 0; /* Disabling to avoid pipeline freeze seen on A0 */
    }
    pgt_entry.dfp_en = 1;
    pgt_entry.arp_en = 1;

    pgt_entry.src0 = 9;  /* IP_DSCP V4 or V6  */
    pgt_entry.src1 = 1;  /* C-tag */
    pgt_entry.src2 = 2;  /* S-tag */
    pgt_entry.arb0 = 3;
    pgt_entry.arb1 = 0;
    pgt_entry.arb2 = 1;
    pgt_entry.arb3 = 1;
    pgt_entry.arb4 = 2;
    pgt_entry.arb5 = 2;
    pgt_entry.arb6 = 1;
    pgt_entry.arb7 = 1;
    pgt_entry.pri_base2 = pgt_entry.pri_base1; /* C-tag, S-tag use the same TC,DP entry in CPMT */

    /* *************** Native type mismatch work-around *************** */
    /* WAR#4 for port 15 (ipv4/ipv6 parsing disabled)
     * WAR#3 for ports 0 - 14*/

    if ((((port == CBX_PORT_ICPU) || (port == CBX_PORT_ECPU))
           && (unit == CBX_AVENGER_PRIMARY)) ||
           (port_params->port_type == cbxPortTypeCascade)) {
#ifdef CONFIG_BC_MODE1
            pgt_entry.slic_map_en = 1;
#else
            pgt_entry.slic_map_en = 0;
#endif
    /* ******* WAR#4 for Native type mismatch ******* */
        if ((port_params->port_type == cbxPortTypeCascade) &&
            (SOC_DRIVER(unit)->type != SOC_CHIP_BCM53158_A0)) {
            /* Do nothing */
        } else {
#ifdef CONFIG_PTP
            pgt_entry.ipv4_en = 1;
#else
            pgt_entry.ipv4_en = 0;
#endif
            pgt_entry.ipv6_en = 0;
            pgt_entry.metadata_len = 0x1;
        }
    } else {
        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
    /* ******* WAR#3 for Native type mismatch ******* */
            pgt_entry.metadata_len = 0x1;
            pgt_entry.tlv_parse_en = 1;
        }
    }
    /* *************** Native type mismatch work-around *************** */
    if (((port == CBX_PORT_ICPU) || (port == CBX_PORT_ECPU)) &&
         (unit == CBX_AVENGER_PRIMARY)) {
#if 0
    if((IS_LOCAL_CPU_PORT(unit, port)) ||
       (IS_EXTERNAL_CPU_PORT(unit, port)))
#endif
        pgt_entry.hdr_words_m1 = 5; /* Process IMP header */
        pgt_entry.tlv_parse_en = 0;
        pgt_entry.soft_tag_en = 0x3;
#ifdef CONFIG_PTP
        pgt_entry.tlv_parse_en = 1;
        pgt_entry.hdr_words_m1 = 0xb;
        pgt_entry.metadata_len = 0x2;
        pgt_entry.ptp_en = 1;
#endif
    } else if (port_params->port_type == cbxPortTypeCascade) {
        pgt_entry.hdr_words_m1 = 5; /* Process IMP header */
        pgt_entry.soft_tag_en = 0x3;
    } else {
        /* Process upto 192 Bytes of packet */
        pgt_entry.hdr_words_m1 = (192 / 16) - 1;
        /*Enable if TS is enabled on the port*/
        /*pgt_entry.tlv_parse_en = 1;*/

        /* Enable soft tag for LLDP packets */
        pgt_entry.soft_tag_en = 1 << SOFT_TAG_LLDP;

        /* Enable soft tag for EAPOL packets */
        pgt_entry.soft_tag_en |= 1 << SOFT_TAG_EAPOL;

#ifdef CONFIG_PTP
        /*********** PTP changes *****************/
        pgt_entry.metadata_len = 0x2;
        pgt_entry.tlv_parse_en = 1;
        pgt_entry.ptp_en = 1;
        pgt_entry.slic_map_en = 1;
        /*********** PTP changes *****************/
#endif
    }

    if ((unit) && (port_params->port_type == cbxPortTypeCascade)) {
    /* WAR: Enable SLIC to drop CSD header packets with devid 1 on secondary */
        pgt_entry.slic_map_en = 1;
    }

    /* Remove metadata at egress */
    CBX_IF_ERROR_RETURN(
        REG_READ_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(unit, &regval));
    /* 00 = No metadata; 01 = 16B MetaData ; 10 = 32B */
    regval &= ~(0x3 << (lpgid * 2));/* Clear the md_size field for given port */
    regval |= ((pgt_entry.metadata_len & 0x3) << (lpgid * 2));
    CBX_IF_ERROR_RETURN(
        REG_WRITE_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(unit, &regval));

    if (port_params->flags & CBX_PORT_DROP_BCAST) {
        pgt_entry.drop_bcast = 1;
    }
    if (port_params->flags & CBX_PORT_DROP_MLF) {
        pgt_entry.drop_mlf = 1;
    }
    if (port_params->flags & CBX_PORT_DROP_SLF) {
        pgt_entry.drop_slf = 1;
    }
    if (port_params->flags & CBX_PORT_DROP_ULF) {
        pgt_entry.drop_ulf = 1;
    }

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        pe_en = 1;
    }
#endif /* CONFIG_PORT_EXTENDER */
    if (pe_en != 1) {
        /* Set default vid only if not port extender */
        rv = cbxi_vlan_vsit_index_get(
                        port_params->default_vid, &vsi);
        if (rv < 0) {
            pgt_entry.default_sli =
                CBXI_PV_LIID_SET(pgid, port_params->default_vid);
        } else {
            pgt_entry.default_sli = CBXI_PV_LIID_SET(pgid, vsi);
        }
        pgt_entry.default_vid = port_params->default_vid;
    }
    pgt_entry.default_tc = port_params->default_tc;
    pgt_entry.default_dp = port_params->default_dp;
    pgt_entry.vid_src = port_params->sli_mode;

    if( port_params->flags & CBX_PORT_VIRTUAL_PORTS_ENABLE) {
        pgt_entry.pepper |= CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
    }
    if( port_params->flags & CBX_PORT_DROP_UNTAGGED) {
        pgt_entry.pepper |= CBX_PORT_PEPPER_DROP_UNTAGGED;
    }
    if( port_params->flags & CBX_PORT_DROP_TAGGED) {
        pgt_entry.pepper |= CBX_PORT_PEPPER_DROP_TAGGED;
    }
    pgt_entry.pepper |= port_params->port_type <<
                           CBX_PORT_PEPPER_PORT_TYPE_SHIFT;

    if (port_params->port_type == cbxPortTypeProvider) {
        pgt_entry.stag_en = 1;
    }

    if (port_params->port_type == cbxPortTypePortVLAN) {
        pgt_entry.ctag_en = 0;
    }

    /* Update PGT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_set( unit, lpgid, &pgt_entry, &status));

    /* Read entry from LPG2AP and set admission profile */
    lpg2ap_entry.adm_profile = (uint8)port_params->ac_policy;

    rv = soc_robo2_lpg2ap_set(unit, lpgid, &lpg2ap_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..Failed to update  \
                                  LPG2AP entry \n")));
        return rv;
    }

    /* Read entry from PGLCT and set learn limit */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpgid, &pgid));
    rv = soc_robo2_pglct_get( unit, pgid, &pglct_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..failed to read \
                                  PGLCT table entry \n")));
        return rv;
    }
    pglct_entry.learn_limit = port_params->learn_limit;
    rv = soc_robo2_pglct_set(unit, pgid, &pglct_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..Failed to update \
                                  PGLCT entry \n")));
        return rv;
    }
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Since GPG is unique across Avengers, configure same values in partner
         * Avenger */
        rv = soc_robo2_pglct_set(CBXI_CASC_PARTNER_UNIT(unit),
                                        pgid, &pglct_entry, &status);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbx_port_info_set()..Failed to update \
                                  PGLCT entry \n")));
            return rv;
        }
    }


    /* Read entry from EPGT and set MTU */
    rv = soc_robo2_epgt_get( unit, lpgid, &epgt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..failed to \
                     read EPGT table entry \n")));
        return rv;
    }
    epgt_entry.mtu = port_params->mtu;
    rv = soc_robo2_epgt_set(unit, lpgid, &epgt_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..Failed to update \
                                  EPGT entry \n")));
        return rv;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_lpg2ppfov_update
 * Purpose:
 *  Update PPFOV for the PG passed
 *
 */
int
cbxi_robo2_lpg2ppfov_update(int unit, int lpgid,
                            cbx_port_t port, uint32_t flags) {

    lpg2ppfov_t   ppfov_entry;
    uint32        status  = 0;
    pbmp_t        ppfov = 0;

    /* Read entry from LPP2PGFOV */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit, lpgid, &ppfov_entry));

    ppfov = ppfov_entry.ppfov;

    if (flags == CBXI_PORT_TABLE_ENTRY_REMOVE) {
        CBX_PBMP_PORT_REMOVE(ppfov, port);
    } else {
        CBX_PBMP_PORT_ADD(ppfov, port);
    }
    ppfov_entry.ppfov = ppfov & CBX_PORT_LOCAL_PP_MASK;

    /* Update LPP2PGFOV entry */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_set(unit, lpgid,
                                                &ppfov_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_spg2ppconfig_update
 * Purpose:
 *  Update PPFOV for the PG passed
 *
 */
int
cbxi_robo2_spg2ppconfig_update(int unit, int lpgid,
                            cbx_port_t port, uint32_t flags) {

    spg_pp_bmp_config_t   spg_pp_config;
    uint32        status  = 0;
    pbmp_t        ppfov = 0;

    /* Read entry from LPP2PGFOV */
    CBX_IF_ERROR_RETURN(soc_robo2_spg_pp_bmp_config_get(unit, lpgid, &spg_pp_config));

    ppfov = spg_pp_config.spg_pp_bmp;

    if (flags == CBXI_PORT_TABLE_ENTRY_REMOVE) {
        CBX_PBMP_PORT_REMOVE(ppfov, port);
    } else {
        CBX_PBMP_PORT_ADD(ppfov, port);
    }
    spg_pp_config.spg_pp_bmp = ppfov & CBX_PORT_LOCAL_PP_MASK;

    /* Update LPP2PGFOV entry */
    CBX_IF_ERROR_RETURN(soc_robo2_spg_pp_bmp_config_set(unit, lpgid,
                                             &spg_pp_config, &status));
    return CBX_E_NONE;
}
/*
 * Function:
 *  cbxi_robo2_svt_update
 * Purpose:
 *  Update SVT for the PP passed
 *
 */
int
cbxi_robo2_svt_update(int unit, cbx_port_t port, uint32_t flags) {
    svt_t         svt_entry;
    uint32        status  = 0;
    pbmp_t        svt_mask = 0;
    int           index = 0;
    int           max_index;


    max_index =  soc_robo2_svt_max_index(unit);

    for ( index = 0; index <= max_index; index++) {

        /* Read entry from SVT */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));

        svt_mask = svt_entry.svt_mask;

        if (flags == CBXI_PORT_TABLE_ENTRY_REMOVE) {
            CBX_PBMP_PORT_REMOVE(svt_mask, port);
        } else {
            CBX_PBMP_PORT_ADD(svt_mask, port);
        }
        svt_entry.svt_mask = svt_mask & CBX_PORT_LOCAL_PP_MASK;

        /* Update SVT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                &svt_entry, &status));
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_robo2_pqm_pp2lpg_update
 * Purpose:
 *  Update PP2LPG for the PP passed
 *
 */
int
cbxi_robo2_pqm_pp2lpg_update(int unit, int lpgid,
                             cbx_port_t port) {

    pqm_pp2lpg_t  pp2lpg_entry;
    uint32        status  = 0;

    /* Read entry from SVT */
    CBX_IF_ERROR_RETURN(soc_robo2_pqm_pp2lpg_get(unit, port, &pp2lpg_entry));
    pp2lpg_entry.pgid = lpgid;
    CBX_IF_ERROR_RETURN(soc_robo2_pqm_pp2lpg_set(unit, port,
                                                 &pp2lpg_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_port_duplex_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */

int
cbxi_port_duplex_get(int unit, cbx_port_t port, uint32_t *duplex) {

    int        rv = CBX_E_NONE;

#ifndef EMULATION_BUILD
    if(!(IS_EXTERNAL_CPU_PORT(unit, port))) {
        rv = avng_phy_duplex_get(unit, port, duplex);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
               (BSL_META("FSAL API : cbxi_port_duplex_get()..phy read failed \n")));
            return CBX_E_FAIL;
        }
        return CBX_E_NONE;
    }
#endif /* EMULATION_BUILD */
    rv = mac_duplex_get(unit, port, (int *)duplex);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
           (BSL_META("FSAL API : cbxi_port_duplex_get()..mac read failed \n")));
        return rv;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_port_duplex_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX -  Error
 * Notes:
 *
 */
int
cbxi_port_duplex_set(int unit, cbx_port_t port, int duplex) {

    int rv = CBX_E_NONE;
#ifndef EMULATION_BUILD
    int an = 0;
    if(!(IS_EXTERNAL_CPU_PORT(unit, port))) {
        /* Check if Autoneg is set, if so, disable it */
        rv = cbxi_port_autoneg_get(unit, port, &an);
        if( rv == CBX_E_UNAVAIL) {
            an = 0;
        } else if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
               (BSL_META("FSAL API : cbxi_port_duplex_set().. Unable to get \
                                                         autoneg \n")));
            return rv;
        }

        if (an) {
            an = 0;
            rv = cbxi_port_autoneg_set(unit, port, an);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_PORT,
                   (BSL_META("FSAL API : cbxi_port_duplex_set().. Unable to reset \
                                                               autoneg \n")));
               return rv;
            }
        }
        /* Set duplex at Phy level */
        rv = avng_phy_duplex_set(unit, port, duplex);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_duplex_set()..phy duplex set \
                                                         failed \n")));
            return CBX_E_FAIL;
        }
    }
#endif /* EMULATION BUILD */
    /* Set duplex at MAC level */
    rv = mac_duplex_set(unit, port, duplex);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_duplex_set()..mac duplex set \
                                                         failed \n")));
        return rv;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *      cbxi_port_speed_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error.
 * Notes:
 *
 */
int
cbxi_port_speed_get(int unit,  cbx_port_t port, uint32_t *speed ) {

    int rv = CBX_E_NONE;

#ifndef EMULATION_BUILD
    if(!(IS_EXTERNAL_CPU_PORT(unit, port))) {
        rv = avng_phy_speed_get(unit, port, speed);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_speed_get()..phy read failed \n")));
            return CBX_E_FAIL;
        }
        return CBX_E_NONE;
    }
#endif /* EMULATION_BUILD */
    rv = mac_speed_get(unit, port, (int *)speed);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
           (BSL_META("FSAL API : cbxi_port_speed_get()..mac read failed \n")));
        return rv;
    }
    return CBX_E_NONE;
}

STATIC int
cbxi_port_speed_support_check(int unit,  cbx_port_t port, int speed ) {

    if (port < 8) {
        if ((speed >= 1000) && (otp_flags & SOC_OTP_FLAG_GPHY_1G_DIS)) {
            return CBX_E_DISABLED;
        }
    } else if (port < 12) {
        if ((speed == 2500) && (otp_flags & SOC_OTP_FLAG_SGMII_2G5_DIS)) {
            return CBX_E_DISABLED;
        }
        if ((speed == 1000) && (otp_flags &SOC_OTP_FLAG_SGMII_1G_DIS)) {
            return CBX_E_DISABLED;
        }
    } else if (port == 12) {
        if ((speed > 2500) && (otp_flags & SOC_OTP_FLAG_XFI_12_MAX_2G5)) {
            return CBX_E_DISABLED;
        }
    } else if (port == 13) {
        if ((speed > 2500) && (otp_flags & SOC_OTP_FLAG_XFI_13_MAX_2G5)) {
            return CBX_E_DISABLED;
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_port_speed_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_speed_set(int unit,  cbx_port_t port, int speed ) {
    uint32 en;
    int rv = CBX_E_NONE;
#ifdef CONFIG_QSGMII
    if ((port >=8) && (port <=11)) {
        sal_printf(" Speed change not supported on port 0x%x configured in \
                     Copper mode \n",port);
        return CBX_E_UNAVAIL;
    }
#endif
    rv = cbxi_port_speed_support_check(unit, port, speed);
    if (rv == CBX_E_DISABLED) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbxi_port_speed_set().. Requested speed %d \
                                 is not supported on port %d\n"),speed, port));
    }
    if (CBX_FAILURE(rv)) {
        return rv;
    }
    rv = MAC_ENABLE_GET(port_info[unit][port].p_mac, unit, port, &en);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_speed_set().. Unable to get admin "
                      "status on port %d\n"), port));
        return rv;
    }
#ifndef EMULATION_BUILD
    if(!(IS_EXTERNAL_CPU_PORT(unit, port))) {

        if ((port == 12) || (port == 13)) {
        } else {
            int an = 0;
            /* Check if Autoneg is set, if so, disable it */
            rv = cbxi_port_autoneg_get(unit, port, &an);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_duplex_set().. Unable to get \
                                                         autoneg \n")));
                return rv;
            }
            if (an) {
                an = 0;
                rv = cbxi_port_autoneg_set(unit, port, an);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_PORT,
                    (BSL_META("FSAL API : cbxi_port_duplex_set().. Unable to reset \
                                                               autoneg \n")));
                    return rv;
                }
            }
        }
        /* Disable MAC */
        rv = MAC_ENABLE_SET(port_info[unit][port].p_mac,
                            unit, port, FALSE);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbx_port_speed_set()..Failed  \
                        to disable MAC as part of port speed set \n")));
            return rv;
        }
        {
            /* Set Phy speed */
            rv = avng_phy_speed_set(unit, port, speed);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_speed_set()..phy read failed \n")));
                return CBX_E_FAIL;
            }
            /* Notify phy that we are disabling MAC */
            rv = avng_phy_notify_mac_enable(unit, port, 0);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_speed_set()..phy_notify mac disable failed \n")));
                return CBX_E_FAIL;
            }
        }
    } else {
        /* Disable MAC */
        rv = MAC_ENABLE_SET(port_info[unit][port].p_mac,
                            unit, port, FALSE);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbx_port_speed_set()..Failed  \
                        to disable MAC as part of port speed set \n")));
            return rv;
        }
    }
#endif /* EMULATION_BUILD */
    /* Set Mac speed */
    rv = mac_speed_set(unit, port, speed);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_speed_set(). \
                                       Mac speed set failed \n")));
        return rv;
    }
#ifndef  EMULATION_BUILD
    if(!(IS_EXTERNAL_CPU_PORT(unit, port))) {
        /* Notify phy that we have enabled MAC */
        rv = avng_phy_notify_mac_enable(unit, port, 1);
        if (CBX_FAILURE(rv)) {
             LOG_ERROR(BSL_LS_FSAL_PORT,
                 (BSL_META("FSAL API : cbxi_port_speed_set()..phy notify mac enable failed \n")));
             return CBX_E_FAIL;
        }
    }
#endif
    /* Re-enable MAC, if enabled before */
    if (en) {
        rv = MAC_ENABLE_SET(port_info[unit][port].p_mac, unit, port, TRUE);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                         (BSL_META("FSAL API : cbx_port_speed_set()..Failed  \
                           to enable MAC as part of port speed set \n")));
            return rv;
        }
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_port_frame_max_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX -  Error
 * Notes:
 *
 */
int
cbxi_port_frame_max_get(int unit,  cbx_port_t port, uint32_t *size ) {

    int rv = CBX_E_NONE;

    rv = mac_max_frame_size_get(unit, port, size);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_frame_max_get(). \
                                           .Mac read failed \n")));
        return rv;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *      cbxi_port_frame_max_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error.
 * Notes:
 *
 */
int
cbxi_port_frame_max_set(int unit,  cbx_port_t port, int size ) {

    int rv = CBX_E_NONE;
    if ((size < CBX_PORT_FRAME_SIZE_MIN) || (size > CBX_PORT_FRAME_SIZE_MAX)) {
        return CBX_E_PARAM;
    }
    rv = mac_max_frame_size_set(unit, port, size);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_frame_max_set()... \
                                           Mac write failed \n")));
        return rv;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *      cbxi_port_link_status_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - failure
 * Notes:
 *
 */
int
cbxi_port_link_status_get(int unit, cbx_port_t port, uint32_t *link_status ) {
#ifdef EMULATION_BUILD
    return CBX_E_UNAVAIL;
#else /* non EMULATION_BUILD */
    int      admin_status = 0;
    uint32_t val = 0;
    uint32_t regval = 0;
    int rv = CBX_E_NONE;
    uint32   flush_ctrl = 0;
    uint32   fval = 0;
#ifndef READ_LINK_STATUS_FROM_PHY
    rv = REG_READ_NPA_NPA_RX_LINK_STATUSr(unit, &regval);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_link_status_get().. \
                                           Link status read failed \n")));
        return rv;
    }
    val = ((regval >> port) & 0x1);
    *link_status = val;
#else /* READ_LINK_STATUS_FROM_PHY */
    rv = avng_phy_link_get(unit, port, link_status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_link_status_get().. \
                                                phy read failed \n")));
        return CBX_E_FAIL;
    }
#endif /* READ_LINK_STATUS_FROM_PHY */
    /* Read admin status */
    // sal_printf("begin cbxi_port_admin_status_get(%d, %d)\n", unit,port);
    rv = cbxi_port_admin_status_get(unit, port, &admin_status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_link_status_get().. \
                                 failed to read admin state \n")));
        return rv;
    }
    *link_status &= admin_status;
    /* link status == 0, enable flush else disable */
    if ((port < 10) || (port == 14)) {
        if(*link_status == 0) {
            fval = 1;
        }
        soc_UMAC_FLUSH_CONTROLr_field_set(unit, &flush_ctrl, FLUSHf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_FLUSH_CONTROLr(unit, port, &flush_ctrl));
    }
    return CBX_E_NONE;
#endif /* EMULATION_BUILD */
}


/*
 * Function:
 *      cbxi_port_link_status_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - failure
 * Notes:
 *
 */
int
cbxi_port_link_status_set(int unit,  cbx_port_t port, int link_status ) {

#if 0
    int rv = CBX_E_NONE;
    rv = avng_phy_link_set(unit, port, link_status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_link_status_set().. \
                                                phy read failed \n")));
        return CBX_E_FAIL;
    }
#endif
    return CBX_E_UNAVAIL;
}

/*
 * Function:
 *      cbxi_port_link_status_change_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_link_status_change_get(int unit,  cbx_port_t port, int *change ) {

    return CBX_E_UNAVAIL;
}


/*
 * Function:
 *      cbxi_port_loopback_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX -  Failure
 * Notes:
 *
 */
int
cbxi_port_loopback_get(int unit, cbx_port_t port, int *loopback) {
    int rv = CBX_E_NONE;
    int phy_lb = 0;
    int mac_lb = 0;

    if(!(IS_EXTERNAL_CPU_PORT(unit, port))) {
        rv = avng_phy_loopback_get(unit, port, (uint32 *)&phy_lb);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_loopback_get().. \
                                                    phy read failed \n")));
            return CBX_E_FAIL;
        }
    }
    rv = mac_loopback_get(unit, port, &mac_lb);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_loopback_get().. \
                                                phy read failed \n")));
        return rv;
    }
    if (mac_lb) {
        *loopback = CBX_PORT_LOOPBACK_MAC;
    } else if(phy_lb) {
        *loopback = CBX_PORT_LOOPBACK_PHY;
    } else {
        *loopback = CBX_PORT_LOOPBACK_NONE;
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_loopback_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_loopback_set(int unit, cbx_port_t port, int loopback) {

    int rv = CBX_E_NONE;
    int phy_lb = 1;
    int mac_lb = 1;

    if (loopback == CBX_PORT_LOOPBACK_MAC) {
        rv = mac_loopback_set(unit, port, mac_lb);
    } else if (loopback == CBX_PORT_LOOPBACK_PHY) {
        if(IS_EXTERNAL_CPU_PORT(unit, port)) {
            return CBX_E_UNAVAIL;
        }
        rv = avng_phy_loopback_set(unit, port, phy_lb);
        if (CBX_FAILURE(rv)) { rv = CBX_E_FAIL; }
    } else {
        mac_lb = 0;
        phy_lb = 0;
        rv = mac_loopback_set(unit, port, mac_lb);
        rv = avng_phy_loopback_set(unit, port, phy_lb);
        if (CBX_FAILURE(rv)) { rv = CBX_E_FAIL; }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_loopback_set()...failed \n")));
    }
    return rv;;
}


/*
 * Function:
 *      cbxi_port_autoneg_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_autoneg_set(int unit, cbx_port_t port, int autoneg) {

    int rv = CBX_E_NONE;
#ifdef CONFIG_QSGMII
    if ((port >=8) && (port <=11)) {
        sal_printf(" Autoneg not supported on port 0x%x configured in \
                     Copper mode \n",port);
        return CBX_E_UNAVAIL;
    }
#endif

    if ((autoneg != CBX_PORT_AUTO_NEGOIATE_TRUE) &&
        (autoneg != CBX_PORT_AUTO_NEGOIATE_FALSE)) {
        return CBX_E_PARAM;
    }
    if(IS_EXTERNAL_CPU_PORT(unit, port)) {
        return CBX_E_UNAVAIL;
    }
    rv = avng_phy_autoneg_set(unit, port, autoneg);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_autoneg_set()...failed \n")));
        rv = CBX_E_FAIL;
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_autoneg_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_autoneg_get(int unit, cbx_port_t port, int *autoneg) {

    int rv = CBX_E_NONE;
    if(IS_EXTERNAL_CPU_PORT(unit, port)) {
        return CBX_E_UNAVAIL;
    }
#ifdef CONFIG_QSGMII
    if ((port >=8) && (port <=11)) {
        sal_printf(" Autoneg not supported on port 0x%x configured in \
                     Copper mode \n",port);
        return CBX_E_UNAVAIL;
    }
#endif
    rv = avng_phy_autoneg_get(unit, port, (uint32 *)autoneg);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_autoneg_get()...failed \n")));
        rv = CBX_E_FAIL;
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_admin_status_set
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_admin_status_set(int unit, cbx_port_t port_out, int value) {
    int rv = CBX_E_NONE;
    int en = 0;
    if ((value != CBX_PORT_ADMIN_ENABLE) &&
        (value != CBX_PORT_ADMIN_DISABLE)) {
        return CBX_E_PARAM;
    }
    en = value;
    rv = MAC_ENABLE_SET(port_info[unit][port_out].p_mac,
                        unit, port_out, en);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_admin_status_set()..Failed  \
                        to set port admin status\n")));
        return rv;
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_admin_status_get
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_admin_status_get(int unit, cbx_port_t port_out, int *value) {

    int rv = CBX_E_NONE;
    uint32 en = 0;
    *value = 0;
    // sal_printf("begin MAC_ENABLE_GET, unit=%d, port_out=%d)\n", unit,port_out);
    rv = MAC_ENABLE_GET(port_info[unit][port_out].p_mac,
                        unit, port_out, &en);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_admin_status_get()..Failed  \
                        to get port admin status\n")));
        return rv;
    }
    *value = en;
    return rv;
}


/*
 * Function:
 *      cbxi_port_eee_set
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_eee_set(int unit, cbx_port_t port, int eee) {

    int         rv = CBX_E_NONE;
    uint32      eee_control;
    uint32      eng_eee_control;
    uint32      mask;
    uint32_t    port_speed;
    eeecfg_t    eeecfg_entry;
    uint32_t    status;
    BOOL        enable;

    if ((eee != CBX_PORT_PHY_EEE_NONE) &&
        (eee != CBX_PORT_PHY_EEE_802_3) &&
        (eee != CBX_PORT_PHY_EEE_AUTO)) {
        return CBX_E_PARAM;
    }
    if(IS_EXTERNAL_CPU_PORT(unit, port)) {
        return CBX_E_UNAVAIL;
    }

    cbxi_port_speed_get(unit, port, &port_speed);

    rv = avng_phy_eee_set(unit, port, eee);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_eee_set()...failed \n")));
        return CBX_E_FAIL;
    }

    if (port_speed == 1000) {
        eeecfg_entry.eee_timer = 50;
    } else {
        eeecfg_entry.eee_timer = 500;
    }
    eeecfg_entry.eee_thresh = 20;

    mask = (1 << port);
    CBX_IF_ERROR_RETURN(REG_READ_NPA_NPA_EEE_CONTROLr(unit, &eee_control));
    CBX_IF_ERROR_RETURN(REG_READ_NPA_NPA_ENG_EEE_CONTROLr(unit, &eng_eee_control));
    if (eee == CBX_PORT_PHY_EEE_NONE) {
        eee_control = eee_control & (~mask);
        eng_eee_control = eng_eee_control & (~mask);
        eeecfg_entry.eee_en = 0;
        enable = FALSE;
    } else {
        eee_control = eee_control | mask;
        eng_eee_control = eng_eee_control | mask;
        eeecfg_entry.eee_en = 1;
        enable = TRUE;
    }
    soc_robo2_eeecfg_set(unit, port, &eeecfg_entry, &status);
    CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_EEE_CONTROLr(unit, &eee_control));
    CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_ENG_EEE_CONTROLr(unit, &eng_eee_control));

    if (rv == CBX_E_NONE) {
        rv = MAC_CONTROL_SET(port_info[unit][port].p_mac,
                                unit, port, SOC_MAC_CONTROL_EEE_ENABLE, enable);
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_eee_get
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_eee_get(int unit, cbx_port_t port, int *mode) {

    int rv = CBX_E_NONE;
    if(IS_EXTERNAL_CPU_PORT(unit, port)) {
        return CBX_E_UNAVAIL;
    }
    rv = avng_phy_eee_get(unit, port, (uint32 *)mode);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_eee_get()...failed \n")));
        rv = CBX_E_FAIL;
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_media_type_set
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_media_type_set(int unit, cbx_port_t port, int media) {
    int   rv = CBX_E_NONE;
    rv = avng_phy_media_type_set(unit, port, media);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbxi_port_media_type_set()...failed \n")));
        return CBX_E_FAIL;
    }
    return rv;
}

/*
 * Function:
 *      cbxi_port_intf_type_get
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
*/
int
cbxi_port_mdeia_type_get(int unit, cbx_port_t port, int *media) {

    int rv = CBX_E_NONE;
    rv = avng_phy_media_type_get(unit, port, (uint32 *)media);
    if (CBX_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_FSAL_PORT,
                 (BSL_META("FSAL API : cbxi_port_media_type_get()...failed \n")));
         rv = CBX_E_FAIL;
    }
    return rv;
}


/*
 * Function:
 *  cbxi_port_info_get
 * Purpose:
 *  Get all the parameters of a PG.
 *
 */
int
cbxi_port_info_get(int unit, cbx_port_t port_out,
                   cbxi_pgid_t lpgid, cbx_port_params_t *port_params) {

    cbxi_pgid_t    pgid;
    pgt_t          pgt_entry;
    epgt_t         epgt_entry;
    lpg2ap_t       lpg2ap_entry;
    pglct_t        pglct_entry;

    /* clear port_params structure */
    cbx_port_params_t_init(port_params);

    /* Read entry from PGT */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, lpgid, &pgt_entry));

    port_params->default_vid = pgt_entry.default_vid;
    port_params->default_tc = pgt_entry.default_tc;
    port_params->default_dp = pgt_entry.default_dp;
    port_params->sli_mode = pgt_entry.vid_src;

    /* Read entry from LPG2AP */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ap_get(unit, lpgid, &lpg2ap_entry));
    port_params->ac_policy = (cbx_cosq_threshold_t)lpg2ap_entry.adm_profile;

    /* Read entry from PGLCT */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpgid, &pgid));
    CBX_IF_ERROR_RETURN(soc_robo2_pglct_get(unit, pgid, &pglct_entry));
    port_params->learn_limit = pglct_entry.learn_limit;

    /* Read entry from EPGT */
    CBX_IF_ERROR_RETURN(soc_robo2_epgt_get(unit, lpgid, &epgt_entry));
    port_params->mtu = epgt_entry.mtu;

    if (pgt_entry.pepper & CBX_PORT_PEPPER_EN_VIRTUAL_PORT_MASK) {
        port_params->flags |= CBX_PORT_VIRTUAL_PORTS_ENABLE;
    }
    port_params->port_type = ((pgt_entry.pepper &
                                  CBX_PORT_PEPPER_PORT_TYPE_MASK)
                                   >> CBX_PORT_PEPPER_PORT_TYPE_SHIFT);
    port_params->port_index = (port_out +
                              (unit * CBX_MAX_PORT_PER_UNIT));
    if (pgt_entry.pepper & CBX_PORT_PEPPER_DROP_UNTAGGED_MASK) {
        port_params->flags |= CBX_PORT_DROP_UNTAGGED;
    }
    if (pgt_entry.pepper & CBX_PORT_PEPPER_DROP_TAGGED_MASK) {
        port_params->flags |= CBX_PORT_DROP_TAGGED;
    }
    if (pgt_entry.drop_bcast == 1) {
        port_params->flags |= CBX_PORT_DROP_BCAST;
    }
    if (pgt_entry.drop_ulf == 1) {
        port_params->flags |= CBX_PORT_DROP_ULF;
    }
    if (pgt_entry.drop_slf == 1) {
        port_params->flags |= CBX_PORT_DROP_SLF;
    }
    if (pgt_entry.drop_mlf == 1) {
        port_params->flags |= CBX_PORT_DROP_MLF;
    }

    if (pgt_entry.ctag_en == 0) {
        port_params->port_type = cbxPortTypePortVLAN;
    }

    port_params->port_group = pgid;
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_port_pgt_context_save
 * Purpose:
 *      Save the PGT table entry.
 *      This function to be called when a physical port is added to a LAG
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */
int
cbxi_port_pgt_context_save(int unit, cbx_port_t port, int lpgid) {
    int rv = CBX_E_NONE;
    rv = cbxi_port_info_get(unit, port, lpgid,
                            &(port_info[unit][port].port_param));
    return rv;
}


/*
 * Function:
 *      cbxi_port_pgt_context_recover
 * Purpose:
 *      Recover the PGT table entry.
 *      This function to be called when a physical port is removed from a LAG
 *
 * Parameters:
 *
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX - Error
 * Notes:
 *
 */

int
cbxi_port_pgt_context_recover(int unit, cbx_port_t port, int lpgid) {

    int rv = CBX_E_NONE;

    rv = cbxi_port_info_set(unit, port, lpgid, &port_info[unit][port].port_param);
    if (rv == CBX_E_NONE) {
        rv = cbxi_port_egress_info_set(unit, port, lpgid,
                                       &port_info[unit][port].port_param);
    }
    return rv;
}

/*
 * Function:
 *  cbxi_port_pg_update
 * Purpose:
 *  Update LPG2PG and PG2LPG entry
 *
 */
int
cbxi_port_pg_update(int unit, cbx_port_t lpgid, cbx_port_t pgid) {
    /* Update LPG2PGID */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2pg_update(unit, lpgid, pgid));
    /* Update PG2LPGID */
    CBX_IF_ERROR_RETURN(cbxi_robo2_pg2lpg_update(unit, pgid, lpgid));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_port_phy_info
 * Purpose:
 *  Dump the phy drivers attached to the port
 *
 */
void
cbxi_port_phy_info() {
    int i = 0;
    int num_units = 1;
    int unit = 0;
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    for (unit = 0; unit < num_units; unit++) {
        for (i = 0; i < CBX_MAX_PORT_PER_UNIT; i++) {
            if (CBX_PBMP_MEMBER(pp_created_pbmp[unit], i)) {
                avng_show_phy_info(unit, i);
            }
        }
    }
    return;
}
/*
 * Function:
 *  cbxi_port_display_eye_scan
 * Purpose:
 *  Dump eye_scan for port ports 12/13
 *
 */
int
cbxi_port_display_eye_scan(cbx_portid_t portid) {
    cbx_port_t port_out;
    cbxi_pgid_t pgid;
    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;

   /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &pgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_display_eye_scan()..invalid port\n")));
        return rv;
    }
    if (!(CBX_PBMP_MEMBER(pp_created_pbmp[unit], port_out))) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_display_eye_scan()..No \
                                  port exists for the index passed \n")));
        return CBX_E_PORT;
    }
    if((port_out < 12) || (port_out > 13)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_port_display_eye_scan()..Eye scan \
                        supported only on 10G ports(12/13) \n")));
        return CBX_E_PORT;
    }
    rv = avng_phy_display_eye_scan(unit, port_out);

    return rv;
}
/**************************************************************************
 *                 PORT FSAL API IMPLEMENTATION                           *
 **************************************************************************/

/*
 * Function:
 *      cbx_port_params_t_init
 * Purpose:
 *      Initialize Port entry parameters.
 * Parameters:
 *      port_params - Pointer to port parameter structure.
 * Returns:
 *      None
 */

void
cbx_port_params_t_init(cbx_port_params_t *port_params) {
    if ( NULL != port_params ) {
        sal_memset(port_params, 0, sizeof (cbx_port_params_t));
        if (otp_flags & SOC_OTP_FLAG_ARL_SIZE_8K) {
             port_params->learn_limit = CBXI_L2_ARL_NUM_OF_ENTRIES_8K;
        } else {
            port_params->learn_limit = CBXI_L2_ARL_NUM_OF_ENTRIES_16K;
        }
        port_params->default_vid = CBX_VLAN_DEFAULT;
    }
    return;
}


/**
 * Function:
 *      cbx_port_isolation_get
 * Purpose:
 *      This routine is used to get the isolation group on a port.
 *
 * @param portid   PORT Identifier
 * @parm  port_iso Isolation group
 *
 * @return CBX_E_XXX Failure
 * @retval CBX_E_NONE on success
 */
int
cbx_port_isolation_get(cbx_portid_t port, cbx_port_isolation_t *port_iso) {

    lpg2ig_t igt_entry;
    int rv = CBX_E_NONE;
    int unit = CBX_AVENGER_PRIMARY;
    cbx_port_t port_out;
    cbxi_pgid_t lpg;
    rv = cbxi_robo2_port_validate(port, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_isolation_get()... \
                                      Port validation failed \n")));
        return CBX_E_PARAM;
    }
    /* Read entry from LPG2IG */
    rv = soc_robo2_lpg2ig_get(unit, lpg, &igt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_isolation_get()..LPG2IG  \
                                                           read failed\n")));
            return rv;

    }
    port_iso->group = (cbx_port_isolation_group_t)(igt_entry.ig &
                                                   CBX_PORT_ISO_MAP_MASK);
    return rv;
}



/**
 * Function:
 *     cbx_port_isolation_set
 * Purpose:
 *      This routine is used to set the isolation group on a port.
 *
 * @param portid   PORT Identifier
 * @parm  port_iso Isolation group
 *
 * @return return code
 * @return return CBX_E_XXX code
 * @retval CBX_E_NONE on success
 */
int
cbx_port_isolation_set(cbx_portid_t port, cbx_port_isolation_t *port_iso) {

    int rv = CBX_E_NONE;
    lpg2ig_t lpg2ig_entry;
    gspg2ig_t gspg2ig_entry;
    uint32 status  = 0;
    int unit = CBX_AVENGER_PRIMARY;
    cbx_port_t port_out;
    cbxi_pgid_t lpg;
    cbxi_pgid_t gspg;

    rv = cbxi_robo2_port_validate(port, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_isolation_set()..Port validation \
                                  failed \n")));
        return CBX_E_PARAM;
    }
    if (port_iso->group >= cbxPortIsolationMax) {
        return CBX_E_PARAM;
    }
    lpg2ig_entry.ig = (uint8)(port_iso->group);

    /* write entry in LPG2IG */
    rv = soc_robo2_lpg2ig_set( unit, lpg, &lpg2ig_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_isolation_set()..LPG2IG  \
                                                           write failed\n")));
        return rv;

    }
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gspg));
    gspg2ig_entry.ig = (uint8)(port_iso->group);

    /* write entry in GSPG2IG */
    rv = soc_robo2_gspg2ig_set(CBX_AVENGER_PRIMARY, gspg,
                                                     &gspg2ig_entry, &status);
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        rv += soc_robo2_gspg2ig_set(CBX_AVENGER_SECONDARY, gspg,
                                                     &gspg2ig_entry, &status);
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_isolation_set()..GSPG2IG write failed\n")));
        return rv;
    }
    return rv;
}


/**
 * Function:
 *      cbx_port_info_get  - PORT Information get
 * Purpose :
 *      This routine is used to get the parameters of a Port.
 *
 * @param portid   PORT Identifier
 * @parm  port_params Port parameters, including:
 * - port type
 * - drop tagged packets
 * - drop untagged packets
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int
cbx_port_info_get(cbx_portid_t       portid,
                  cbx_port_params_t *port_params) {

    cbx_port_t     port_out;
    cbxi_pgid_t    lpgid;
    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;

    /* Validate the port number passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &lpgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_get()..Invalid \
                                  port number \n")));
        return rv;
    }
    rv = cbxi_port_info_get(unit, port_out, lpgid, port_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_get()..failed to \
                                  read port info \n")));
        return rv;
    }
    return rv;
}



/**
 * Function:
 *   cbx_port_info_set - PORT Information set
 * Purpose :
 *   This routine is used to set the parameters of a Port.
 *
 * @param portid   PORT Identifier
 * @parm  port_params Port parameters, including:
 * - port type
 * - drop tagged packets
 * - drop untagged packets
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_port_info_set(cbx_portid_t       portid,
                  cbx_port_params_t *port_params) {

    cbx_port_t     port_out;
    cbxi_pgid_t    lpgid;
    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_lag_info_t lag_info;

    /* Validate the port number passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &lpgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..Invalid \
                                  port number \n")));
        return rv;
    }
    if ( port_params->learn_limit > CBX_PORT_MAX_LEARN_LIMIT) {
        return CBX_E_PARAM;
    }
    if ( port_params->mtu > CBX_PORT_MAX_MTU) {
        return CBX_E_PARAM;
    }
#ifdef CONFIG_CASCADED_MODE
    if (port_params->port_type == cbxPortTypeCascade) {
        CBX_IF_ERROR_RETURN(cbxi_cascade_port_add(portid));
    }
#endif
    rv = cbxi_port_info_set(unit, port_out, lpgid, port_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..Failed \
                                  to update PGT for the port \n")));
        return rv;
    }
#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        if (port_params->port_type == cbxPortTypeExtenderUpstream) {
            CBX_IF_ERROR_RETURN(cbxi_pe_uplink_port_set(portid));
        } else if (port_params->port_type == cbxPortTypeExtenderCascade) {
            CBX_IF_ERROR_RETURN(cbxi_pe_mode_set(cbxiPEModeTransit));
            CBX_IF_ERROR_RETURN(cbxi_pe_csd_port_init(portid));
        } else if (port_params->port_type == cbxPortTypeExtender) {
            CBX_IF_ERROR_RETURN(cbxi_pe_ext_port_init(portid));
        }
    }
#endif /* CONFIG_PORT_EXTENDER */

    if (CBX_PORTID_IS_TRUNK(portid)) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        if (CBX_PBMP_NOT_NULL(lag_info.lag_pbmp[CBX_AVENGER_PRIMARY])) {
            CBX_PBMP_ITER(lag_info.lag_pbmp[CBX_AVENGER_PRIMARY], port_out) {
                rv = cbxi_port_egress_info_set(unit, port_out, lpgid, port_params);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_PORT,
                        (BSL_META("FSAL API : cbx_port_info_set()..Failed \
                                   to update PET for the port %d\n"), port_out));
                    return rv;
                }
            }
        }
        if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
                 (CBX_PBMP_NOT_NULL(lag_info.lag_pbmp[CBX_AVENGER_SECONDARY]))) {
            CBX_PBMP_ITER(lag_info.lag_pbmp[CBX_AVENGER_SECONDARY], port_out) {
                rv = cbxi_port_egress_info_set(unit, port_out, lpgid, port_params);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_PORT,
                        (BSL_META("FSAL API : cbx_port_info_set()..Failed \
                                   to update PGT for the port %d\n"), port_out));
                    return rv;
                }
            }
        }
    } else {
        rv = cbxi_port_egress_info_set(unit, port_out, lpgid, port_params);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbx_port_info_set()..Failed \
                           to update PGT for the port %d\n"), port_out));
            return rv;
        }
    }

    return rv;
}

/**
 * Function:
 *    cbx_port_create
 * Purpose :
 *    Create a port
 * This routine is used to create a port interface.
 * It allocates an entry in the Port Group Table (PGT)
 * It sets this IPP Table:
 *  - Physical Port to Port Group (PP2PG) table
 * It sets these PQM Tables:
 *  - Port Group to Physical Port Fan-Out Vector (LPG2PPFOV) table
 *  - Physical Port to Port Group Identifier (PP2PG) table
 *  - Squelch Vector Table (SVT)
 * These tables are also used by the LAG module.
 * The set of Ports and LAGs make up the Global Port Number space
 *
 * port_params   (IN)  Port parameters.
 * portid        (OUT) Handle of the port created
 *                            CBX_PORT_INVALID: Port could not be created
 *
 * return code
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */

int
cbx_port_create(cbx_port_params_t *port_params,
                cbx_portid_t      *portid) {

    ipp_pp2lpg_t   pp2lpg_entry;
#ifdef CONFIG_PORT_EXTENDER
    lpg2pg_t       lpg2pg_entry;
#endif
    cbx_port_t     port_out;
    cbxi_pgid_t    lpgid;
    cbxi_pgid_t    gpgid;
    uint32_t       flag = CBXI_PORT_TABLE_ENTRY_ADD;
    uint32         status = 0;
    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
#ifndef EMULATION_BUILD
    uint32_t       link_status = 0;
#endif

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(
                        port_params->port_index, &port_out, &lpgid, &unit);
    /* pgid contains default value. Neglect pgid value */
    if (CBX_FAILURE(rv) && (rv != CBX_E_NOT_FOUND)) {
        return CBX_E_PORT;
    }
    if (port_params->port_type >= cbxPortTypeMax) {
        return CBX_E_PARAM;
    }
    if (port_params->learn_limit > CBX_PORT_MAX_LEARN_LIMIT) {
        return CBX_E_PARAM;
    }
    if (port_params->mtu > CBX_PORT_MAX_MTU) {
        return CBX_E_PARAM;
    }

    /* Check if this port is already created */
    if (CBX_PBMP_MEMBER(pp_created_pbmp[unit], port_out)) {
        LOG_WARN(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_create()..port is already \
                                  created \n")));
        return CBX_E_EXISTS;
    } else {
#ifdef CONFIG_PORT_EXTENDER
        if (unit && (port_out == CBX_PE_LB_PORT)) {
            /* Allocate same LPG, PG as Primary LB port */
            CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(
                        CBX_AVENGER_PRIMARY, port_out, &pp2lpg_entry));
            lpgid = pp2lpg_entry.pgid;

            CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(
                        CBX_AVENGER_PRIMARY, lpgid, &lpg2pg_entry));
            gpgid = lpg2pg_entry.pg;
            CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(
                        unit, lpgid, &lpg2pg_entry, &status));
        } else {
            /* Allocate LPG and PG for this port */
            CBX_IF_ERROR_RETURN(cbxi_robo2_pp2lpg_slot_get(unit, &lpgid));

            pp2lpg_entry.pgid = lpgid;
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2pg_slot_get(unit, &gpgid));
        }
#else
        /* Allocate LPG and PG for this port */
        CBX_IF_ERROR_RETURN(cbxi_robo2_pp2lpg_slot_get(unit, &lpgid));

        pp2lpg_entry.pgid = lpgid;
        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2pg_slot_get(unit, &gpgid));
#endif
        rv = cbxi_port_pg_update(unit, lpgid, gpgid);
        if (CBX_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_create()..Failed to update \
                                  PP2LPG entry \n")));
           return rv;
        }
        /* Update PP2LPG */
        rv = soc_robo2_pqm_pp2lpg_set(unit, port_out, (pqm_pp2lpg_t*)&pp2lpg_entry, &status);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_create()..Failed to update \
                                  PQM_PP2LPG entry \n")));
            return rv;
        }
        rv = soc_robo2_ipp_pp2lpg_set(unit, port_out, &pp2lpg_entry, &status);
        if (CBX_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_create()..Failed to update \
                                  IPP_PP2LPG entry \n")));
           return rv;
        }

        /* Update SVT */
        rv = cbxi_robo2_svt_update(unit, port_out, flag);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_create()..Failed to update \
                                  SVT entry \n")));
            return rv;
        }

        /* Update LPG2PPFOV */
        rv = cbxi_robo2_lpg2ppfov_update(unit, lpgid,
                                         port_out, flag);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_create()..Failed to update \
                                  LPG2PPFOV entry \n")));
            return rv;
        }
        /* Update BMU SPG PP Config */
        rv = cbxi_robo2_spg2ppconfig_update(unit, lpgid,
                                         port_out, flag);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
             (BSL_META("FSAL API : cbx_port_create()..Failed to update \
                                  BMU SPG PP Config entry \n")));
            return rv;
        }
    }

    rv = cbxi_port_info_set(unit, port_out, lpgid, port_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_create()..Failed \
                        to set port parameters as part of port create \n")));
        return rv;
    }
    rv = cbxi_port_egress_info_set(unit, port_out, lpgid, port_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_info_set()..Failed \
                       to update PET for the port %d\n"), port_out));
        return rv;
    }
    /* Form portid for this port */
    CBX_PORTID_LOCAL_SET(*portid, port_params->port_index);

    /* Update port created pbmp */
    CBX_PBMP_PORT_ADD(pp_created_pbmp[unit], port_out);

    if(IS_LOCAL_CPU_PORT(unit, port_out)) {
        return rv;
    }
#ifdef CONFIG_WEB_SERVER
    /* Ensure MAC is disabled until the configuration is loaded later */
    rv = MAC_ENABLE_SET(port_info[unit][port_out].p_mac,
                                unit, port_out, FALSE);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_create()..Failed  \
                        to disable MAC as part of port create \n")));
        return rv;
    }
#else
    /* Enable MAC */
    rv = MAC_ENABLE_SET(port_info[unit][port_out].p_mac,
                                unit, port_out, TRUE);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_create()..Failed  \
                        to enable MAC as part of port create \n")));
        return rv;
    }
#endif
#ifndef EMULATION_BUILD
    rv = cbxi_port_link_status_get(unit, port_out, &link_status);
#endif

    return rv;
}

/**
 * Function:
 *    cbx_port_destroy
 * Purpose :
 *    Destroy a port previously created by cbx_port_create()
 * param :
 *    portid   (IN) Handle of the port to be destroyed
 * return code:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 *
 * note: This routine destroys a previously created Port.
 */

int
cbx_port_destroy(cbx_portid_t portid) {

    ipp_pp2lpg_t       pp2lpg_entry;
    cbx_port_params_t  port_params;
    cbx_port_t         port_out;
    cbxi_pgid_t        lpgid;
    uint32             status  = 0;
    int                unit = CBX_AVENGER_PRIMARY;
    int                rv = CBX_E_NONE;
    uint32_t           flag = CBXI_PORT_TABLE_ENTRY_REMOVE;

    if ((CBX_PORTID_IS_TRUNK(portid)) || (CBX_PORTID_IS_VLAN_PORT(portid))) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_destroy()..Invalid  \
                               portid passed \n")));
        return CBX_E_PORT;

    }
    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &lpgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_destroy()..Invalid  \
                               portid passed \n")));
        return CBX_E_PARAM;
    }
    /* Disable MAC */
    if(IS_LOCAL_CPU_PORT(unit, port_out)) {
        return rv;
    }
    rv = MAC_ENABLE_SET(port_info[unit][port_out].p_mac,
                        unit, port_out, FALSE);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_destroy()..Failed  \
                        to disable MAC as part of port destroy \n")));
        return rv;
    }

    /* Get LPG for this port from PP2LPG */
    rv = soc_robo2_ipp_pp2lpg_get(unit, port_out, &pp2lpg_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_destroy()..Failed \
                              to get PGT for the port \n")));
        return rv;
    }

    /* Update SVT */
    rv = cbxi_robo2_svt_update(unit, port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
         (BSL_META("FSAL API : cbx_port_destroy()..Failed to update \
                              SVT entry \n")));
        return rv;
    }

    /* Update PP2LPG */
    rv = cbxi_robo2_pqm_pp2lpg_update(unit, lpgid, port_out);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
         (BSL_META("FSAL API : cbx_port_destroy()..Failed to update \
                              PP2LPG entry \n")));
        return rv;
    }

    /* Update PG2PPFOV  */
    rv = cbxi_robo2_lpg2ppfov_update(unit, pp2lpg_entry.pgid, port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
         (BSL_META("FSAL API : cbx_port_destroy()..Failed to update \
                              LPG2PPFOV entry \n")));
         return rv;
    }
    /* Update SPGPPBMP config  */
    rv = cbxi_robo2_spg2ppconfig_update(unit, pp2lpg_entry.pgid, port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
         (BSL_META("FSAL API : cbx_port_destroy()..Failed to update \
                              SPGPPBMP config entry \n")));
         return rv;
    }
    /* Update PP2LPG entry */
    rv = soc_robo2_ipp_pp2lpg_set(unit, port_out, &pp2lpg_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
         (BSL_META("FSAL API : cbx_port_destroy()..Failed to update  \
                              PP2LPG entry \n")));
        return rv;
    }

    /* Update PGT */
    cbx_port_params_t_init(&port_params);
    rv = cbxi_port_info_set(unit, port_out, pp2lpg_entry.pgid, &port_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_destroy()..Failed  \
                        to set port parameters as part of port destroy \n")));
        return rv;
    }
    /* Free LPG and PG */
    rv = cbxi_lag_pg_context_clear(unit, lpgid, pp2lpg_entry.pgid);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_destroy()..Failed  \
                        to set clear LAG PG context for deleted port \n")));
        return rv;
    }

    /* Update port created pbmp */
    CBX_PBMP_PORT_REMOVE(pp_created_pbmp[unit], port_out);

    return CBX_E_NONE;
}

/**
 * Function:
 *   cbx_port_lookup
 * Purpose :
 *   This routine is used to get the Port Identifier matching lookup criteria.
 *
 * param :
 *    lookup     (IN)  port lookkup criteria
 *    portid     (OUT) PORT Identifier
 *
 * return code :
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_port_lookup(cbx_port_lookup_t *lookup,
                cbx_portid_t      *portid) {

    cbx_port_t     port_out;
    cbxi_pgid_t    lpgid;
    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(
                        lookup->port_index, &port_out, &lpgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_lookup()..Invalid  \
                                  port index passed : %d\n"), lookup->port_index));
        return CBX_E_NOT_FOUND;
    }
    /* Form portid for this port */
    CBX_PORTID_LOCAL_SET(*portid, lookup->port_index);
    return CBX_E_NONE;
}


/*
 * Function:
 *      cbx_port_init
 * Purpose:
 *      Initialize Port interface layer for specified SOC device.
 * Parameters:
 *      None.
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX -  Error
 * Notes:
 *      All ports set in disabled state.
 */

int
cbx_port_init(void) {
    int rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_init()..\n")));

    CBX_PBMP_CLEAR(pp_created_pbmp[CBX_AVENGER_PRIMARY]);
#ifdef CONFIG_CASCADED_MODE
    cascade_ports[CBX_AVENGER_PRIMARY] = CBXI_PGID_INVALID;
#endif
    if ( SOC_IS_CASCADED(0) ) {
        CBX_PBMP_CLEAR(pp_created_pbmp[CBX_AVENGER_SECONDARY]);
#ifdef CONFIG_CASCADED_MODE
        cascade_ports[CBX_AVENGER_SECONDARY] = CBXI_PGID_INVALID;
#endif
    }

    /* Enable all the tables related to port module */
    rv = cbxi_port_table_init();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_init()..Table init failed\n")));
        return rv;
    }
    /* TBD: PBMP_ALL yet to be defined by switch module.
     * Assuming all 16 ports */
    PBMP_ALL(0) = 0x3003;  //default 0xFFFF 0x300F
    PBMP_XL_ALL(0) = 0x3000;  //default 0x3C00
    PBMP_GE_ALL(0) = 0x0003;  //default 0x03ff 0x000F
    PBMP_FE_ALL(0) = 0x0;
    PBMP_CASCADE(0) = 0x0;

    if ( SOC_IS_CASCADED(0) ) {
        PBMP_ALL(1) = 0x3003;
#if CONFIG_PORT_EXTENDER
        /* Include Loopback port */
        PBMP_ALL(1) |= (1 << CBX_PE_LB_PORT);
#endif
        PBMP_XL_ALL(1) = 0x3000;  //default 0x3C00;
        PBMP_GE_ALL(1) = 0x0003;  //default 0x03ff; 0x000F
        PBMP_FE_ALL(1) = 0x0;
        PBMP_CASCADE(1) = 0x0;
    }

    /* Remove unsupported ports based on OTP */
    if (otp_flags & SOC_OTP_FLAG_GPHY_0_3_DIS) {
        PBMP_ALL(0) &= ~(0x000F);
        PBMP_GE_ALL(0) &= ~(0x000F);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x000F);
            PBMP_GE_ALL(1) &= ~(0x000F);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_GPHY_4_7_DIS) {
        PBMP_ALL(0) &= ~(0x00F0);
        PBMP_GE_ALL(0) &= ~(0x00F0);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x00F0);
            PBMP_GE_ALL(1) &= ~(0x00F0);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_XFI_13_DIS) {
        PBMP_ALL(0) &= ~(0x2000);
        PBMP_XL_ALL(0) &= ~(0x2000);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x2000);
            PBMP_XL_ALL(1) &= ~(0x2000);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_XFI_12_DIS) {
        PBMP_ALL(0) &= ~(0x1000);
        PBMP_XL_ALL(0) &= ~(0x1000);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x1000);
            PBMP_XL_ALL(1) &= ~(0x1000);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_RGMII_DIS) {
        PBMP_ALL(0) &= ~(0x4000);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x4000);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_QSGMII_DIS) {
        PBMP_ALL(0) &= ~(0x0F00);
        PBMP_GE_ALL(0) &= ~(0x0300);
        PBMP_XL_ALL(0) &= ~(0x0C00);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x0F00);
            PBMP_GE_ALL(1) &= ~(0x0300);
            PBMP_XL_ALL(1) &= ~(0x0C00);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_XLPORT_DISABLE) {
        PBMP_ALL(0) &= ~(0x3C00);
        PBMP_XL_ALL(0) &= ~(0x3C00);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x3C00);
            PBMP_XL_ALL(1) &= ~(0x3C00);
        }
    }
    if (otp_flags & SOC_OTP_FLAG_PORT_9_DISABLE) {
        PBMP_ALL(0) &= ~(0x0200);
        PBMP_GE_ALL(0) &= ~(0x0200);
        if ( SOC_IS_CASCADED(0) ) {
            PBMP_ALL(1) &= ~(0x0200);
            PBMP_GE_ALL(0) &= ~(0x0200);
        }
    }

    /* Initialize mac */
    if (otp_flags & SOC_OTP_FLAG_XLPORT_DISABLE) {
        rv = soc_mac_pre_init(1);
    } else {
        rv = soc_mac_pre_init(0);
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_init()..mac probe failed\n")));
        return rv;
    }
#ifndef EMULATION_BUILD /* TBD: Commented to do port init in emulation */
    /* Probe phy and initialize */
    rv = soc_phy_probe();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_init()..phy probe failed\n")));
        return rv;
    }
#endif /* !EMULATION_BUILD */
    /* Initialize mac */
    if (otp_flags & SOC_OTP_FLAG_XLPORT_DISABLE) {
        rv = soc_mac_init(1);
    } else {
        rv = soc_mac_init(0);
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_init()..mac probe failed\n")));
        return rv;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *      cbx_port_phy_get
 * Purpose:
 *      Get the value of a phy register
 * Parameters:
 *
 * Returns:
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 * Notes:
 *
 */
int
cbx_port_phy_get( cbx_portid_t portid,
                      uint32_t flags,
                      uint32_t phy_reg_addr,
                      uint32_t *phy_data ) {

#ifdef EMULATION_BUILD
    return CBX_E_UNAVAIL;
#endif /* EMULATION_BUILD */
    cbx_port_t port_out;
    cbxi_pgid_t pgid;
    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;

   /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &pgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_phy_set()..invalid port\n")));
        return rv;
    }
    rv = avng_phy_reg_read(unit, port_out, phy_reg_addr, phy_data);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_phy_set()..unable to read  \
                                  from specified phy register\n")));
        return (rv == SYS_ERR_PARAMETER) ? CBX_E_PARAM : CBX_E_FAIL;
    }

    return CBX_E_NONE;
}


/*
 * Function:
 *      cbx_port_phy_set
 * Purpose:
 *      Set the value of a phy register
 * Parameters:
 *
 * Returns:
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 * Notes:
 *
 */
int
cbx_port_phy_set( cbx_portid_t portid,
                  uint32_t flags,
                  uint32_t phy_reg_addr,
                  uint32_t phy_data ) {

#ifdef EMULATION_BUILD
    return CBX_E_UNAVAIL;
#endif /* EMULATION_BUILD */
    cbx_port_t port_out;
    cbxi_pgid_t pgid;
    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;

   /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &pgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_phy_set()..invalid port\n")));
        return rv;
    }
    rv = avng_phy_reg_write(unit, port_out, phy_reg_addr, phy_data);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_phy_set()..unable to write  \
                                  to specified phy register\n")));
        return (rv == SYS_ERR_PARAMETER) ? CBX_E_PARAM : CBX_E_FAIL;
    }
    return CBX_E_NONE;
}

/**
 * Function:
 *     PORT attribute get
 * Purpose:
 *     This routine is used to get an attribute value on a port.
 *
 * @param portid     (IN)  PORT Identifier
 * @parm  attr       (OUT) attribute value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int
cbx_port_attribute_get(cbx_portid_t       port,
                       cbx_port_attr_t  attr,
                       uint32_t         *value) {

    cbx_port_t port_out;
    cbxi_pgid_t pgid;
    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;

   /* validate the port index passed */
    rv = cbxi_robo2_port_validate(port, &port_out, &pgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_attribute_get()..invalid port\n")));
        return rv;
    }
    if(IS_LOCAL_CPU_PORT(unit, port_out)) {
        if (attr != cbxPortAttrLinkStatus) {
            LOG_ERROR(BSL_LS_FSAL_PORT,
                (BSL_META("FSAL API : cbx_port_attribute_get()..No MAC/PHY \
                                  on local CPU port \n")));
            return CBX_E_PORT;
        }
    }

    switch(attr) {
        /** Port duplex mode */
        case cbxPortAttrDuplex:
            rv = cbxi_port_duplex_get(unit, port_out, value);
            break;

        /** Port speed */
        case cbxPortAttrSpeed:
            rv = cbxi_port_speed_get(unit, port_out, value);
            break;

        /** Port MTU - frame max size */
        case cbxPortAttrMTU:
            rv = cbxi_port_frame_max_get(unit, port_out, value);
            break;

        /** Port link status */
        case cbxPortAttrLinkStatus:
            rv = cbxi_port_link_status_get(unit, port_out, value);
            break;

        case cbxPortAttrAutoNegotiate:
            rv = cbxi_port_autoneg_get(unit, port_out, (int *)value);
            break;

        case cbxPortAttrAdmin:
            rv = cbxi_port_admin_status_get(unit, port_out, (int *)value);
            break;

        case cbxPortPhyEEE:
            if (port_out >= 8) {
                return CBX_E_UNAVAIL;
            }
            rv = cbxi_port_eee_get(unit, port_out, (int *)value);
            break;

        default:
            break;
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_attribute_get()..Attribute get \
                                  failed\n")));
    }
    return rv;
}

/**
 * Function:
 *      cbx_port_attribute_set
 * Purpose:
 *      This routine is used to set an attribute value on a port.
 *
 * @param portid     (IN)  PORT Identifier
 * @parm  attr       (IN)  attribute type
 * @parm  value      (OUT) attribute value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int
cbx_port_attribute_set(cbx_portid_t       port,
                       cbx_port_attr_t  attr,
                       uint32_t         value) {

    cbx_port_t port_out;
    cbxi_pgid_t pgid;
    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;

   /* validate the port index passed */
    rv = cbxi_robo2_port_validate(port, &port_out, &pgid, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_attribute_set()..invalid port\n")));
        return rv;
    }
    if (!(CBX_PBMP_MEMBER(pp_created_pbmp[unit], port_out))) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_attribute_set()..No \
                                  port exists for the index passed \n")));
        return CBX_E_PORT;
    }
    if(IS_LOCAL_CPU_PORT(unit, port_out)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_attribute_set()..No MAC/PHY \
                               on local CPU port \n")));
        return CBX_E_PORT;
    }

    switch(attr) {
        /** Port duplex mode */
        case cbxPortAttrDuplex:
            rv = cbxi_port_duplex_set(unit, port_out, value);
            break;

        /** Port speed */
        case cbxPortAttrSpeed:
            if(value == 10000) {
                rv = cbxi_port_speed_set(unit, port_out, 1000);
            }
            rv = cbxi_port_speed_set(unit, port_out, value);
            break;

        /** Port MTU - frame max size */
        case cbxPortAttrMTU:
            rv = cbxi_port_frame_max_set(unit, port_out, value);
            break;

        /** Port link status */
        case cbxPortAttrLinkStatus:
            rv = CBX_E_UNAVAIL;
            break;

        case cbxPortAttrAutoNegotiate:
            rv = cbxi_port_autoneg_set(unit, port_out, value);
            break;

        case cbxPortAttrAdmin:
            rv = cbxi_port_admin_status_set(unit, port_out, value);
            break;

        case cbxPortPhyEEE:
            if (port_out >= 8) {
                return CBX_E_UNAVAIL;
            }
            rv = cbxi_port_eee_set(unit, port_out, value);
            break;

        case cbxPortPhyMediaType:
            if ((port_out == 12) || (port_out == 13)) {
                rv = cbxi_port_media_type_set(unit, port_out, value);
            } else {
                return CBX_E_UNAVAIL;
            }
            break;

        default:
            break;
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbx_port_attribute_set()..Attribute set \
                           failed\n")));
    }
    return rv;
}
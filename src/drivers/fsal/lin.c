/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     lin.c
 * * Purpose:
 * *     Defines internal functions to handle Logincal Interfaces.
 * *
 * */

#include <fsal/error.h>
#include <fsal_int/lin.h>
#include <fsal_int/slic.h>
#include <bsl_log/bsl.h>
#include <soc/drv.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <shared/bitop.h>

static SHR_BITDCL  *lin_bitmap;

/*
 * Function:
 *  cbxi_lin_table_init
 * Purpose:
 *  Global Initialization for all logical interface related tables.
 *
 */
STATIC int
cbxi_lin_table_init( void )
{
    uint32        num_units = 1;
    int           unit = 0;

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        /* Enable and reset DLI2LDPG */
        CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_reset(unit));

        /* Enable and reset LITET */
        CBX_IF_ERROR_RETURN(soc_robo2_litet_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_litet_reset(unit));

        /* Enable and reset LIN2VSI */
        CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_reset(unit));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lin_init
 * Purpose:
 *  Initialize logical interface bitmap and tables.
 *
 */
int cbxi_lin_init( void )
{
    int           alloc_size=0;

    CBX_IF_ERROR_RETURN(cbxi_lin_table_init());

    /* Allocate lin bitmap */
    alloc_size = SHR_BITALLOCSIZE(CBXI_LOGICAL_IF_MAX);
    if (lin_bitmap == NULL) {
        lin_bitmap = sal_alloc(alloc_size, "LIN-bitmap");
    }

    if (lin_bitmap == NULL) {
        return CBX_E_MEMORY;
    }
    sal_memset(lin_bitmap, 0, alloc_size);

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lin_slot_get
 * Purpose:
 *  Get free ID to be used for lin entry.
 *
 */
int cbxi_lin_slot_get(int *linid )
{
    int i=0;
    int rv = CBX_E_FULL;

    /* Find free lin slot, start slot from 1, slot 0 is not used */
    for (i = CBXI_LOGICAL_IF_MIN; i < CBXI_LOGICAL_IF_MAX; i++) {
        if (!(SHR_BITGET(lin_bitmap, i))) {
            SHR_BITSET(lin_bitmap, i);
            *linid = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return rv;
}

/*
 * Function:
 *  cbxi_lin_slot_release
 * Purpose:
 *  Release lin entry.
 *
 */
int cbxi_lin_slot_release(int  linid )
{
    litet_t ent_litet;
    dli2ldpg_t  ent_dli2ldpg;
    uint32_t status = 0;
    uint32_t num_units = 1;
    int      unit = 0;

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    /* free given lin slot */
    SHR_BITCLR(lin_bitmap, linid);

    ent_litet.tx_ena = 0;
    for(unit = 0; unit < num_units; unit++) {
        /* Clear dli2ldpg mapping */
        CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(unit, linid, &ent_dli2ldpg));
        ent_dli2ldpg.ldpgid = 0;/* Clear lpg entries */
        CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_set(unit, linid,
                                                &ent_dli2ldpg, &status));
        CBX_IF_ERROR_RETURN(soc_robo2_litet_set(unit, linid,
                                             &ent_litet, &status));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lin_dest_get
 * Purpose:
 *  Obtain the destination port group for a given logical interface.
 *
 */
int cbxi_lin_dest_get(int unit, int linid, cbxi_pgid_t *lpgid)
{
    dli2ldpg_t  ent_dli2ldpg;
    litet_t     ent_litet;

    CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(unit, linid, &ent_dli2ldpg));
    CBX_IF_ERROR_RETURN(soc_robo2_litet_get(unit, linid, &ent_litet));

    if (!ent_litet.tx_ena) {
        *lpgid = CBXI_PGID_INVALID;
    } else {
        *lpgid = ent_dli2ldpg.ldpgid;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lin_dest_set
 * Purpose:
 *  Map lin to a destination port.
 *
 */
int cbxi_lin_dest_set(int unit, int linid, cbxi_pgid_t lpgid)
{
    uint32_t    status = 0;
    dli2ldpg_t  ent_dli2ldpg;
    litet_t     ent_litet;

    CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(unit, linid, &ent_dli2ldpg));
    ent_dli2ldpg.ldpgid = lpgid;
    CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_set(unit, linid,
                                            &ent_dli2ldpg, &status));
    /* Enable tx on this Logical interface */
    ent_litet.tx_ena = TRUE;
    CBX_IF_ERROR_RETURN(soc_robo2_litet_set(unit, linid, &ent_litet, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_ptp_lin_source_set
 * Purpose:
 *  Map source port to lin and vsi.
 *
 */
int cbxi_ptp_lin_source_set(int unit, int linid, cbxi_pgid_t lpgid,
                            uint32_t vsiid, uint8_t slic_type)
{
    uint32_t    status = 0;
    pbmp_t pg_map = 0;
    fpslict_t ent_fpslict;
    lin2vsi_t ent_lin2vsi;
    cbxi_slicid_t slicid = SLICID_PTP_ETH_PP0;
    cbxi_slic_rule_id_t rule_idx = SLIC_PTP_ETH_PORT_0;

    if (slic_type == 1) {
        /* SLIC for Ethernet PTP */
        rule_idx = (SLIC_PTP_ETH_PORT_0 + lpgid);
        slicid = (SLICID_PTP_ETH_PP0 + lpgid);
    } else if (slic_type == 2) {
        /* SLIC for UDP PTP */
        rule_idx = (SLIC_PTP_UDP_PORT_0 + lpgid);
        slicid = (SLICID_PTP_UDP_PP0 + lpgid);
    }
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, rule_idx, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, lpgid);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, rule_idx, pg_map));

#if defined(CONFIG_BC_MODE1) && defined(CBX_PORT_ECPU)
    // ECPU Port use diff: PV type sli=0x20FFE and vsi=4094, dst_key=0
    if (lpgid == CBX_PORT_ECPU) {
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, slicid, &ent_fpslict));
        // SPG set to 0, so later SLI is not equal DLI for the ecpu port
        // so it can loopback to the ecpu
        ent_fpslict.default_sli = (0x20000 + CBXI_BC_MODE1_VSI);
        ent_fpslict.dst_key = 0x0;  // do not use ARL look up for destination
        ent_fpslict.src_key = 0x0;  // do not use ARL look up for MAC learning
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                unit, slicid, &ent_fpslict, &status));
    } else {
#endif // CONFIG_BC_MODE1

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, slicid, &ent_fpslict));
    ent_fpslict.default_sli = linid;
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                unit, slicid, &ent_fpslict, &status));

    CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_get(unit, linid, &ent_lin2vsi));
    ent_lin2vsi.vsi = vsiid;
    CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_set(
                                unit, linid, &ent_lin2vsi, &status));

#if defined(CONFIG_BC_MODE1) && defined(CBX_PORT_ECPU)
    }
#endif

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_ptp_lin_source_release
 * Purpose:
 *  Release source port mapping of lin and vsi.
 *
 */
int cbxi_ptp_lin_source_release(int unit, int linid, cbxi_pgid_t lpgid,
                            uint32_t vsiid, uint8_t slic_type)
{
    uint32_t    status = 0;
    pbmp_t pg_map = 0;
    fpslict_t ent_fpslict;
    lin2vsi_t ent_lin2vsi;
    cbxi_slicid_t slicid = SLICID_PTP_ETH_PP0;
    cbxi_slic_rule_id_t rule_idx = SLIC_PTP_ETH_PORT_0;

    if (slic_type == 1) {
        /* SLIC for Ethernet PTP */
        rule_idx = (SLIC_PTP_ETH_PORT_0 + lpgid);
        slicid = (SLICID_PTP_ETH_PP0 + lpgid);
    } else if (slic_type == 2) {
        /* SLIC for UDP PTP */
        rule_idx = (SLIC_PTP_UDP_PORT_0 + lpgid);
        slicid = (SLICID_PTP_UDP_PP0 + lpgid);
    }
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, rule_idx, &pg_map));
    CBX_PBMP_PORT_REMOVE(pg_map, lpgid);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, rule_idx, pg_map));

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, slicid, &ent_fpslict));
    ent_fpslict.default_sli = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                unit, slicid, &ent_fpslict, &status));

    CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_get(unit, linid, &ent_lin2vsi));
    ent_lin2vsi.vsi = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_set(
                                unit, linid, &ent_lin2vsi, &status));

    return CBX_E_NONE;
}


/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:	trap.c
 * Purpose:	Robo2 trap programming routines
 */

#include <fsal/error.h>
#include <fsal_int/trap.h>
#include <fsal_int/port.h>
#include <fsal_int/lin.h>
#include <soc/robo2/common/tables.h>
#include <bsl_log/bslenum.h>

/*
 * Function:
 *  cbxi_trap_control_get
 * Purpose:
 *  Obtain the trap control configuration.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_trap_control_get(int unit,
            cbxi_trapid_t trap_id, cbxi_trap_ctrl_t *trap_ctl)
{
    tct_t    ent_tct;

    CBX_IF_ERROR_RETURN(soc_robo2_tct_get(unit, trap_id, &ent_tct));
    trap_ctl->drop = ent_tct.drop;
    trap_ctl->term = ent_tct.term;
    if (ent_tct.trap_group == TRAP_GROUP_TRAP2CP) {
        trap_ctl->trap_flags = CBXI_TRAP_TO_CPU;
    } else if (ent_tct.trap_group == TRAP_GROUP_TRAP2CASC) {
        trap_ctl->trap_flags = CBXI_TRAP_TO_CASCADED_PORT;
    }else if (ent_tct.trap_group == TRAP_GROUP_DROP) {
        trap_ctl->trap_flags = CBXI_TRAP_DROP;
    } else if (!ent_tct.trap_group) {
        trap_ctl->trap_flags = 0;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_trap_control_set
 * Purpose:
 *  Configure trap control table.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_trap_control_set(int unit,
            cbxi_trapid_t trap_id, cbxi_trap_ctrl_t *trap_ctl)
{
    tct_t    ent_tct;
    uint32_t status = 0;

    CBX_IF_ERROR_RETURN(soc_robo2_tct_get(unit, trap_id, &ent_tct));
    ent_tct.drop = trap_ctl->drop;
    ent_tct.term = trap_ctl->term;
    if (trap_ctl->trap_flags & CBXI_TRAP_TO_CPU) {
        ent_tct.trap_group = TRAP_GROUP_TRAP2CP;
        if (unit) {
            ent_tct.trap_group = TRAP_GROUP_TRAP2CP + 1;
        }
    } else if (trap_ctl->trap_flags & CBXI_TRAP_TO_CASCADED_PORT) {
        ent_tct.trap_group = TRAP_GROUP_TRAP2CASC;
    } else if (trap_ctl->trap_flags & CBXI_TRAP_DROP) {
        ent_tct.trap_group = TRAP_GROUP_DROP;
    } else if (trap_ctl->trap_flags & CBXI_TRAP_TO_CPU_TS) {
        ent_tct.trap_group = TRAP_GROUP_TRAP2CP_TS;
        if (unit) {
            ent_tct.trap_group = TRAP_GROUP_TRAP2CP_TS + 1;
        }
    } else {
        ent_tct.trap_group = 0;
    }
    CBX_IF_ERROR_RETURN(soc_robo2_tct_set(unit,
                            trap_id, &ent_tct, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_trap_table_init
 * Purpose:
 *  Initialize trap tables.
 *
 */

STATIC int
cbxi_trap_table_init(int unit)
{
    /* Enable TCT table */
    CBX_IF_ERROR_RETURN(soc_robo2_tct_enable(unit));
    /* Reset TCT table */
    CBX_IF_ERROR_RETURN(soc_robo2_tct_reset(unit));

    /* Enable MTGT table */
    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_enable(unit));
    /* Reset MTGT table */
    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_reset(unit));

    return CBX_E_NONE;
}
/*
 * Function:
 *  cbxi_trap_dest_update
 * Purpose:
 *  Update the Dlin for trap packets to CP
 *
 * @return return code
 * @retval 0 success.
 */
int
cbxi_trap_dest_update(int unit, cbx_portid_t portid)
{
    cbxi_pgid_t  lpgid;
    cbx_port_t   port;
    int          u;
    mtgt_t       mtgt;

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port, &lpgid, &u));
    if (unit != u) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META(
                "Unit %d: Trap Dest %d does not exit on this unit(%d)\n"),
                unit, port, u));
        return CBX_E_PARAM;
    }
    if (unit == CBX_AVENGER_SECONDARY) {
        if (!(CBX_PBMP_MEMBER(PBMP_CASCADE(unit), port))) {
            LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META(
                "Unit %d: Trap Dest %d is set to Non-Cascade Port\n"), unit, port));
            return CBX_E_PARAM;
        }
    } else {
#ifdef CONFIG_EXTERNAL_HOST
        if (port != CBX_PORT_ECPU) {
            LOG_ERROR(BSL_LS_FSAL_COMMON,
                (BSL_META("Unit %d Trap Dest %d is not CPU port\n"), unit, port));
            return CBX_E_PARAM;
        }
#else
        if (port != CBX_PORT_ICPU) {
            LOG_ERROR(BSL_LS_FSAL_COMMON,
                (BSL_META("Unit %d Trap Dest cannot be non-CPU port\n"), unit, port));
            return CBX_E_PARAM;
        }
#endif
    }
    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(unit, (TRAP_GROUP_TRAP2CP + 64), &mtgt));
    CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, mtgt.dli_n, lpgid));
    return CBX_E_NONE;
}

#ifdef CONFIG_CASCADED_MODE
/*
 * Function:
 *  cbxi_trap_cascade_set
 * Purpose:
 * Create a N - type destination logical interface to
 * trap packets to cascade port
 * @return return code
 * @retval 0 success.
 */
int
cbxi_dest_cascade_set(int unit, cbx_portid_t portid)
{
    mtgt_t       ent_mtgt;
    cbxi_pgid_t  lpgid;
    cbx_port_t   port;
    int          i;

    if (!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        return CBX_E_NONE;
    }

    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                        unit, (TRAP_GROUP_TRAP2CASC + 64), &ent_mtgt));

    /* Map port to lpgid and set it as destination */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port, &lpgid, &i));
    CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, ent_mtgt.dli_n, lpgid));
    if (unit) {
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                            unit, (TRAP_GROUP_TRAP2CP_CSD + 64), &ent_mtgt));
        cbxi_cascade_port_get(unit, &portid);
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port, &lpgid, &i));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, ent_mtgt.dli_n, lpgid));
    }
    return CBX_E_NONE;
}
#endif /* CONFIG_CASCADED_MODE */

/*
 * Function:
 *  cbxi_dest_cp_set
 * Purpose:
 *  Create a N - type destination logical interface to
 *  trap packets to CP
 *
 * @return return code
 * @retval 0 success.
 */
STATIC int
cbxi_dest_cp_set(int unit, int linid, int ts_en)
{
    dliet_t ent_dliet;
    uint32_t status = 0;
    cbxi_pgid_t  lpgid;
    cbx_portid_t portid;
    cbx_port_t   port;
    int          i;

#ifdef CONFIG_EXTERNAL_HOST
    portid = CBX_PORT_ECPU;
#else
    portid = CBX_PORT_ICPU;
#endif
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && unit) {
#ifdef CONFIG_CASCADED_MODE
        /* Enable LIN assigned to secondary in primary to forward traffic to CPU*/
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port, &lpgid, &i));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(CBX_AVENGER_PRIMARY, linid, lpgid));

        portid = CBX_CASCADE_TRAP_DEST;
        if (CBX_CASCADE_TRAP_DEST < CBX_MAX_PORT_PER_UNIT) {
             /* Expect Trap Dest to be in Global PG range */
             return CBX_E_PARAM;
        }
#endif /* CONFIG_CASCADED_MODE */
    } else if (unit == CBX_AVENGER_SECONDARY) {
        return CBX_E_UNIT;
    }

    sal_memset(&ent_dliet, 0, sizeof(dliet_t));
    ent_dliet.encap0 = 0xFF; /* Start with no encap */
    if (ts_en == 1) {
        /* Insert Timestamp IMP tag */
        ent_dliet.encap0 = CBX_EGR_DIRECTIVE_INSERT_IMP_TIMESTAMP_TAG;
        ent_dliet.encap1 = 0xFF; /* EOR */
#ifdef CONFIG_BCM_TAG /* BCM TAG for SPP info */
        ent_dliet.encap1 = CBX_EGR_DIRECTIVE_INSERT_BROADCOM_HEADER;
        ent_dliet.encap2 = 0xFF; /* EOR */
#endif
    } else {
#ifdef CONFIG_BCM_TAG /* BCM TAG for SPP info */
        ent_dliet.encap0 = CBX_EGR_DIRECTIVE_INSERT_BROADCOM_HEADER;
        ent_dliet.encap1 = 0xFF; /* EOR */
#endif
    }

    CBX_IF_ERROR_RETURN(soc_robo2_dliet_set(unit, linid, &ent_dliet, &status));


    /* Map port to lpgid and set it as destination */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port, &lpgid, &i));
    CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, linid, lpgid));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slic_trap_init
 * Purpose:
 *  Initialize trap ID for SLIC groups
 *
 * @return return code
 * @retval 0 success.
 */
STATIC int
cbxi_slic_trap_init(int unit)
{
    cbxi_trap_ctrl_t trap_ctl;


    sal_memset(&trap_ctl, 0, sizeof(cbxi_trap_ctrl_t));
    /* SLIC_TRAP_GROUP1 : Trap to CP */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SLIC_GROUP1, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CPU;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SLIC_GROUP1, &trap_ctl));

    /* SLIC_TRAP_GROUP2 : Drop & Trap to CP */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SLIC_GROUP2, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CPU;
    trap_ctl.drop = 1;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SLIC_GROUP2, &trap_ctl));

    /* SLIC_TRAP_GROUP3 : Drop all pkts */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SLIC_GROUP3, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SLIC_GROUP3, &trap_ctl));

    /* SLIC_TRAP_GROUP4 : Trap to CP with TS */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SLIC_GROUP4, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CPU_TS;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SLIC_GROUP4, &trap_ctl));

    /* SLIC_TRAP_GROUP5 : Drop & Trap to CP with TS */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SLIC_GROUP5, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CPU_TS;
    trap_ctl.drop = 1;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SLIC_GROUP5, &trap_ctl));

    /* SNOOP_DHCP4 : Trap to CP */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SNOOP_DHCP4, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CPU;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SNOOP_DHCP4, &trap_ctl));

    /* CFP_TRAP :  Drop all packets */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, CFP_GROUP1, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, CFP_GROUP1, &trap_ctl));

    /* CFP_TRAP :  Drop & Trap to CP */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, CFP_GROUP2, &trap_ctl));
    trap_ctl.trap_flags |= CBXI_TRAP_TO_CPU;
    trap_ctl.drop = 1;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, CFP_GROUP2, &trap_ctl));

    /* CFP_TRAP :  Trap to CP and forward */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, CFP_GROUP3, &trap_ctl));
    trap_ctl.trap_flags |= CBXI_TRAP_TO_CPU;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, CFP_GROUP3, &trap_ctl));

    /* CFP_TRAP :  Ignore trap and forward */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, CFP_GROUP4, &trap_ctl));
    trap_ctl.drop = 0;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, CFP_GROUP4, &trap_ctl));

    /* IPV4_HDR_CHECKSUM error :  Drop the packet */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, IPV4_HDR_CHECKSUM, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, IPV4_HDR_CHECKSUM, &trap_ctl));

    /* IPV4_HDR_VERSION error :  Drop the packet */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, IPV4_HDR_VERSION, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, IPV4_HDR_VERSION, &trap_ctl));

    /* IPV4_HDR_LENGTH error :  Drop the packet */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, IPV4_HDR_LENGTH, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, IPV4_HDR_LENGTH, &trap_ctl));

    /* IPV4_MIN_LENGTH error :  Drop the packet */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, IPV4_MIN_LENGTH, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, IPV4_MIN_LENGTH, &trap_ctl));

    /* IPV6_TLEN_CHK error :  Drop the packet */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, IPV6_TLEN_CHK, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, IPV6_TLEN_CHK, &trap_ctl));

    /* TCP_HDR_LENGTH error :  Drop the packet */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, TCP_HDR_LENGTH, &trap_ctl));
    trap_ctl.drop = 1;
    trap_ctl.term = 1;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, TCP_HDR_LENGTH, &trap_ctl));

#ifdef CONFIG_CASCADED_MODE
    /* Trap to Cascade */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, SA_MOVE, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CASCADED_PORT;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, SA_MOVE, &trap_ctl));

    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, LEARN_FRAME, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_TO_CASCADED_PORT;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, LEARN_FRAME, &trap_ctl));

    /* Trap from Cascade (Drop) */
    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, CA_SA_MOVE, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_DROP;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, CA_SA_MOVE, &trap_ctl));

    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, CA_LEARN_FRAME, &trap_ctl));
    trap_ctl.trap_flags = CBXI_TRAP_DROP;
    trap_ctl.drop = 0;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, CA_LEARN_FRAME, &trap_ctl));

#endif
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_trap_group_init
 * Purpose:
 *  Initialize trap groups
 *
 * @return return code
 * @retval 0 success.
 */
STATIC int
cbxi_trap_group_init(void)
{
    mtgt_t   ent_mtgt;
    uint32_t status = 0;
    int      dlinid;
    int      unit = 0;
    uint8_t  num_units = 1;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        /* set destination for cpu traps */
        /* Use different dlinids for primary and secondary to avoid having
         * double headers at CPU */
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&dlinid));

        CBX_IF_ERROR_RETURN(cbxi_dest_cp_set(unit, dlinid, 0));
        /* Entries 64 - 79 in MTGT corresponds to Trap groups .
         * TRAP_GROUP_TRAP2CP = 1 corresponds to MTGT index 65
         * TRAP_GROUP_TRAP2CP_CSD = 2 corresponds to MTGT index 66*/
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                        unit, (TRAP_GROUP_TRAP2CP + 64 + unit), &ent_mtgt));
        ent_mtgt.dli_n = dlinid;
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(
                        unit, (TRAP_GROUP_TRAP2CP + 64 + unit), &ent_mtgt, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            /* Enable same dlin on partner Avenger */
            CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(
                        (unit ^ 0x1), (TRAP_GROUP_TRAP2CP + 64 + unit), &ent_mtgt, &status));
        }

        /*********** TRAP2CPU with Timestamp **********/
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&dlinid));

        CBX_IF_ERROR_RETURN(cbxi_dest_cp_set(unit, dlinid, 1));
        /* TRAP_GROUP_TRAP2CP_TS = 4 corresponds to MTGT index 68
         * TRAP_GROUP_TRAP2CP_CSD_TS = 5 corresponds to MTGT index 69 */
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                        unit, (TRAP_GROUP_TRAP2CP_TS + 64 + unit), &ent_mtgt));
        ent_mtgt.dli_n = dlinid;
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(
                        unit, (TRAP_GROUP_TRAP2CP_TS + 64 + unit), &ent_mtgt, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            /* Enable same dlin on partner Avenger */
            CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set((unit ^ 0x1),
                (TRAP_GROUP_TRAP2CP_TS + 64 + unit), &ent_mtgt, &status));
        }

        /* TRAP_GROUP_DROP = 6 corresponds to MTGT index 70 */
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                        unit, (TRAP_GROUP_DROP + 64), &ent_mtgt));
        ent_mtgt.dp = 3;
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(
                        unit, (TRAP_GROUP_DROP + 64), &ent_mtgt, &status));


    }

#ifdef CONFIG_CASCADED_MODE
    if (!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        return CBX_E_NONE;
    }
    /* Set destination for Cascade traps */
    CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&dlinid));

    for (unit = 0; unit < num_units; unit++) {
        /* Entries 64 - 79 in MTGT corresponds to Trap groups .
         * TRAP_GROUP_TRAP2CASC= 2 corresponds to MTGT index 66 */
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(
                        unit, (TRAP_GROUP_TRAP2CASC + 64), &ent_mtgt));
        ent_mtgt.dli_n = dlinid;
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(
                        unit, (TRAP_GROUP_TRAP2CASC + 64), &ent_mtgt, &status));

    }
#endif /* CONFIG_CASCADED_MODE */

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_trap_init
 * Purpose:
 *  Initialize trap module.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_trap_init(void)
{
    int unit = 0;
    uint8_t num_units = 1;

    LOG_INFO(BSL_LS_FSAL_COMMON,
                (BSL_META("cbxi_trap_init()..\n")));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(cbxi_trap_table_init(unit));
    }
    CBX_IF_ERROR_RETURN(cbxi_trap_group_init());
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(cbxi_slic_trap_init(unit));
    }

    return CBX_E_NONE;
}

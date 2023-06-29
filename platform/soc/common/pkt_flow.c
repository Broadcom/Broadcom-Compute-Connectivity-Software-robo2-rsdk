/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:	pkt_flow.c
 * Purpose:	Temporary file to do basic configuration for pkt flow
 */

#include <sal_types.h>
#include <shared/bitop.h>
#include <shared/types.h>
#include <soc/error.h>
#include <fsal_int/types.h>
#include <bsl_log/bslenum.h>
#include <soc/robo2/common/tables.h>
#include <soc/drv_if.h>
#include <soc/robo2/common/memregs.h>

/* Check for values in particular compile for emulation */
/* /projects/avenger_sv1/agrawalv/avenger_ToT_30June_vikas/avenger/regsfile/a0/socregs_full.h  */


#define REG_UMAC0_BASE                    0x41000000 /* UNIMAC0_STAT_mib_en */
#define REG_UMAC_MAC_MODE_OFFSET          0x00000044
#define REG_MIB_COUNTER_OFFSET            0x000004b0
#define REG_STAT_MIB_EN_OFFSET            0x0000080c
#define MIB_COUNTER_MAX                   30
#define REG_UMAC4_BASE                    0x41008000
#define REG_UMAC10_BASE                   0x41018000
#define REG_UMAC8_BASE                    0x41010000
#define REG_UMAC9_BASE                    0x41011000
#define REG_CB_PQS_PER_QUEUE_EN_QUEUE_A   0x4117121c

#define REG_UMAC_COMMAND_CONFIG_OFFSET                   0x008
#define FLD_UMAC_COMMAND_CONFIG_CRC_FWD_MASK             0x40
#define FLD_UMAC_COMMAND_CONFIG_PAUSE_FWD_MASK           0x80

int soc_robo2_stat_mib_en(int unit)
{
    uint8 port;
    uint32 address, val;
    for (port = 0; port < 8; port++) {
        if (port < 4) {
            address = REG_UMAC0_BASE + (port * 0x1000);
        } else {
            address = REG_UMAC4_BASE + ((port - 4) * 0x1000);
        }
        address = address + REG_STAT_MIB_EN_OFFSET;
        val = 0x7fff7fff;
        DRV_REG_WRITE(unit, address, &val, 4);
    }
    address = REG_UMAC8_BASE + REG_STAT_MIB_EN_OFFSET;
    DRV_REG_WRITE(unit, address, &val, 4);
    address = REG_UMAC9_BASE + REG_STAT_MIB_EN_OFFSET;
    DRV_REG_WRITE(unit, address, &val, 4);
    address = REG_UMAC10_BASE + REG_STAT_MIB_EN_OFFSET;
    DRV_REG_WRITE(unit, address, &val, 4);
    /* XLMAC MIB reset */
    val = 0xf;
    xlreg_write(unit, 0, (DRV_REG_ADDR(unit, XLPORT_MIB_RESETr, REG_PORT_ANY, 0)), val, 0);
    val = 0x0;
    xlreg_write(unit, 0, (DRV_REG_ADDR(unit, XLPORT_MIB_RESETr, REG_PORT_ANY, 0)), val, 0);
    return 0;
}

int soc_robo2_crc_pause_dis(int unit)
{
    uint8 port;
    uint32 address, val;
    for (port = 0; port < 16; port++) {
        if (port < 4) {
            address = REG_UMAC0_BASE + (port * 0x1000) +
                                REG_UMAC_COMMAND_CONFIG_OFFSET;
        } else if (port < 8) {
            address = REG_UMAC4_BASE + ((port - 4) * 0x1000) +
                                REG_UMAC_COMMAND_CONFIG_OFFSET;
        } else if (port == 14) {
            address = REG_UMAC10_BASE + REG_UMAC_COMMAND_CONFIG_OFFSET;
        } else {
            /* skip over port 8 ~ 13 and port 15 */
            continue;
        }

        DRV_REG_READ(unit, address, &val, 4);
        val &= ~FLD_UMAC_COMMAND_CONFIG_CRC_FWD_MASK;
        val &= ~FLD_UMAC_COMMAND_CONFIG_PAUSE_FWD_MASK;
        DRV_REG_WRITE(unit, address, &val, 4);
    }
    return 0;
}

int soc_robo2_en_queue(int unit)
{
    uint8 port;
    uint32 address, val;
    for (port = 0; port < 8; port++) {
        address = REG_CB_PQS_PER_QUEUE_EN_QUEUE_A + (port * 4);
        val = 0xff;
        DRV_REG_WRITE(unit, address, &val, 4);
    }
    return 0;
}

STATIC int soc_robo2_en_nisi(int unit)
{
    uint32 reg_val;

    reg_val = 0x7fff;
    SOC_IF_ERROR_RETURN(REG_WRITE_CB_SIA_NISI_SI_CONFIGr(unit, &reg_val));
    return SOC_E_NONE;
}

STATIC int soc_robo2_epp_sid_port_en_config(int unit)
{
    uint32 reg_val;

    reg_val = 0xffff;

    SOC_IF_ERROR_RETURN(
        REG_WRITE_CB_EPP_EPP_SID_PORT_EN_CONFIGr(unit, &reg_val));

    return SOC_E_NONE;

}

STATIC int soc_robo2_table_init(int unit)
{
#if 0    /* vlan_init initiates following tables */
    SOC_IF_ERROR_RETURN(soc_robo2_vsit_enable(unit));
#endif
    SOC_IF_ERROR_RETURN(soc_robo2_cpmt_enable(unit));
#if 1   /* remove once l2_init is enabled */

    if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
        SOC_IF_ERROR_RETURN(soc_robo2_arlfm_enable(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_arlfm_reset(unit));
    } else {
        SOC_IF_ERROR_RETURN(soc_robo2_arlfm0_enable(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_arlfm1_ght_h0_enable(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_arlfm1_ght_h1_enable(unit));
    }
#endif
#if 0    /* port_init initiates following tables */
    SOC_IF_ERROR_RETURN(soc_robo2_pglct_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_pgt_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_pqm_pp2lpg_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_lpg2pg_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_pg2lpg_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_lpg2ap_enable(unit));
    SOC_IF_ERROR_RETURN(soc_robo2_svt_enable(unit));
#endif

    return SOC_E_NONE;
}

int soc_robo2_arl_table_init(void)
{
    int unit = 0;
    uint8 num_units = 1;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
            SOC_IF_ERROR_RETURN(soc_robo2_arlfm_enable(unit));
            SOC_IF_ERROR_RETURN(soc_robo2_arlfm_reset(unit));
        }
    }

    return SOC_E_NONE;
}

int soc_robo2_pkt_flow_init(void)
{
    int unit = 0;
    uint8 num_units = 1;
#if 0
    svt_t ent_svt;
    uint32_t status = 0;
#endif

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        SOC_IF_ERROR_RETURN(soc_robo2_table_init(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_en_queue(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_stat_mib_en(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_crc_pause_dis(unit));
#if 0  /* Port create API populates SVT entries */
        ent_svt.svt_mask = 0xFFFF;
        SOC_IF_ERROR_RETURN(soc_robo2_svt_set(unit, 0, &ent_svt, &status));
#endif
#ifdef CONFIG_EXTERNAL_HOST
        SOC_IF_ERROR_RETURN(soc_robo2_en_nisi(unit));
#else
#ifndef CONFIG_WEB_SERVER
        SOC_IF_ERROR_RETURN(soc_robo2_en_nisi(unit));
#endif
#endif
        SOC_IF_ERROR_RETURN(soc_robo2_epp_sid_port_en_config(unit));
    }
    return SOC_E_NONE;
}

int soc_robo2_pkt_flow_enable()
{
    int unit = 0;
    uint8 num_units = 1;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        SOC_IF_ERROR_RETURN(soc_robo2_en_nisi(unit));
    }
    return SOC_E_NONE;
}

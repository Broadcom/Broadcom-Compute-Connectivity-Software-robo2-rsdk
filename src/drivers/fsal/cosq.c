/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     cosq.c
 * * Purpose:
 * *     Robo2 cosq module.
 * *
 * */
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <soc/robo2/common/intenum.h>
#include <fsal/error.h>
#include <fsal/cosq.h>
#include <fsal/ts.h>
#include <fsal_int/port.h>
#include <fsal_int/types.h>
#include <fsal_int/cosq.h>

/* Init default thresholds and bit maps for port and PFC flowcontrol */
#define N_WORDS_PER_PAGE                (16)
#define ARL_ENTRIES                     (256)
#define ARL_PAGES                       (ARL_ENTRIES/(2*N_WORDS_PER_PAGE))
#define ARL_PAGES_START                 1
#define ARL_PAGES_END                   (ARL_PAGES + 4000)

#define GFCD_RED_THRESH                 ARL_PAGES_END
#define GFCD_YELLOW_THRESH              ARL_PAGES_END
#define GFCD_GREEN_THRESH               ARL_PAGES_END
#define GFCD_XON_THRESH                 (ARL_PAGES_END - 780)
#define GFCD_XOFF_THRESH                (ARL_PAGES_END - 680)

#define FCD_RED_THRESH                  ARL_PAGES_END
#define FCD_YELLOW_THRESH               ARL_PAGES_END
#define FCD_GREEN_THRESH                ARL_PAGES_END
#define FCD_XON_THRESH                  (10)
#define FCD_XOFF_THRESH                 (100)
#if defined(CONFIG_1000X_1000) || (CONFIG_SGMII_SPEED == 1000) || defined(CONFIG_QSGMII)
#define PBM_2p5G                        (0x000)
#else
#define PBM_2p5G                        (0xf00)
#endif
#define PBM_10G                         (0x3000)
/*
 * Function:
 *  cbxi_cosq_table_init
 * Purpose:
 *  Global Initialization for all CoSQ related tables.
 *
 */
STATIC int
cbxi_cosq_table_init(int unit) {
    int          max_index = 0;
    int          index = 0;
    tc2qd_t      entry;
    uint32       status = 0;
    apt_t        apt;
    qfcqdaddr_t  qfcqdaddr;
    int          qfc_addr = 0;
    int          port = 0;
    int          cos_num = 0;
    fcd_cntr_t   fcd_cntr;
    gfcd_cntr_t  gfcd_cntr;
    /* Enable and reset CPMT */
    CBX_IF_ERROR_RETURN(soc_robo2_cpmt_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_cpmt_reset(unit));

    /* Enable and reset EPMT */
    CBX_IF_ERROR_RETURN(soc_robo2_epmt_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_epmt_reset(unit));

    /* Enable and reset TC2QD */
    CBX_IF_ERROR_RETURN(soc_robo2_tc2qd_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_tc2qd_reset(unit));

    /* Set TC to QD mapping for all ports */
    max_index =  soc_robo2_tc2qd_max_index(unit);
    for (index = 0; index <= max_index; index++) {
        entry.qd = index;
        CBX_IF_ERROR_RETURN(soc_robo2_tc2qd_set(unit, index, &entry, &status));
    }
    /* Enable and reset PQM_QDSCHCFG - default is RR */
    CBX_IF_ERROR_RETURN(soc_robo2_qdschcfg_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_qdschcfg_reset(unit));

    /* Enable and reset PQM_apt_ia*/
    CBX_IF_ERROR_RETURN(soc_robo2_apt_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_apt_reset(unit));
#if 1
    /* retain the default values for PQM_apt_ia - drop thresh = 8191
       wred_m = 0, wred_c = 0; */
    max_index =  soc_robo2_apt_max_index(unit);
    for (index = 0; index <= max_index; index++) {
        /* PQM_apt_ia-drop thresh values provided by Arch/SV team */
        if (index % 4 == 3) {
            apt.drop_thresh = 16;
        } else if (index % 4 == 2) {
            apt.drop_thresh = 64;
        } else if (index % 4 == 1) {
#ifdef CONFIG_TIMESYNC
            apt.drop_thresh = 800;
#else
            apt.drop_thresh = 200;
#endif
        } else {
            apt.drop_thresh = 0x1fff;
        }
        apt.wred_m = 0;
        apt.wred_c = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_apt_set(unit, index, &apt, &status));
    }
    /* Enable and reset qfcqadd*/
    CBX_IF_ERROR_RETURN(soc_robo2_qfcqdaddr_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_qfcqdaddr_reset(unit));
    index = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_qfcqdaddr_get(unit, index, &qfcqdaddr));
    for (port = 0 ; port < CBX_MAX_PORT_PER_UNIT; port++) {
        for (cos_num = 0; cos_num < 8; cos_num++) { /* 16x8 */
            qfcqdaddr.qfc_addr =  qfc_addr;
            CBX_IF_ERROR_RETURN(soc_robo2_qfcqdaddr_set(unit, ((port * 8) + cos_num),
                                                        &qfcqdaddr, &status));
            if ((port >=8) && (port < 12)) {
                qfc_addr += 35;
            } else if ((port >= 12) && (port < 14)) {
                qfc_addr += 141;
#ifdef CONFIG_PORT_EXTENDER
            } else if (port == CBX_PE_LB_PORT) {
                /* Configure Loopback port to handle 12G bandwidth */
                qfc_addr += 168;
#endif
            } else {
                qfc_addr += 14;
            }
        }
    }
#endif
    /* Enable gfcd config */
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_reset(unit));

    /* Enable GFCD counters */
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_cntr_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_cntr_reset(unit));

    gfcd_cntr.page_cnt = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_cntr_init(unit, 0,
                           soc_robo2_gfcd_cntr_max_index(unit), 0, &gfcd_cntr));

    /* Enable fcd config */
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_config_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_config_reset(unit));

    /* Enable FCD counters */
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_cntr_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_cntr_reset(unit));

    fcd_cntr.page_cnt = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_cntr_init(unit, 0,
                          soc_robo2_fcd_cntr_max_index(unit), 0, &fcd_cntr));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_queue_init
 * Purpose:
 *  Initialization of 8 queues for all the ports
 *
 */
int
cbxi_cosq_queue_init(int unit) {
    int          index = 0;
    uint32       queue_en = 0;
    uint32       fval = 0xFF;
    uint32       reg_data = 0;
    soc_CB_PQS_PER_QUEUE_EN_QUEUE_Ar_field_set(unit, &queue_en, Q_ENABLEf, &fval);
    for (index = 0; index < CBX_MAX_PORT_PER_UNIT; index++) {
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE(unit, CB_PQS_PER_QUEUE_EN_QUEUE_Ar,
                                               REG_PORT_ANY, index, &queue_en));
    }
    /* Initialize QFC memory */
    fval= 0;
    soc_CB_PQM_QFC_MEM_CFGr_field_set(unit, (uint32 *)&reg_data, QFC_CFGf, &fval);
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_PQM_QFC_MEM_CFGr(unit, &reg_data));
    fval = 0x1;
    soc_CB_PQM_QFC_MEM_CFGr_field_set(unit, (uint32 *)&reg_data, QFC_CFGf, &fval);
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_PQM_QFC_MEM_CFGr(unit, &reg_data));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_portid_validate
 * Purpose:
 *  Validate the portid passed to cosq API's
 *
 */
int
cbxi_cosq_portid_validate(cbx_portid_t portid, cbx_port_t *port_out,
                          cbxi_pgid_t *lpg, int *unit) {
    int rv = CBX_E_NONE;
    rv = cbxi_robo2_port_validate(portid, port_out, lpg, unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_PORT,
            (BSL_META("FSAL API : cbxi_cosq_portid_validate()... \
                                      Port validation failed \n")));
        return CBX_E_PORT;
    }
    return rv;
}


/*
 * Function:
 *  cbxi_cosq_cpmt_update
 * Purpose:
 *  Update CPMT table entry at the index specified
 *
 */
int
cbxi_cosq_cpmt_update(int unit, int table_index, cbx_tc_t tc) {

    cpmt_t         cpmt_entry;
    uint32         status  = 0;

    /* Get CPMT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_cpmt_get(unit, table_index, &cpmt_entry));

    /* update CPMT entry */
    cpmt_entry.tc = tc.int_pri;
    cpmt_entry.dp = tc.int_dp;

    CBX_IF_ERROR_RETURN(
              soc_robo2_cpmt_set(unit, table_index, &cpmt_entry, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_epmt_update
 * Purpose:
 *  Update EPMT table entry at the index specified
 *
 */
int
cbxi_cosq_epmt_update(int unit, int table_index, uint32 data,
                      int int_pri, int flag) {

    epmt_t         epmt_entry;
    uint32         status  = 0;
    uint32         mask = 0;

    if (flag == CBXI_COSQ_4BITS_MAP) {
        mask = ~(0xF << ((CBXI_COSQ_MAX_INT_PRI - int_pri) * 4));
        data = data << ((CBXI_COSQ_MAX_INT_PRI - int_pri) * 4);
    } else if (flag == CBXI_COSQ_8BITS_MAP) {
        mask = ~(0xFF << ((0x3 - int_pri) * 8));
        data = data << ((3 - int_pri) * 8);
    } else {
        return CBX_E_INTERNAL;
    }
    /* Get EPMT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_epmt_get(unit, table_index, &epmt_entry));

    /* update EPMT entry */
    epmt_entry.pmt_data &= mask;
    epmt_entry.pmt_data |= data;

    CBX_IF_ERROR_RETURN(
              soc_robo2_epmt_set(unit, table_index, &epmt_entry, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_etct_update
 * Purpose:
 *  Update ETCT table entry at the index specified
 *
 */
int
cbxi_cosq_etct_update(int unit, int table_index, uint32 primap_index) {

    etct_t         etct_entry;
    uint32         status  = 0;

    /* Get ETCT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_etct_get(unit, table_index, &etct_entry));

    /* update ETCT entry */
    etct_entry.primap_ndx = primap_index;

    CBX_IF_ERROR_RETURN(
              soc_robo2_etct_set(unit, table_index, &etct_entry, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_sched_weight_op
 * Purpose:
 *  Set/get per queue weight for port scheduler
 *
 */
int
cbxi_cosq_sched_weight_op(int unit, cbx_port_t port,
                          cbx_schedule_mode_t *sched_mode,
                          int flag,
                          int *weight)  {

    uint32         q_weight_hi = CBXI_COSQ_RR_WEIGHT;/* default value for RR */
    uint32         q_weight_low = CBXI_COSQ_RR_WEIGHT;/* default value for RR */
    uint32 weight_low_port_regs[] = {
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A0r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A1r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A2r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A3r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A4r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A5r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A6r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A7r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A8r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A9r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A10r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A11r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A12r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A13r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A14r,
        CB_PQS_WEIGHT_LOW_QUEUE_PORT_A15r
   };
   uint32 weight_high_port_regs[] = {
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A0r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A1r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A2r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A3r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A4r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A5r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A6r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A7r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A8r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A9r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A10r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A11r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A12r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A13r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A14r,
        CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A15r
    };
    if (flag == CBXI_COSQ_OP_SET) {
        if (*sched_mode == cbxScheduleStrict) {
            return CBX_E_NONE;
        } else if(*sched_mode != cbxScheduleRR) {
            q_weight_hi = ((weight[4]) + (weight[5] << 8) +
                           (weight[6] << 16) + (weight[7] << 24));
            q_weight_low = ((weight[0]) + (weight[1] << 8) +
                            (weight[2] << 16) + (weight[3] << 24));
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit,
                                            weight_high_port_regs[port],
                                            REG_PORT_ANY, 0, &q_weight_hi));
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit,
                                           weight_low_port_regs[port],
                                           REG_PORT_ANY, 0, &q_weight_low));
    } else if (flag == CBXI_COSQ_OP_GET) {
        if (*sched_mode == cbxScheduleStrict) {
            return CBX_E_NONE;
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit,
                                               weight_high_port_regs[port],
                                               REG_PORT_ANY, 0, &q_weight_hi));
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit,
                                           weight_low_port_regs[port],
                                           REG_PORT_ANY, 0, &q_weight_low));
        weight[0] = q_weight_low & 0XFF;
        weight[1] = (q_weight_low >> 8) & 0XFF;
        weight[2] = (q_weight_low >> 16) & 0XFF;
        weight[3] = (q_weight_low >> 24) & 0XFF;
        weight[4] = q_weight_hi & 0XFF;
        weight[5] = (q_weight_hi >> 8) & 0XFF;
        weight[6] = (q_weight_hi >> 16) & 0XFF;
        weight[7] = (q_weight_hi >> 24) & 0XFF;
        if ((q_weight_hi == CBXI_COSQ_RR_WEIGHT) &&
            (q_weight_low == CBXI_COSQ_RR_WEIGHT)) {
            *sched_mode = cbxScheduleRR;
        }
    } else {
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}
/*
 * Function:
 *  cbxi_cosq_port_sched_op
 * Purpose:
 *  Set/get port scheduler
 *
 */
int
cbxi_cosq_port_sched_op(int unit, cbx_port_t port,
                        cbx_schedule_mode_t *sched_mode,
                        int flag)  {

    uint32         qtogrp = 0;
    uint32         config_port = 0;

    static uint16 queue_to_group_regs[] = {
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A0r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A1r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A2r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A3r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A4r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A5r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A6r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A7r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A8r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A9r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A10r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A11r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A12r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A13r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A14r,
        CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A15r
    };

    soc_robo2_reg_int_t config_port_regs[] = {
        CB_PQS_PQS_CONFIG_PORT_A0r,
        CB_PQS_PQS_CONFIG_PORT_A1r,
        CB_PQS_PQS_CONFIG_PORT_A2r,
        CB_PQS_PQS_CONFIG_PORT_A3r,
        CB_PQS_PQS_CONFIG_PORT_A4r,
        CB_PQS_PQS_CONFIG_PORT_A5r,
        CB_PQS_PQS_CONFIG_PORT_A6r,
        CB_PQS_PQS_CONFIG_PORT_A7r,
        CB_PQS_PQS_CONFIG_PORT_A8r,
        CB_PQS_PQS_CONFIG_PORT_A9r,
        CB_PQS_PQS_CONFIG_PORT_A10r,
        CB_PQS_PQS_CONFIG_PORT_A11r,
        CB_PQS_PQS_CONFIG_PORT_A12r,
        CB_PQS_PQS_CONFIG_PORT_A13r,
        CB_PQS_PQS_CONFIG_PORT_A14r,
        CB_PQS_PQS_CONFIG_PORT_A15r
    };

    if (flag == CBXI_COSQ_OP_SET) {
        if (*sched_mode == cbxScheduleStrict) {
            qtogrp = CBXI_COSQ_QUEUE_TO_GROUP_SP;/* 1 bit for each queue-0-SP */
        } else {
            qtogrp = CBXI_COSQ_QUEUE_TO_GROUP_RR;  /* 1 bit for each Q. 1- RR */
        }
        /* PQS_queue_to_group_map_port_A0-A15 */
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit,
                                               queue_to_group_regs[port],
                                               REG_PORT_ANY, 0, &qtogrp));
        /* CB_PQS_pqs_config_port_a0-A15 */
        if (*sched_mode == cbxScheduleWRR) {
            config_port = 1;  /* WRR */
        } else {
            config_port = 0;  /* WDRR */
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit,
                                               config_port_regs[port],
                                               REG_PORT_ANY, 0, &config_port));
    } else if(flag == CBXI_COSQ_OP_GET) {
        /* PQS_queue_to_group_map_port_A0-A15 */
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit,
                                               queue_to_group_regs[port],
                                               REG_PORT_ANY, 0, &qtogrp));
        if (CBXI_COSQ_QUEUE_TO_GROUP_SP == qtogrp) {
            *sched_mode = cbxScheduleStrict;
            return CBX_E_NONE;
        } else {
            *sched_mode = cbxScheduleRR;
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, config_port_regs[port],
                                               REG_PORT_ANY, 0, &config_port));
        if (0 == config_port) {
            *sched_mode = cbxScheduleWDRR;
        } else {
            *sched_mode = cbxScheduleWRR;
        }
    } else {
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_pgt_update
 * Purpose:
 *  Update PGT entry FCD value
 *
 */
int
cbxi_cosq_pgt_update(int unit, int lpg, pgt_t *pgt) {
    pgt_t          pgt_entry;
    uint32         status = 0;

    /* Get PGT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, lpg, &pgt_entry));
    /* Set fcd value */
    pgt_entry.fcd0 = pgt->fcd0;
    pgt_entry.fcd1 = pgt->fcd1;
    pgt_entry.fcd2 = pgt->fcd2;
    pgt_entry.fcd3 = pgt->fcd3;
    pgt_entry.fcd4 = pgt->fcd4;
    pgt_entry.fcd5 = pgt->fcd5;
    pgt_entry.fcd6 = pgt->fcd6;
    pgt_entry.fcd7 = pgt->fcd7;
    /* Set PGT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(unit, lpg, &pgt_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_flowcontrol_set
 * Purpose:
 *  Set flow control domain
 *
 */
int
cbxi_cosq_flowcontrol_set(int unit, cbx_flowcontrol_t flowcontrol_mode) {
    int            rv = CBX_E_NONE;
    pgt_t          pgt_entry;
    uint32         status  = 0;
    int            max_index = 0;
    uint32         index = 0;
    uint32         meter_config;
    uint32         fval = 0;
    mtr2tcb_t      entry;
    uint32         pfc_ctrl = 0x7fff;

    CBX_IF_ERROR_RETURN(cbxi_cosq_admission_control_set(unit, flowcontrol_mode));

    max_index =  soc_robo2_pgt_max_index(unit);
    if (max_index < 0) {
        return CBX_E_INTERNAL;
    }
    CBX_IF_ERROR_RETURN(REG_READ_CB_BMU_MTR_CONFIGr(unit, &meter_config));

    if(flowcontrol_mode == cbxFlowcontrolPause) {
        pgt_entry.fcd0 = 0;
        pgt_entry.fcd1 = 0;
        pgt_entry.fcd2 = 0;
        pgt_entry.fcd3 = 0;
        pgt_entry.fcd4 = 0;
        pgt_entry.fcd5 = 0;
        pgt_entry.fcd6 = 0;
        pgt_entry.fcd7 = 0;
        for (index = 0; index <= max_index; index++) {
            rv += cbxi_cosq_pgt_update(unit, index, &pgt_entry);
        }
        fval = 2;
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       METER_ID_SELf, &fval);
    } else if(flowcontrol_mode == cbxFlowcontrolPFC) {
        pgt_entry.fcd0 = 0;
        pgt_entry.fcd1 = 1;
        pgt_entry.fcd2 = 2;
        pgt_entry.fcd3 = 3;
        pgt_entry.fcd4 = 4;
        pgt_entry.fcd5 = 5;
        pgt_entry.fcd6 = 6;
        pgt_entry.fcd7 = 7;
        for (index = 0; index <= max_index; index++) {
            rv += cbxi_cosq_pgt_update(unit, index, &pgt_entry);
        }
        fval = 0;
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       METER_ID_SELf, &fval);
    } else if(flowcontrol_mode == cbxFlowcontrolMeter) {
        fval = 1;
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       METER_ID_SELf, &fval);
        entry.shp_en = 0x0; /* to be enabled as part of meter create */

    } else {
        /* cbxFlowcontrolNone */
        fval = 1;    /* keep it same as meter..but no shaping */
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       METER_ID_SELf, &fval);
        entry.shp_en = 0x0;

    }
    /* Update MTR config with flow control mode */
    CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_BMU_MTR_CONFIGr(unit, &meter_config));

    if (flowcontrol_mode == cbxFlowcontrolNone) {
        /* Enable shaping and set tc bitmap  */
        max_index =  soc_robo2_mtr2tcb_max_index(unit);
        if (max_index < 0) {
            return CBX_E_INTERNAL;
        }
        for (index = 0; index <= max_index; index++) {
            entry.tc_bmp = 0x0;
            CBX_IF_ERROR_RETURN(soc_robo2_mtr2tcb_set(unit, index,
                                                      &entry, &status));
        }
    }
    if (flowcontrol_mode == cbxFlowcontrolPFC) {
        pfc_ctrl = 0;
    }
    CBX_IF_ERROR_RETURN(
          REG_WRITE_NPA_NPA_PFC_CONTROLr(unit, &pfc_ctrl));

    return rv;
}

/*
 * Function:
 *  cbxi_cosq_egress_shaper_get_reg
 * Purpose:
 *  Get the shaper config register for a given portid and int_pri value
 *
 */
int
cbxi_cosq_egress_shaper_get_reg(cbx_port_t port, int int_pri,
                                uint32 *cfg_reg, uint32 *burst_reg,
                                uint32 *refresh_reg) {

    int  reg_offset_value;
    reg_offset_value = port * 9 + int_pri;

    switch (reg_offset_value) {
        case 0:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A0r;
            break;
        case 1:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A1r;
            break;
        case 2:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A2r;
            break;
        case 3:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A3r;
            break;
        case 4:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A4r;
            break;
        case 5:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A5r;
            break;
        case 6:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A6r;
            break;
        case 7:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A7r;
            break;
        case 8:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A8r;
            break;
        /* Port 1 */
        case 9:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A0r;
            break;
        case 10:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A1r;
            break;
        case 11:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A2r;
            break;
        case 12:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A3r;
            break;
        case 13:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A4r;
            break;
        case 14:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A5r;
            break;
        case 15:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A6r;
            break;
        case 16:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A7r;
            break;
        case 17:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_1_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_1_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_1_SHAPER_A8r;
            break;
        /* Port 2 */
        case 18:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A0r;
            break;
        case 19:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A1r;
            break;
        case 20:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A2r;
            break;
        case 21:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A3r;
            break;
        case 22:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A4r;
            break;
        case 23:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A5r;
            break;
        case 24:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A6r;
            break;
        case 25:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A7r;
            break;
        case 26:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_2_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_2_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_2_SHAPER_A8r;
            break;
        /* Port 3 */
        case 27:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A0r;
            break;
        case 28:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A1r;
            break;
        case 29:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A2r;
            break;
        case 30:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A3r;
            break;
        case 31:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A4r;
            break;
        case 32:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A5r;
            break;
        case 33:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A6r;
            break;
        case 34:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A7r;
            break;
        case 35:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_3_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_3_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_3_SHAPER_A8r;
            break;
        /* port 4 */
        case 36:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A0r;
            break;
        case 37:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A1r;
            break;
        case 38:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A2r;
            break;
        case 39:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A3r;
            break;
        case 40:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A4r;
            break;
        case 41:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A5r;
            break;
        case 42:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A6r;
            break;
        case 43:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A7r;
            break;
        case 44:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_4_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_4_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_4_SHAPER_A8r;
            break;
        /* Port 5 */
        case 45:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A0r;
            break;
        case 46:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A1r;
            break;
        case 47:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A2r;
            break;
        case 48:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A3r;
            break;
        case 49:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A4r;
            break;
        case 50:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A5r;
            break;
        case 51:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A6r;
            break;
        case 52:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A7r;
            break;
        case 53:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_5_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_5_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_5_SHAPER_A8r;
            break;
        /* Port 6 */
        case 54:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A0r;
            break;
        case 55:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A1r;
            break;
        case 56:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A2r;
            break;
        case 57:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A3r;
            break;
        case 58:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A4r;
            break;
        case 59:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A5r;
            break;
        case 60:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A6r;
            break;
        case 61:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A7r;
            break;
        case 62:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_6_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_6_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_6_SHAPER_A8r;
            break;
        /* port 7 */
        case 63:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A0r;
            break;
        case 64:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A1r;
            break;
        case 65:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A2r;
            break;
        case 66:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A3r;
            break;
        case 67:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A4r;
            break;
        case 68:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A5r;
            break;
        case 69:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A6r;
            break;
        case 70:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A7r;
            break;
        case 71:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_7_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_7_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_7_SHAPER_A8r;
            break;
        /* Port 8 */
        case 72:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A0r;
            break;
        case 73:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A1r;
            break;
        case 74:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A2r;
            break;
        case 75:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A3r;
            break;
        case 76:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A4r;
            break;
        case 77:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A5r;
            break;
        case 78:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A6r;
            break;
        case 79:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A7r;
            break;
        case 80:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_8_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_8_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_8_SHAPER_A8r;
            break;
        /* Port 9 */
        case 81:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A0r;
            break;
        case 82:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A1r;
            break;
        case 83:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A2r;
            break;
        case 84:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A3r;
            break;
        case 85:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A4r;
            break;
        case 86:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A5r;
            break;
        case 87:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A6r;
            break;
        case 88:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A7r;
            break;
        case 89:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_9_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_9_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_9_SHAPER_A8r;
            break;
        /* Port 10 */
        case 90:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A0r;
            break;
        case 91:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A1r;
            break;
        case 92:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A2r;
            break;
        case 93:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A3r;
            break;
        case 94:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A4r;
            break;
        case 95:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A5r;
            break;
        case 96:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A6r;
            break;
        case 97:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A7r;
            break;
        case 98:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_10_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_10_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_10_SHAPER_A8r;
            break;
        /* Port 11 */
        case 99:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A0r;
            break;
        case 100:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A1r;
            break;
        case 101:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A2r;
            break;
        case 102:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A3r;
            break;
        case 103:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A4r;
            break;
        case 104:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A5r;
            break;
        case 105:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A6r;
            break;
        case 106:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A7r;
            break;
        case 107:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_11_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_11_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_11_SHAPER_A8r;
            break;
        /* Port 12 */
        case 108:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A0r;
            break;
        case 109:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A1r;
            break;
        case 110:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A2r;
            break;
        case 111:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A3r;
            break;
        case 112:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A4r;
            break;
        case 113:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A5r;
            break;
        case 114:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A6r;
            break;
        case 115:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A7r;
            break;
        case 116:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_12_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_12_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_12_SHAPER_A8r;
            break;
        /* Port 13 */
        case 117:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A0r;
            break;
        case 118:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A1r;
            break;
        case 119:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A2r;
            break;
        case 120:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A3r;
            break;
        case 121:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A4r;
            break;
        case 122:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A5r;
            break;
        case 123:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A6r;
            break;
        case 124:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A7r;
            break;
        case 125:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_13_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_13_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_13_SHAPER_A8r;
            break;
        /* Port 14 */
        case 126:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A0r;
            break;
        case 127:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A1r;
            break;
        case 128:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A2r;
            break;
        case 129:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A3r;
            break;
        case 130:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A4r;
            break;
        case 131:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A5r;
            break;
        case 132:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A6r;
            break;
        case 133:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A7r;
            break;
        case 134:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_14_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_14_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_14_SHAPER_A8r;
            break;
        /* Port 15 */
        case 135:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A0r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A0r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A0r;
            break;
        case 136:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A1r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A1r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A1r;
            break;
        case 137:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A2r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A2r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A2r;
            break;
        case 138:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A3r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A3r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A3r;
            break;
        case 139:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A4r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A4r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A4r;
            break;
        case 140:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A5r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A5r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A5r;
            break;
        case 141:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A6r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A6r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A6r;
            break;
        case 142:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A7r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A7r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A7r;
            break;
        case 143:
            *cfg_reg = CB_PQS_SHAPER_CFG_PORT_15_SHAPER_A8r;
            *burst_reg = CB_PQS_BURST_SIZE_PORT_15_SHAPER_A8r;
            *refresh_reg = CB_PQS_REFRESH_SIZE_PORT_15_SHAPER_A8r;
            break;
        default:
            return CBX_E_INTERNAL;
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbxi_cosq_egress_shaper_get_reg().. \
                           failed to get PQS_SHAPER_CFG_PORT Shaper reg  \n")));
            break;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq__rate_validate
 * Purpose:
 *  Validate passed shaper rate.
 *
 */
int
cbxi_cosq_rate_validate(cbx_cosq_shaper_params_t *shaper) {

    uint32  rate = shaper->bits_sec;
    uint32  burst = shaper->bits_burst;
    /* If packet mode, convert pakets/sec to kbps */
    if (shaper->flags & CBX_COSQ_SHAPER_MODE_PACKETS) {
        rate = ((shaper->bits_sec * CBXI_COSQ_PKT_MODE_PKT_LEN) / 1000);
        burst = ((shaper->bits_burst * CBXI_COSQ_PKT_MODE_PKT_LEN) / 1000);
    }
    /* Check if meter rate is below 64kbps */
    if ((rate < CBXI_COSQ_SHAPER_RATE_MIN) ||
        (burst < CBXI_COSQ_SHAPER_BURST_MIN)) {
        LOG_VERBOSE(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbxi_cosq_rate_validate()...\
                      rate/burst passed is less than minimum\n")));
            return CBX_E_PARAM;
    }
    /* Check if meter rate is above 10gbps */
    if ((rate > CBXI_COSQ_SHAPER_RATE_MAX) ||
        (burst > CBXI_COSQ_SHAPER_BURST_MAX)) {
        LOG_VERBOSE(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbxi_cosq_rate_validate()...\
                      rate/ burst passed is more than max allowed\n")));
            return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_bucket_encoding_to_rate
 * Purpose:
 *  Get/Set shaper config registers
 *
 */
int
cbxi_cosq_bucket_encoding_to_rate(uint32 scale, uint32 ref_size,
                                  uint32 burst_size,
                                  cbx_cosq_shaper_params_t *shaper) {

    uint32  rate = 0;
    uint32  burst = 0;
    if (scale > CBXI_COSQ_SHAPER_SCALE_MAX) {
        return CBX_E_CONFIG;
    }
    /* rate in kbps and burst in Bytes */
    rate =  (ref_size * (CBXI_COSQ_SHAPER_MAX_SCALE_MULTIPLIER / ((1 << scale) * 1000)));
    burst = (burst_size *  (1 << scale));

    if (shaper->flags & CBX_COSQ_SHAPER_MODE_PACKETS) {
        rate = rate * 8;
    } else {
        burst = ((burst * 8)/1000);
    }
    shaper->bits_sec = rate;
    shaper->bits_burst = burst;
    return CBX_E_NONE;
}
/*
 * Function:
 *  cbxi_cosq_rate_to_bucket_encoding
 * Purpose:
 *  Convert rate and burst to scale and refresh size
 *  The rate of the shaper would be
 *   rate(in bits/sec) = (Refresh rate * Refresh Size* 8)/Token scaling factor
 *   Burst(byets) = burst_size * 2^ (token scaling factor)
 *   Scale factor of 0 means 1024000 bits/sec at 128000 hz of refresh rate
 *
 * scal_factor   Refresh Rate(hz)     scale_multiplier
 *      0         128000                1024000
 *      1         128000                512000
 *      2         128000                256000
 *      3         128000                128000
 *      4         128000                64000
 *      5         128000                32000
 *      6         128000                16000
 *      7         128000                8000
 *      8         128000                4000
 *      9         128000                2000
 *     10         128000                1000
 *     11         128000                500
 *     12         128000                250
 *     13         128000                125
 *     14         128000                62.5
 *     15         128000                31.25
 */
int
cbxi_cosq_rate_to_bucket_encoding(cbx_cosq_shaper_params_t *shaper,
                                  uint32 *scale, uint32 *refresh_size,
                                  uint32 *burst_size) {
    uint32  rate = 0;
    uint32  burst = 0;
    uint32  ref_val;
    int     index = 0;

    if (shaper->flags & CBX_COSQ_SHAPER_MODE_PACKETS) {
        burst = shaper->bits_burst;
        rate = shaper->bits_sec / 8;
    } else {
        /* convert burst from bits to bytes */
        burst = (shaper->bits_burst * 1000 / 8);
        rate = shaper->bits_sec;
    }
    /* Find the scale factor and refresh_size */
    for (index = CBXI_COSQ_SHAPER_SCALE_MAX; index >=0; index--) {
       ref_val = (rate /
                  ((CBXI_COSQ_SHAPER_MAX_SCALE_MULTIPLIER >> index) / 1000));
       if (ref_val < CBXI_COSQ_SHAPER_MAX_REFESH_SIZE) {
           *refresh_size = ref_val + 1;
           *burst_size = (burst / (1 << index)) + 1;
           *scale = index;
           return CBX_E_NONE;
       }
    }
    return CBX_E_PARAM;
}

/*
 * Function:
 *  cbxi_cosq_port_shaper_op
 * Purpose:
 *  Get/Set shaper config registers
 *
 */
int
cbxi_cosq_port_shaper_op(cbx_portid_t             port,
                         int                      int_pri,
                         cbx_cosq_shaper_params_t *shaper,
                         int flag) {
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    uint32         pqs_burst;
    uint32         pqs_config;
    uint32         pqs_refresh;
    uint32         reg_val = 0;
    uint32         fval = 0;
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         scale = 0;
    uint32         refresh_size = 0;
    uint32         burst_size = 0;

    soc_robo2_reg_int_t config_port_regs[] = {
        CB_PQS_PQS_CONFIG_PORT_A0r,
        CB_PQS_PQS_CONFIG_PORT_A1r,
        CB_PQS_PQS_CONFIG_PORT_A2r,
        CB_PQS_PQS_CONFIG_PORT_A3r,
        CB_PQS_PQS_CONFIG_PORT_A4r,
        CB_PQS_PQS_CONFIG_PORT_A5r,
        CB_PQS_PQS_CONFIG_PORT_A6r,
        CB_PQS_PQS_CONFIG_PORT_A7r,
        CB_PQS_PQS_CONFIG_PORT_A8r,
        CB_PQS_PQS_CONFIG_PORT_A9r,
        CB_PQS_PQS_CONFIG_PORT_A10r,
        CB_PQS_PQS_CONFIG_PORT_A11r,
        CB_PQS_PQS_CONFIG_PORT_A12r,
        CB_PQS_PQS_CONFIG_PORT_A13r,
        CB_PQS_PQS_CONFIG_PORT_A14r,
        CB_PQS_PQS_CONFIG_PORT_A15r
    };

    if (CBX_PORTID_IS_TRUNK(port) || CBX_PORTID_IS_VLAN_PORT(port) ||
        CBX_PORTID_IS_MCAST(port)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_set/get..Invalid port  \
                          type passed...only physical port supported  \n")));
        return CBX_E_PARAM;
    }
    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(port, &port_out,
                                                  &lpg, &unit));
    /* Get the register to be configured */
    CBX_IF_ERROR_RETURN(cbxi_cosq_egress_shaper_get_reg(port_out, int_pri,
                                        &pqs_config, &pqs_burst, &pqs_refresh));

    if (flag == CBXI_COSQ_OP_SET) {
        /* validate the rate and bus passed */
        CBX_IF_ERROR_RETURN(cbxi_cosq_rate_validate(shaper));
        /* AVB mode - only for queue shaper */
        if((shaper->flags & CBX_COSQ_SHAPER_MODE_AVB) &&
           (int_pri == CBXI_COSQ_PORT_SHAPER_INDEX)) {
            return CBX_E_PARAM;
        }
        /* Set mode */
        if (shaper->flags & CBX_COSQ_SHAPER_MODE_PACKETS) {
            fval = 1;
        } else {
            fval = 0;
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit,
                                            config_port_regs[port_out],
                                            REG_PORT_ANY, 0, &reg_val));
        soc_CB_PQS_PQS_CONFIG_PORT_A0r_field_set(unit, &reg_val,
                                            SHAPER_PACKET_MODEf, &fval);
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit,
                                            config_port_regs[port_out],
                                            REG_PORT_ANY, 0, &reg_val));
        /* Configure shaper mode, scaling factor, avb and freeze mode */
        fval = 0;
        soc_CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r_field_set(unit, &reg_val,
                                            SHAPER_FREEZE_MODEf, &fval);
        /* AVB */
        if (shaper->flags & CBX_COSQ_SHAPER_MODE_AVB) {
            fval = 2;
        } else {
            fval = 0;
        }
        soc_CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r_field_set(unit, &reg_val,
                                            SHAPER_AVB_MODEf, &fval);
        /* shaper mode - blocked, Enabled or Normal */
        fval = 2; /* Normal - perform rate limiting */
        soc_CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r_field_set(unit, &reg_val,
                                            SHAPER_MODEf, &fval);
        /* convert rate and burst to scale and refresh size */
        CBX_IF_ERROR_RETURN(cbxi_cosq_rate_to_bucket_encoding(shaper,
                                                     &scale, &refresh_size,
                                                     &burst_size));
        /* Token scaling factor */
        soc_CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r_field_set(unit, &reg_val,
                                            TOKEN_SCALING_FACTORf, &scale);
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, pqs_config,
                                        REG_PORT_ANY, 0, &reg_val));
        /* Configure burst */
        soc_CB_PQS_BURST_SIZE_PORT_0_SHAPER_A0r_field_set(unit, &reg_val,
                                            BURST_SIZEf, &burst_size);
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, pqs_burst,
                                        REG_PORT_ANY, 0, &reg_val));
        /* Configure refresh size */
        soc_CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A0r_field_set(unit, &reg_val,
                                           REFRESH_SIZEf, &refresh_size);
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, pqs_refresh,
                                        REG_PORT_ANY, 0, &reg_val));
    } else if (flag == CBXI_COSQ_OP_GET) {
        shaper->flags = 0;
        /* Read per port shaper config reg to get byte/packet mode */
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit,
                                               config_port_regs[port_out],
                                               REG_PORT_ANY, 0, &reg_val));
        soc_CB_PQS_PQS_CONFIG_PORT_A0r_field_get(unit, &reg_val,
                                            SHAPER_PACKET_MODEf, &fval);
        if (fval == 1) {
            shaper->flags |= CBX_COSQ_SHAPER_MODE_PACKETS;
        }
        /* Read avb mode */
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, pqs_config,
                                        REG_PORT_ANY, 0, &reg_val));
        soc_CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r_field_get(unit, &reg_val,
                                            SHAPER_AVB_MODEf, &fval);
        if (fval == 2) {
            shaper->flags |= CBX_COSQ_SHAPER_MODE_AVB;
        }
        /* Read scale */
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, pqs_config,
                                        REG_PORT_ANY, 0, &reg_val));
        soc_CB_PQS_SHAPER_CFG_PORT_0_SHAPER_A0r_field_get(unit, &reg_val,
                                            TOKEN_SCALING_FACTORf, &scale);
        /* Read refresh size */
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, pqs_refresh,
                                        REG_PORT_ANY, 0, &reg_val));
        soc_CB_PQS_REFRESH_SIZE_PORT_0_SHAPER_A0r_field_get(unit, &reg_val,
                                           REFRESH_SIZEf, &refresh_size);
        /* Read burst */
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, pqs_burst,
                                        REG_PORT_ANY, 0, &reg_val));
        soc_CB_PQS_BURST_SIZE_PORT_0_SHAPER_A0r_field_get(unit, &reg_val,
                                            BURST_SIZEf, &burst_size);

        /* get rate and burst from scale and refresh size */
        CBX_IF_ERROR_RETURN(cbxi_cosq_bucket_encoding_to_rate(scale,
                                                      refresh_size,
                                                      burst_size, shaper));
    } else {
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cosq_admission_control_set
 * Purpose:
 *  Set  admission control thresholds for fcd and gfcd
 *
 */
int cbxi_cosq_admission_control_set(int unit, cbx_flowcontrol_t flowcontrol_mode) {

    int             table_max_index = 0;
    fcd_config_t    fcd_config;
    gfcd_config_t   gfcd_config;
    int             i = 0;
    uint32          status  = 0;
    int             port = 0;
    uint16_t        pbm_10g = PBM_10G;
    uint16_t        pbm_2p5g = PBM_2p5G;

    table_max_index = soc_robo2_gfcd_config_max_index(unit);

    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_get(unit, 0, &gfcd_config));
    for ( i = 0 ; i <= table_max_index ; i++ ) {
        gfcd_config.thresh0 = GFCD_XOFF_THRESH/8;
        gfcd_config.thresh1 = GFCD_RED_THRESH;
        gfcd_config.thresh2 = GFCD_YELLOW_THRESH;
        gfcd_config.thresh3 = GFCD_GREEN_THRESH;
        gfcd_config.hysteresis = GFCD_XON_THRESH/8;
        gfcd_config.counter_enable = 0;
        gfcd_config.tc_pp_bmp_0 = 0xff;
        gfcd_config.tc_pp_bmp_1 = 0xff;
        gfcd_config.tc_pp_bmp_2 = 0xff;
        gfcd_config.tc_pp_bmp_3 = 0xff;
        gfcd_config.tc_pp_bmp_4 = 0xff;
        gfcd_config.tc_pp_bmp_5 = 0xff;
        gfcd_config.tc_pp_bmp_6 = 0xff;
        gfcd_config.tc_pp_bmp_7 = 0xff;
        gfcd_config.tc_pp_bmp_8 = 0xff;
        gfcd_config.tc_pp_bmp_9 = 0xff;
        gfcd_config.tc_pp_bmp_10 = 0xff;
        gfcd_config.tc_pp_bmp_11 = 0xff;
        gfcd_config.tc_pp_bmp_12 = 0x0; // Do not flow control the cascade connections
        gfcd_config.tc_pp_bmp_13 = 0x0;
        gfcd_config.tc_pp_bmp_14 = 0xff;
        gfcd_config.tc_pp_bmp_15 = 0xff;
        gfcd_config.gfc_enable = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_set(unit,i,&gfcd_config,&status));
    }

    table_max_index = soc_robo2_fcd_config_max_index(unit);
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_config_get(unit, 0, &fcd_config));
    for ( i = 0 ; i <= table_max_index ; i++ ) {
        port = i / 8;
        /*
         * FCD threholds for BMU buffer count of 3924 and Jumbo packet
         * size of 9720.
         */
        if ( (pbm_2p5g & ( 0x1 << port )) > 0 ) {
            fcd_config.thresh0 = 253;
            fcd_config.thresh1 = 351;
            fcd_config.thresh2 = 351;
            fcd_config.thresh3 = 351;
            fcd_config.hysteresis = 39;
        } else if ((pbm_10g & ( 0x1 << port )) > 0 ) {
            fcd_config.thresh0 = 409;
            fcd_config.thresh1 = 507;
            fcd_config.thresh2 = 507;
            fcd_config.thresh3 = 507;
            fcd_config.hysteresis = 39;
        } else {
            fcd_config.thresh0 = 69;
            fcd_config.thresh1 = 167;
            fcd_config.thresh2 = 167;
            fcd_config.thresh3 = 167;
            fcd_config.hysteresis = 39;
        }
        fcd_config.tc_bmp = 0xff; // 1 << (i % 8);
#ifdef CONFIG_TIMESYNC
        fcd_config.counter_enable = 0;
#else
        fcd_config.counter_enable = 1;
#endif
        if (flowcontrol_mode == cbxFlowcontrolPause) {
            fcd_config.fc_enable = 1;
        } else {
            fcd_config.fc_enable = 0;
        }
        CBX_IF_ERROR_RETURN(soc_robo2_fcd_config_set(unit,i,&fcd_config,&status));
    }
    return CBX_E_NONE;
}

/**************************************************************************
 *                 CoSQ FSAL API IMPLEMENTATION                           *
 **************************************************************************/

/*
 * Function:
 *      cbx_cosq_init
 * Purpose:
 *      Initialize CoSQ module
 * Parameters:
 *      None.
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX -  Error
 */

int
cbx_cosq_init(void) {

    int       rv = CBX_E_NONE;
    uint32    base = CBXI_COSQ_EMPT_TC_TO_DSCP_BASE;
    int       port = 0;
    uint32    status = 0;
    int       unit, max_units;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_init()..\n")));

    max_units = (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) == 1) ? (2) : (1);
    for (unit = 0; unit < max_units; unit++) {

        /* Enable all the tables related to CoSQ module */
        rv = cbxi_cosq_table_init(unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_init()..Table init failed\n")));
            return rv;
        }
        /* Set the base for 8bit maps used for DSCP remap  */
        CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_EPP_EGRESS_PORT_MAP_BASEr(unit, &base));
        /* Enable all the 128 queues */
        rv = cbxi_cosq_queue_init(unit);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_init()..Queue init failed\n")));
        }
        rv = cbxi_cosq_admission_control_set(unit, cbxFlowcontrolNone);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_init().. \
                           cbxi_cosq_admission_control_set failed\n")));
        }
        SOC_IF_ERROR_RETURN(soc_robo2_max_config_enable(unit));
        SOC_IF_ERROR_RETURN(soc_robo2_max_config_reset(unit));
        max_config_t max_config;
        SOC_IF_ERROR_RETURN(soc_robo2_max_config_get(unit,0,&max_config));
        for ( port = 0 ; port <  16 ; port++ ) {
            max_config.max_tx_pkts  = 5;
            max_config.max_tx_bytes = 0x1200;
#ifdef CONFIG_PORT_EXTENDER
            if (port == CBX_PE_LB_PORT) {
                max_config.max_tx_pkts  = 20;
                max_config.max_tx_bytes = 0x1200;
            }
#endif

            SOC_IF_ERROR_RETURN(
                soc_robo2_max_config_set(unit, port, &max_config, &status));
        }
    }
    if (CBX_SUCCESS(rv)) {
        rv = cbx_cosq_flowcontrol_set(cbxFlowcontrolNone);
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_create
 * Purpose:
 *    This routine is used to create a cosq.
 * Parameters:
 *    cosq_params   (IN)  COSQ parameters.
 *    cosqid        (OUT) Handle of the cosq created
 *                            CBX_COSQ_INVALID: COSQ could not be created
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */
int
cbx_cosq_create ( cbx_cosq_params_t *cosq_params,
                  cbx_cosqid_t      *cosqid ) {

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_create()..\n")));
    return CBX_E_UNAVAIL;
}

/**
 * Function:
 *    cbx_cosq_destroy
 * Purpose:
 *    Destroy a cosq previously created by cbx_cosq_create()
 * Parameters:
 *    cosqid    Handle of the cosq to be destroyed
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 *
 */

int
cbx_cosq_destroy( cbx_cosqid_t cosqid ) {
    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_destroy()..\n")));
    return CBX_E_UNAVAIL;
}

/**
 * Function:
 *    cbx_cosq_port_default_tc_set
 * Purpose:
 *    This routine is used to set the default ingress traffic class on a port
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_default_tc_set( cbx_portid_t portid,
                              cbx_tc_t tc ) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    pgt_t          pgt_entry;
    uint32         status  = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_tc_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* validate int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp > CBXI_COSQ_MAX_INT_DP)) {
        return CBX_E_PARAM;
    }
    /* Get PGT entry */
    rv = soc_robo2_pgt_get(unit, lpg, &pgt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_tc_set()..failed  \
                                  to read pgt entry \n")));
        return rv;
    }
    /* update PGT entry */
    pgt_entry.default_tc = tc.int_pri;
    pgt_entry.default_dp = tc.int_dp;

    rv = soc_robo2_pgt_set(unit, lpg, &pgt_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_tc_set()..failed to  \
                                  write to pgt table \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_default_tc_get
 * Purpose:
 *    This routine is used to get the default ingress traffic class on a port
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_default_tc_get(cbx_portid_t portid,
                             cbx_tc_t *tc) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    pgt_t          pgt_entry;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_tc_get()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* Get PGT entry */
    rv = soc_robo2_pgt_get(unit, lpg, &pgt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_tc_set()..failed  \
                                  to read pgt entry \n")));
        return rv;
    }
    /* extract int pri and dp from pgt entry */
    tc->int_pri = pgt_entry.default_tc;
    tc->int_dp = pgt_entry.default_dp;
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_default_pcp_set
 * Purpose:
 *    This routine is used to set the default pcp,dei on a port
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */
int
cbx_cosq_port_default_pcp_set(cbx_portid_t portid,
                              int pcp,
                              int dei) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    etct_t         etct_entry;
    uint32         status  = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_pcp_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* validate pcp and dei */
    if ((pcp > CBXI_COSQ_MAX_PCP) || (dei > CBXI_COSQ_MAX_DEI)) {
        return CBX_E_PARAM;
    }
    /* Get ETCT entry */
    rv = soc_robo2_etct_get(unit, lpg, &etct_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_pcp_set()..failed  \
                                  to read etct entry \n")));
        return rv;
    }
    /* update ETCT entry */
    etct_entry.pcp = pcp;
    etct_entry.dei = dei;
    etct_entry.primap_ndx = 0;

    rv = soc_robo2_etct_set(unit, lpg, &etct_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_pcp_set()..failed to  \
                                  write to etct table \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_default_pcp_get
 * Purpose:
 *    This routine is used to get the default egress pcp,dei on a port
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    pcp      (OUT)  Priority Code Point (3 bit encoding)
 *    dei      (OUT)  Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_default_pcp_get(cbx_portid_t portid,
                              int *pcp,
                              int *dei) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    etct_t         etct_entry;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_pcp_get()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* Get ETCT entry */
    rv = soc_robo2_etct_get(unit, lpg, &etct_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_default_pcp_get()..failed  \
                                  to read etct entry \n")));
        return rv;
    }
    /* extract int pcp and dei from etct entry */
    *pcp = etct_entry.pcp;
    *dei = etct_entry.dei;
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_pcp2tc_set
 * Purpose:
 *    This routine is used to set the ingress mapping of {pcp,dei} to
 *    traffic class
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_pcp2tc_set(cbx_portid_t portid,
                          int pcp,
                          int dei,
                          cbx_tc_t tc) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    uint32         table_index = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_pcp2tc_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* validate tc - int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp > cbxDpDrop)) {
        return CBX_E_PARAM;
    }

    /* validate pcp and dei */
    if ((pcp > CBXI_COSQ_MAX_PCP) || (dei > CBXI_COSQ_MAX_DEI)) {
        return CBX_E_PARAM;
    }
    /* Compute CPMT table index */
    table_index = ((port_out * CBXI_COSQ_CPMT_ENTRIES_PER_PORT) +
                 ((pcp << CBXI_COSQ_PCP_SHIFT) | (dei << CBXI_COSQ_DEI_SHIFT)));

    /* Update the CPMT entry */
    rv = cbxi_cosq_cpmt_update(unit, table_index, tc);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_pcp2tc_set()..failed  \
                                  to update cpmt entry \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_pcp2tc_get
 * Purpose:
 *    This routine is used to get the ingress mapping of {pcp,dei} to
 *    traffic class
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_pcp2tc_get ( cbx_portid_t portid,
                           int pcp,
                           int dei,
                           cbx_tc_t *tc ) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    cpmt_t         cpmt_entry;
    int            table_index = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_pcp2tc_get()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));

    /* validate pcp and dei */
    if ((pcp > CBXI_COSQ_MAX_PCP) || (dei > CBXI_COSQ_MAX_DEI)) {
        return CBX_E_PARAM;
    }

    /* Compute CPMT table index */
    table_index = ((port_out * CBXI_COSQ_CPMT_ENTRIES_PER_PORT) +
                 ((pcp << CBXI_COSQ_PCP_SHIFT) | (dei << CBXI_COSQ_DEI_SHIFT)));

    /* Get CPMT entry */
    rv = soc_robo2_cpmt_get(unit, table_index, &cpmt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_pcp2tc_set()..failed  \
                                  to read cpmt entry \n")));
        return rv;
    }
    /* Extract TC and DP from entry */
    tc->int_pri = cpmt_entry.tc;
    tc->int_dp = cpmt_entry.dp;
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_tc2pcp_set
 * Purpose:
 *    This routine is used to set the egress mapping of traffic class
 *    to {pcpi,dei}
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    tc       (IN)  Traffic Class structure
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_tc2pcp_set(cbx_portid_t portid,
                          cbx_tc_t tc,
                          int pcp,
                          int dei) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    uint32         primap_index = 0;
    uint32         data = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* validate tc - int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp >= cbxDpDrop)) {
        return CBX_E_PARAM;
    }

    /* validate pcp and dei */
    if ((pcp > CBXI_COSQ_MAX_PCP) || (dei > CBXI_COSQ_MAX_DEI)) {
        return CBX_E_PARAM;
    }

    /* update ETCT entry - primap_index = 0 means take pcp and dei from etct.
       so we need to incrment the lpg by 1 while setting primap_index */
    rv = cbxi_cosq_etct_update(unit, lpg, lpg+1);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_set()..failed  \
                                  to update etct entry \n")));
        return rv;
    }
    /* get EPMT table index */
    primap_index = ((lpg * CBXI_COSQ_EPMT_ENTRIES_PER_PORT) + tc.int_dp);

    /* EPMT data */
    data = ((pcp << CBXI_COSQ_PCP_SHIFT) + (dei << CBXI_COSQ_DEI_SHIFT));

    /* Update the EPMT entry */
    rv = cbxi_cosq_epmt_update(unit, primap_index, data,
                               tc.int_pri, CBXI_COSQ_4BITS_MAP);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_set()..failed  \
                                  to update epmt entry \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_tc2pcp_get
 * Purpose:
 *    This routine is used to get the egress mapping of traffic class
 *    to {pcp,dei}
 * Parameters:
 *    portid   (IN)  PORT Identifier
 *    tc       (IN)  Traffic Class structure
 *    pcp      (OUT) Priority Code Point (3 bit encoding)
 *    dei      (OUT) Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_tc2pcp_get(cbx_portid_t portid,
                         cbx_tc_t tc,
                         int *pcp,
                         int *dei) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    uint32         primap_index = 0;
    uint32         pmt_data = 0;
    epmt_t         epmt_entry;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_get()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* validate tc - int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp > cbxDpDrop)) {
        return CBX_E_PARAM;
    }
    /* get EPMT table index */
    primap_index = ((lpg * CBXI_COSQ_EPMT_ENTRIES_PER_PORT) + tc.int_dp);

    /* Get EPMT data and extract pcp and dei from the data */
    rv = soc_robo2_epmt_get(unit, primap_index, &epmt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_get()..failed  \
                                  to read epmt entry \n")));
        return rv;
    }
    /* Extract pcp and dei from epmt data */
    pmt_data = (epmt_entry.pmt_data >> ((CBXI_COSQ_MAX_INT_PRI - tc.int_pri) * 4));
    *dei = ((pmt_data & CBXI_COSQ_DEI_MASK) >> CBXI_COSQ_DEI_SHIFT);
    *pcp  = ((pmt_data & CBXI_COSQ_PCP_MASK) >> CBXI_COSQ_PCP_SHIFT);
    return rv;
}

/**
 * Function:
 *    cbx_cosq_dscp2tc_set
 * Purpose:
 *    This routine is used to set the ingress mapping of dscp to traffic class
 * Parameters:
 *    dscp     (IN)  DiffServ structure
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_dscp2tc_set(cbx_dscp_t dscp,
                     cbx_tc_t   tc) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    uint32         table_index = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_dscp2tc_set()..\n")));

    /* validate tc - int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp >= cbxDpDrop)) {
        return CBX_E_PARAM;
    }

    /* validate dscp */
    if (dscp.dscp_cp > CBXI_COSQ_MAX_DSCP) {
        return CBX_E_PARAM;
    }

    /* Compute CPMT table index */
    if (dscp.is_ipv6) {
        table_index = (CBXI_COSQ_DSCPV6_TO_TC_BASE + dscp.dscp_cp);
    } else {
        table_index = (CBXI_COSQ_DSCPV4_TO_TC_BASE + dscp.dscp_cp);
    }

    /* Update the CPMT entry */
    rv = cbxi_cosq_cpmt_update(unit, table_index, tc);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_dscptc_set()..failed  \
                                  to update cpmt entry \n")));
        return rv;
    }
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        rv = cbxi_cosq_cpmt_update(unit, table_index, tc);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_port_dscptc_set()..failed  \
                                  to update cpmt entry \n")));
            return rv;
        }
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_dscp2tc_get
 * Purpose:
 *    This routine is used to get the ingress mapping of dscp to traffic class
 * Parameters:
 *    dscp     (IN)  DiffServ structure
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_dscp2tc_get(cbx_dscp_t  dscp,
                     cbx_tc_t   *tc ) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    int            table_index = 0;
    cpmt_t         cpmt_entry;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_dscp2tc_get()..\n")));

    /* validate dscp */
    if (dscp.dscp_cp > CBXI_COSQ_MAX_DSCP) {
        return CBX_E_PARAM;
    }
    /* Compute CPMT table index */
    if (dscp.is_ipv6) {
        table_index = (CBXI_COSQ_DSCPV6_TO_TC_BASE + dscp.dscp_cp);
    } else {
        table_index = (CBXI_COSQ_DSCPV4_TO_TC_BASE + dscp.dscp_cp);
    }
    /* Get CPMT entry */
    rv = soc_robo2_cpmt_get(unit, table_index, &cpmt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_dscp2tc_get()..failed  \
                                  to read cpmt entry \n")));
        return rv;
    }
    /* Extract TC and DP from entry */
    tc->int_pri = cpmt_entry.tc;
    tc->int_dp = cpmt_entry.dp;
    return rv;
}

/**
 * Function:
 *    cbx_cosq_tc2dscp_set
 * Purpose:
 *    This routine is used to set the egress mapping of traffic class to dscp
 * Parameters:
 *    dscp     (IN)  DiffServ structure
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_tc2dscp_set(cbx_tc_t   tc,
                     cbx_dscp_t dscp ) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    uint32         primap_index = 0;
    uint32         data = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_tc2dscp_set()..\n")));

    /* validate tc - int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp >= cbxDpDrop)) {
        return CBX_E_PARAM;
    }

    /* validate dscp */
    if (dscp.dscp_cp > CBXI_COSQ_MAX_DSCP) {
        return CBX_E_PARAM;
    }

    /* Compute ETCT table index base + TC[2] * 3 + DP[1:0] */
    primap_index = CBXI_COSQ_EMPT_TC_TO_DSCP_BASE;
    primap_index += (tc.int_dp + (((tc.int_pri & 0x4) >> 2) * 3));
    data = (dscp.dscp_cp << 2);

    /* Update the EPMT entry */
    rv = cbxi_cosq_epmt_update(unit, primap_index, data,
                               (tc.int_pri & 0x3), CBXI_COSQ_8BITS_MAP);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_set()..failed  \
                                  to update epmt entry \n")));
        return rv;
    }
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        rv = cbxi_cosq_epmt_update(unit, primap_index, data,
                               (tc.int_pri & 0x3), CBXI_COSQ_8BITS_MAP);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_port_tc2pcp_set()..failed  \
                                  to update epmt entry \n")));
            return rv;
        }
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_tc2dscp_get
 * Purpose:
 *    This routine is used to get the egress mapping of traffic class to dscp
 * Parameters:
 *    dscp     (IN)  DiffServ structure
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_tc2dscp_get(cbx_tc_t    tc,
                     cbx_dscp_t *dscp ) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    uint32         primap_index = 0;
    uint32         data = 0;
    epmt_t         epmt_entry;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_tc2dscp_get()..\n")));

    /* validate tc - int pri and dp */
    if ((tc.int_pri > CBXI_COSQ_MAX_INT_PRI) ||
                                      (tc.int_dp > cbxDpDrop)) {
        return CBX_E_PARAM;
    }
    /* Compute EPMT table base index */
    primap_index = CBXI_COSQ_EMPT_TC_TO_DSCP_BASE;
    primap_index += (tc.int_dp + (((tc.int_pri & 0x4) >> 2) * 3));
    /* Get EPMT data and extract pcp and dei from the data */
    rv = soc_robo2_epmt_get(unit, primap_index, &epmt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_tc2dscp_get()..failed  \
                                  to read epmt entry \n")));
        return rv;
    }
    /* Extract dscp from epmt entry */
    data = epmt_entry.pmt_data >> ((3 - (tc.int_pri & 0x3)) * 8);
    dscp->dscp_cp  = ((data >> 2) & CBXI_COSQ_MAX_DSCP);
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_get
 * Purpose:
 *    This routine is used to get the COSQ parameters of a port
 * Parameters:
 *    portid        (IN)  Port Identifier
 *    cosq_params   (OUT) COSQ parameters
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_get(cbx_portid_t       portid,
                  cbx_cosq_params_t *cosq_params) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    pgt_t          pgt_entry;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_get()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* Get PGT entry */
    rv = soc_robo2_pgt_get(unit, lpg, &pgt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_get()..failed  \
                                  to read pgt entry \n")));
        return rv;
    }
    if (pgt_entry.pepper & CBX_PORT_PEPPER_TRUST_PORT_TCDP_MASK) {
        cosq_params->en_tc = 0;
    } else {
        cosq_params->en_tc = 1;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_port_set
 * Purpose:
 *    This routine is used to set the COSQ parameters of a port
 * Parameters:
 *    portid        (IN)  Port Identifier
 *    cosq_params   (IN)  COSQ parameters
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_set(cbx_portid_t      portid,
                  cbx_cosq_params_t cosq_params) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    pgt_t          pgt_entry;
    uint32         status  = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* Get PGT entry */
    rv = soc_robo2_pgt_get(unit, lpg, &pgt_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_set()..failed  \
                                  to read pgt entry \n")));
        return rv;
    }
    /* update PGT entry */
    if ( 0 == cosq_params.en_tc) {
        pgt_entry.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
    } else {
        pgt_entry.pepper &= (~CBX_PORT_PEPPER_TRUST_PORT_TCDP_MASK);
    }

    rv = soc_robo2_pgt_set(unit, lpg, &pgt_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_set()..failed to  \
                                  write to pgt table \n")));
        return rv;
    }
    return rv;
}

/**
 * Initialize Shaper parameters.
 *
 * portid         Port Identifier
 * shaper_params  Parameter object
 */
void cbx_cosq_shaper_params_t_init( cbx_portid_t              portid,
                                    cbx_cosq_shaper_params_t *shaper_params )
{
    if (shaper_params != NULL) {
        sal_memset(shaper_params, 0, sizeof(cbx_cosq_shaper_params_t));
    }
    return;
}

/**
 * Function:
 *     cbx_cosq_port_sched_get
 * Purpose:
 *     This routine is used to get the COSQ scheduling of a port
 * Parameters:
 *     portid        (IN)  Port Identifier
 *     sched_mode    (OUT) COSQ Scheduling mode
 *     weights       (OUT) Array of scheduling weights
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */

int
cbx_cosq_port_sched_get(cbx_portid_t         portid,
                        cbx_schedule_mode_t *sched_mode,
                        int                  weights[CBX_COS_COUNT]) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_sched_get()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));

    /* Get scheduler mode */
    rv = cbxi_cosq_port_sched_op(unit, port_out, sched_mode, CBXI_COSQ_OP_GET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_sched_get()..failed to  \
                             get scheduler type\n")));
        return rv;
    }

    /* Get scheduler weight */
    rv = cbxi_cosq_sched_weight_op(unit, port_out, sched_mode,
                                   CBXI_COSQ_OP_GET, &weights[0]);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_sched_get()..failed to  \
                       set per queue weight for port scheduler \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *     cbx_cosq_port_sched_set
 * Purpose:
 *     This routine is used to set the COSQ scheduling of a port
 * Parameters:
 *     portid        (IN)  Port Identifier
 *     sched_mode    (IN)  COSQ Scheduling mode
 *     weights       (IN)  Array of scheduling weights
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */

int
cbx_cosq_port_sched_set(cbx_portid_t         portid,
                            cbx_schedule_mode_t  sched_mode,
                            const int            weights[CBX_COS_COUNT]) {

    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    qdschcfg_t     qdschcfg_entry;
    uint32         status = 0;
    int            weight[CBX_COS_COUNT];
    int            index = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_sched_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    /* PQM_QDSCHCFG - Set RR or SP - default is RR */
    if (sched_mode == cbxScheduleStrict) {
        /* One bit for each Queue Domain - 1 - SP, 0 - RR */
        qdschcfg_entry.sp_rr_sch = CBXI_COSQ_QDS_SCHEDULER_SP;
    } else {
        qdschcfg_entry.sp_rr_sch = CBXI_COSQ_QDS_SCHEDULER_RR;
    }
    rv = soc_robo2_qdschcfg_set(unit, port_out, &qdschcfg_entry, &status);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_sched_set()..failed to  \
                                  write to PQM_QDSCHCFG table \n")));
        return rv;
    }
    /* Set scheduler type */
    rv = cbxi_cosq_port_sched_op(unit, port_out, &sched_mode, CBXI_COSQ_OP_SET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_sched_set()..failed to  \
                             set scheduler type\n")));
        return rv;
    }

    /*Set scheduler weight - No weight for SP */
    if (sched_mode == cbxScheduleStrict) {
        return CBX_E_NONE;
    } else {
        for (index = 0; index < CBX_COS_COUNT; index++) {
            weight[index] = weights[index];
        }
        rv = cbxi_cosq_sched_weight_op(unit, port_out, &sched_mode,
                                       CBXI_COSQ_OP_SET,
                                       &weight[0]);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_port_sched_set()..failed to  \
                              set per queue weight for port scheduler \n")));
            return rv;
        }
    }
    return rv;
}

/**
 * Initialize Flow control parameters.
 *
 * portid              Port Identifier
 * flowcontrol_params  Parameter object
 */

void cbx_flowcontrol_params_t_init( cbx_portid_t              portid,
                                    cbx_flowcontrol_params_t *flowcontrol_params )
{
    if (flowcontrol_params != NULL) {
        sal_memset(flowcontrol_params, 0, sizeof(cbx_flowcontrol_params_t));
    }
    return;
}

/**
 * Function:
 *    cbx_cosq_flowcontrol_get
 * Purpose:
 *    This routine is used to get the global flowcontrol configuration
 * Parameters:
 *    flowcontrol_mode   (OUT)  Global Flow Control mode
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_flowcontrol_get(cbx_flowcontrol_t *flowcontrol_mode) {
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         index = 0;
    uint32         meter_config;
    mtr2tcb_t      entry;
    uint32         fval = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_flowcontrol_get()..\n")));

    if ( NULL == flowcontrol_mode ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_flowcontrol_get().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }
    CBX_IF_ERROR_RETURN(REG_READ_CB_BMU_MTR_CONFIGr(unit, &meter_config));
    soc_CB_BMU_MTR_CONFIGr_field_get(unit, &meter_config, METER_ID_SELf, &fval);
    if (fval == 0) {
        *flowcontrol_mode = cbxFlowcontrolPFC;
    } else if(fval == 1) {
        *flowcontrol_mode = cbxFlowcontrolMeter;
        CBX_IF_ERROR_RETURN(soc_robo2_mtr2tcb_get(unit, index, &entry));
        if ((entry.shp_en == 0) && (entry.tc_bmp == 0x0)) {
            *flowcontrol_mode = cbxFlowcontrolNone;
        }
    } else if(fval == 2) {
        *flowcontrol_mode = cbxFlowcontrolPause;
    } else {
        *flowcontrol_mode = cbxFlowcontrolNone;
    }
    return CBX_E_NONE;
}

/**
 * Function:
 *    cbx_cosq_flowcontrol_set
 * Purpose:
 *    This routine is used to set the global flowcontrol configuration
 *
 *    There are three options in Avenger:
 *    1) Source Port based pause packets
 *      programming:
 *       PGT.fcd   set all fcd 0-7 to same value to have one count
 *       thresdholds and hysteresis in BMU_fcd_config
 *       scale: 128 of these, but only 16 (one per port) are used
 *
 *    2) PFC based on traffic class - global for all port together
 *      programming:
 *       PGT.fcd   set fcd 0-7 to traffic class 0-7
 *       thresholds and hysteresis in BMU_gfcd_config
 *       scale: 8 of these
 *    3) BMU Meters to shape per port flowcontrol
 * Parameters:
 *    flowcontrol_mode   (IN)  Global Flow Control mode
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_flowcontrol_set(cbx_flowcontrol_t flowcontrol_mode) {

    int           unit = CBX_AVENGER_PRIMARY;
    int           rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_flowcontrol_set()..\n")));

    rv = cbxi_cosq_flowcontrol_set(unit, flowcontrol_mode);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_flowcontrol_set()..failed to  \
                          set flow control mode  \n")));
        return rv;
    }
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        rv = cbxi_cosq_flowcontrol_set(unit, flowcontrol_mode);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_COSQ,
                (BSL_META("FSAL API : cbx_cosq_flowcontrol_set()..failed to  \
                              set flow control mode  \n")));
            return rv;
        }
    }
    return CBX_E_NONE;
}

/**
 * Function:
 *    cbx_cosq_port_flowcontrol_get
 * Purpose:
 *    This routine is used to get the port flowcontrol configuration
 * Parameters:
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 *    portid        (IN)  Port Identifier
 *    fc_params     (IN)  Flow Control parameters
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_flowcontrol_get(cbx_portid_t             portid,
                              cbx_flowcontrol_params_t *fc_params) {

    int            unit = CBX_AVENGER_PRIMARY;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    fcd_config_t   fcd_config;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_flowcontrol_get()..\n")));

    if ( NULL == fc_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_flowcontrol_get().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }
    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));

    port_out = lpg;  /* we need to use PG i.s.o pp */
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_config_get(unit, port_out * 8, &fcd_config));
    fc_params->thresh0 = fcd_config.thresh0 ;
    fc_params->thresh1= fcd_config.thresh1;
    fc_params->thresh2 = fcd_config.thresh2;
    fc_params->thresh3 = fcd_config.thresh3;
    fc_params->hysteresis = fcd_config.hysteresis;
    fc_params->enable = fcd_config.fc_enable;
    return CBX_E_NONE;
}

/**
 * Function:
 *    cbx_cosq_port_flowcontrol_set
 * Purpose:
 *    This routine is used to set the port flowcontrol configuration
 * Parameters:
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 *    portid        (IN)  Port Identifier
 *    fc_params     (IN)  Flow Control parameters
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_flowcontrol_set(cbx_portid_t              portid,
                              cbx_flowcontrol_params_t  fc_params) {

    int            unit = CBX_AVENGER_PRIMARY;
    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    fcd_config_t   fcd_config;
    uint32         status = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_flowcontrol_set()..\n")));

    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    if ((fc_params.thresh0 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh1 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh2 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh3 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh2 > CBXI_COSQ_THREHOLD_MAX)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_flowcontrol_set().. \
                          Threashold passed is more than max  \n")));
        return CBX_E_PARAM;
    }
    fcd_config.thresh0 = fc_params.thresh0;
    fcd_config.thresh1 = fc_params.thresh1;
    fcd_config.thresh2 = fc_params.thresh2;
    fcd_config.thresh3 = fc_params.thresh3;
    fcd_config.hysteresis = fc_params.hysteresis;
    fcd_config.fc_enable = fc_params.enable;
    fcd_config.counter_enable = fc_params.enable;
    fcd_config.tc_bmp = CBXI_COSQ_PORT_FLOW_CONTROL_TC_BMP; /* 0xFF */

    port_out = lpg;  /* we need to use PG i.s.o pp */
    CBX_IF_ERROR_RETURN(soc_robo2_fcd_config_set(unit, port_out * 8,
                                                 &fcd_config, &status));
    return CBX_E_NONE;

}

/**
 * Function:
 *    cbx_cosq_pfc_flowcontrol_get
 * Purpose:
 *    This routine is used to get the PFC flowcontrol configuration
 *
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 * Parameters:
 *    int_pri       (IN)  Internal traffic class priority
 *    fc_params     (IN)  Flow Control parameters
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_pfc_flowcontrol_get(int                       int_pri,
                             cbx_flowcontrol_params_t *fc_params) {
    gfcd_config_t   gfcd_config;
    int             unit = CBX_AVENGER_PRIMARY;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_pfc_flowcontrol_get()..\n")));

    if ( NULL == fc_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_pfc_flowcontrol_get().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }
    if (int_pri > CBXI_COSQ_MAX_INT_PRI) {
        return CBX_E_PARAM;
    }
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_get(unit, int_pri, &gfcd_config));
    fc_params->thresh0 = gfcd_config.thresh0 ;
    fc_params->thresh1= gfcd_config.thresh1;
    fc_params->thresh2 = gfcd_config.thresh2;
    fc_params->thresh3 = gfcd_config.thresh3;
    fc_params->hysteresis = gfcd_config.hysteresis;
    fc_params->enable = gfcd_config.gfc_enable;
    return CBX_E_NONE;

}

/**
 * Function:
 *    cbx_cosq_pfc_flowcontrol_set
 * Purpose:
 *    This routine is used to set the PFC flowcontrol configuration
 *
 * Parameters:
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 *    int_pri       (IN)  Internal traffic class priority
 *    fc_params     (IN)  Flow Control parameters
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_pfc_flowcontrol_set(int                       int_pri,
                             cbx_flowcontrol_params_t  fc_params) {

    int           unit = CBX_AVENGER_PRIMARY;
    gfcd_config_t gfcd_entry;
    uint8         bmp;
    uint32        status = 0;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_pfc_flowcontrol_set()..\n")));

    if (int_pri > CBXI_COSQ_MAX_INT_PRI) {
        return CBX_E_PARAM;
    }
    if ((fc_params.thresh0 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh1 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh2 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh3 > CBXI_COSQ_THREHOLD_MAX) ||
        (fc_params.thresh2 > CBXI_COSQ_THREHOLD_MAX)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_flowcontrol_set().. \
                          Threashold passed is more than max  \n")));
        return CBX_E_PARAM;
    }
    bmp = (1 << int_pri);
    gfcd_entry.gfc_enable = fc_params.enable;
    gfcd_entry.counter_enable = fc_params.enable;
    gfcd_entry.tc_pp_bmp_0 = bmp;
    gfcd_entry.tc_pp_bmp_1 = bmp;
    gfcd_entry.tc_pp_bmp_2 = bmp;
    gfcd_entry.tc_pp_bmp_3 = bmp;
    gfcd_entry.tc_pp_bmp_4 = bmp;
    gfcd_entry.tc_pp_bmp_5 = bmp;
    gfcd_entry.tc_pp_bmp_6 = bmp;
    gfcd_entry.tc_pp_bmp_7 = bmp;
    gfcd_entry.tc_pp_bmp_8 = bmp;
    gfcd_entry.tc_pp_bmp_9 = bmp;
    gfcd_entry.tc_pp_bmp_10 = bmp;
    gfcd_entry.tc_pp_bmp_11 = bmp;
    gfcd_entry.tc_pp_bmp_12 = bmp;
    gfcd_entry.tc_pp_bmp_13 = bmp;
    gfcd_entry.tc_pp_bmp_14 = bmp;
    gfcd_entry.tc_pp_bmp_15 = bmp;
    gfcd_entry.thresh0 = fc_params.thresh0;
    gfcd_entry.thresh1 = fc_params.thresh1;
    gfcd_entry.thresh2 = fc_params.thresh2;
    gfcd_entry.thresh3 = fc_params.thresh3;
    gfcd_entry.hysteresis = fc_params.hysteresis;
    CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_set(unit, int_pri,
                                                  &gfcd_entry,&status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        CBX_IF_ERROR_RETURN(soc_robo2_gfcd_config_set(unit,int_pri,
                                                      &gfcd_entry,&status));
    }
    return CBX_E_NONE;
}

/**
 * Function:
 *    cbx_cosq_port_shaper_get
 * Purpose:
 *    This routine is used to get an egress shaper on a port.
 *
 * Parameters:
 *    portid         (IN)  PORT Identifier
 *    shaper_params  (OUT) Shaper parameters.
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_shaper_get(cbx_portid_t             portid,
                         cbx_cosq_shaper_params_t *shaper_params) {

    int            rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_get()..\n")));

    if ( NULL == shaper_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_get().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }

    /* Every port has 9 shapers. First 8(0-7) queue level and the last
       one(8) is per port shaper.
     */
    rv = cbxi_cosq_port_shaper_op(portid, CBXI_COSQ_PORT_SHAPER_INDEX,
                                  shaper_params, CBXI_COSQ_OP_GET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_get()..failed to  \
                          get shaper parameters  \n")));
        return rv;
    }
    return rv;
 }


/**
 * Function:
 *    cbx_cosq_port_shaper_set
 * Purpose:
 *    This routine is used to set an egress shaper on a port.
 *
 * Parameters:
 *    portid         (IN)  PORT Identifier
 *    shaper_params  (IN)  Shaper parameters.
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_port_shaper_set(cbx_portid_t             portid,
                         cbx_cosq_shaper_params_t *shaper_params) {

    int            rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_set()..\n")));

    if ( NULL == shaper_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_set().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }

    /* Every port has 9 shapers. First 8(0-7) queue level and the last
       one(8) is per port shaper.
     */
    rv = cbxi_cosq_port_shaper_op(portid, CBXI_COSQ_PORT_SHAPER_INDEX,
                                  shaper_params, CBXI_COSQ_OP_SET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_port_shaper_set()..failed to  \
                          set shaper parameters  \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_queue_shaper_get
 * Purpose:
 *    This routine is used to get an egress shaper on a qos queue.
 *
 * Parameters:
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    shaper_params  (OUT) Shaper parameters.
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_queue_shaper_get(cbx_portid_t             portid,
                          int                      int_pri,
                          cbx_cosq_shaper_params_t *shaper_params) {

    int            rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_get()..\n")));

    if ( NULL == shaper_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_get().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }

    if ((int_pri > CBXI_COSQ_MAX_INT_PRI) || (int_pri < 0)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_get().. \
                          invalid int pri  \n")));
        return CBX_E_PARAM;
    }
    /* Every port has 9 shapers. First 8(0-7) queue level and the last
       one(8) is per port shaper.
     */
    rv = cbxi_cosq_port_shaper_op(portid, int_pri,
                                  shaper_params, CBXI_COSQ_OP_GET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_get()..failed to  \
                          get shaper parameters  \n")));
        return rv;
    }
    return rv;
 }

/**
 * Function:
 *    cbx_cosq_queue_shaper_set
 * Purpose:
 *    This routine is used to set an egress shaper on a qos queue.
 *
 * Parameters:
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    shaper_params  (IN)  Shaper parameters.
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

int
cbx_cosq_queue_shaper_set(cbx_portid_t             portid,
                          int                      int_pri,
                          cbx_cosq_shaper_params_t *shaper_params) {

    int            rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_set()..\n")));

    if ( NULL == shaper_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_set().. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }
    if ((int_pri > CBXI_COSQ_MAX_INT_PRI) || (int_pri < 0)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_set().. \
                          invalid int pri  \n")));
        return CBX_E_PARAM;
    }
    /* Every port has 9 shapers. First 8(0-7) queue level and the last
       one(8) is per port shaper.
     */
    rv = cbxi_cosq_port_shaper_op(portid, int_pri,
                                  shaper_params, CBXI_COSQ_OP_SET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_shaper_set()..failed to  \
                          set shaper parameters  \n")));
        return rv;
    }
    return rv;
}

#ifdef CONFIG_TIMESYNC
/*
 * Function:
 *  cbxi_cosq_tsn_validate_params
 * Purpose:
 *  Validates TSN config parameters
 *
 */
int
cbxi_cosq_tsn_validate_params(uint8 cfg_exist,
                              int tick_period,
                              int sync_src,
                              cbx_cosq_tsn_params_t *tsn_params) {
    uint32 tick = tsn_params->tick_period;
    uint32 tick_max;
    uint32 duration_max;

    if (cfg_exist && (tick != tick_period)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set.. \
                      tick period is different from previous cfg value\n")));
        return CBX_E_PARAM;
    }
    if (cfg_exist && (tsn_params->sync_source != sync_src)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set.. \
                      sync source is different from previous cfg value\n")));
        return CBX_E_PARAM;
    }
    if ((tsn_params->sync_source != CBX_TS_GEN_ID_TAS_1588_SYNC_A) &&
        (tsn_params->sync_source != CBX_TS_GEN_ID_TAS_1588_SYNC_B)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set.. \
                      invalid sync source value\n")));
        return CBX_E_PARAM;
    }
    if ((tsn_params->start_state != cbxTsnStateGb) &&
        (tsn_params->start_state != cbxTsnStateHp) &&
        (tsn_params->start_state != cbxTsnStateLp)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set.. \
                      invalid start state\n")));
        return CBX_E_PARAM;
    }
    /* tick period is 10 bits */
    tick_max = 1 << 10;
    /* durations are 12 bits */
    duration_max = 1 << 12;
    if ((tick == 0) || (tick >= tick_max)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set.. \
                      invalid tick period value\n")));
        return CBX_E_PARAM;
    }
    if ((tsn_params->start_duration >= duration_max) ||
        (tsn_params->lp_duration >= duration_max) ||
        (tsn_params->hp_duration >= duration_max) ||
        (tsn_params->gb_duration >= duration_max)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set.. \
                      invalid duration values\n")));
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/**
 * Initialize TSN parameters.
 *
 * tsn_params  TSN Parameter object
 */
void cbx_cosq_tsn_params_t_init(cbx_cosq_tsn_params_t *tsn_params) {
    if (tsn_params != NULL) {
        sal_memset(tsn_params, 0, sizeof(cbx_cosq_tsn_params_t));
    }
    return;
}

/*
 * Function:
 *  cbxi_cosq_port_tsn_op
 * Purpose:
 *  Get/Set TSN config registers
 *
 */
int
cbxi_cosq_port_tsn_op(cbx_portid_t portid,
                      int int_pri,
                      cbx_cosq_tsn_params_t *tsn_params,
                      int flag) {

    cbx_port_t     port_out;
    cbxi_pgid_t    lpg;
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         reg_val;
    uint32         cfg1_reg_val;
    uint32         status1_reg_val;
    uint32         fval;
    uint32         start_dur_port_regs = 0;
    uint32         low_dur_port_regs = 0;
    uint32         high_dur_port_regs = 0;
    uint32         guard_dur_port_regs = 0;
    uint32         sync_sel;
    uint32         tick_period = 0;
    uint32         tas_en;
    uint8          cfg_exist = 0;
    uint32         current_cfg = 0;
    uint32 cfg0_port_regs[] = {
        CB_PQS_PQS_TAS_CONFIG0_A0r,
        CB_PQS_PQS_TAS_CONFIG0_A1r,
        CB_PQS_PQS_TAS_CONFIG0_A2r,
        CB_PQS_PQS_TAS_CONFIG0_A3r,
        CB_PQS_PQS_TAS_CONFIG0_A4r,
        CB_PQS_PQS_TAS_CONFIG0_A5r,
        CB_PQS_PQS_TAS_CONFIG0_A6r,
        CB_PQS_PQS_TAS_CONFIG0_A7r,
        CB_PQS_PQS_TAS_CONFIG0_A8r,
        CB_PQS_PQS_TAS_CONFIG0_A9r,
        CB_PQS_PQS_TAS_CONFIG0_A10r,
        CB_PQS_PQS_TAS_CONFIG0_A11r,
        CB_PQS_PQS_TAS_CONFIG0_A12r,
        CB_PQS_PQS_TAS_CONFIG0_A13r,
        CB_PQS_PQS_TAS_CONFIG0_A14r,
        CB_PQS_PQS_TAS_CONFIG0_A15r
    };
    uint32 cfg1_port_regs[] = {
        CB_PQS_PQS_TAS_CONFIG1_A0r,
        CB_PQS_PQS_TAS_CONFIG1_A1r,
        CB_PQS_PQS_TAS_CONFIG1_A2r,
        CB_PQS_PQS_TAS_CONFIG1_A3r,
        CB_PQS_PQS_TAS_CONFIG1_A4r,
        CB_PQS_PQS_TAS_CONFIG1_A5r,
        CB_PQS_PQS_TAS_CONFIG1_A6r,
        CB_PQS_PQS_TAS_CONFIG1_A7r,
        CB_PQS_PQS_TAS_CONFIG1_A8r,
        CB_PQS_PQS_TAS_CONFIG1_A9r,
        CB_PQS_PQS_TAS_CONFIG1_A10r,
        CB_PQS_PQS_TAS_CONFIG1_A11r,
        CB_PQS_PQS_TAS_CONFIG1_A12r,
        CB_PQS_PQS_TAS_CONFIG1_A13r,
        CB_PQS_PQS_TAS_CONFIG1_A14r,
        CB_PQS_PQS_TAS_CONFIG1_A15r
    };
    uint32 q6s_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q6_S_DURATION_A0r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A1r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A2r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A3r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A4r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A5r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A6r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A7r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A8r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A9r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A10r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A11r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A12r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A13r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A14r,
        CB_PQS_PQS_TAS_Q6_S_DURATION_A15r
    };
    uint32 q7s_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q7_S_DURATION_A0r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A1r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A2r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A3r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A4r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A5r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A6r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A7r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A8r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A9r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A10r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A11r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A12r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A13r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A14r,
        CB_PQS_PQS_TAS_Q7_S_DURATION_A15r
    };
    uint32 q6l_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q6_L_DURATION_A0r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A1r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A2r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A3r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A4r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A5r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A6r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A7r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A8r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A9r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A10r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A11r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A12r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A13r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A14r,
        CB_PQS_PQS_TAS_Q6_L_DURATION_A15r
    };
    uint32 q7l_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q7_L_DURATION_A0r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A1r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A2r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A3r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A4r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A5r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A6r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A7r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A8r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A9r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A10r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A11r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A12r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A13r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A14r,
        CB_PQS_PQS_TAS_Q7_L_DURATION_A15r
    };
    uint32 q6p_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q6_P_DURATION_A0r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A1r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A2r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A3r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A4r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A5r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A6r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A7r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A8r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A9r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A10r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A11r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A12r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A13r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A14r,
        CB_PQS_PQS_TAS_Q6_P_DURATION_A15r
    };
    uint32 q7p_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q7_P_DURATION_A0r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A1r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A2r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A3r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A4r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A5r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A6r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A7r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A8r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A9r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A10r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A11r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A12r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A13r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A14r,
        CB_PQS_PQS_TAS_Q7_P_DURATION_A15r
    };
    uint32 q6g_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q6_G_DURATION_A0r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A1r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A2r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A3r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A4r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A5r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A6r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A7r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A8r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A9r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A10r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A11r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A12r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A13r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A14r,
        CB_PQS_PQS_TAS_Q6_G_DURATION_A15r
    };
    uint32 q7g_dur_port_regs[] = {
        CB_PQS_PQS_TAS_Q7_G_DURATION_A0r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A1r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A2r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A3r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A4r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A5r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A6r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A7r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A8r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A9r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A10r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A11r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A12r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A13r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A14r,
        CB_PQS_PQS_TAS_Q7_G_DURATION_A15r
    };
    uint32 status1_regs[] = {
        CB_PQS_PQS_TAS_STATUS1_A0r,
        CB_PQS_PQS_TAS_STATUS1_A1r,
        CB_PQS_PQS_TAS_STATUS1_A2r,
        CB_PQS_PQS_TAS_STATUS1_A3r,
        CB_PQS_PQS_TAS_STATUS1_A4r,
        CB_PQS_PQS_TAS_STATUS1_A5r,
        CB_PQS_PQS_TAS_STATUS1_A6r,
        CB_PQS_PQS_TAS_STATUS1_A7r,
        CB_PQS_PQS_TAS_STATUS1_A8r,
        CB_PQS_PQS_TAS_STATUS1_A9r,
        CB_PQS_PQS_TAS_STATUS1_A10r,
        CB_PQS_PQS_TAS_STATUS1_A11r,
        CB_PQS_PQS_TAS_STATUS1_A12r,
        CB_PQS_PQS_TAS_STATUS1_A13r,
        CB_PQS_PQS_TAS_STATUS1_A14r,
        CB_PQS_PQS_TAS_STATUS1_A15r
    };

    if ( NULL == tsn_params ) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_get/set.. \
                          NULL pointer passed  \n")));
        return CBX_E_PARAM;
    }

    if ((int_pri != (CBXI_COSQ_MAX_INT_PRI - 1)) &&
        (int_pri != CBXI_COSQ_MAX_INT_PRI)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_get/set.. \
                          TAS not supported on int pri  \n")));
        /* TAS is only supported on Q6 and Q7 */
        return CBX_E_PARAM;
    }
    if (CBX_PORTID_IS_TRUNK(portid) || CBX_PORTID_IS_VLAN_PORT(portid) ||
        CBX_PORTID_IS_MCAST(portid)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_get/set..Invalid port  \
                          type passed...only physical port supported  \n")));
        return CBX_E_PARAM;
    }
    /* Validate portid */
    CBX_IF_ERROR_RETURN(cbxi_cosq_portid_validate(portid, &port_out,
                                                  &lpg, &unit));
    if ((tsn_params->cfg_id != 0) && (tsn_params->cfg_id != 1)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_get/set..Invalid  \
                          TSN configuration id \n")));
        return CBX_E_PARAM;
    }
    if (int_pri == 6) {
        start_dur_port_regs = q6s_dur_port_regs[port_out];
        low_dur_port_regs = q6l_dur_port_regs[port_out];
        high_dur_port_regs = q6p_dur_port_regs[port_out];
        guard_dur_port_regs = q6g_dur_port_regs[port_out];
    } else if (int_pri == 7) {
        start_dur_port_regs = q7s_dur_port_regs[port_out];
        low_dur_port_regs = q7l_dur_port_regs[port_out];
        high_dur_port_regs = q7p_dur_port_regs[port_out];
        guard_dur_port_regs = q7g_dur_port_regs[port_out];
    }

    if (flag == CBXI_COSQ_OP_SET) {
        CBX_IF_ERROR_RETURN(
            ROBO2_REG_READ_WITH_ID(unit, cfg0_port_regs[port_out],
                                   REG_PORT_ANY, 0, &reg_val));
        CBX_IF_ERROR_RETURN(
            ROBO2_REG_READ_WITH_ID(unit, cfg1_port_regs[port_out],
                                   REG_PORT_ANY, 0, &cfg1_reg_val));
        CBX_IF_ERROR_RETURN(
            ROBO2_REG_READ_WITH_ID(unit, status1_regs[port_out],
                                   REG_PORT_ANY, 0, &status1_reg_val));
        if (int_pri == 6) {
            soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_get(unit, &cfg1_reg_val,
                                                     Q7_TAS_ENf, &tas_en);
            soc_CB_PQS_PQS_TAS_STATUS1_A0r_field_get(unit, &status1_reg_val,
                                              Q7_CURRENT_CONFIGf, &current_cfg);
            if ((tas_en) && (tsn_params->cfg_id == current_cfg)) {
                cfg_exist = 1;
            }
        } else if (int_pri == 7) {
            soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_get(unit, &cfg1_reg_val,
                                                     Q6_TAS_ENf, &tas_en);
            soc_CB_PQS_PQS_TAS_STATUS1_A0r_field_get(unit, &status1_reg_val,
                                              Q6_CURRENT_CONFIGf, &current_cfg);
            if ((tas_en) && (tsn_params->cfg_id == current_cfg)) {
                cfg_exist = 1;
            }
        }
        if (tsn_params->cfg_id == 0) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                  TICK_PERIOD_0f, &tick_period);
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                   SYNC_SEL_1588_0f, &sync_sel);
        } else if (tsn_params->cfg_id == 1) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                  TICK_PERIOD_1f, &tick_period);
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                   SYNC_SEL_1588_1f, &sync_sel);
        }
        CBX_IF_ERROR_RETURN(
            cbxi_cosq_tsn_validate_params(cfg_exist, tick_period,
                                          sync_sel, tsn_params));
        tick_period = tsn_params->tick_period;
        if (tsn_params->sync_source == CBX_TS_GEN_ID_TAS_1588_SYNC_A) {
            fval = 0;
        } else if (tsn_params->sync_source == CBX_TS_GEN_ID_TAS_1588_SYNC_B) {
            fval = 1;
        }
        if (tsn_params->cfg_id == 0) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     SYNC_SEL_1588_0f, &fval);
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                  TICK_PERIOD_0f, &tick_period);
        } else if (tsn_params->cfg_id == 1) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     SYNC_SEL_1588_1f, &fval);
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     TICK_PERIOD_1f, &tick_period);
        }

        fval = tsn_params->start_state;
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     Q6_START_PHASE_0f, &fval);
        } else if((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     Q7_START_PHASE_0f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     Q6_START_PHASE_1f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_set(unit, &reg_val,
                                                     Q7_START_PHASE_1f, &fval);
        }
        /* write to the config0 register */
        CBX_IF_ERROR_RETURN(
            ROBO2_REG_WRITE_WITH_ID(unit, cfg0_port_regs[port_out],
                                    REG_PORT_ANY, 0, &reg_val));

        fval = tsn_params->start_duration;
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, start_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_S_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_S_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_S_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_S_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_S_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_S_DURATION_1f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_S_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_S_DURATION_1f, &fval);
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, start_dur_port_regs,
                                                    REG_PORT_ANY, 0, &reg_val));

        fval = tsn_params->lp_duration;
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, low_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_L_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_L_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_L_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_L_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_L_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_L_DURATION_1f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_L_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_L_DURATION_1f, &fval);
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, low_dur_port_regs,
                                                    REG_PORT_ANY, 0, &reg_val));

        fval = tsn_params->hp_duration;
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, high_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_P_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_P_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_P_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_P_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_P_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_P_DURATION_1f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_P_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_P_DURATION_1f, &fval);
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, high_dur_port_regs,
                                                    REG_PORT_ANY, 0, &reg_val));

        fval = tsn_params->gb_duration;
        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, guard_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_G_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_G_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_G_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_G_DURATION_0f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_G_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q6_G_DURATION_1f, &fval);
        } else if((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_G_DURATION_A0r_field_set(unit, &reg_val,
                                                   TAS_Q7_G_DURATION_1f, &fval);
        }
        CBX_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, guard_dur_port_regs,
                                                    REG_PORT_ANY, 0, &reg_val));

        if (tsn_params->flags & CBX_COSQ_TSN_QUEUE_CONFIG_ENABLE) {
            /* If enabled, start cycle timer and duration fsm */
            fval = 1;
            if (int_pri == 6) {
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q6_TAS_ENf, &fval);
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q6_ARM_Df, &fval);
            } else if (int_pri == 7) {
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q7_TAS_ENf, &fval);
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q7_ARM_Df, &fval);
            }
            soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                     ARM_CTf, &fval);
            if (tsn_params->flags & CBX_COSQ_TSN_CT_CONFIG_ONE_SHOT) {
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         CLEAR_ARMf, &fval);
            } else {
                fval = 0;
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         CLEAR_ARMf, &fval);
            }
            if (tsn_params->flags & CBX_COSQ_TSN_CONFIG_CHANGE) {
                fval = 1;
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         ARM_CCf, &fval);
                fval = tsn_params->cfg_id;
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         NEXT_CONFIGf, &fval);
            }
        } else {
            /* If disabled, stop cycle timer and duration fsm */
            fval = 0;
            if (int_pri == 6) {
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q6_TAS_ENf, &fval);
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q6_ARM_Df, &fval);
            } else if (int_pri == 7) {
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q7_TAS_ENf, &fval);
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         Q7_ARM_Df, &fval);
            }
            soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                     ARM_CTf, &fval);
            if (tsn_params->flags & CBX_COSQ_TSN_CT_CONFIG_ONE_SHOT) {
                fval = 1;
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         CLEAR_ARMf, &fval);
            } else {
                fval = 0;
                soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_set(unit, &cfg1_reg_val,
                                                         CLEAR_ARMf, &fval);
            }
        }
        /* write to the config1 register */
        CBX_IF_ERROR_RETURN(
            ROBO2_REG_WRITE_WITH_ID(unit, cfg1_port_regs[port_out],
                                    REG_PORT_ANY, 0, &cfg1_reg_val));
    } else if (flag == CBXI_COSQ_OP_GET) {
        CBX_IF_ERROR_RETURN(
            ROBO2_REG_READ_WITH_ID(unit, cfg0_port_regs[port_out],
                                   REG_PORT_ANY, 0, &reg_val));
        if (tsn_params->cfg_id == 0) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                   SYNC_SEL_1588_0f, &sync_sel);
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                  TICK_PERIOD_0f, &tick_period);
        } else if (tsn_params->cfg_id == 1) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                   SYNC_SEL_1588_1f, &sync_sel);
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                  TICK_PERIOD_1f, &tick_period);
        }
        if (sync_sel == 0) {
            tsn_params->sync_source = CBX_TS_GEN_ID_TAS_1588_SYNC_A;
        } else if (sync_sel == 1) {
            tsn_params->sync_source = CBX_TS_GEN_ID_TAS_1588_SYNC_B;
        }
        tsn_params->tick_period = tick_period;
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                     Q6_START_PHASE_0f, &fval);
        } else if ((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                     Q7_START_PHASE_0f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                     Q6_START_PHASE_1f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_CONFIG0_A0r_field_get(unit, &reg_val,
                                                     Q7_START_PHASE_1f, &fval);
        }
        tsn_params->start_state = fval;

        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, start_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_S_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_S_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_S_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_S_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_S_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_S_DURATION_1f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_S_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_S_DURATION_1f, &fval);
        }
        tsn_params->start_duration = fval;

        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, low_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_L_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_L_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_L_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_L_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_L_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_L_DURATION_1f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_L_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_L_DURATION_1f, &fval);
        }
        tsn_params->lp_duration = fval;

        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, high_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_P_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_P_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_P_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_P_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_P_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_P_DURATION_1f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_P_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_P_DURATION_1f, &fval);
        }
        tsn_params->hp_duration = fval;

        CBX_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, guard_dur_port_regs,
                                                   REG_PORT_ANY, 0, &reg_val));
        if ((tsn_params->cfg_id == 0) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_G_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_G_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 0) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_G_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_G_DURATION_0f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 6)) {
            soc_CB_PQS_PQS_TAS_Q6_G_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q6_G_DURATION_1f, &fval);
        } else if ((tsn_params->cfg_id == 1) && (int_pri == 7)) {
            soc_CB_PQS_PQS_TAS_Q7_G_DURATION_A0r_field_get(unit, &reg_val,
                                                  TAS_Q7_G_DURATION_1f, &fval);
        }
        tsn_params->gb_duration = fval;

        CBX_IF_ERROR_RETURN(
            ROBO2_REG_READ_WITH_ID(unit, cfg1_port_regs[port_out],
                                   REG_PORT_ANY, 0, &reg_val));
        if (int_pri == 6) {
            soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_get(unit, &reg_val,
                                                     Q6_TAS_ENf, &tas_en);
        } else if (int_pri == 7) {
            soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_get(unit, &reg_val,
                                                     Q7_TAS_ENf, &tas_en);
        }
        if (tas_en == 1) {
            tsn_params->flags |= CBX_COSQ_TSN_QUEUE_CONFIG_ENABLE;
        }
        soc_CB_PQS_PQS_TAS_CONFIG1_A0r_field_get(unit, &reg_val,
                                                 CLEAR_ARMf, &fval);
        if (fval == 1) {
            tsn_params->flags |= CBX_COSQ_TSN_CT_CONFIG_ONE_SHOT;
        }
    } else {
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/**
 * Function:
 *    cbx_cosq_tsn_config_set
 * Purpose:
 *    This routine is used to set an egress TSN parameters on a qos queue.
 *
 * Parameters:
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    tsn_params     (IN)  TSN parameters.
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */
int
cbx_cosq_queue_tsn_config_set(cbx_portid_t portid,
                              int int_pri,
                              cbx_cosq_tsn_params_t *tsn_params) {

    int rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set..\n")));
    rv = cbxi_cosq_port_tsn_op(portid, int_pri, tsn_params, CBXI_COSQ_OP_SET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_set..failed to  \
                          set TSN parameters  \n")));
        return rv;
    }
    return rv;
}

/**
 * Function:
 *    cbx_cosq_queue_tsn_config_get
 * Purpose:
 *    This routine is used to get an egress TSN parameters on a qos queue.
 *
 * Parameters:
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    tsn_params     (OUT) TSN parameters.
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */
int
cbx_cosq_queue_tsn_config_get(cbx_portid_t portid,
                              int int_pri,
                              cbx_cosq_tsn_params_t *tsn_params) {

    int rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_get..\n")));
    rv = cbxi_cosq_port_tsn_op(portid, int_pri, tsn_params, CBXI_COSQ_OP_GET);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_COSQ,
            (BSL_META("FSAL API : cbx_cosq_queue_tsn_config_get..failed to  \
                          get TSN parameters  \n")));
        return rv;
    }
    return rv;
}
#endif


/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     meter.h
 * * Purpose:
 * *     This file contains definitions internal to meter module.
 */

#ifndef _CBX_INT_METER_H
#define _CBX_INT_METER_H

#include <shared/types.h>
#include <fsal/meter.h>
#include <fsal/cosq.h>
#include <fsal/port.h>
#include <fsal_int/types.h>
#include <fsal_int/lag.h>
#include <soc/robo2/common/tables.h>

#define CBXI_METER_PKT_MODE_PKT_LEN           125  /* bytes */
#define CBXI_METER_UNIT_MASK                  0x8000
#define CBXI_METER_UNIT_SHIFT                 15

#define CBXI_METER_MODE_MASK                  0xF0000000
#define CBXI_METER_MODE_SHIFT                 28

#define CBXI_METER_TYPE_MASK                  0xF000000
#define CBXI_METER_TYPE_SHIFT                 24

#define CBXI_METER_PROFILE_INDEX_MASK         0xFF0000
#define CBXI_METER_PROFILE_INDEX_SHIFT        16

#define CBXI_METER_INDEX_MASK                 0x3FFF

#define CBXI_METER_INDEX_VALID                0x4000
#define CBXI_METER_INDEX_VALID_SHIFT          14

#define CBXI_MAX_IPP_METER_PROFILE   32
#define CBXI_MAX_BMU_METER_PROFILE   32
#define CBXI_MAX_PQM_METER_PROFILE   16
#define CBXI_MAX_CFP_METER_PROFILE   128

#define CBXI_METER_DEFAULT_PROFILE  0
#define CBXI_METER_PROFILE_CREATE   1
#define CBXI_METER_PROFILE_UPDATE   2

#define CBXI_METER_CREATE   1
#define CBXI_METER_DESTROY  2

#define CBXI_METER_PROFILE_DATA_VALID           0x80
#define CBXI_METER_PROFILE_DATA_REF_COUNT       0x7F

#define CBXI_METER_MIN_RATE   8          /* 8 kbps */
#define CBXI_METER_MAX_RATE   10000000   /* 10 Gbps */

#define CBXI_METER_PM_ENABLE  1
#define CBXI_METER_PM_DISABLE 2
#define CBXI_METER_PM_CHECK   4

#define CBXI_METER_BMU_PER_PORT_METERS 8
#define CBXI_METER_IPP_PER_PORT_METERS 4

#define CBXI_METER_PM_GREEN_CTR_INDEX      0
#define CBXI_METER_PM_YELLOW_CTR_INDEX     2
#define CBXI_METER_PM_RED_CTR_INDEX        4
#define CBXI_METER_PM_RESERVED_CTR_INDEX   6
#define CBXI_METER_PM_G2Y_CTR_INDEX        8
#define CBXI_METER_PM_G2R_CTR_INDEX       10
#define CBXI_METER_PM_Y2R_CTR_INDEX       12
#define CBXI_METER_PM_G2D_CTR_INDEX       14
#define CBXI_METER_PM_Y2D_CTR_INDEX       16
#define CBXI_METER_PM_R2D_CTR_INDEX       18
#define CBXI_METER_PM_Y2G_CTR_INDEX       20
#define CBXI_METER_PM_R2G_CTR_INDEX       22
#define CBXI_METER_PM_R2Y_CTR_INDEX       24

#define CBXI_METER_BYTES_PER_SEC_LOG2     20 /*125Mhz -log2(1000/(125*10^6)*8)*/
#define CBXI_METER_TRAP_GROUP_MAX_INDEX   15
#define CBXI_METER_MAX_INT_PRI            0x7

#define CBXI_METER_REFRESH_TIMER          125000000 /* 125Mhz  */

#define CBXI_METER_CONTROL_OP_ENABLE         1
#define CBXI_METER_CONTROL_OP_DISABLE        0

#define CBXI_METER_INITIAL_BUCKET_COUNT    0x7ffffff

/* structure for holding meter profile info */
typedef struct cbxi_meter_info_s {
    uint8_t  pid;           /* Meter profile id */
    uint32_t ckbits_sec;    /* Committed rate in kbps or packets per second. */
    uint32_t ckbits_burst;  /* Committed burst size in kbits or number of pkts*/
    uint32_t pkbits_sec;    /* Peak rate in kbps or packets per second. */
    uint32_t pkbits_burst;  /* Peak burst size in kbits or number of packets. */
    cbx_meter_mode_t mode;
} cbxi_meter_info_t;

/* Function to validate passed meter rate */
int cbxi_meter_individual_rate_validate(uint32_t rate, uint32_t flags);

/* Function to Validate cir,cbs,eir,ebs */
int cbxi_meter_rate_validate(cbx_meter_profile_params_t *meter_params);

/* Function to compute log2 of the number passed */
int cbxi_meter_compute_log2(uint32_t rate, uint8_t *exp);

/* Function to convert rate in kbps/packet per second to exp and mantissa */
int cbxi_meter_convert_rate_to_exp_mantissa(uint32_t rate, uint32_t flags,
                                            uint8_t *exp,
                                            uint16_t *mantissa);

/* Function to convert burst in kbps/packet per second to exp and mantissa */
int cbxi_meter_convert_burst_to_exp_mantissa(uint32_t burst,
                                            uint32_t flags,
                                            uint8_t *exp,
                                            uint8_t *mantissa);

/* Function to convert exp and mantissa to rate in kbps/packets */
int cbxi_meter_convert_exp_mantissa_to_rate(uint8_t exp,
                                            uint16_t mantissa,
                                            uint32_t flags,
                                            uint32_t *rate);

/* Function to convert exp and mantissa to burst in kbps/packets */
int cbxi_meter_convert_exp_mantissa_to_burst(uint8_t exp,
                                             uint8_t mantissa,
                                             uint32_t flags,
                                             uint32_t *burst);

/* Function to fill the profile entry fields as per HW format */
int cbxi_meter_fill_profile_data(bmu_profile_t *profile_entry,
                                 cbx_meter_profile_params_t *meter_params);

/* Function to fill the profile FSAL entry fields from entry in HW format */
int cbxi_meter_profile_entry_to_param(bmu_profile_t *profile_entry,
                                      cbx_meter_profile_params_t *meter_params);

/* Funtion to compare 2 profile entries */
int cbxi_meter_profile_entry_compare(bmu_profile_t *new, bmu_profile_t *old);

/* Funtion to ccreate a default profile entry */
int cbxi_meter_default_profile_create(void);

/* Function to read a profile entry from HW given type and index */
int cbxi_meter_profile_entry_get(int unit, cbx_meter_type_t type,
                             int index, bmu_profile_t *entry);

/* Function to write a profile entry to HW given type and index and entry */
int cbxi_meter_profile_entry_set(int unit, cbx_meter_type_t type,
                             int index, bmu_profile_t *entry);

/* Function to create/modify a profile entry in HW */
int cbxi_meter_profile_update(bmu_profile_t *profile_entry,
                              cbx_meter_profile_params_t *meter_params,
                              cbx_meter_type_t type,
                              cbx_meter_profile_t *profile_index,
                              int              flag);

/* Function to validate profile data and create/modify a profile entry in HW */
int cbxi_meter_profile_set(cbx_meter_profile_params_t *meter_params,
                           cbx_meter_profile_t      *profileid,
                           int                      flag);
/* Function to check wheter IPP/BMU/PQM/CFP meter index passed is out of
   range or not */
int cbxi_meter_index_validate(int unit,
                             cbx_meter_type_t type, int meter_index);

/* Function to set meter control with pkt mode len, disable bypass mode
   and enable refresh  */
int cbxi_meter_mtr_control_set(void);

/* Function to set meter refresh timer */
int cbxi_meter_mtr_ref_timer_set(void);

/* Function to Allocate/free a meter index and update PPTR pointer table and
   increment/decrement profile usage count */
int cbxi_meter_op(int unit,
                  cbx_port_t port_out,
                  cbx_meter_profile_t profileid,
                  cbx_meter_params_t *meter_params,
                  cbx_meterid_t      *meterid,
                  int flag);
/* Function to Enable/Disable performance monitoring for given
   IPP/BMU/PQM/CFP meter */
int cbxi_meter_pm_op(int unit, cbx_meter_type_t type,
                     uint32 meter_index, int flag);

/* Function to retrieve PM counter value at the index passed
   for IPP/BMU/PQM/CFP */
int cbxi_meter_read_pm_reg(int unit, cbx_meter_type_t type,
                           int index, bmu_pmon_t *counter);

/* Function to retrieve PM packet and byte counter values for IPP/BMU/PQM/CFP */
int cbxi_meter_pm_counter_pair_get(int unit, cbx_meter_type_t type,
                                   int index,
                                   cbx_meter_pm_counter_t *pm_counter);

/* Function to retrieve all PM packet and byte counter values for
   IPP/BMU/PQM/CFP */
int cbxi_meter_pm_get(int unit, cbx_meter_type_t type,
                      cbx_meter_pm_params_t *pm_params);

/* Function to initialize meter profile entry parameters */
void cbxi_meter_profile_params_t_init(cbx_meter_profile_params_t *meter_params);

/* Function to initialize meter entry parameters */
void cbxi_meter_params_t_init(cbx_meter_params_t *meter_params);

/* Function to meter pm entry parameters */
void cbxi_meter_pm_params_t_init(cbx_meter_pm_params_t *pm_params);

/* Function to Initialize all meter related tables */
int cbxi_meter_table_init();

/* Function to set meter bypass mode */
int cbxi_meter_control_bypass_set(int unit, cbx_meter_type_t type, int flag);

/* Function to set committed and excess bucket values */
int cbxi_meter_control_bucket_set(int unit, int index,
                                  cbx_meter_type_t type,
                                  bmu_bucket_t *bucket_entry);

/* Update BMU MTR CONFIG register - drop_on_red/rfc2698 and color blind */
int cbxi_meter_bmu_mtr_config_update(int unit, bmu_profile_t *profile_entry);

/*  Given meter type and index create meter id */
int cbxi_meter_id_create(int unit, int index, cbx_meter_type_t type,
                         cbx_meterid_t *meterid);
#ifdef MANAGED_MODE
/* initialize CFP meter index allocation mechanism */
int cbxi_meter_cfp_index_management_init();
#endif /* MANAGED_MODE */

int cbx_meter_init(void);
#endif  /* !_CBX_INT_METER_H */

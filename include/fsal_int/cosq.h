/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     cosq.h
 * * Purpose:
 * *     This file contains definitions internal to cosq module.
 */

#ifndef _CBX_INT_COSQ_H
#define _CBX_INT_COSQ_H

#include <shared/types.h>
#include <soc/robo2/common/tables.h>
#include <fsal/cosq.h>
#include <fsal/port.h>
#include <fsal_int/types.h>

#define CBXI_COSQ_MAX_INT_PRI           0x7
#define CBXI_COSQ_MAX_INT_DP            0x2
#define CBXI_COSQ_MAX_PCP               0x7
#define CBXI_COSQ_PCP_MASK              0xE
#define CBXI_COSQ_PCP_SHIFT             0x1
#define CBXI_COSQ_DEI_SHIFT             0x0
#define CBXI_COSQ_MAX_DEI               0x1
#define CBXI_COSQ_DEI_MASK              0x1

#define CBXI_COSQ_MAX_DSCP              0x3F
/* CPMT table offset for V4 and V6 Maps */
#define CBXI_COSQ_DSCPV4_TO_TC_BASE     0x100
#define CBXI_COSQ_DSCPV6_TO_TC_BASE     0x140

/*TBD -  EPMT table offset for V4 and V6 Maps */
#define CBXI_COSQ_TC_TO_DSCPV4_BASE     0x20
#define CBXI_COSQ_TC_TO_DSCPV6_BASE     0x40

#define CBXI_COSQ_CPMT_ENTRIES_PER_PORT 0x10
#define CBXI_COSQ_EPMT_ENTRIES_PER_PORT 0x3
#define CBXI_COSQ_EMPT_TC_TO_DSCP_BASE  0x30

#define CBXI_COSQ_4BITS_MAP             0x1
#define CBXI_COSQ_8BITS_MAP             0x2

#define CBXI_COSQ_OP_SET                 0x1
#define CBXI_COSQ_OP_GET                 0x2
#define CBXI_COSQ_RR_WEIGHT              0x01010101

#define CBXI_COSQ_PKT_MODE_PKT_LEN       1024 /* 1024 bits */
#define CBXI_COSQ_SHAPER_RATE_MIN        64   /* Kbps */
#define CBXI_COSQ_SHAPER_RATE_MAX        10000000 /* 10Gbps */
#define CBXI_COSQ_SHAPER_BURST_MIN       64   /* 512 bits - 64 bytes */
#define CBXI_COSQ_SHAPER_BURST_MAX       16000 /* 16MB */

#define  CBXI_COSQ_SHAPER_SCALE_MIN        0
#define  CBXI_COSQ_SHAPER_SCALE_MAX        6
#define  CBXI_COSQ_SHAPER_MAX_REFESH_SIZE  0x3ffff
#define  CBXI_COSQ_SHAPER_MAX_REFESH_RATE  128000   /* Hz */
#define  CBXI_COSQ_SHAPER_MAX_REFESH_RATE  128000   /* Hz */
#define  CBXI_COSQ_SHAPER_MAX_SCALE_MULTIPLIER  1024000 /* 128000 * 1 * 8/2^0 */

#define  CBXI_COSQ_PORT_SHAPER_INDEX        8

#define  CBXI_COSQ_QUEUE_TO_GROUP_SP        0 /* 1 bit for each queue - 0-SP */
#define  CBXI_COSQ_QUEUE_TO_GROUP_RR        0xFF /* One bit for each Q 1 -RR */
#define  CBXI_COSQ_QUEUE_SCHEDULER_SP       0 /* 1 bit for each queue - 0 -SP */
#define  CBXI_COSQ_QUEUE_SCHEDULER_RR       0xFF /* One bit for each Q-1 - RR */
#define  CBXI_COSQ_QDS_SCHEDULER_SP         0x1 /* 1 bit for each queue - 1 -SP */
#define  CBXI_COSQ_QDS_SCHEDULER_RR         0x0 /* One bit for each Q-0 - RR */

#define  CBXI_COSQ_PORT_FLOW_CONTROL_TC_BMP 0xFF /* One bit for each TC */

#define  CBXI_COSQ_THREHOLD_MAX          0x1FFF /* Max threshold value for port
                                                      and pfc flowcontrol  */

int cbx_cosq_init(void);
int cbxi_cosq_portid_validate(cbx_portid_t portid, cbx_port_t *port_out,
                              cbxi_pgid_t *lpg, int *unit);
int cbxi_cosq_cpmt_update(int unit, int table_index, cbx_tc_t tc);
int cbxi_cosq_epmt_update(int unit, int table_index,
                          uint32 data, int tc, int flag);
int cbxi_cosq_etct_update(int unit, int table_index, uint32 primap_index);

int cbxi_cosq_sched_weight_op(int unit, cbx_port_t port,
                          cbx_schedule_mode_t *sched_mode,
                          int flag,
                          int *weights);

int cbxi_cosq_port_shaper_op(cbx_portid_t             port,
                             int                      int_pri,
                             cbx_cosq_shaper_params_t *shaper,
                             int flag);
int cbxi_cosq_rate_to_bucket_encoding(cbx_cosq_shaper_params_t *shaper,
                                      uint32 *scale, uint32 *refresh_size,
                                      uint32 *burst_size);
int cbxi_cosq_bucket_encoding_to_rate(uint32 scale, uint32 ref_size,
                                      uint32 burst_size,
                                      cbx_cosq_shaper_params_t *shaper);
int cbxi_cosq_rate_validate(cbx_cosq_shaper_params_t *shaper);
int cbxi_cosq_egress_shaper_get_reg(cbx_port_t port, int int_pri,
                                    uint32 *cfg_reg, uint32 *burst_reg,
                                    uint32 *refresh_reg);
int cbxi_cosq_flowcontrol_set(int unit, cbx_flowcontrol_t flowcontrol_mode);
int cbxi_cosq_pgt_update(int unit, int lpg, pgt_t *pgt);
int cbxi_cosq_port_sched_op(int unit, cbx_port_t port,
                            cbx_schedule_mode_t *sched_mode, int flag);
int cbxi_cosq_admission_control_set(int unit, cbx_flowcontrol_t flowcontrol_mode);
#endif  /* !_CBX_INT_COSQ_H */

/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     extender.h
 * * Purpose:
 * *     This file contains definitions internal to port extender module.
 */

#ifndef CBX_INT_EXTENDER_H
#define CBX_INT_EXTENDER_H

#include <shared/types.h>
#include <shared/bitop.h>
#include <soc/types.h>
#include <fsal_int/types.h>
#include <fsal/port.h>
#include <fsal/l2.h>
#include <fsal_int/lag.h>

#define CBX_TPID_ETAG   0x893F

#define CHECK_EXTENDER_INIT(ext_info)       \
    if (!ext_info->init)                \
        return CBX_E_INIT

#define CHECK_ECID_IS_MCAST(e_cid)       (e_cid & 0x3000)

typedef struct cbxi_pqs_cal_info_s {
    int8  idx;
    uint8 slot_en;
    uint8 slot_port;
} cbxi_pqs_cal_info_t;

typedef struct cbxi_epp_cal_info_s {
    int8  idx;
    uint8 slot_port;
} cbxi_epp_cal_info_t;

typedef enum cbxi_extender_mode_e {
    cbxiPEModeAccess    = 0,
    cbxiPEModeTransit   = 1,
    cbxiPEModeNone      = 2,
} cbxi_extender_mode_t;

typedef struct cbx_extender_info_s {
    int          init;       /* TRUE if extender module has been inited */
    SHR_BITDCL  *pe_cascade_bitmap; /* Bitmap of PE cascade ports */
    cbx_portid_t uplink_port[CBX_MAX_UNITS]; /* Uplink portid */
    int  uplink_vsi;/* vsi to be used for default_sli in cascade port */
    int  downlink_vsi;/* vsi to be used during downlink */
    int  downlink_vsi_no_csd;/* vsi without avg csd port */
    int  downlink_vsi_no_csd_in;/* vsi without avg csd port */
    uint16_t dwn_str_dpp_dli[CBX_PORT_MAX];
    uint16_t dwn_str_lag_dli[CBX_LAG_MAX];
    uint16_t in_ecid_port_sli[CBX_PORT_MAX];
    uint16_t in_ecid_lag_sli[CBX_LAG_MAX];
    uint16_t up_str_lpg_dli[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT];
    cbxi_extender_mode_t mode;
} cbx_extender_sw_info_t;

extern int cbxi_check_port_extender_mode(void);
extern int cbxi_pe_avg_csd_config(void);
extern int cbxi_pe_mode_get(cbxi_extender_mode_t *mode);
extern int cbxi_pe_mode_set(cbxi_extender_mode_t mode);
extern int cbxi_pe_ext_port_init(cbx_portid_t portid);
extern int cbxi_pe_csd_port_init(cbx_portid_t portid);
extern int cbxi_pe_lag_pbmp_update(cbx_lag_info_t *lag_info);
extern int cbxi_pe_lag_dest_get(int unit, cbx_port_t lag_out, cbxi_pgid_t *lpg);
extern int cbxi_pe_lag_dest_set(cbx_lag_info_t  *lag_info);
extern int cbxi_pe_lin_port_resolve(int lin, cbx_portid_t *portid);
extern int cbxi_pe_port_lin_get(cbx_portid_t portid, int *lin);
extern int cbxi_pe_lag_dli_get(cbx_portid_t portid, int *lin);
extern int cbxi_pe_port_sli_get(cbx_portid_t ex_port, int *s_lin);
extern int cbxi_pe_uplink_failover_set(cbx_portid_t up_portid, int link_sts);
extern int cbxi_pe_mcast_ecid_get(cbx_mcastid_t mcastid, uint32_t *e_cid);
extern int cbxi_pe_mcast_ecid_set(cbx_mcastid_t mcastid, uint32_t e_cid);
extern int cbxi_pe_release_lin_slot(cbx_l2_entry_t *l2entry);
extern int cbxi_pe_uplink_port_set(cbx_portid_t up_port);
extern int cbxi_pe_extender_port_set(
                cbx_portid_t ex_port, uint32_t e_cid, cbx_l2_entry_t *l2entry);
extern int cbxi_pe_extender_port_clear(cbx_portid_t ex_port);
extern int cbxi_pe_cascade_port_set(
                cbx_portid_t cs_port, uint32_t e_cid, cbx_l2_entry_t *l2entry);
extern int cbxi_pe_cfp_rule_remove(uint32_t e_cid);
extern int cbxi_pe_cfp_action_set(cbx_portid_t portid, int enable);

#ifdef CORTEX_M7
extern void npa_intr_enable(int unit, int port);
extern void npa_intr_disable(int unit, int port);
extern int npa_init(void);
extern void npa_uninit(void);
#endif /* CORTEX_M7 */

extern void gpio_intr_enable(int unit, int gpio);
extern void gpio_intr_disable(int unit, int gpio);
extern int gpio_init(void);
extern void gpio_uninit(void);
extern void extern_intr_enable(int unit, int intr);
extern void extern_intr_ack(int unit, int intr);

#endif

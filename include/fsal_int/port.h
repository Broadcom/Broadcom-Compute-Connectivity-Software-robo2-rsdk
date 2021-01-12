/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     port.h
 * * Purpose:
 * *     This file contains definitions internal to PORT module.
 */

#ifndef _CBX_INT_PORT_H
#define _CBX_INT_PORT_H

#include <sal_types.h>
#include <fsal/port.h>
#include <soc/mac.h>
#include <fsal_int/types.h>
#include <fsal_int/encap.h>
#include <soc/robo2/common/tables.h>

#define CBX_PORT_PEPPER_PORT_TYPE_MASK          0xE00
#define CBX_PORT_PEPPER_PORT_TYPE_SHIFT         9

#define CBX_PORT_PEPPER_RX_DISABLE_MASK         0x100
#define CBX_PORT_PEPPER_RX_DISABLE_SHIFT        8

#define CBX_PORT_PEPPER_TX_DISABLE_MASK         0x80
#define CBX_PORT_PEPPER_TX_DISABLE_SHIFT        7

#define CBX_PORT_PEPPER_DROP_UNTAGGED_MASK      0x40
#define CBX_PORT_PEPPER_DROP_UNTAGGED           0x40
#define CBX_PORT_PEPPER_DROP_UNTAGGED_SHIFT     6

#define CBX_PORT_PEPPER_DROP_TAGGED_MASK        0x20
#define CBX_PORT_PEPPER_DROP_TAGGED             0x20
#define CBX_PORT_PEPPER_DROP_TAGGED_SHIFT       5

#define CBX_PORT_PEPPER_TRUST_PORT_TCDP_MASK    0x10
#define CBX_PORT_PEPPER_TRUST_PORT_TCDP         0x10
#define CBX_PORT_PEPPER_TRUST_PORT_TCDP_SHIFT   4

#define CBX_PORT_PEPPER_EN_VIRTUAL_PORT_MASK    0x8
#define CBX_PORT_PEPPER_EN_VIRTUAL_PORT         0x8
#define CBX_PORT_PEPPER_EN_VIRTUAL_PORT_SHIFT   3


#define CBX_PORT_LOCAL_PP_MASK                  0xFFFF
#define CBX_PORT_DEFAULT_MTU                    0x0600
#define CBX_PORT_MAX_MTU                        0x4000
#define CBX_PORT_DEFAULT_LEARN_LIMIT            0x4000
#define CBX_PORT_MAX_LEARN_LIMIT                0x4000

#define CBX_PORT_ISO_MAP_MASK                   0x3


/* Port loopback modes. */
typedef enum cbx_port_loopback_e {
    CBX_PORT_LOOPBACK_NONE = 0,
    CBX_PORT_LOOPBACK_MAC  = 1,
    CBX_PORT_LOOPBACK_PHY  = 2,
    CBX_PORT_LOOPBACK_COUNT = 3
} cbx_port_loopback_t;

/* CPU Ports */
#define CBX_PORT_ICPU       15  /* Internal CPU Port */
#ifdef CONFIG_EXTERNAL_HOST
/*
 * There is no rgmii port coming out from NS2 connector. 
 * For testing purpose port 0 is used as external cpu port.
 * This can be overriden with CONFIG_ECPU_PORT=<number> Make parameter
 * Default external cpu port is 14.
 */
#ifndef CBX_PORT_ECPU
#define CBX_PORT_ECPU       ((PBMP_ALL(0) & (1 << 0)) ? 0 : 14)  /* External CPU Port, Non RGMII */
#endif
#else /* !CONFIG_EXTERNAL_HOST */
#ifdef CONFIG_PORT_EXTENDER
#define CBX_PORT_ECPU       15 /* Port 14 is used for loopback */
#else /* !CONFIG_PORT_EXTENDER */
#define CBX_PORT_ECPU       14  /* External CPU Port, RGMII */
#endif /* CONFIG_PORT_EXTENDER */
#endif /* CONFIG_EXTERNAL_HOST */


#define CBX_PE_LB_PORT      14
#define CBX_PORT_GPHY_MAX    8
#define CBX_PORT_PBMP_10G_PORTS ((1<< 12 | 1 << 13))

#ifdef CONFIG_CASCADED_MODE

/* Cascade Configuration, Fixme */
#ifdef CONFIG_CPM_BITMAP
#define CBX_CASCADE_PORTS_MASTER    CONFIG_CPM_BITMAP
#else
#define CBX_CASCADE_PORTS_MASTER    0x2000     /* Port 13 */
#endif

#ifdef CONFIG_CPS_BITMAP
#define CBX_CASCADE_PORTS_SLAVE    CONFIG_CPS_BITMAP 
#else
#define CBX_CASCADE_PORTS_SLAVE    0x2000     /* Port 13 */
#endif

#ifdef CONFIG_CASCADE_TRAP_DEST
#define CBX_CASCADE_TRAP_DEST  CONFIG_CASCADE_TRAP_DEST
#else
#define CBX_CASCADE_TRAP_DEST  29
#endif

#define  CBXI_PQM_CASCADE_PPFOV_ADD  0
#define  CBXI_PQM_CASCADE_PPFOV_DELETE  1

#endif /* CONFIG_CASCADED_MODE */

typedef struct cbxi_port_info_s {
    mac_driver_t            *p_mac;       /* Per port MAC driver */
    cbx_port_params_t       port_param;
} cbxi_port_info_t;


extern
cbxi_port_info_t port_info[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT + 1];

int cbx_port_init(void);
int cbxi_port_table_init();
void cbxi_pgt_default_umplus_value_set(pgt_t *pgt_entry);
extern int cbxi_robo2_portid_validate(
    cbx_portid_t port_in, cbx_port_t *port_out);
extern int cbxi_robo2_lpg_to_gpg(int unit, cbxi_pgid_t lpg, cbxi_pgid_t *gpg);
extern int cbxi_robo2_port_to_lpg(int unit, cbx_port_t port, cbxi_pgid_t *lpg);
extern int cbxi_robo2_port_validate(cbx_portid_t  portid,
                                    cbx_port_t *port_out,
                                    cbxi_pgid_t *local_pg, int *unit);
int cbxi_port_info_set( int unit, int port, int pgid, cbx_port_params_t *port_params);
int cbxi_robo2_lpg2ppfov_update(int unit, int pgid,
                                cbx_port_t port, uint32_t flags);
int cbxi_robo2_svt_update(int unit, cbx_port_t port, uint32_t flags);
int cbxi_robo2_pqm_pp2lpg_update(int unit, int pgid, cbx_port_t port);
int cbxi_port_duplex_get(int unit, cbx_port_t port, uint32_t *duplex);
int cbxi_port_duplex_set(int unit, cbx_port_t port, int duplex);
int cbxi_port_speed_get(int unit, cbx_port_t port, uint32_t *speed);
int cbxi_port_speed_set(int unit, cbx_port_t port, int speed);
int cbxi_port_frame_max_get(int unit, cbx_port_t port, uint32_t *size);
int cbxi_port_frame_max_set(int unit, cbx_port_t port, int size);
int cbxi_port_link_status_get(int unit, cbx_port_t port, uint32_t *link_status);
int cbxi_port_link_status_set(int unit, cbx_port_t port, int link_status);
int cbxi_port_link_status_change_get(int unit, cbx_port_t port, int *change);
int cbxi_port_loopback_set(int unit, cbx_port_t port, int loopback);
int cbxi_port_loopback_get(int unit, cbx_port_t port, int *loopback);
int cbxi_port_autoneg_set(int unit, cbx_port_t port, int autoneg);
int cbxi_port_autoneg_get(int unit, cbx_port_t port, int *autoneg);
int cbxi_port_admin_status_set(int unit, cbx_port_t port, int value);
int cbxi_port_admin_status_get(int unit, cbx_port_t port, int *Value);

int cbxi_port_pgt_context_save(int unit, cbx_port_t port, int pgid);
int cbxi_port_pgt_context_recover(int unit, cbx_port_t port, int pgid);
int cbxi_port_info_get(int unit, cbx_port_t port_out,
                       cbxi_pgid_t lpgid, cbx_port_params_t *port_params);
int cbxi_port_pg_update(int unit, cbx_port_t lpgid, cbx_port_t pgid);
int cbxi_cascade_port_delete(cbx_portid_t portid);
int cbxi_pqm_cascade_ppfov_modify(int flags );

void cbxi_port_phy_info();
int cbxi_port_display_eye_scan(cbx_portid_t portid);
int cbxi_robo2_spg2ppconfig_update(int unit, int lpgid,
                                   cbx_port_t port, uint32_t flags);
int cbxi_cascade_port_get(int unit, cbx_portid_t *portid);
int cbxi_cascade_port_check(cbx_portid_t portid);
#endif  /* !_CBX_INT_PORT_H */



/*
 *  mac.h
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 *  $Copyright: (c) 2020 Broadcom Inc.
 *
 * Filename:
 *    mac.h
 * Description:
 *    Contains the defintion for various chip features
 */

#ifndef __MAC_H
#define __MAC_H

#include <soc/types.h>

#define  SOC_TSC_CLK               312
#define  SOC_AVERAGE_IPG_IEEE      96
#define  SOC_NUM_UNIMAC            11
#define  SOC_NUM_XLAMC             4
#define  SOC_MAX_NUM_UNITS         2
#define  JUMBO_MAXSZ               9720

/* Device specific MAC control settings */
typedef enum soc_mac_control_e {
    SOC_MAC_CONTROL_RX_SET,
    SOC_MAC_CONTROL_TX_SET,
    SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
    SOC_MAC_CONTROL_SW_RESET,
    SOC_MAC_CONTROL_DISABLE_PHY,
    SOC_MAC_PASS_CONTROL_FRAME,
    SOC_MAC_CONTROL_PFC_TYPE,
    SOC_MAC_CONTROL_PFC_OPCODE,
    SOC_MAC_CONTROL_PFC_CLASSES,
    SOC_MAC_CONTROL_PFC_MAC_DA_OUI,
    SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI,
    SOC_MAC_CONTROL_PFC_RX_PASS,
    SOC_MAC_CONTROL_PFC_RX_ENABLE,
    SOC_MAC_CONTROL_PFC_TX_ENABLE,
    SOC_MAC_CONTROL_PFC_FORCE_XON,
    SOC_MAC_CONTROL_PFC_STATS_ENABLE,
    SOC_MAC_CONTROL_PFC_REFRESH_TIME,
    SOC_MAC_CONTROL_PFC_XOFF_TIME,
    SOC_MAC_CONTROL_LLFC_RX_ENABLE,
    SOC_MAC_CONTROL_LLFC_TX_ENABLE,
    SOC_MAC_CONTROL_EEE_ENABLE,
    SOC_MAC_CONTROL_EEE_TX_IDLE_TIME,
    SOC_MAC_CONTROL_EEE_TX_WAKE_TIME,
    SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE,
    SOC_MAC_CONTROL_FAULT_LOCAL_STATUS,
    SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE,
    SOC_MAC_CONTROL_FAULT_REMOTE_STATUS,
    SOC_MAC_CONTROL_FAILOVER_RX_SET,
    SOC_MAC_CONTROL_EGRESS_DRAIN,
    SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID,
    SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID,
    SOC_MAC_CONTROL_COUNT            /* always last */
} soc_mac_control_t;

/*
 * Typedef:
 *      mac_driver_t
 * Purpose:
 *      Maps out entry points into MAC drivers.
 * Notes:
 *      Supplied functions are:
 *
 *      md_init       - Initialize the MAC to a known good state.
 *      md_enable_set - Enable MAC or disable and quiesce MAC.
 *      md_enable_get - Return current MAC enable state.
 *      md_duplex_set - Set the current operating duplex mode for the MAC.
 *      md_duplex_get - Return the current operating duplex mode for the MAC.
 *      md_speed_set  - Set the current operating speed for the MAC
 *      md_speed_get  - Return the current operating speed for the MAC.
 *      md_pause_set  - Set current flow control mode.
 *      md_pause_get  - Return current flow control mode.
 *      md_lb_set     - Set current Loopback mode (enable/disable) for MAC.
 *      md_lb_get     - Return current Loopback mode for MAC.
 *      md_interface_set - set MAC-PHY interface type (SOC_PORT_IF_*)
 *      md_interface_get - retrieve MAC-PHY interface type
 *      md_frame_max_set - Set the maximum receive frame size
 *      md_frame_max_get - Return the current maximum receive frame size
 *      md_encap_set  - select port encapsulation mode (RAW/IEEE/HG/HG2)
 *      md_encap_get  - get current port encapsulation mode
 *
 * All routines return SOC_E_XXX values.
 * Null pointers for routines will cause SOC_E_UNAVAIL to be returned.
 *
 * Macros are provided to access the fields.
 */
typedef struct mac_driver_s {
    char        *drv_name;
    int         (*md_init)(int, int);
    int         (*md_enable_set)(int, int, uint32);
    int         (*md_enable_get)(int, int, uint32 *);
    int         (*md_duplex_set)(int, int, uint32);
    int         (*md_duplex_get)(int, int, uint32 *);
    int         (*md_speed_set)(int, int, uint32);
    int         (*md_speed_get)(int, int, uint32 *);
    int         (*md_pause_set)(int, int, uint32, uint32);
    int         (*md_pause_get)(int, int, uint32 *, uint32 *);
    int         (*md_lb_set)(int, int, uint32);
    int         (*md_lb_get)(int, int, uint32 *);
    int         (*md_frame_max_set)(int, int, uint32);
    int         (*md_frame_max_get)(int, int, uint32 *);
    int         (*md_control_set)(int, int, soc_mac_control_t, uint32);
} mac_driver_t;

#define _MAC_CALL(_md, _mf, _ma) \
        ((_md) == 0 ? SOC_E_PARAM : \
         ((_md)->_mf == 0 ? SOC_E_UNAVAIL : (_md)->_mf _ma))

#define MAC_INIT(_md, _u, _p) \
        _MAC_CALL((_md), md_init, ((_u), (_p)))

#define MAC_ENABLE_SET(_md, _u, _p, _e) \
        _MAC_CALL((_md), md_enable_set, ((_u), (_p), (_e)))

#define MAC_ENABLE_GET(_md, _u, _p, _e) \
        _MAC_CALL((_md), md_enable_get, ((_u), (_p), (_e)))

#define MAC_DUPLEX_SET(_md, _u, _p, _d) \
        _MAC_CALL((_md), md_duplex_set, ((_u), (_p), (_d)))

#define MAC_DUPLEX_GET(_md, _u, _p, _d) \
        _MAC_CALL((_md), md_duplex_get, ((_u), (_p), (_d)))

#define MAC_SPEED_SET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_speed_set, ((_u), (_p), (_s)))

#define MAC_SPEED_GET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_speed_get, ((_u), (_p), (_s)))

#define MAC_PAUSE_SET(_md, _u, _p, _tx, _rx) \
        _MAC_CALL((_md), md_pause_set, ((_u), (_p), (_tx), (_rx)))

#define MAC_PAUSE_GET(_md, _u, _p, _tx, _rx) \
        _MAC_CALL((_md), md_pause_get, ((_u), (_p), (_tx), (_rx)))

#define MAC_PAUSE_ADDR_SET(_md, _u, _p, _m) \
        _MAC_CALL((_md), md_pause_addr_set, ((_u), (_p), (_m)))

#define MAC_PAUSE_ADDR_GET(_md, _u, _p, _m) \
        _MAC_CALL((_md), md_pause_addr_get, ((_u), (_p), (_m)))

#define MAC_LOOPBACK_SET(_md, _u, _p, _l) \
        _MAC_CALL((_md), md_lb_set, ((_u), (_p), (_l)))

#define MAC_LOOPBACK_GET(_md, _u, _p, _l) \
        _MAC_CALL((_md), md_lb_get, ((_u), (_p), (_l)))

#define MAC_FRAME_MAX_SET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_frame_max_set, ((_u), (_p), (_s)))

#define MAC_FRAME_MAX_GET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_frame_max_get, ((_u), (_p), (_s)))

#define MAC_CONTROL_SET(_md, _u, _p, _e, _s) \
        _MAC_CALL((_md), md_control_set, ((_u), (_p), (_e), (_s)))

extern mac_driver_t soc_mac_uni;  /* UniMAC */
extern mac_driver_t soc_mac_xl;   /* XLMAC */


extern int soc_mac_init();
extern int soc_mac_pre_init(int no_xlport);
extern int mac_duplex_set(int unit, cbx_port_t port_out, int duplex);
extern int mac_duplex_get(int unit, cbx_port_t port_out, int *duplex);
extern int mac_speed_set(int unit, cbx_port_t port_out, int speed);
extern int mac_speed_get(int unit, cbx_port_t port_out, int *speed);
extern int mac_loopback_set(int unit, cbx_port_t port_out, int loopback);
extern int mac_loopback_get(int unit, cbx_port_t port_out, int *loopback);
extern int mac_max_frame_size_get(int unit, cbx_port_t port_out,
                                  uint32_t *size);
extern int mac_max_frame_size_set(int unit, cbx_port_t port_out, uint32_t size);

#endif /* __MAC_H */

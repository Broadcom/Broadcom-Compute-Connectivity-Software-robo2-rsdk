/*
 * $Id: avng_phy_ctrl.h,v 1.5 2015/01/06 09:39:35 justinch Exp $
 *
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 */

#ifndef __AVNG_PHY_CTRL_H__
#define __AVNG_PHY_CTRL_H__

#include <mdk_phy.h>
#define AVNG_CONFIG_MAX_PORTS   (32)

/* Allow backward comaptibility */
#define AVNG_PHY_DRVLIST_INSTALLED

/* Callback used by avng_phy_reset() */
extern int (*phy_reset_cb)(phy_ctrl_t *pc);

/* Callback used by avng_phy_init() */
extern int (*phy_init_cb)(phy_ctrl_t *pc);

typedef struct avng_phy_port_info_s {
    /* List of default PHY buses available for this port */
    phy_bus_t **phy_bus;
    /* Pointer to first (outermost) PHY */
    phy_ctrl_t *phy_ctrl;
} avng_phy_port_info_t;

typedef struct avng_phy_info_s {
    avng_phy_port_info_t phy_port_info[AVNG_CONFIG_MAX_PORTS];
} avng_phy_info_t;

extern avng_phy_info_t avng_phy_info[];

/* Default list of PHY drivers based on PHY library configuration */
extern phy_driver_t *avng_phy_drv_list[];

typedef int (*avng_phy_probe_func_t)(int, int, phy_driver_t **);

#define AVNG_PHY_INFO(_u) avng_phy_info[_u]

#define AVNG_PHY_PORT_INFO(_u,_p) AVNG_PHY_INFO(_u).phy_port_info[_p]

#define AVNG_PORT_PHY_BUS(_u,_p) AVNG_PHY_PORT_INFO(_u,_p).phy_bus
#define AVNG_PORT_PHY_CTRL(_u,_p) AVNG_PHY_PORT_INFO(_u,_p).phy_ctrl

#define EXT_PHY_DRV(_u, _p) AVNG_PHY_PORT_INFO(_u,_p).phy_ctrl->drv

/*
 * Configure PHY buses
 *
 * Note that the default buses are installed by avng_attach().
 */
extern int
avng_phy_bus_set(int unit, int port, phy_bus_t **bus_list);

/*
 * Utility function for PHY probing function
 */
extern int
avng_phy_bus_get(int unit, int port, phy_bus_t ***bus_list);

extern int
avng_phy_add(int unit, int port, phy_ctrl_t *pc);

extern phy_ctrl_t *
avng_phy_del(int unit, int port);

/*
 * Install PHY probing function and drivers
 */
extern int 
avng_phy_probe_init(avng_phy_probe_func_t probe, phy_driver_t **drv_list);

/* 
 * Default PHY probing function.
 *
 * Note that this function is not referenced directly from the AVNG,
 * but it can be installed using avng_phy_probe_init().
 */
extern int 
avng_phy_probe_default(int unit, int port, phy_driver_t **drv_list);

/*
 * Register PHY reset callback function.
 *
 * The callback function can be used for board specific configuration
 * like XAUI lane remapping and/or polarity flip.
 */
extern int 
avng_phy_reset_cb_register(int (*reset_cb)(phy_ctrl_t *pc));

/*
 * Register PHY init callback function.
 *
 * The callback function can be used for board specific configuration
 * like LED modes, special MDIX setup and PHY-sepcific extensions.
 */
extern int 
avng_phy_init_cb_register(int (*init_cb)(phy_ctrl_t *pc));


#ifdef CONFIG_8488X
extern int plp_orca_phy_8481_probe(int unit);
extern int plp_orca_phy_8481_init(int unit);
#endif /* CONFIG_8488X */
#endif /* __AVNG_PHY_CTRL_H__ */

/*
 * $Id: avng_phy.c,v 1.0 2016/03/14 08:17:33 mohanm Exp $
 *
 * $Copyright: (c) 2016 Broadcom Ltd.
 * All Rights Reserved.$
 *
 */

#include <sal_libc.h>
#include <sal_types.h>
#include <sal_console.h>
#include <sal_alloc.h>
#include <sal_time.h>
#include <sal_sync.h>
#include "avng_miim.h"
#include <cdk/cdk_symbols.h>
#include "avng_phy_ctrl.h"
#include <avng_phy.h>
#include <cdk/cdk_chip.h>
#include <cdk/cdk_error.h>
#include <cdk/cdk_printf.h>
#include <cdk/cdk_debug.h>
#include <drv_if.h>
#include <common/memregs.h>
#include <fsal/port.h>
#ifdef CONFIG_8488X
#include <bcm8488x.h>
#endif
#ifndef CDK_DEBUG_PRINTF
#define CDK_DEBUG_PRINTF CDK_PRINTF
#endif

uint32_t cdk_debug_level = CDK_DBG_ERR | CDK_DBG_WARN;
int (*cdk_debug_printf)(const char *format, ...) = CDK_DEBUG_PRINTF;

extern int phymod_tsc_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);
extern int phymod_tsc_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);
#define CDK_XGSM_MIIM_IBUS_NUM(_a)  ((_a & ~0x200) >> 6)


#define CDK_CONFIG_MIIM_MAX_POLLS   100000
#define MIIM_PARAM_ID_OFFSET 		16
#define MIIM_PARAM_REG_ADDR_OFFSET	24

extern phy_bus_t phy_bus_avng_miim_int;
extern phy_bus_t phy_bus_avng_miim_ext;

static phy_bus_t *avng_phy_bus[] = {
    &phy_bus_avng_miim_int,
#ifdef CONFIG_EXT_PHY
    &phy_bus_avng_miim_ext,
#endif
    NULL
};

extern phy_driver_t bcm_gphy_drv;
extern phy_driver_t bcmi_viper_xgxs_drv;
extern phy_driver_t bcmi_tsce_xgxs_drv;
#ifdef CONFIG_EXT_PHY
extern phy_driver_t bcm542xx_drv;
#endif
phy_driver_t *avng_phy_drv_list[] = {
    &bcm_gphy_drv,
    &bcmi_viper_xgxs_drv,
    &bcmi_tsce_xgxs_drv,
#ifdef CONFIG_EXT_PHY
    &bcm542xx_drv,
#endif
    NULL
};

#define PHY_EXTERNAL_MODE(lport) (phy_external_mode & (0x1 << lport))
#define PHY_CTRL_NUM_MAX  (16)

static uint32_t phy_external_mode = 0;
avng_phy_info_t avng_phy_info[SOC_MAX_NUM_SWITCH_DEVICES];

/*
 * There are multiple threads that each want to access PHY registers. However,
 * such accesses are themselves not atomic, so only one actual access can be
 * performed at once. Therefore, protect all potentially concurrent accesses
 * with a mutex. The same bus is used for multiple ports, so the mutex is
 * global for now. In the future, we may allow some forms of concurrency (e.g.,
 * based on port type) if desired--hence the unused 'pc' parameter.
 */
static sal_mutex_t avng_phy_mutex = NULL;

#define PHY_LOCK(pc)   sal_mutex_take(avng_phy_mutex, sal_mutex_FOREVER)
#define PHY_UNLOCK(pc) sal_mutex_give(avng_phy_mutex)

int phy_debug = 0;
void phy_debug_set(int enable)
{
    phy_debug = enable;
}
/*
 * We do not want to rely on dynamic memory allocation,
 * so we allocate phy_ctrl blocks from a static pool.
 * The 'bus' member of the structure indicates whether
 * the block is free or in use.
 */
static phy_ctrl_t _phy_ctrl[SOC_MAX_NUM_SWITCH_DEVICES][PHY_CTRL_NUM_MAX];

int (*phy_reset_cb)(phy_ctrl_t *pc);
int (*phy_init_cb)(phy_ctrl_t *pc);

/*
 * Register PHY init callback function
 */
int
avng_phy_init_cb_register(int (*init_cb)(phy_ctrl_t *pc))
{
    phy_init_cb = init_cb;

    return CDK_E_NONE;
}

/*
 * Register PHY reset callback function
 */
int
avng_phy_reset_cb_register(int (*reset_cb)(phy_ctrl_t *pc))
{
    phy_reset_cb = reset_cb;

    return CDK_E_NONE;
}

int
avng_phy_add(int unit, int lport, phy_ctrl_t *pc)
{
    pc->next = AVNG_PORT_PHY_CTRL(unit, lport);
    AVNG_PORT_PHY_CTRL(unit, lport) = pc;
    return CDK_E_NONE;
}

phy_ctrl_t *
avng_phy_del(int unit, int lport)
{
    phy_ctrl_t *pc;

    if ((pc = AVNG_PORT_PHY_CTRL(unit, lport)) != 0) {
        AVNG_PORT_PHY_CTRL(unit, lport) = pc->next;
    }
    return pc;
}

phy_ctrl_t *
phy_ctrl_alloc(int unit)
{
    int idx;
    phy_ctrl_t *pc;

    for (idx = 0, pc = &_phy_ctrl[unit][0];
                            idx < PHY_CTRL_NUM_MAX; idx++, pc++) {
        if (pc->bus == 0) {
            return pc;
        }
    }
    return NULL;
}

void
phy_ctrl_free(phy_ctrl_t *pc)
{
    pc->bus = 0;
}


/*
 * Probe all PHY buses associated with AVNG device
 */
int
avng_phy_probe_default(int unit, int lport, phy_driver_t **phy_drv)
{
    phy_bus_t **bus;
    phy_driver_t **drv;
    phy_ctrl_t pc_probe;
    phy_ctrl_t *pc;
    int rv;
#ifdef CONFIG_EXT_PHY
    phy_ctrl_t *pc_added;
#endif
    /* Remove any existing PHYs on this lport */
    while ((pc = avng_phy_del(unit, lport)) != 0) {
        phy_ctrl_free(pc);
    }

    /* Bail if not PHY driver list is provided */
    if (phy_drv == NULL) {
        return CDK_E_NONE;
    }

    /* Check that we have PHY bus list */
    bus = AVNG_PORT_PHY_BUS(unit, lport);
    if (bus == NULL) {
        return CDK_E_CONFIG;
    }

    /* Before creating any PHY objects, initialize the global mutex. */
    if (avng_phy_mutex == NULL) {
        avng_phy_mutex = sal_mutex_create("avng_phy");
        if (avng_phy_mutex == NULL) {
            sal_printf("avng_phy: unable to allocate mutex\n");
            return CDK_E_RESOURCE;
        }
    }

    /* Loop over PHY buses for this lport */
    while (*bus != NULL) {
        drv = phy_drv;
        /* Probe all PHY drivers on this bus */
        while (*drv != NULL) {
            /* Initialize PHY control used for probing */
            pc = &pc_probe;
            sal_memset(pc, 0, sizeof(*pc));
            pc->unit = unit;
            pc->port = lport;
            pc->bus = *bus;
            pc->drv = *drv;
#ifdef CONFIG_EXT_PHY
            /* check if already PHY driver is attached on internal bus, if yes
             * do not probe on external bus */
            pc_added = AVNG_PORT_PHY_CTRL(unit, lport);
            if (pc_added) {
                if (!sal_strcmp(pc_added->drv->drv_name, pc->drv->drv_name)) {
                    drv++;
                    continue;
                }
            }
#endif
            if (CDK_SUCCESS(PHY_PROBE(pc))) {
                /* Found known PHY on bus */
                pc = phy_ctrl_alloc(unit);
                if (pc == NULL) {
                    return CDK_E_MEMORY;
                }
                sal_memcpy(pc, &pc_probe, sizeof(*pc));
                /* Install PHY */
                rv = avng_phy_add(unit, lport, pc);
                if (CDK_FAILURE(rv)) {
                    return rv;
                }
                /* Move to next bus */
                break;
            }
            drv++;
        }
        bus++;
    }

    pc = AVNG_PORT_PHY_CTRL(unit, lport);
    if (pc == NULL) {
        return CDK_E_NOT_FOUND;
    }
    if (pc && pc->next) {
        /* If both external PHY and serdes are attached. */
        phy_external_mode |= (0x1 << lport);
    }
    return CDK_E_NONE;
}

int
soc_phyctrl_notify(phy_ctrl_t *pc, phy_event_t event, uint32_t value)
{
    int rv = CDK_E_NONE;
    int lport;
    PHY_CTRL_CHECK(pc);

    lport = pc->port;
    if (!PHY_EXTERNAL_MODE(lport) || !(pc->next)) {
        return CDK_E_NONE;
    }

    switch (event) {
    case PhyEvent_MacEnable:
        rv = PHY_NOTIFY(pc->next, event);
        break;
    case PhyEvent_MacDisable:
        rv = PHY_NOTIFY(pc->next, event);
        break;
    case PhyEvent_Speed:
        rv = PHY_SPEED_SET(pc->next, value);
        break;
    case PhyEvent_Duplex:
        rv = PHY_DUPLEX_SET(pc->next, value);
        break;
    default:
        return CDK_E_NONE;
    }

    return rv;
}

/*
 *  Function : avng_phy_reg_read
 *
 *  Purpose :
 *      Read a value from a MII register.
 *
 *  Parameters :
 *      reg_addr (input) : PHY regsiter
 *      value (output) : PHY data to write into register
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_reg_read(uint8_t unit, uint8_t lport, uint32_t reg_addr, uint32_t *p_value)
{
    int rv = CDK_E_NONE;
    uint32_t value = 0;
    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);
    if (lport < 8) {
        PHY_LOCK(pc);
        rv = PHY_BUS_READ(pc, (uint32_t)reg_addr, &value);
        PHY_UNLOCK(pc);
    } else if (lport < 14) {
        PHY_LOCK(pc);
        rv = PHY_REG_READ(pc, reg_addr, &value);
        PHY_UNLOCK(pc);
    } else {
        return SYS_ERR_PARAMETER;
    }
    *p_value = (uint16_t)value;

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}

/*
 *  Function : avng_phy_reg_write
 *
 *  Purpose :
 *      Write a value into a MII register.
 *
 *  Parameters :
 *      reg_addr (input) : PHY regsiter
 *      value (input) : PHY data to write into register
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_reg_write(uint8_t unit, uint8_t lport, uint32_t reg_addr, uint32_t value)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);
    if (lport < 8) {
        PHY_LOCK(pc);
        rv = PHY_BUS_WRITE(pc, (uint32_t)reg_addr, value);
        PHY_UNLOCK(pc);
    } else if (lport < 14) {
        PHY_LOCK(pc);
        rv = PHY_REG_WRITE(pc, reg_addr, value);
        PHY_UNLOCK(pc);
    } else {
        return SYS_ERR_PARAMETER;
    }
    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}


/*
 *  Function : avng_phy_duplex_set
 *
 *  Purpose :
 *     Set duplex
 *
 *  Parameters :
 *      lport (input) : port id
 *      value (input) : duplex setting
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_duplex_set(int unit, uint8_t lport, uint32_t value)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_DUPLEX_SET(pc, value);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}


/*
 *  Function : avng_phy_duplex_get
 *
 *  Purpose :
 *     Get duplex
 *
 *  Parameters :
 *      lport (input) : port id
 *      value (output) : duplex setting
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_duplex_get(int unit, uint8_t lport, uint32_t *value)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_DUPLEX_GET(pc, (int *)value);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}

void avng_interface_speed_set(int unit, uint8_t port) {

    phy_ctrl_t *pc;
    uint32     speed = 0;
    pc = AVNG_PORT_PHY_CTRL(unit, port);

    switch(port) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            speed = 1000;
            break;

        case 8:
        case 9:
        case 10:
        case 11:
#if defined(CONFIG_1000X_1000) || (CONFIG_SGMII_SPEED == 1000) || defined(CONFIG_QSGMII)
            speed = 1000;
#else
            speed = 2500;
#endif
            break;

        case 12:
        case 13:
#if (CONFIG_XFI_SPEED == 1000)
            speed = 1000;
#elif (CONFIG_XFI_SPEED == 2500)
            speed = 2500;
#else
            speed = 10000;
#endif
            break;
        default :
            speed = 0;
            break;
    }
    pc->speed = speed;
    return;
}

int
avng_pm_tsc_clk_mux_select_set(int unit, uint8_t port1, uint32_t speed1, uint8_t vco_0, uint32_t vco_1, uint32 *skip_vco_change) {

    uint32 regval = 0;
    uint32 fval = 0;
    uint32 get_fval = 0;
    uint32 skip = 0;

    REG_READ_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    soc_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr_field_get(unit, &regval,
                                                         PM_TSC_CLK_MUX_SELECTf, &get_fval);
    /* port 12 - 2.5G and port 13 - 2.5G */
    if ((vco_0 == 0) && (vco_1 == 0)) {
        if (port1 == 12) {
            if (speed1 == 2500) {
                fval = 0;
            } else {
                fval = 0;
            }
        } else { /* port 13 */
            if (speed1 == 2500) {
                fval = 1;
            } else {
                fval = 0;
            }
        }
    }
    /* port 13 - 100Mbps/1G/10G and port 12 - 2.5G */
    if ((vco_0 == 0) && (vco_1 == 1)) {
        if (port1 == 12) {
            if (speed1 == 2500) {
                fval = 0;
            } else {
                fval = 0;
            }
        } else {
            if (speed1 == 2500) {
                fval = 1;
            } else {
                fval = 0;
            }
        }
    }
    /* port 12 - 100Mbps/1G/10G and port 13 - 2.5G */
    if ((vco_0 == 1) && (vco_1 == 0)) {
        if (port1 == 12) {
            if (speed1 == 2500) {
                fval = 0;
            } else {
                fval = 1;
            }
        } else {
            if (speed1 == 2500) {
                fval = 1;
            } else {
               fval = 1;
            }

        }
    }
    /* port 12 - 100Mbps/1G/10G and port 13 - 100Mbps/1G/10G */
    if ((vco_0 == 1) && (vco_1 == 1)) {
        if (port1 == 12) {
            if (speed1 == 2500) {
                fval = 0;
            } else {
                fval = 0;
            }
        } else {
            if (speed1 == 2500) {
                fval = 1;
            } else {
                fval = 1;
            }
        }
    }
    if(fval == get_fval) {
        skip = 1;
    } else {
        skip = 0;
    }
    if (skip == 0) {
        soc_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr_field_set(unit, &regval,
                                           PM_TSC_CLK_MUX_SELECTf, &fval);
        REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    }

    return SYS_OK;
}

int
avng_portmacro_speed_set(int unit, uint8_t lport, uint32_t value) {
    uint32 regval = 0;
    uint32 speed_config = 0;
    uint8 shift;
    switch(value) {
       case CBX_PORT_SPEED_10:
           speed_config = 0;
           break;
       case CBX_PORT_SPEED_100:
           speed_config = 1;
           break;
       case CBX_PORT_SPEED_1000:
           speed_config = 2;
           break;
       case CBX_PORT_SPEED_2500:
           speed_config = 3;
           break;
       case CBX_PORT_SPEED_10000:
           speed_config = 4;
           break;
        default:
            return SYS_ERR_PARAMETER;
            break;
    }
    if ((lport == 12) || (lport == 13)) {
        REG_READ_AVR_PORTMACRO_PORT_MACRO_SPEED_CONFIGr(unit, &regval);
        /* Lane0 * [bit * 2:0] * = * 0x4, * lane1 * [bit * 6:4] * = * 4 * ;
         * configure * 10G 10M : 'b000 100M : 'b001 1G : 'b010 2.5G : 'b011 10G : 'b100 */
        if (lport == 12) {
            shift = 4;
            regval = ((regval & 0xFFFFFF0F) | (speed_config << shift));
        } else {
            shift = 0;
            regval = ((regval & 0xFFFFFFF0) | (speed_config << shift));
        }
        REG_WRITE_AVR_PORTMACRO_PORT_MACRO_SPEED_CONFIGr(unit, &regval);
    }
    return SYS_OK;
}

/*
 *  Function : avng_phy_speed_set
 *
 *  Purpose :
 *     Set port speed
 *
 *  Parameters :
 *      lport (input) : port id
 *      value (input) : Speed
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_speed_set(int unit, uint8_t lport, uint32_t value)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc;
    phy_ctrl_t *pc1;
    uint32 speed_1 = 0;
    uint32 speed_0 = 0;
    uint8_t vco_5_0 = 0;
    uint8_t vco_5_1 = 0;
    uint32 skip_vco_change = 0;
    uint32 speed  = 0;

    if ((lport == 12) || (lport == 13)) {
        pc = AVNG_PORT_PHY_CTRL(unit, 12);
        PHY_LOCK(pc);
        rv = PHY_SPEED_GET(pc, &speed_0);
        PHY_UNLOCK(pc);
        if (rv == SYS_OK) {
            if ((speed_0 == 100) || (speed_0 == 1000) || (speed_0 == 10000)) {
                vco_5_0 = 1;
            } else {
                vco_5_0 = 0;
            }
        }
        pc1 = AVNG_PORT_PHY_CTRL(unit, 13);
        PHY_LOCK(pc1);
        rv = PHY_SPEED_GET(pc1, &speed_1);
        PHY_UNLOCK(pc1);
        if (rv == SYS_OK) {
            if ((speed_1 == 100) || (speed_1 == 1000) || (speed_1 == 10000)) {
                vco_5_1 = 1;
            } else {
                vco_5_1 = 0;
            }
        }
        speed = value;
        if (speed == 10001) {
            speed = 10000;
        }
        if (speed == 1001) {
            speed = 1000;
        }
        if (rv == SYS_OK) {
            rv = avng_pm_tsc_clk_mux_select_set(unit, lport, speed, vco_5_0, vco_5_1, &skip_vco_change);
        }
        if (rv == SYS_OK) {
            rv = avng_portmacro_speed_set(unit, lport, speed);
        }
    }
    if (rv == SYS_OK) {
        pc = AVNG_PORT_PHY_CTRL(unit, lport);

        PHY_LOCK(pc);  
        rv = PHY_SPEED_SET(pc, value);
        PHY_UNLOCK(pc);
        pc->speed = value;
    }
    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}


/*
 *  Function : avng_phy_speed_get
 *
 *  Purpose :
 *     Get phy speed
 *
 *  Parameters :
 *      lport (input) : port id
 *      value (output) : duplex setting
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_speed_get(int unit, uint8_t lport, uint32_t *value)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_SPEED_GET(pc, value);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}

/*
 *  Function : avng_phy_link_get
 *
 *  Purpose :
 *     Get phy link status
 *
 *  Parameters :
 *      lport (input) : port id
 *      link_status (output) : link status
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_link_get(int unit, uint8_t lport, uint32_t *link_status)
{
    int rv = CDK_E_NONE;
    int an = 0;
    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_LINK_GET(pc, (int *)link_status, &an);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}

/*
 *  Function : avng_phy_loopback_set
 *
 *  Purpose :
 *     Set phy loopback
 *
 *  Parameters :
 *      lport (input) : port id
 *      enable (input) : loopback enable
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_loopback_set(int unit, uint8_t lport, uint32_t enable)
{
    int rv = CDK_E_NONE;
    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_LOOPBACK_SET(pc, enable);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}
/*
 *  Function : avng_phy_loopback_get
 *
 *  Purpose :
 *     Get phy loopback
 *
 *  Parameters :
 *      lport (input) : port id
 *      status (output) : loopback status
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_loopback_get(int unit, uint8_t lport, uint32_t *status)
{
    int rv = CDK_E_NONE;
    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_LOOPBACK_GET(pc, (int *)status);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}

/*
 *  Function : avng_phy_autoneg_set
 *
 *  Purpose :
 *     Set Autoneg
 *
 *  Parameters :
 *      lport (input) : port id
 *      an  (input) :   Autoneg value - set or not set
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_autoneg_set(int unit, uint8_t lport, uint32_t an)
{
    int rv = CDK_E_NONE;
    phy_ctrl_t *pc;
    phy_ctrl_t *pc1;
    uint32 speed_1 = 0;
    uint32 speed_0 = 0;
    uint8_t vco_5_0 = 0;
    uint8_t vco_5_1 = 0;
    uint32 skip_vco_change = 0;
    uint32 speed  = 0;

    if ((lport == 12) || (lport == 13)) {
        pc = AVNG_PORT_PHY_CTRL(unit, 12);
        PHY_LOCK(pc);
        rv = PHY_SPEED_GET(pc, &speed_0);
        PHY_UNLOCK(pc);
        if (rv == SYS_OK) {
            if ((speed_0 == 100) || (speed_0 == 1000) || (speed_0 == 10000)) {
                vco_5_0 = 1;
            } else {
                vco_5_0 = 0;
            }
        }
        pc1 = AVNG_PORT_PHY_CTRL(unit, 13);
        PHY_LOCK(pc1);
        rv = PHY_SPEED_GET(pc1, &speed_1);
        PHY_UNLOCK(pc1);
        if (rv == SYS_OK) {
            if ((speed_1 == 100) || (speed_1 == 1000) || (speed_1 == 10000)) {
                vco_5_1 = 1;
            } else {
                vco_5_1 = 0;
            }
        }
        if (an == 1) {
            speed = 1000;
        } else {
            speed = 10000;
        }
        if (rv == SYS_OK) {
            rv = avng_pm_tsc_clk_mux_select_set(unit, lport, speed, vco_5_0, vco_5_1, &skip_vco_change);
        }
        if (rv == SYS_OK) {
            rv = avng_portmacro_speed_set(unit, lport, speed);
        }
    }
    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_AUTONEG_SET(pc, an);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}

/*
 *  Function : avng_phy_autoneg_get
 *
 *  Purpose :
 *     Get Autoneg value
 *
 *  Parameters :
 *      lport (input) : port id
 *      an    (output): Autoneg value - set or not set
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_autoneg_get(int unit, uint8_t lport, uint32_t *an)
{
    int rv = CDK_E_NONE;
    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    PHY_LOCK(pc);
    rv = PHY_AUTONEG_GET(pc, (int *)an);
    PHY_UNLOCK(pc);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return rv;
    }
}

/*
 *  Function : avng_phy_notify_mac_enable
 *
 *  Purpose :  Notify phy that we are disabling MAC
 *
 *  Parameters :
 *      lport (input) : port id
 *
 *  Return :
 *      SYS_OK : success
 *      SYS_ERR : failed to access the MII register
 *
 */
int
avng_phy_notify_mac_enable(int unit, uint8_t lport, int enable)
{
    int rv = CDK_E_NONE;
    phy_event_t event;
    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    event = (enable) ? PhyEvent_MacEnable : PhyEvent_MacDisable;

    rv = soc_phyctrl_notify(pc, event, 0);

    if (!rv) {
        return SYS_OK;
    }
    else {
        return SYS_ERR;
    }
}


int
cdk_avng_miim_read(int unit, uint32_t phy_addr, uint32_t reg, uint8 ext, uint32_t *val)
{
    int rv = CDK_E_NONE;
    uint32_t polls, data, phy_param;
    uint32 read_start;
    int ioerr = 0;
    uint32 phyaddr;
    phyaddr = phy_addr & 0x1F;
    if (phyaddr < 15) {
       phy_addr |= 0x200;
    }
    if (ext == 0x1) {
       phy_addr &= 0xFFFFFDFF;
    }
    /*
     * Use clause 45 access if DEVAD specified.
     * Note that DEVAD 32 (0x20) can be used to access special DEVAD 0.
     */
    if (reg & 0x003f0000) {
        phy_addr |= CDK_XGSM_MIIM_CLAUSE45;
        reg &= 0x001fffff;
    }

    phy_param = (phy_addr << MIIM_PARAM_ID_OFFSET);
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_PARAMr(unit, (void *)&phy_param);
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_ADDRESSr(unit, (void *)&reg);

    /* Tell CMIC to start */
    read_start = CMIC_MIIM_RD_START;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&read_start);

    /* Poll for completion */
    for (polls = 0; polls < CDK_CONFIG_MIIM_MAX_POLLS; polls++) {
        ioerr += REG_READ_M7SS_MDIO_CMIC_MIIM_STATr(unit, &data);
        if (data & CMIC_MIIM_OPN_DONE) {
            break;
	    }
    }

    /* Check for timeout and error conditions */
    if (polls == CDK_CONFIG_MIIM_MAX_POLLS) {
        rv = -1;
        CDK_DEBUG_MIIM
            (("cdk_xgsm_miim_read[%d]: Timeout at phy_addr=0x%08x"
              "reg_addr=%08x\n",
              unit, phy_addr, reg));
    }


    if (rv >= 0) {
        ioerr = REG_READ_M7SS_MDIO_CMIC_MIIM_READ_DATAr(unit, (void *)val);
        CDK_DEBUG_MIIM
            (("cdk_xgsm_miim_read[%d]: phy_addr=0x%08x"
              "reg_addr=%08x data: 0x%08x\n",
              unit, phy_addr, reg, *val));
    }
    read_start = 0;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&read_start);
    if (phy_debug)
        sal_printf("Reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x PhyDataRead = 0x%x\n",
                                             phy_addr & 0x3f, reg, *val & 0xFFFF);
#if DEBUG_MIIM_READ
    uint32              phy_param;
    uint32              phy_id;
    uint32              phy_miim_addr;
    uint32                 bus_sel;
    uint32                 internal = 1;
    uint32              regval=0;
    int timeout = 200;
    uint32 done;
    uint32             fval = 0;
    int                rv = 0;
    bus_sel = 0;
    phy_miim_addr = 0;
    phy_param = 0;
    phy_addr &= 0x1F;
    if (phy_addr < 15) {
       internal = 1;
    }
    phy_id = phy_addr << 16;
    soc_M7SS_MDIO_CMIC_MIIM_ADDRESSr_field_set(unit, &regval,
                                  CLAUSE_45_REGADRf, &reg);

    REG_WRITE_M7SS_MDIO_CMIC_MIIM_ADDRESSr(unit, &regval);

    fval = 0;
    regval = 0;
    soc_M7SS_MDIO_CMIC_MIIM_PARAMr_field_set(unit, &regval,
                                  C45_SELf, &fval);

    soc_M7SS_MDIO_CMIC_MIIM_PARAMr_field_set(unit, &regval,
                                  INTERNAL_SELf, &internal);
    soc_M7SS_MDIO_CMIC_MIIM_PARAMr_field_set(unit, &regval,
                                  BUS_IDf, &bus_sel);
    soc_M7SS_MDIO_CMIC_MIIM_PARAMr_field_set(unit, &regval,
                                  PHY_IDf, &phy_id);
    regval |= phy_id;
    REG_WRITE_M7SS_MDIO_CMIC_MIIM_PARAMr(unit, &regval);

    fval = 1;
    regval = 0;
    REG_READ_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, &regval);
    soc_M7SS_MDIO_CMIC_MIIM_CTRLr_field_set(unit, &regval,
                                  MIIM_RD_STARTf, &fval);
    REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, &regval);


    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
       fval = 0;
       REG_READ_M7SS_MDIO_CMIC_MIIM_STATr(unit, &regval);
       soc_M7SS_MDIO_CMIC_MIIM_STATr_field_get(unit, &regval,
                                  MIIM_OPN_DONEf, &done);
       if ( done == 1 ) {
          fval = 0;
          REG_READ_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, &regval);
          soc_M7SS_MDIO_CMIC_MIIM_CTRLr_field_set(unit, &regval,
                                  MIIM_RD_STARTf, &fval);
          REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, &regval);
       }

       timeout--;
       if (timeout == 0 ) {
           sal_printf ("ERROR : Timeout while reading from phy register : \n");
           return -1;
       }

       REG_READ_M7SS_MDIO_CMIC_MIIM_READ_DATAr(unit, &regval);
       *val = (uint16)regval;

       if (phy_debug)
           sal_printf ("Reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x PhyDataRead = 0x%x \n", phy_addr & 0x1F,              reg, *val);

    }
#endif
    return rv;
}

int
cdk_avng_miim_write(int unit, uint32_t phy_addr, uint32_t reg, uint8 ext, uint32_t val)
{
    int rv = CDK_E_NONE;
    uint32 polls, data, phy_param;
    uint32 wr_start;
    int ioerr = 0;
    uint32 phyaddr;
    phyaddr = phy_addr & 0x1F;
    if (phyaddr < 15) {
       phy_addr |= 0x200;
    }
    if (ext == 0x1) {
       phy_addr &= 0xFFFFFDFF;
    }
    val = val & 0xFFFF;
    /*
     * Use clause 45 access if DEVAD specified.
     * Note that DEVAD 32 (0x20) can be used to access special DEVAD 0.
     */
    if (reg & 0x003f0000) {
        phy_addr |= CDK_XGSM_MIIM_CLAUSE45;
        reg &= 0x001fffff;
    }

    phy_param = (phy_addr << MIIM_PARAM_ID_OFFSET) | val;

    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_PARAMr(unit, (void *)&phy_param);
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_ADDRESSr(unit, (void *)&reg);

    /* Tell CMIC to start */
    wr_start = CMIC_MIIM_WR_START;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&wr_start);

    /* Poll for completion */
    for (polls = 0; polls < CDK_CONFIG_MIIM_MAX_POLLS; polls++) {
        ioerr += REG_READ_M7SS_MDIO_CMIC_MIIM_STATr(unit, &data);
        if (data & CMIC_MIIM_OPN_DONE) {
            break;
	    }
    }

    /* Check for timeout and error conditions */
    if (polls == CDK_CONFIG_MIIM_MAX_POLLS) {
        rv = -1;
        CDK_DEBUG_MIIM
            (("cdk_xgsm_miim_read[%d]: Timeout at phy_addr=0x%08x"
              "reg_addr=%08x\n",
              unit, phy_addr, reg));
    }
    wr_start = 0;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&wr_start);
    if (phy_debug)
        sal_printf("Writing to phy register   : PhyId = 0x%x PhyRegAddr = 0x%x PhyWrData = 0x%x\n",
                                             phy_addr & 0x3F, reg, val);
    return rv;
}

int cdk_avng_pmi_write(int unit, uint32 phy_id, uint32 phy_reg_addr,
                       uint32 phy_wr_data)

{
    uint32 reg_data;
    uint32 pmi_phy_reg_addr;
    uint32 done;
    int timeout = 200;
    uint32  fval;

    phy_wr_data = (phy_wr_data & 0xFFFF);
    if ((phy_id & 0xFF) == 9 ) {
       pmi_phy_reg_addr = 0x10000 | phy_reg_addr;
       fval = 0;
    } else {
       pmi_phy_reg_addr = phy_reg_addr;
       fval = 1;
    }
    reg_data = 0x0;
    soc_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr_field_set(unit, &reg_data, RESERVED_FOR_ECOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr(unit, &reg_data);

    reg_data = 0x0;
    soc_AVR_PORTMACRO_PMI_LP_WADDRr_field_set(unit, &reg_data, PMI_LP_WRITE_ADDRESSf, &pmi_phy_reg_addr);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_WADDRr(unit, &reg_data);

    reg_data = 0x0;
    soc_AVR_PORTMACRO_PMI_LP_WDATAr_field_set(unit, &reg_data, PMI_LP_WRITE_DATAf, &phy_wr_data);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_WDATAr(unit, &reg_data);

    reg_data = 0x0;
    fval = 1;
    soc_AVR_PORTMACRO_PMI_LP_WRGOr_field_set(unit, &reg_data, PMI_LP_WRITE_GOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_WRGOr(unit, &reg_data);

    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
        reg_data = 0x0;
        REG_READ_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        soc_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr_field_get(unit, &reg_data, PMI_LP_WRITE_DONEf, &done);
        if ( done == 1 ) {
            REG_WRITE_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        }
        timeout--;
    }

    if (timeout == 0 ) {
        sal_printf ("ERROR : Timeout while writing to phy register   : PhyId = 0x%x \
                     PhyRegAddr = 0x%x PhyWrData =  0x%x \n",
                     phy_id, phy_reg_addr, phy_wr_data);
        return -1;
    }
    if (phy_debug) {
        sal_printf ("Writing to phy register   : PhyId = 0x%x PhyRegAddr = 0x%x PhyWrData = 0x%x \n",
                (phy_id & 0xFF), phy_reg_addr, phy_wr_data);
    }
    return 0;
}

int cdk_avng_pmi_read(int unit, uint32 phy_id, uint32 phy_reg_addr,
                                      uint32 *phy_rd_data)
{
    uint32 reg_data;
    uint32 pmi_phy_reg_addr;
    uint32 done;
    int timeout = 200;
    uint32  fval;
    if ( (phy_id & 0xFF) == 9 ) {
        pmi_phy_reg_addr = 0x10000 | phy_reg_addr;
        fval = 0;
    } else {
        pmi_phy_reg_addr = phy_reg_addr;
        fval = 1;
    }
    reg_data = 0x0;
    soc_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr_field_set(unit, &reg_data, RESERVED_FOR_ECOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr(unit, &reg_data);

    reg_data = 0x0;
    soc_AVR_PORTMACRO_PMI_LP_RADDRr_field_set(unit, &reg_data, PMI_LP_READ_ADDRESSf, &pmi_phy_reg_addr);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_RADDRr(unit, &reg_data);

    reg_data = 0x0;
    fval = 1;
    soc_AVR_PORTMACRO_PMI_LP_RDGOr_field_set(unit, &reg_data, PMI_LP_READ_GOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_RDGOr(unit, &reg_data);

    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
        reg_data = 0x0;
        REG_READ_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        soc_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr_field_get(unit, &reg_data, PMI_LP_READ_DONEf, &done);
        if ( done == 1 ) {
            REG_WRITE_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        }
        timeout--;
    }

    if (timeout == 0 ) {
        sal_printf ("ERROR : Timeout while reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x \n", phy_id, phy_reg_addr);
        return -1;
    }
    reg_data = 0x0;
    REG_READ_AVR_PORTMACRO_PMI_LP_RDATAr(unit, &reg_data);
    *phy_rd_data = (uint16)reg_data;
    if (phy_debug)
        sal_printf ("Reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x PhyDataRead = 0x%x \n", (phy_id & 0xFF), phy_reg_addr, reg_data);
    return 0;

}

int
avng_phy_init_with_speed(int unit, uint8_t lport,uint32_t speed)
{
    int rv = CDK_E_NONE;
    phy_ctrl_t *pc;

    if (AVNG_PORT_PHY_CTRL(unit, lport)) {
        pc = AVNG_PORT_PHY_CTRL(unit, lport);
        pc->speed = speed;
        PHY_CTRL_INTF(pc) = phymodInterfaceXFI;
        if (CDK_SUCCESS(rv)) {
            PHY_LOCK(pc);
            rv = PHY_INIT(AVNG_PORT_PHY_CTRL(unit, lport));
            PHY_UNLOCK(pc);
        }
        if (CDK_SUCCESS(rv) && phy_init_cb) {
            rv = phy_init_cb(AVNG_PORT_PHY_CTRL(unit, lport));
        }
    }

    return rv;
}

int
avng_phy_init(int unit, uint8_t lport)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc;

    pc = AVNG_PORT_PHY_CTRL(unit, lport);
    avng_interface_speed_set(unit, lport);
    if ((lport == 12) || (lport == 13)) {
        rv = avng_portmacro_speed_set(unit, lport, pc->speed);
        if (CDK_FAILURE(rv)) {
            return CDK_E_INIT;
        }
    }
    if (AVNG_PORT_PHY_CTRL(unit, lport)) {
        if (CDK_SUCCESS(rv)) {
            PHY_LOCK(pc);
            rv = PHY_INIT(AVNG_PORT_PHY_CTRL(unit, lport));
            PHY_UNLOCK(pc);
        }
        if (CDK_SUCCESS(rv) && phy_init_cb) {
            rv = phy_init_cb(AVNG_PORT_PHY_CTRL(unit, lport));
        }
    }

    return rv;
}

int
avng_phy_attach(int unit, uint8_t lport)
{
    int rv;

    CDK_VERB(("avng_phy_attach: lport=%d\n", lport));
    AVNG_PORT_PHY_BUS(unit, lport) = avng_phy_bus;

    rv = avng_phy_probe_default(unit, lport, avng_phy_drv_list);

    if (CDK_SUCCESS(rv)) {
        rv = avng_phy_init(unit, lport);
    }

    return rv;
}

int
avng_phy_probe(int unit, uint8_t lport)
{
    int rv;
    AVNG_PORT_PHY_BUS(unit, lport) = avng_phy_bus;
    rv = avng_phy_probe_default(unit, lport, avng_phy_drv_list);
    return rv;
}

int
avng_phy_mode_set(int unit, uint8_t lport, char *name, int mode, int enable)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc = AVNG_PORT_PHY_CTRL(unit, lport);

    while (pc != NULL) {
        if (name && pc->drv && pc->drv->drv_name &&
            (sal_strcmp(pc->drv->drv_name, name) != 0)) {
            pc = pc->next;
            continue;
        }
        switch (mode) {
        case AVNG_PHY_MODE_WAN:
            PHY_LOCK(pc);
            rv = PHY_CONFIG_SET(pc, PhyConfig_Mode,
                                enable ? PHY_MODE_WAN : PHY_MODE_LAN, NULL);
            PHY_UNLOCK(pc);
            if (!enable && rv == CDK_E_UNAVAIL) {
                rv = CDK_E_NONE;
            }
            break;
        case AVNG_PHY_MODE_2LANE:
            if (enable) {
                PHY_CTRL_FLAGS(pc) |= PHY_F_2LANE_MODE;
            } else {
                PHY_CTRL_FLAGS(pc) &= ~PHY_F_2LANE_MODE;
            }
            break;
        case AVNG_PHY_MODE_SERDES:
            if (enable) {
                PHY_CTRL_FLAGS(pc) |= PHY_F_SERDES_MODE;
            } else {
                PHY_CTRL_FLAGS(pc) &= ~PHY_F_SERDES_MODE;
            }
            break;
        case AVNG_PHY_MODE_FIBER:
            if (enable) {
                PHY_CTRL_FLAGS(pc) |= PHY_F_FIBER_MODE;
            } else {
                PHY_CTRL_FLAGS(pc) &= ~PHY_F_FIBER_MODE;
            }
            break;
        default:
            rv = CDK_E_PARAM;
            break;
        }
        break;
    }
    return rv;
}

int
avng_phy_fw_base_set(int unit, uint8_t lport, char *name, uint32_t fw_base)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc = AVNG_PORT_PHY_CTRL(unit, lport);

    while (pc != NULL) {
        if (name && pc->drv && pc->drv->drv_name &&
            (sal_strcmp(pc->drv->drv_name, name) != 0)) {
            pc = pc->next;
            continue;
        }
        PHY_LOCK(pc);
        rv = PHY_CONFIG_SET(pc, PhyConfig_RamBase, fw_base, NULL);
        PHY_UNLOCK(pc);
        if (rv == CDK_E_UNAVAIL) {
            rv = CDK_E_NONE;
        }
        break;
    }
    return rv;
}
int
avng_phy_fw_helper_set(int unit, uint8_t lport,
                      int (*fw_helper)(void *, uint32_t, uint32_t, void *))
{
    phy_ctrl_t *pc = AVNG_PORT_PHY_CTRL(unit, lport);

    while (pc != NULL) {
        PHY_CTRL_FW_HELPER(pc) = fw_helper;
        pc = pc->next;
    }
    return CDK_E_NONE;
}

int
avng_phy_line_interface_set(int unit, uint8_t lport, int intf)
{
    int pref_intf;
    phy_ctrl_t *pc = AVNG_PORT_PHY_CTRL(unit, lport);

    if (AVNG_PORT_PHY_CTRL(unit, lport)) {
        switch (intf) {
        case AVNG_PHY_IF_XFI:
            pref_intf = PHY_IF_XFI;
            break;
        case AVNG_PHY_IF_SFI:
            pref_intf = PHY_IF_SFI;
            if (sal_strcmp(pc->drv->drv_name, "bcmi_tsce_xgxs") != 0) {
                PHY_CTRL_FLAGS(pc) |= PHY_F_FIBER_MODE;
            }
            break;
        case AVNG_PHY_IF_KR:
            pref_intf = PHY_IF_KR;
            break;
        case AVNG_PHY_IF_CR:
            pref_intf = PHY_IF_CR;
            break;
        default:
            pref_intf = 0;
            break;
        }
        PHY_CTRL_LINE_INTF(AVNG_PORT_PHY_CTRL(unit, lport)) = pref_intf;
    }
    return CDK_E_NONE;
}

int
avng_phy_external_mode_get(int unit, int lport)
{
    return PHY_EXTERNAL_MODE(lport);
}

int
avng_phy_ability_set(int unit, uint8_t lport, char *name, int ability)
{
    int rv = CDK_E_NONE;

    phy_ctrl_t *pc = AVNG_PORT_PHY_CTRL(unit, lport);
    int phy_abil = 0;

    while (pc != NULL) {
        if (name && pc->drv && pc->drv->drv_name &&
            ((sal_strcmp(pc->drv->drv_name, "bcmi_tsce_xgxs") != 0) &&
              (sal_strcmp(pc->drv->drv_name, "bcmi_viper_xgxs") != 0))) {
            pc = pc->next;
            continue;
        }

        if (ability & AVNG_PHY_ABIL_10MB_HD)
            phy_abil |= PHY_ABIL_10MB_HD;
        if (ability & AVNG_PHY_ABIL_10MB_FD)
            phy_abil |= PHY_ABIL_10MB_FD;
        if (ability & AVNG_PHY_ABIL_100MB_HD)
            phy_abil |= PHY_ABIL_100MB_HD;
        if (ability & AVNG_PHY_ABIL_100MB_FD)
            phy_abil |= PHY_ABIL_100MB_FD;
        if (ability & AVNG_PHY_ABIL_1000MB_HD)
            phy_abil |= PHY_ABIL_1000MB_HD;
        if (ability & AVNG_PHY_ABIL_1000MB_FD)
            phy_abil |= PHY_ABIL_1000MB_FD;
        if (ability & AVNG_PHY_ABIL_2500MB)
            phy_abil |= PHY_ABIL_2500MB;
        if (ability & AVNG_PHY_ABIL_10GB)
            phy_abil |= PHY_ABIL_10GB;

        phy_abil |= (PHY_ABIL_PAUSE_TX | PHY_ABIL_PAUSE_RX);

        PHY_LOCK(pc);
        rv = PHY_CONFIG_SET(pc, PhyConfig_AdvLocal, phy_abil, NULL);
        PHY_UNLOCK(pc);
        if (rv == CDK_E_UNAVAIL) {
            rv = CDK_E_NONE;
        }
        break;
    }
    return rv;
}

int
avng_phy_eee_set(int unit, uint8_t lport, uint32 mode)
{
    if (AVNG_PORT_PHY_CTRL(unit, lport)) {
        uint32_t eee_mode = PHY_EEE_NONE;
        int rv;
        eee_mode = mode;
        PHY_LOCK(pc);
        rv = PHY_CONFIG_SET(AVNG_PORT_PHY_CTRL(unit, lport),
                            PhyConfig_EEE, eee_mode, NULL);
        PHY_UNLOCK(pc);
        if (mode == PHY_EEE_NONE && rv == CDK_E_UNAVAIL) {
            rv = CDK_E_NONE;
        }
        return rv;
    }
    return CDK_E_NONE;
}

int
avng_phy_eee_get(int unit, uint8_t lport, uint32 *mode)
{
    *mode = PHY_EEE_NONE;
    uint32_t eee_mode = 0;

    if (AVNG_PORT_PHY_CTRL(unit, lport)) {
        PHY_LOCK(pc);
        int rv = PHY_CONFIG_GET(AVNG_PORT_PHY_CTRL(unit, lport),
                                PhyConfig_EEE, &eee_mode, NULL);
        PHY_UNLOCK(pc);
        if (CDK_FAILURE(rv) && rv != CDK_E_UNAVAIL) {
            return rv;
        }
        if (eee_mode == PHY_EEE_802_3) {
            *mode = AVNG_PHY_M_EEE_802_3;
        } else if (eee_mode == PHY_EEE_AUTO) {
            *mode = AVNG_PHY_M_EEE_AUTO;
        }
    }
    return CDK_E_NONE;
}


/* Set phy media type */
int
avng_phy_media_type_set(int unit, uint8_t port, uint32 media) {
    int    rv    = CDK_E_NONE;
    phy_ctrl_t *pc;
    uint32   speed = 10000;
    if ((port < 12) || (port > 13)) {
        return CDK_E_PARAM;
    }
    pc = AVNG_PORT_PHY_CTRL(unit, port);
    switch (media) {
        case CBX_PORT_PHY_MEDIA_TYPE_COPPER:
            pc->media = 1;
            speed = 10001;
            break;
         default:
            pc->media = 0;
            break;
     }
     rv = avng_phy_speed_set(unit, port, speed);
     return rv;
}

/* Get phy media type */
int
avng_phy_media_type_get(int unit, uint8_t port, uint32 *media) {
    int rv = CDK_E_NONE;
    phy_ctrl_t *pc;
    pc = AVNG_PORT_PHY_CTRL(unit, port);
    *media = pc->media;
    return rv;
}


int
avng_phy_display_eye_scan(int unit, int port) {

    int rv = CDK_E_NONE;
    uint16_t level = 1 << 3; /* eye scan */
    if (AVNG_PORT_PHY_CTRL(unit, port)) {
        rv = PHY_DISPLAY_DIAG(AVNG_PORT_PHY_CTRL(unit, port),
                                  level);

    }
    return rv;
}



typedef struct _bcast_sig_s {
    const char *drv_name;
    const char *bus_name;
    uint32_t addr;
} _bcast_sig_t;

#define MAX_BCAST_SIG   8
#define MAX_INIT_STAGE  8

int
avng_phy_staged_init(int unit)
{
    int rv = CDK_E_NONE;
    int lport, idx, found;
    phy_ctrl_t *pc;
    uint32_t addr;
    _bcast_sig_t bcast_sig[MAX_BCAST_SIG];
    int num_sig, stage, done;

    num_sig = 0;
    int i = 0, cascade = 0;
    pbmp_t avenger_pbmp = 0;
    for (i=0; i<14; i++) {
        SOC_PBMP_PORT_ADD(avenger_pbmp,i);
        if (cascade) {
            SOC_PBMP_PORT_ADD(avenger_pbmp, i+16);
        }
    }
    SOC_PBMP_ITER(avenger_pbmp, lport) {
        pc = AVNG_PORT_PHY_CTRL(unit, lport);
        for (; pc != NULL; pc = pc->next) {
            /* Let driver know that staged init is being used */
            PHY_CTRL_FLAGS(pc) |= PHY_F_STAGED_INIT;
            /* Mark as broadcast slave by default */
            PHY_CTRL_FLAGS(pc) &= ~PHY_F_BCAST_MSTR;
            /* Get broadcast signature */
            rv = PHY_CONFIG_GET(pc, PhyConfig_BcastAddr, &addr, NULL);
            if (CDK_FAILURE(rv)) {
                continue;
            }
            if (pc->drv == NULL ||  pc->drv->drv_name == NULL) {
                continue;
            }
            if (pc->bus == NULL ||  pc->bus->drv_name == NULL) {
                continue;
            }
            /* Check if broadcast signature exists */
            found = 0;
            for (idx = 0; idx < num_sig; idx++) {
                if (bcast_sig[idx].drv_name == pc->drv->drv_name &&
                    bcast_sig[idx].bus_name == pc->bus->drv_name &&
                    bcast_sig[idx].addr == addr) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                continue;
            }
            if (idx >= MAX_BCAST_SIG) {
                return CDK_E_FAIL;
            }
            /* Add new broadcast signature */
            bcast_sig[idx].drv_name = pc->drv->drv_name;
            bcast_sig[idx].bus_name = pc->bus->drv_name;
            bcast_sig[idx].addr = addr;
            CDK_VERB(("PHY init: new bcast sig: %s %s 0x%04x\n",
                      bcast_sig[idx].drv_name,
                      bcast_sig[idx].bus_name,
                      bcast_sig[idx].addr));
            num_sig++;
            /* Mark as master for this broadcast domain */
            PHY_CTRL_FLAGS(pc) |= PHY_F_BCAST_MSTR;
        }
    }

    /* Reset all PHYs */
    SOC_PBMP_ITER(avenger_pbmp, lport) {
        pc = AVNG_PORT_PHY_CTRL(unit, lport);
        if (pc == NULL) {
            continue;
        }
        rv = PHY_RESET(pc);
        if (CDK_FAILURE(rv)) {
            return rv;
        }
    }

    /* Perform reset callbacks */
    if (phy_reset_cb) {
        SOC_PBMP_ITER(avenger_pbmp, lport) {
            pc = AVNG_PORT_PHY_CTRL(unit, lport);
            if (pc == NULL) {
                continue;
            }
            rv = phy_reset_cb(pc);
            if (CDK_FAILURE(rv)) {
                return rv;
            }
        }
    }

    /* Repeat staged initialization until no more work */
    stage = 0;
    do {
        CDK_VERB(("PHY init: stage %d\n", stage));
        done = 1;
        SOC_PBMP_ITER(avenger_pbmp, lport) {
            CDK_VVERB(("PHY init: stage %d, lport %d\n", stage, lport));
            pc = AVNG_PORT_PHY_CTRL(unit, lport);
            for (; pc != NULL; pc = pc->next) {
                rv = PHY_CONFIG_SET(pc, PhyConfig_InitStage, stage, NULL);
                if (rv == CDK_E_UNAVAIL) {
                    /* Perform standard init if stage 0 is unsupported */
                    if (stage == 0) {
                        rv = PHY_INIT(pc);
                        if (CDK_FAILURE(rv)) {
                            return rv;
                        }
                        while (pc->next != NULL) {
                            pc = pc->next;
                        }
                    }
                    rv = CDK_E_NONE;
                    continue;
                }
                if (CDK_FAILURE(rv)) {
                    return rv;
                }
                done = 0;
            }
        }
        /* Add safety guard against loops */
        if (++stage > MAX_INIT_STAGE) {
            return CDK_E_INTERNAL;
        }
    } while (!done);

    /* Perform init callbacks */
    if (phy_init_cb) {
        SOC_PBMP_ITER(avenger_pbmp, lport) {
            pc = AVNG_PORT_PHY_CTRL(unit, lport);
            if (pc == NULL) {
                continue;
            }
            rv = phy_init_cb(pc);
            if (CDK_FAILURE(rv)) {
                return rv;
            }
        }
    }
    return rv;
}

extern int
phy_brcm_xe_read(phy_ctrl_t *pc, uint32_t addr, uint32_t *data);
extern int
phy_tsc_iblk_read(phy_ctrl_t *pc, uint32_t addr, uint32_t *data);

#define BCM542xx_MII_ANPr             (0x00000005)
#define BCMI_TSC_XGXS_LP_BASE_PAGE1r  (0x0000c198)
#define SOC_PA_PAUSE_TX        (1 << 0)       /* TX pause capable */
#define SOC_PA_PAUSE_RX        (1 << 1)       /* RX pause capable */
#if 0
int
phy_pause_get(uint8_t unit, uint8_t lport, uint8_t *tx_pause, uint8_t *rx_pause)
{
    int rv = 0;
    phy_ctrl_t *pc = AVNG_PORT_PHY_CTRL(unit, lport);
    uint32_t remote_advert, local_advert = (SOC_PA_PAUSE_RX | SOC_PA_PAUSE_TX);
    uint32_t ability = 0;
    PHY_CTRL_CHECK(pc);

    if (PHY_EXTERNAL_MODE(lport)) {
        if (!sal_strcmp(pc->drv->drv_name,"bcm542xx")) {
            PHY_LOCK(pc);
            rv = PHY_CONFIG_GET(AVNG_PORT_PHY_CTRL(unit, lport),
                               PhyConfig_AdvRemote, &ability, NULL);
            PHY_UNLOCK(pc);
        }
    } else {
        if (!sal_strcmp(pc->drv->drv_name,"bcmi_tsce_xgxs") ||
            !sal_strcmp(pc->drv->drv_name,"bcmi_viper_xgxs")) {
            /* Assume CL73 AN Bit[11:10] */
            PHY_LOCK(pc);
            rv = PHY_CONFIG_GET(AVNG_PORT_PHY_CTRL(unit, lport),
                               PhyConfig_AdvRemote, &ability, NULL);
            PHY_UNLOCK(pc);

        } else {
            return -4; /* SYS_ERR_PARAMETER */
        }
    }

    if (CDK_FAILURE(rv)) {
        return rv;
    }

    remote_advert = 0;
    if (ability & PHY_ABIL_PAUSE_TX) {
        remote_advert |= SOC_PA_PAUSE_TX;
    }
    if (ability & PHY_ABIL_PAUSE_RX) {
        remote_advert |= SOC_PA_PAUSE_RX;
    }

    /*
     * IEEE 802.3 Flow Control Resolution.
     * Please see $SDK/doc/pause-resolution.txt for more information.
     * Assume local always advertises PASUE and ASYM PAUSE.
     */
    *tx_pause =
            ((remote_advert & SOC_PA_PAUSE_RX) &&
             (local_advert & SOC_PA_PAUSE_RX)) ||
             ((remote_advert & SOC_PA_PAUSE_RX) &&
             !(remote_advert & SOC_PA_PAUSE_TX) &&
             (local_advert & SOC_PA_PAUSE_TX));

    *rx_pause =
             ((remote_advert & SOC_PA_PAUSE_RX) &&
             (local_advert & SOC_PA_PAUSE_RX)) ||
             ((local_advert & SOC_PA_PAUSE_RX) &&
             (remote_advert & SOC_PA_PAUSE_TX) &&
             !(local_advert & SOC_PA_PAUSE_TX));

    return(rv);
}
#endif

void *cdk_memset(void *dest,int c,size_t cnt) {
    return sal_memset(dest, c, cnt);
}

void *cdk_memcpy(void *dest,const void *src,size_t cnt) {
    return sal_memcpy(dest, src, cnt);
}


size_t cdk_strlen(const char *str) {
   return sal_strlen(str);
}

int cdk_strncmp(const char *dest,const char *src,size_t cnt) {
   return sal_strncmp(dest, src, cnt);
}

int cdk_strcmp(const char *dest,const char *src) {
   return sal_strcmp(dest, src);
}

char *cdk_strcat(char *dest,const char *src) {
   return sal_strcat(dest, src);
}

char *cdk_strncpy(char *dest,const char *src,size_t cnt) {
	return sal_strncpy(dest, src, cnt);
}

char *cdk_strcpy(char *dest,const char *src) {
	return sal_strcpy(dest, src);
}

int
cdk_sprintf(char *buf, const char *fmt, ...)
{
    va_list arg_ptr;

    if (buf == NULL || fmt == NULL) {
        return 0;
    }

    va_start(arg_ptr, fmt);
    sal_vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    return sal_strlen(buf) + 1;
}


void
usleep(uint32 usec)
{
    sal_usleep(usec);
}


int
cdk_printf(const char *fmt, ...)
{
    va_list arg_ptr;
    char buf[256];

    va_start(arg_ptr, fmt);
    sal_vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);
    sal_printf(buf);
    return 0;
}


int
phymod_is_write_disabled(const phymod_access_t *pa, uint32_t *data)
{
    if(pa && PHYMOD_ACC_BUS(pa)) {
        if (PHYMOD_ACC_BUS(pa)->is_write_disabled == NULL) {
            *data = 0;
        } else {
           return PHYMOD_ACC_BUS(pa)->is_write_disabled(PHYMOD_ACC_USER_ACC(pa),data);
        }
    } else {
          *data = 0;
    }
    return 0;
}



int
avng_show_phy_info(int unit, int port)
{
    phy_ctrl_t *pc;
    uint32_t phy_addr;
    int px;

    sal_printf("Unit %d Port %d\n", unit, port);
    pc = AVNG_PORT_PHY_CTRL(unit, port);
    px = 0;

    if (pc == NULL) {
        CDK_PRINTF("    (no PHYs)\n");
    }

    while (pc && pc->drv) {
        phy_addr = 0xffff;
        if (pc->bus && pc->bus->phy_addr) {
           phy_addr = pc->bus->phy_addr(port);
        }
        CDK_PRINTF("    PHY[%d] at 0x%04"PRIx32": %s\n",
                             px, phy_addr, pc->drv->drv_name);
        pc = pc->next;
        px++;
    }
    return 0;
}


void qsgmii_init(int unit)  {
    phy_ctrl_t *pc;
    uint8_t lport;
    int phyid;
    lport = 8;
    pc = AVNG_PORT_PHY_CTRL(unit, lport);

    pc->interface = phymodInterfaceQSGMII;

    for (phyid=16 ;phyid <= 19;phyid++) {
        uint32_t phy_rd_data;
     // Enable RDB
       cdk_avng_miim_write(unit,phyid , 0x17, 0, 0xF7E);
       cdk_avng_miim_write(unit,phyid , 0x15, 0, 0x0);
  // Target the REG with RBD address 0x2A (Legacy address 0x18 Shadow 0x2) ,
       cdk_avng_miim_write(unit,phyid , 0x1E, 0, 0x2A);
       cdk_avng_miim_read(unit, phyid, 0x1f,  0,&phy_rd_data);
       phy_rd_data = phy_rd_data & 0xFFDF;
       cdk_avng_miim_write(unit,phyid, 0x1F ,  0,phy_rd_data);
       cdk_avng_miim_read(unit, phyid, 0x1f,  0,&phy_rd_data);

     }

#ifdef QSGMII_EXTERNAL_PHY_5G
     // 5G enable in External phy
     // -------------------------
     // Set AER as QSGMII[0]
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0xFFD0);
     cdk_avng_miim_write(unit,0x18 ,0x1E, 0, 0x0);
     // Broadcast to all lanes
     cdk_avng_miim_write(unit,0x18 ,0x1E, 0, 0x1F);
     // Write reg 0x8000, 0x10
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0x8000);
     cdk_avng_miim_write(unit,0x18 ,0x10, 0, 0x202F);
     // Write reg 0x8000 0x1E
     cdk_avng_miim_write(unit,0x18 ,0x1E, 0, 0x1);
     // Write reg 0x0000 0x0
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0x0);
     cdk_avng_miim_write(unit,0x18 ,0x0, 0, 0xA040);
     // Write reg 0x8000 0x10
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0x8000);
     cdk_avng_miim_write(unit,0x18 ,0x10, 0, 0x62F);
     // Write reg 0x8300 0x18
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0x8300);
     cdk_avng_miim_write(unit,0x18 ,0x18, 0, 0x5800);
     // Write reg 0x8000 0x10
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0x8000);
      cdk_avng_miim_write(unit,0x18 ,0x10, 0, 0x263F);
     // Write reg 0x8010 0x16
     cdk_avng_miim_write(unit,0x18 ,0x1F, 0, 0x8010);
     cdk_avng_miim_write(unit,0x18 ,0x16, 0, 0x303);
#endif /* QSGMII_EXTERNAL_PHY_5G */

     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x0);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800d);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x4000);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800f);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xb),0x1F, 0,  0x0);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xb),0x0 , 0, 0x1140);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x0);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800d);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x4000);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800f);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xc),0x1F, 0,  0x0);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xc),0x0 , 0, 0x1140);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x0);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800d);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x4000);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800f);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xd),0x1F, 0,  0x0);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xd),0x0 , 0, 0x1140);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x0);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe, 0,  0x800d);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xd, 0,  0x4000);
     cdk_avng_miim_write(unit, (CDK_XGSM_MIIM_INTERNAL + 11), 0xe,  0, 0x800f);
     cdk_avng_miim_write(unit,(CDK_XGSM_MIIM_INTERNAL + 0xe),0x0, 0, 0x1140);
     return ;
}

#ifdef CONFIG_8488X

#define PHY_BRCM_OUI6   0xd40129        /* Broadcom OUI */
#define PHY_BCM8488X_OUI        PHY_BRCM_OUI6
#define PHY_BCM84888_MODEL      0x14
#define PHY_BCM84887_MODEL      0x14
#define PHY_BCM84886_MODEL      0x17
#define PHY_BCM84884_MODEL      0x14
#define PHY_BCM84881_MODEL      0x15
#define PHY_BCM84885_MODEL      0x17


#define PHYCTRL_INIT_STATE_DEFAULT 0
#define PHYCTRL_INIT_STATE_PASS1   1
#define PHYCTRL_INIT_STATE_PASS2   2
#define PHYCTRL_INIT_STATE_PASS3   3
#define PHYCTRL_INIT_STATE_PASS4   4
#define PHYCTRL_INIT_STATE_PASS5   5
#define PHYCTRL_INIT_STATE_PASS6   6
#define PHYCTRL_INIT_STATE_PASS7   7


#define CHIP_IS_BCM8488X(_o, _m)    ((_o == PHY_BCM8488X_OUI) && \
                                     ((_m == PHY_BCM84888_MODEL) || \
                                      (_m == PHY_BCM84886_MODEL) || \
                                      (_m == PHY_BCM84884_MODEL) || \
                                      (_m == PHY_BCM84881_MODEL)) )

#define WRITE_BLK_SIZE 4
#define  BIT(_n_)      (1U << (_n_))


extern sys_error_t
mdio_read(uint8 unit, uint8 phy_addr, uint16 reg_address, uint8 dev_addr, uint8 cl45, uint16 *rdata);

extern sys_error_t
mdio_write(uint8 unit, uint8 phy_addr, uint16 reg_address, uint8 dev_addr, uint8 cl45, uint16 wdata);

extern uint8 bcm_84888_firmware[];
extern int bcm_84888_firmware_size;

uint32_t phyctrl_init_state = PHYCTRL_INIT_STATE_PASS1;
uint32_t phy_oui_model;
uint32_t phy_rev;

typedef struct soc_timeout_s {
    uint32_t		expire;
    uint32_t		usec;
    int			min_polls;
    int			polls;
    uint32_t		exp_delay;
} soc_timeout_t;
soc_timeout_t to;
phy_ctrl_t pc_val;
phy_ctrl_t *pc = &pc_val;

uint8 debug_mdio = 0;

#define BCM8488x_REG_READ(unit, phy_id, reg_addr, dev_id, rdata)  cdk_avng_miim_read(unit, phy_id, (reg_addr | dev_id << 16), 1, rdata)
    /* mdio_read((uint8)unit, phy_id, reg_addr, dev_id, 0xFF, rdata) */
#define BCM8488x_REG_WRITE(unit, phy_id, reg_addr, dev_id, wdata) cdk_avng_miim_write(unit, phy_id, (reg_addr | dev_id << 16), 1, wdata)
    /* mdio_write((uint8)unit,phy_id, reg_addr, dev_id, 0xFF, wdata) */

/******** Register read/write ******************************************/
int
plp_orca_phy_reg_read_8481(int unit, phy_ctrl_t *pc, uint32 reg_addr, uint16 *reg_data)
{
    uint32_t  data32;

    CDK_IF_ERROR_RETURN(
            BCM8488x_REG_READ(unit, pc->port, (reg_addr & 0xFFFF), pc->dev_id, (uint32 *)&data32) );
    *reg_data = (uint16) data32 & 0xffff;

    return SOC_E_NONE;
}

int
plp_orca_phy_reg_write_8481(int unit, phy_ctrl_t *pc, uint32 reg_addr, uint16 reg_data)
{
    uint32_t  data32 = (uint32_t) reg_data;

    CDK_IF_ERROR_RETURN(
            BCM8488x_REG_WRITE(unit, pc->port, (reg_addr & 0xFFFF), pc->dev_id, data32) );

    return SOC_E_NONE;
}

int
plp_orca_phy_reg_modify_8481(int unit, phy_ctrl_t *pc, uint32 reg_addr,
               uint16 reg_data, uint16 reg_mask)
{
    uint16  tmp, otmp;

    reg_data = reg_data & reg_mask;
    CDK_IF_ERROR_RETURN(PHY8481_REG_READ(unit, pc, reg_addr, &tmp));
    otmp = tmp; tmp &= ~(reg_mask);
    tmp |= reg_data;
    if (otmp != tmp) {
        CDK_IF_ERROR_RETURN(PHY8481_REG_WRITE(unit, pc, reg_addr, tmp));
    }
    return SOC_E_NONE;
}
int
plp_orca_phy_8481_reg_xge_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 data, uint16 mask)
{
    sal_printf("No support for plp_orca_phy_8481_reg_xge_modify\n");
    return SOC_E_NONE;

}

/* reverse the bits in each byte of a 32 bit long */
uint32_t
_phy_rev_by_byte_word32(uint32_t n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    return n;
}

#define PHY_OUI(id0, id1) \
    _phy_rev_by_byte_word32((uint32)(id0) << 6 | ((id1) >> 10 & 0x3f))

#define PHY_MODEL(id0, id1) ((id1) >> 4 & 0x3f)

#define PHY_OUI_MODEL(oui, model)       (((oui) << 8) | (model))

#define PHY_REV(id0, id1)   ((id1) & 0xf)

STATIC int
_plp_orca_phy_8481_halt(int unit, phy_ctrl_t *pc)
{
    int i = 0;

    pc->dev_id = 0x1e;
    for(i= 0; i< 4; i++) {
        pc->port = i;
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x418c,0x0000));
    }
    for(i= 0; i< 4; i++) {
        pc->port = i;
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4188,0x48f0));
    }
    for(i= 0; i< 4; i++) {
        pc->port = i;
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4186,0x8000)); /* enable global reset */
    }
    for(i= 0; i< 4; i++) {
        pc->port = i;
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0017c)); /* assert reset for the whole ARM system */
    }
    for(i= 0; i< 4; i++) {
        pc->port = i;
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040)); /* deassert reset for the whole ARM system
                                                                 but the ARM processor */
    }

    pc->dev_id = 0x1;
    pc->port = 0;
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0000));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xc300));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x0010));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0x0000));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0000));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x1018));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xe59f));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0004));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x1f11));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xee09));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0008));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x0000));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xe3a0));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x000c));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x1806));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xe3a0));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));


    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0010));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x0002));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xe8a0));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0014));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x0001));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xe150));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0018));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0xfffc));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0x3aff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x001c));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0xfffe));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0xeaff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_LOW,0x0020));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_ADDR_HIGH,0xffff));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_LOW,0x0021));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_DATA_HIGH,0x0004));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, MDIO2ARM_CTL,0x0009));

    pc->dev_id = 0x1e;
    for(i= 0; i< 4; i++) {
        pc->port = i;
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0)); /* deassert reset  */
    }

    return (SOC_E_NONE);
}

/* This routine should be used only when the ARM CPU is halted/looping. */
STATIC int
_plp_orca_phy_8481_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len)
{
    int n_writes, i;

    sal_printf("8488x start of ucode download\n");
    pc->dev_id = 0x1;
    pc->port = 0;
    n_writes = (len + WRITE_BLK_SIZE -1) / WRITE_BLK_SIZE;
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 0x38));

    debug_mdio = 0;
    for ( i = 0; i < n_writes; i++ ) {

        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc,
                ((*( data + WRITE_BLK_SIZE/2 + 1 ))<<8)|( *( data + WRITE_BLK_SIZE/2 ))));
        CDK_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, ((*( data + 1 ))<<8)|(*( data + 0 ))));

        data += WRITE_BLK_SIZE;

    }
    CDK_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG( unit, pc, 0 ));
    sal_printf("8488x end of ucode download \n");
    debug_mdio = 0;

    return (SOC_E_NONE);
}

#ifdef CHECK_FIRMWARE
STATIC int
_plp_orca_phy_8481_check_firmware(int unit,phy_ctrl_t *pc) {
    uint16 tmp16;
    uint8  fw_ver_chip_rev, fw_ver_main, fw_ver_branch, fw_ver_sub_branch,
           fw_date_month, fw_date_day, fw_date_year, spirom_crc_check_status;

    CDK_IF_ERROR_RETURN
                (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400f, &tmp16));
    fw_ver_chip_rev = (tmp16 >> 12) & 0xf;
    fw_ver_main     = (tmp16 >> 7) & 0x1f;
    fw_ver_branch   =  tmp16 & 0x7f;

    fw_ver_sub_branch = fw_ver_chip_rev;

    CDK_IF_ERROR_RETURN
        (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x4010,&tmp16));

    fw_date_month = (tmp16 >> 9) & 0xf;                      /* BIT[12:9] */
    fw_date_day   = (tmp16 >> 4) & 0x1f;                     /* BIT[9:4]  */
    fw_date_year  = (tmp16 & 0xf) | ((tmp16 & BIT(13) >> 9));  /* [13,3:0] */

    sal_printf(" Firmware Chip rev : 0x%x main: 0x%x branch : 0x%x sub branch :  0x%x\n",
                           fw_ver_chip_rev,fw_ver_main, fw_ver_branch,fw_ver_sub_branch);
    sal_printf(" Firmware  date : year : 0x%x month : 0x%x day : 0x%x \n",
                            fw_date_year,fw_date_month,fw_date_day);
    CDK_IF_ERROR_RETURN
        (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400d, &tmp16));
    spirom_crc_check_status = (tmp16 >> 14) & 0x3;
    if (spirom_crc_check_status == 0x2) {
        sal_printf(" bad CRC \n");
    }
    return SOC_E_NONE;
}

#endif

/*
 * Function:
 *      plp_orca_phy_8481_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */


int
plp_orca_phy_8481_probe(int unit)
{
    uint16 id0, id1;
    pc->dev_id = 1;
    pc->port = 0;
    sal_usleep(100000);
    if (READ_PHY8481_TOPLVL1_ID0r(unit, pc, &id0) < 0 ) {
        return SOC_E_NOT_FOUND;
    }

    if ( READ_PHY8481_TOPLVL1_ID1r(unit, pc, &id1) < 0 ) {
        return SOC_E_NOT_FOUND;
    }

    phy_oui_model = PHY_OUI_MODEL(PHY_OUI(id0, id1), PHY_MODEL(id0, id1));
    phy_rev = PHY_REV(id0, id1);
    sal_printf("8488x phy_oui_model is 0x%x and revision is 0x%x\n",phy_oui_model,phy_rev);
    /* check OUI & MODEL ID to identify the PHY chip */
    switch (phy_oui_model) {

        case  (PHY_OUI_MODEL(PHY_BCM8488X_OUI, PHY_BCM84888_MODEL)):
              break;
        case  (PHY_OUI_MODEL(PHY_BCM8488X_OUI, PHY_BCM84886_MODEL)):
              break;
        case  (PHY_OUI_MODEL(PHY_BCM8488X_OUI, PHY_BCM84881_MODEL)):
              break;
        default:
             return SOC_E_NOT_FOUND;

    }
    return SOC_E_NONE;
}

/* Function:
 *    plp_orca_phy_8481_init
 * Purpose:
 *    Initialize 8481 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 * Returns:
 *    SOC_E_NONE
 */


int
plp_orca_phy_8481_init(int unit)
{
    uint16         fw_ver;
    int            i = 0;
    if ((phyctrl_init_state == PHYCTRL_INIT_STATE_PASS1) ||
        (phyctrl_init_state == PHYCTRL_INIT_STATE_DEFAULT)) {

        /* Read the f/w version register,                         *\
        \* if != 0, that means f/w is running and no need to load */
        CDK_IF_ERROR_RETURN
               (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400f, &fw_ver));

        /* Disable loopback */
        /* PCS Control 1 Reg. DEVAD=3 Addr=0, bit[14] PCS loopback mode  */
        CDK_IF_ERROR_RETURN
                (MODIFY_PHY8481_PCS_CTRLr(unit, pc,
                                0, PCS_IEEE_CTL1_LPBK));

        uint8 *firmware = bcm_84888_firmware;
        uint32 firmware_len = bcm_84888_firmware_size;


        if (firmware) {
           /* indicate second pass of init is needed */
           phyctrl_init_state = PHYCTRL_INIT_STATE_PASS2;
            /* Turn on broadcast mode */
           pc->dev_id = 0x1e;
           for (i = 0; i < 4; i++) {
               pc->port = i;
               CDK_IF_ERROR_RETURN(WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4117, 0xf003));
           }
           for (i = 0; i < 4; i++) {
               pc->port = i;
               CDK_IF_ERROR_RETURN(WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4107, 0x0401));
           }
        }

        CDK_IF_ERROR_RETURN
           (_plp_orca_phy_8481_halt(unit, pc));

        CDK_IF_ERROR_RETURN
        (_plp_orca_phy_8481_write_to_arm(unit, pc, 0, firmware, firmware_len));

        pc->dev_id = 0x1;
        pc->port = 0;
        /* write_addr=0xc300_0000, data=0x0000_002c, enable bottom write protection
          and set VINITHI signal to 0 (reset vector at 0x0000_0000) */

        CDK_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 0x0000));

         /* Reset the processor to start execution of the code in the on-chip memory */
         CDK_IF_ERROR_RETURN(WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0xa008, 0x0));
         pc->dev_id = 0x1e;
         for (i = 0; i < 4; i++) {
             pc->port = i;
             CDK_IF_ERROR_RETURN(WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x8004, 0x5555));
         }
         pc->dev_id = 0x1;
         pc->port = 0;
         CDK_IF_ERROR_RETURN
                   (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa003 , 0x0));
         CDK_IF_ERROR_RETURN
                   (WRITE_PHY8481_PMAD_REG(unit, pc, 0x0000 , 0x8000));

        sal_usleep(2000000);

        uint16 data = 0;
        pc->dev_id = 0x1;
        for (i = 0; i < 4; i++) {
           pc->port = i;
           CDK_IF_ERROR_RETURN(READ_PHY8481_TOPLVL1_REG(unit, pc, 0x0000, &data));
        }
        pc->port = 1;
        pc->dev_id = 0x1e;
        CDK_IF_ERROR_RETURN(READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400d, &data));
        phyctrl_init_state = PHYCTRL_INIT_STATE_PASS2;

        /* Remove super isolate */
        for (i = 0; i < 4; i++) {
            pc->port = i;
            CDK_IF_ERROR_RETURN
               (MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, 0, (1U << 15)));
            CDK_IF_ERROR_RETURN
               (WRITE_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, 0));
        }
    }
    return SOC_E_NONE;
}
#endif /* CONFIG_8488X */

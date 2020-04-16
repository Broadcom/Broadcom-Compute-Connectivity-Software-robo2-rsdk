/*
 * $Id: robo2_miim_ext.c,v 1.8 2016/03/10 03:06:57 mohanm  $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#include <phy/phy.h>
#include <avng_miim.h>

static uint32_t
_phy_addr(int port)
{
    return port + 0x80;
}

static int
_read(int unit, uint32_t addr, uint32_t reg, uint32_t *val)
{
    return cdk_avng_miim_read(unit, addr, reg, 0, val);
}

static int
_write(int unit, uint32_t addr, uint32_t reg, uint32_t val)
{
    return cdk_avng_miim_write(unit, addr, reg, 0, val);
}

static int
_phy_inst(int port)
{
    if (port <= 7) {
        return 0;
    } else if(port <= 11) {
        return (port - 8);
    } else {
        return 0;
    }
}

phy_bus_t phy_bus_avng_miim_ext = {
    "robo2_miim_ext",
    _phy_addr,
    _read,
    _write,
    _phy_inst
};


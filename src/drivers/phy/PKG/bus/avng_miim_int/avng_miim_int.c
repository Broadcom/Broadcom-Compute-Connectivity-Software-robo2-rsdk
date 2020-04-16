/*
 * $Id: avng_miim_int.c,v 1.8 2016/03/10 03:06:57 mohanm  $
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
    if (port <= 7) {
        return (port + 0x1 + CDK_XGSM_MIIM_INTERNAL);
    } else if(port <= 11) {
        return (11 + CDK_XGSM_MIIM_INTERNAL);
    } else if(port == 12) {
        return (10 + CDK_XGSM_MIIM_INTERNAL);
    } else if(port == 13) {
        return (9 + CDK_XGSM_MIIM_INTERNAL);
    } else {
        return (port + CDK_XGSM_MIIM_INTERNAL);
    }
}

static int
_read(int unit, uint32_t addr, uint32_t reg, uint32_t *val)
{
    if (((addr & 0xFF) == 0x9) || ((addr & 0xFF) == 0xa)) {
        return cdk_avng_pmi_read(unit, addr, reg, val);
    } else {
        return cdk_avng_miim_read(unit, addr, reg, 0, val);
    }
}

static int
_write(int unit, uint32_t addr, uint32_t reg, uint32_t val)
{
    if (((addr & 0xFF) == 0x9) || ((addr & 0xFF) == 0xa)) {
        return cdk_avng_pmi_write(unit, addr, reg, val);
    } else {
        return cdk_avng_miim_write(unit, addr, reg, 0, val);
    }
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


phy_bus_t phy_bus_avng_miim_int = {
    "avng_miim_int",
    _phy_addr,
    _read,
    _write,
    _phy_inst
};



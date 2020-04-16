/*
 * $Id: phymod_sys.c $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.All Rights Reserved.$
 *
 * Implementation of custom config functions of PHYMOD library.
 */

#include <phymod/phymod_sys.h>

void
phymod_udelay(uint32_t usecs)
{
    PHY_SYS_USLEEP(usecs);
}

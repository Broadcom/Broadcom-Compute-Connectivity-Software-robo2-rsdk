/*
 * $Id: phymod_sys.c $
 * $Copyright: (c) 2017 Broadcom Corporation All Rights Reserved.$
 *
 * Implementation of custom config functions of PHYMOD library.
 */

#include <phymod/phymod_sys.h>

void
phymod_udelay(uint32_t usecs)
{
    PHY_SYS_USLEEP(usecs);
}

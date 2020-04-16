/*
 *         
 * $Id: phymod_definitions.h,v 1.2.2.12 2013/09/03 06:54:51 dayad Exp $
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: 
 * All Rights Reserved.$
 *         
 * Shell diagnostics of Phymod    
 *
 */

#ifndef _PHYMOD_UTIL_H_
#define _PHYMOD_UTIL_H_

#include <phymod/phymod.h>


/******************************************************************************
Functions
******************************************************************************/


int phymod_util_lane_config_get(const phymod_access_t *phys, int *start_lane, int *num_of_lane);

#endif /*_PHYMOD_UTIL_H_*/

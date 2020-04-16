/*
 * $Id: auto_voip.h.h Exp $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#ifndef _AUTO_VOIP_H_
#define _AUTO_VOIP_H_

extern sys_error_t bcm53158_auto_voip_tc_set(uint8 unit, uint8 tc_val);
extern sys_error_t bcm53158_auto_voip_slic_rule_set(uint8 unit, uint32 smac_hi_24);
extern sys_error_t bcm53158_auto_voip_init(uint8 unit);

#endif /* _AUTO_VOIP_H_ */


/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 * File:
 *    robo2_drv.h
 * Description
 *    robo2_drv.h
 *
 */


#ifndef _SOC_ROBO2_DRV_H
#define _SOC_ROBO2_DRV_H

typedef enum soc_robo2_rev_id_e {
    SocRobo2RevA,
} soc_robo2_rev_id_t;


typedef enum soc_robo2_arch_type_e {
    SOC_ROBO2_ARCH_ROBO2,
} soc_robo2_arch_type_t;

typedef enum soc_robo2_chip_type_e {
    SOC_ROBO2_CHIP_53158,
} soc_robo2_chip_type_t;


typedef struct soc_robo2_control_s {
    soc_robo2_chip_type_t	chip_type;
    soc_robo2_rev_id_t	rev_id;
    int			rev_num;
    uint32			chip_bonding;
    soc_robo2_arch_type_t	arch_type;
} soc_robo2_control_t;


/* TBD:Update Bonding option for Avenger */
#define SOC_ROBO2_BOND_M                     0x0001
#define SOC_ROBO2_BOND_S                     0x0002
#define SOC_ROBO2_BOND_V                     0x0004
#define SOC_ROBO2_BOND_PHY_S3MII             0x0008
#define SOC_ROBO2_BOND_PHY_COMBOSERDES       0x0010


/*
 * SOC_ROBO_* control driver macros
 */
#define SOC_ROBO2_CONTROL(unit) \
        ((soc_robo2_control_t *)(SOC_CONTROL(unit)->drv))


#define SOC_IS_AVENGER(unit)   \
        ((SOC_ROBO2_CONTROL(unit) == NULL) ? 0: \
         (SOC_ROBO2_CONTROL(unit)->chip_type == SOC_ROBO_CHIP_53158))

#define SOC_IS_ROBO2_GE_SWITCH(unit) (SOC_IS_AVENGER(unit)

#endif

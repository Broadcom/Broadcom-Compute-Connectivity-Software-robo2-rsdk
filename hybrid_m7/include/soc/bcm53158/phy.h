/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/*
 * phy.h
 */
#ifndef SOC_PHY_H
#define SOC_PHY_H

/* EEE Advertisement Register */
#define PHY_ADV_EEE_1000T       (1 << 2)
#define PHY_ADV_EEE_100TX       (1 << 1)

/* These are not Register values. These are Software internal values. */
/* EEE Supported PHY types */
#define PHY_EEE_100MB_BASETX         (1 << 0)    /* EEE for 100M-BaseTX */
#define PHY_EEE_1GB_BASET            (1 << 1)    /* EEE for 1G-BaseT */
#define PHY_EEE_10GB_BASET           (1 << 2)    /* EEE for 10G-BaseT */
#define PHY_EEE_10GB_KX              (1 << 3)    /* EEE for 10G-KX */
#define PHY_EEE_10GB_KX4             (1 << 4)    /* EEE for 10G-KX4 */
#define PHY_EEE_10GB_KR              (1 << 5)    /* EEE for 10G-KR */


#define PHY_POWER_AUTO      1
#define PHY_POWER_AUTO_OFF  2
#define PHY_POWER_DLL_DOWN  3
#define PHY_APD_MODE        0x20

#define PHY_NONE     0
#define PHY_INTERNAL 1
#define PHY_EXTERNAL 2

extern sys_error_t
bcm53158_phy_autoneg_get(uint8 unit, uint8 port, BOOL *an, BOOL *an_done, uint16 *speed);

extern sys_error_t
APIFUNC(bcm53158_phy_eee_ability_remote_get)(uint8 unit, uint8 port, uint8 *types) REENTRANT;

extern sys_error_t
APIFUNC(bcm53158_phy_link_up)(uint8 unit, uint8 port, BOOL link);

extern sys_error_t
bcm53158_phy_power_mode_set(uint8 unit, uint8 dev, uint16 mode) REENTRANT;

sys_error_t
bcm53158_phy_eee_enable_set(uint8 unit, uint8 port, BOOL gig_dis, BOOL enable);

extern void
bcm53158_dummy_mdio_access(uint8 unit);

extern sys_error_t bcm53158_gphy_amplitude_set(uint8 unit);

extern sys_error_t bcm53158_phy_cable_diag(uint8 unit);
#endif /* SOC_PHY_H */


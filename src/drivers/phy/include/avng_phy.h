/*
 * $Id: avng_phy.h,v 1.9 2015/09/22 06:09:30 hchang Exp $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#ifndef __AVNG_PHY_H__
#define __AVNG_PHY_H__

#define FW_ALIGN_BYTES                  16
#define FW_ALIGN_MASK                   (FW_ALIGN_BYTES - 1)

typedef enum {
    SYS_OK                          =     0,
    SYS_ERR_FALSE                   =    -1,   /* FALSE condition */
    SYS_ERR                         =    -2,   /* generic error */
    SYS_ERR_CANCELLED               =    -3,
    SYS_ERR_PARAMETER               =    -4,
    SYS_ERR_TIMEOUT                 =    -5,
    SYS_ERR_FULL                    =    -6,
    SYS_ERR_STATE                   =    -7,
    SYS_ERR_OUT_OF_RESOURCE         =    -8,
    SYS_ERR_NOT_IMPLEMENTED         =    -9,
    SYS_ERR_EXISTS                  =    -10,
    SYS_ERR_NOT_FOUND               =    -11
} sys_error_t;

typedef enum phymod_interface_e {
    phymodInterfaceBypass = 0,
    phymodInterfaceSR,
    phymodInterfaceSR4,
    phymodInterfaceKX,
    phymodInterfaceKX4,
    phymodInterfaceKR,
    phymodInterfaceKR2,
    phymodInterfaceKR4,
    phymodInterfaceCX,
    phymodInterfaceCX2,
    phymodInterfaceCX4,
    phymodInterfaceCR,
    phymodInterfaceCR2,
    phymodInterfaceCR4,
    phymodInterfaceCR10,
    phymodInterfaceXFI,
    phymodInterfaceSFI,
    phymodInterfaceSFPDAC,
    phymodInterfaceXGMII,
    phymodInterface1000X,
    phymodInterfaceSGMII,
    phymodInterfaceXAUI,
    phymodInterfaceRXAUI,
    phymodInterfaceX2,
    phymodInterfaceXLAUI,
    phymodInterfaceXLAUI2,
    phymodInterfaceCAUI,
    phymodInterfaceQSGMII,
    phymodInterfaceLR4,
    phymodInterfaceLR,
    phymodInterfaceLR2,
    phymodInterfaceER,
    phymodInterfaceER2,
    phymodInterfaceER4,
    phymodInterfaceSR2,
    phymodInterfaceSR10,
    phymodInterfaceCAUI4,
    phymodInterfaceVSR,
    phymodInterfaceLR10,
    phymodInterfaceKR10,
    phymodInterfaceCAUI4_C2C,
    phymodInterfaceCAUI4_C2M,
    phymodInterfaceZR,
    phymodInterfaceLRM,
    phymodInterfaceXLPPI,
    phymodInterfaceCount
} phymod_interface_t;

/*
 * AVNG PHY interface.
 *
 * These functions act as glue for the external PHY driver interface
 * in order to reduce impact in case the external PHY interface
 * changes.
 */

#define AVNG_PHY_MODE_WAN        1
#define AVNG_PHY_MODE_2LANE      2
#define AVNG_PHY_MODE_SCRAMBLE   3
#define AVNG_PHY_MODE_SERDES     4
#define AVNG_PHY_MODE_FIBER      5

#define AVNG_PHY_M_EEE_OFF       0
#define AVNG_PHY_M_EEE_802_3     1
#define AVNG_PHY_M_EEE_AUTO      2

#define AVNG_PHY_IF_DEFAULT      0
#define AVNG_PHY_IF_NULL         1
#define AVNG_PHY_IF_MII          2
#define AVNG_PHY_IF_GMII         3
#define AVNG_PHY_IF_SGMII        4
#define AVNG_PHY_IF_TBI          5
#define AVNG_PHY_IF_XGMII        6
#define AVNG_PHY_IF_RGMII        7
#define AVNG_PHY_IF_RVMII        8
#define AVNG_PHY_IF_XAUI         9
#define AVNG_PHY_IF_XLAUI        10
#define AVNG_PHY_IF_XFI          11
#define AVNG_PHY_IF_SFI          12
#define AVNG_PHY_IF_KR           13
#define AVNG_PHY_IF_CR           14
#define AVNG_PHY_IF_FIBER        15
#define AVNG_PHY_IF_HIGIG        16
#define AVNG_PHY_IF_KX           17
#define AVNG_PHY_IF_RXAUI        18

#define AVNG_PHY_ABIL_LOOPBACK       (1L << 0)
#define AVNG_PHY_ABIL_AN             (1L << 1)
#define AVNG_PHY_ABIL_TBI            (1L << 2)
#define AVNG_PHY_ABIL_MII            (1L << 3)
#define AVNG_PHY_ABIL_GMII           (1L << 4)
#define AVNG_PHY_ABIL_SGMII          (1L << 5)
#define AVNG_PHY_ABIL_XGMII          (1L << 6)
#define AVNG_PHY_ABIL_SERDES         (1L << 7)
#define AVNG_PHY_ABIL_AN_SGMII       (1L << 8)
#define AVNG_PHY_ABIL_RGMII          (1L << 9)
#define AVNG_PHY_ABIL_RVMII          (1L << 10)
#define AVNG_PHY_ABIL_XAUI           (1L << 11)
#define AVNG_PHY_ABIL_PAUSE_TX       (1L << 13)
#define AVNG_PHY_ABIL_PAUSE_RX       (1L << 14)
#define AVNG_PHY_ABIL_PAUSE_ASYMM    (1L << 15)
#define AVNG_PHY_ABIL_10MB_HD        (1L << 16)
#define AVNG_PHY_ABIL_10MB_FD        (1L << 17)
#define AVNG_PHY_ABIL_100MB_HD       (1L << 18)
#define AVNG_PHY_ABIL_100MB_FD       (1L << 19)
#define AVNG_PHY_ABIL_1000MB_HD      (1L << 20)
#define AVNG_PHY_ABIL_1000MB_FD      (1L << 21)
#define AVNG_PHY_ABIL_2500MB         (1L << 22)
#define AVNG_PHY_ABIL_3000MB         (1L << 23)
#define AVNG_PHY_ABIL_10GB           (1L << 24)
#define AVNG_PHY_ABIL_13GB           (1L << 25)
#define AVNG_PHY_ABIL_16GB           (1L << 26)
#define AVNG_PHY_ABIL_21GB           (1L << 27)
#define AVNG_PHY_ABIL_25GB           (1L << 28)
#define AVNG_PHY_ABIL_30GB           (1L << 29)
#define AVNG_PHY_ABIL_40GB           (1L << 30)
#define AVNG_PHY_ABIL_PAUSE          (AVNG_PHY_ABIL_PAUSE_TX | AVNG_PHY_ABIL_PAUSE_RX)
#define AVNG_PHY_ABIL_10MB           (AVNG_PHY_ABIL_10MB_HD | AVNG_PHY_ABIL_10MB_FD)
#define AVNG_PHY_ABIL_100MB          (AVNG_PHY_ABIL_100MB_HD | AVNG_PHY_ABIL_100MB_FD)
#define AVNG_PHY_ABIL_1000MB         (AVNG_PHY_ABIL_1000MB_HD | AVNG_PHY_ABIL_1000MB_FD)

#ifdef CONFIG_10G_INTF_KR
#define REG_ACCESS_METHOD_SHIFT                  28
#define REG_ACC_TSC_IBLK                         (0x7 << REG_ACCESS_METHOD_SHIFT)
#define TSCE_RX_X4_STATUS1_PMA_PMD_LIVE_STATUS   0xc470
#define TSCE_RX_X4_STATUS1_PCS_LIVE_STATUS       0xc474
#define TSCE_AN_X4_ABILITIES_ENABLES             0xc480
#define TSCE_AN_X4_ABILITIES_ENABLES_AN               (1L << 14)
#define TSCE_AN_X4_ABILITIES_ENABLES_CL73             (1L << 8)
#define TSCE_AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART  (1L << 0)
#endif

extern int
avng_phy_probe(int unit, uint8_t port);

extern int
avng_phy_init(int unit, uint8_t port);

extern int
avng_phy_init_with_speed(int unit, uint8_t port, uint32_t speed);

extern int
avng_phy_staged_init(int unit);

extern int
avng_phy_attach(int unit, uint8_t port);

extern int
avng_phy_detach(int unit, uint8_t port);

extern int
avng_phy_mode_set(int unit, uint8_t port, char *name, int mode, int enable);

extern int
avng_phy_notify_mac_enable(int unit, uint8_t port, int enable);

extern int
avng_phy_notify_mac_loopback(int unit, uint8_t port, uint32_t enable);

extern int
avng_phy_link_get(int unit, uint8_t port, uint32_t *link);

extern int
avng_phy_autoneg_set(int unit, uint8_t port, uint32_t an);

extern int
avng_phy_autoneg_get(int unit, uint8_t port, uint32_t *an);

extern int
avng_phy_speed_set(int unit, uint8_t port, uint32_t speed);

extern int
avng_phy_speed_get(int unit, uint8_t port, uint32_t *speed);

extern int
avng_phy_duplex_set(int unit, uint8_t port, uint32_t duplex);

extern int
avng_phy_duplex_get(int unit, uint8_t port, uint32_t *duplex);

extern int
avng_phy_loopback_set(int unit, uint8_t port, uint32_t enable);

extern int
avng_phy_loopback_get(int unit, uint8_t port, uint32_t *enable);

extern int
avng_phy_remote_loopback_set(int unit, uint8_t port, int enable);

extern int
avng_phy_remote_loopback_get(int unit, uint8_t port, int *enable);

extern int
avng_phy_line_interface_set(int unit, uint8_t port, int intf);

extern int
avng_phy_line_interface_get(int unit, uint8_t port, int *intf);

extern int
avng_phy_eee_set(int unit, uint8_t port, uint32_t mode);

extern int
avng_phy_eee_get(int unit, uint8_t port, uint32_t *mode);

extern int
avng_phy_media_type_set(int unit, uint8_t port, uint32_t media);

extern int
avng_phy_media_type_get(int unit, uint8_t port, uint32_t *media);

extern int
avng_phy_fw_helper_set(int unit, uint8_t port,
                      int (*fw_helper)(void *, uint32_t, uint32_t, void *));

extern int
avng_phy_fw_info_get(void *ctx, int *unit, int *port, const char **drv_name);

extern int
avng_phy_fw_base_set(int unit, uint8_t port, char *name, uint32_t fw_base);

extern int
avng_phy_laneswap_set(int unit, uint8_t port);

extern int
avng_phy_external_mode_get(int unit, int lport);

extern int
avng_phy_ability_set(int unit, uint8_t port, char *name, int ability);

extern int soc_phy_probe(void);

extern int
avng_phy_reg_read(uint8_t unit, uint8_t lport, uint32_t reg_addr, uint32_t *p_value);

extern int
avng_phy_reg_write(uint8_t unit, uint8_t lport, uint32_t reg_addr, uint32_t value);

extern int
avng_show_phy_info(int unit, int port);

extern int
avng_phy_display_eye_scan(int unit, int port);

extern void
qsgmii_init();
/* MII write access. */
extern int
cdk_avng_miim_write(int unit, uint32_t phy_addr, uint32_t reg, uint8 ext, uint32_t val);
/* MII read access. */
extern int
cdk_avng_miim_read(int unit, uint32_t phy_addr, uint32_t reg, uint8 ext, uint32_t *val);
#endif /* __AVNG_PHY_H__ */

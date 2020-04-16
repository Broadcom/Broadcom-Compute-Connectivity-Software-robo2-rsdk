/*
 * $Id: soc_phy.h,v 1.0 2016/03/14 11:53:37 mohanm Exp $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#ifndef _SOC_PHY_H
#define _SOC_PHY_H

typedef uint8  soc_port_t;
typedef uint32 soc_timeout_t;


#define SOC_SUCCESS(rv)          ((rv) ==  0)
#define SOC_FAILURE(rv)          ((rv) < 0)

/* Standard MII Registers */

#define MII_CTRL_REG            0x00    /* MII Control Register : r/w */
#define MII_STAT_REG            0x01    /* MII Status Register: ro */
#define MII_PHY_ID0_REG         0x02    /* MII PHY ID register: r/w */
#define MII_PHY_ID1_REG         0x03    /* MII PHY ID register: r/w */
#define MII_ANA_REG             0x04    /* MII Auto-Neg Advertisement: r/w */
#define MII_ANP_REG             0x05    /* MII Auto-Neg Link Partner: ro */
#define MII_AN_EXP_REG          0x06    /* MII Auto-Neg Expansion: ro */
#define MII_GB_CTRL_REG         0x09    /* MII 1000Base-T control register */
#define MII_GB_STAT_REG         0x0a    /* MII 1000Base-T Status register */
#define MII_ESR_REG             0x0f    /* MII Extended Status register */

/* Non-standard MII Registers */

#define MII_ECR_REG             0x10    /* MII Extended Control Register */
#define MII_AUX_REG             0x18 /* MII Auxiliary Control/Status Register */
#define MII_ASSR_REG            0x19    /* MII Auxiliary Status Summary Reg */
#define MII_GSR_REG             0x1c    /* MII General status (BROADCOM) */
#define MII_MSSEED_REG          0x1d    /* MII Master/slave seed (BROADCOM) */
#define MII_AUX_MULTI_PHY_REG   0x1e    /* Auxiliary Multiple PHY (BROADCOM) */
#define MII_TEST2_REG           0x1f    /* MII Test reg (BROADCOM) */

#define MII_SHA_CD_CTRL_REG     0x13 /*13h-0;13h-1;13h-2;13h-3;
                                  *13h-4;13h-5;13h-6;13h-7 */
#define MII_SHA_CD_SEL_REG      0x14
#define MII_SHA_AUX_STAT2_REG   0x1B
#define MII_BRCM_TEST_REG       0x1F


/* MII Control Register: bit definitions */

#define MII_CTRL_FS_2500        (1 << 5) /* Force speed to 2500 Mbps */
#define MII_CTRL_SS_MSB         (1 << 6) /* Speed select, MSb */
#define MII_CTRL_CST            (1 << 7) /* Collision Signal test */
#define MII_CTRL_FD             (1 << 8) /* Full Duplex */
#define MII_CTRL_RAN            (1 << 9) /* Restart Autonegotiation */
#define MII_CTRL_IP             (1 << 10) /* Isolate Phy */
#define MII_CTRL_PD             (1 << 11) /* Power Down */
#define MII_CTRL_AE             (1 << 12) /* Autonegotiation enable */
#define MII_CTRL_SS_LSB         (1 << 13) /* Speed select, LSb */
#define MII_CTRL_LE             (1 << 14) /* Loopback enable */
#define MII_CTRL_RESET          (1 << 15) /* PHY reset */

#define MII_CTRL_SS(_x)         ((_x) & (MII_CTRL_SS_LSB|MII_CTRL_SS_MSB))
#define MII_CTRL_SS_10          0
#define MII_CTRL_SS_100         (MII_CTRL_SS_LSB)
#define MII_CTRL_SS_1000        (MII_CTRL_SS_MSB)
#define MII_CTRL_SS_INVALID     (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)
#define MII_CTRL_SS_MASK        (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)

/*
 * MII Status Register: See 802.3, 1998 pg 544
 */
#define MII_STAT_EXT            (1 << 0) /* Extended Registers */
#define MII_STAT_JBBR           (1 << 1) /* Jabber Detected */
#define MII_STAT_LA             (1 << 2) /* Link Active */
#define MII_STAT_AN_CAP         (1 << 3) /* Autoneg capable */
#define MII_STAT_RF             (1 << 4) /* Remote Fault */
#define MII_STAT_AN_DONE        (1 << 5) /* Autoneg complete */
#define MII_STAT_MF_PS          (1 << 6) /* Preamble suppression */
#define MII_STAT_ES             (1 << 8) /* Extended status (R15) */
#define MII_STAT_HD_100_T2      (1 << 9) /* Half duplex 100Mb/s supported */
#define MII_STAT_FD_100_T2      (1 << 10)/* Full duplex 100Mb/s supported */
#define MII_STAT_HD_10          (1 << 11)/* Half duplex 100Mb/s supported */
#define MII_STAT_FD_10          (1 << 12)/* Full duplex 100Mb/s supported */
#define MII_STAT_HD_100         (1 << 13)/* Half duplex 100Mb/s supported */
#define MII_STAT_FD_100         (1 << 14)/* Full duplex 100Mb/s supported */
#define MII_STAT_100_T4         (1 << 15)/* Full duplex 100Mb/s supported */

/*
 * MII Link Advertisment
 */
#define MII_ANA_ASF             (1 << 0)/* Advertise Selector Field */
#define MII_ANA_HD_10           (1 << 5)/* Half duplex 10Mb/s supported */
#define MII_ANA_FD_10           (1 << 6)/* Full duplex 10Mb/s supported */
#define MII_ANA_HD_100          (1 << 7)/* Half duplex 100Mb/s supported */
#define MII_ANA_FD_100          (1 << 8)/* Full duplex 100Mb/s supported */
#define MII_ANA_T4              (1 << 9)/* T4 */
#define MII_ANA_PAUSE           (1 << 10)/* Pause supported */
#define MII_ANA_ASYM_PAUSE      (1 << 11)/* Asymmetric pause supported */
#define MII_ANA_RF              (1 << 13)/* Remote fault */
#define MII_ANA_NP              (1 << 15)/* Next Page */

#define MII_ANA_ASF_802_3       (1)     /* 802.3 PHY */

/*
 * MII Link Advertisment (Clause 37)
 */
#define MII_ANA_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANA_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANA_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANA_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANA_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANA_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANA_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANA_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANA_C37_FD          (1 << 5)   /* Full duplex */

/*
 * MII Link Partner Ability (Clause 37)
 */
#define MII_ANP_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANP_C37_ACK         (1 << 14)  /* Acknowledge */
#define MII_ANP_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANP_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANP_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANP_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANP_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANP_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANP_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANP_C37_FD          (1 << 5)   /* Full duplex */

/*
 * MII Link Partner Ability (SGMII)
 */
#define MII_ANP_SGMII_MODE      (1 << 0)   /* Link parneter in SGMII mode */
#define MII_ANP_SGMII_FD        (1 << 12)  /* SGMII duplex */
#define MII_ANP_SGMII_SS_10     (0 << 10)  /* 10Mbps SGMII */
#define MII_ANP_SGMII_SS_100    (1 << 10)  /* 100Mbps SGMII */
#define MII_ANP_SGMII_SS_1000   (2 << 10)  /* 1000Mbps SGMII */
#define MII_ANP_SGMII_SS_MASK   (3 << 10)  /* SGMII speed mask */
#define MII_ANP_SGMII_ACK       (1 << 14)  /* SGMII Ackonwledge */
#define MII_ANP_SGMII_LINK      (1 << 15)  /* SGMII Link */


/*
 * 1000Base-T Control Register
 */
#define MII_GB_CTRL_MS_MAN      (1 << 12) /* Manual Master/Slave mode */
#define MII_GB_CTRL_MS          (1 << 11) /* Master/Slave negotiation mode */
#define MII_GB_CTRL_PT          (1 << 10) /* Port type */
#define MII_GB_CTRL_ADV_1000FD  (1 << 9) /* Advertise 1000Base-T FD */
#define MII_GB_CTRL_ADV_1000HD  (1 << 8) /* Advertise 1000Base-T HD */

/*
 * 1000Base-T Status Register
 */
#define MII_GB_STAT_MS_FAULT    (1 << 15) /* Master/Slave Fault */
#define MII_GB_STAT_MS          (1 << 14) /* Master/Slave, 1 == Master */
#define MII_GB_STAT_LRS         (1 << 13) /* Local receiver status */
#define MII_GB_STAT_RRS         (1 << 12) /* Remote receiver status */
#define MII_GB_STAT_LP_1000FD   (1 << 11) /* Link partner 1000FD capable */
#define MII_GB_STAT_LP_1000HD   (1 << 10) /* Link partner 1000HD capable */
#define MII_GB_STAT_IDE         (0xff << 0) /* Idle error count */

/*
 * IEEE Extended Status Register
 */
#define MII_ESR_1000_X_FD       (1 << 15) /* 1000Base-T FD capable */
#define MII_ESR_1000_X_HD       (1 << 14) /* 1000Base-T HD capable */
#define MII_ESR_1000_T_FD       (1 << 13) /* 1000Base-T FD capable */
#define MII_ESR_1000_T_HD       (1 << 12) /* 1000Base-T FD capable */

/*
 * MII Extended Control Register (BROADCOM)
 */
#define MII_ECR_FE              (1 << 0) /* FIFO Elasticity */
#define MII_ECR_TLLM            (1 << 1) /* Three link LED mode */
#define MII_ECR_ET_IPG          (1 << 2) /* Extended XMIT IPG mode */
#define MII_ECR_FLED_OFF        (1 << 3) /* Force LED off */
#define MII_ECR_FLED_ON         (1 << 4) /* Force LED on */
#define MII_ECR_ELT             (1 << 5) /* Enable LED traffic */
#define MII_ECR_RS              (1 << 6) /* Reset Scrambler */
#define MII_ECR_BRSA            (1 << 7) /* Bypass Receive Sym. align */
#define MII_ECR_BMLT3           (1 << 8) /* Bypass MLT3 Encoder/Decoder */
#define MII_ECR_BSD             (1 << 9) /* Bypass Scramble/Descramble */
#define MII_ECR_B4B5B           (1 << 10) /* Bypass 4B/5B Encode/Decode */
#define MII_ECR_FI              (1 << 11) /* Force Interrupt */
#define MII_ECR_ID              (1 << 12) /* Interrupt Disable */
#define MII_ECR_TD              (1 << 13) /* XMIT Disable */
#define MII_ECR_DAMC            (1 << 14) /* DIsable Auto-MDI Crossover */
#define MII_ECR_10B             (1 << 15) /* 1 == 10B, 0 == GMII */

/*
 * GSR (BROADCOM)
 */
#define MII_GSR_FD              (1 << 0) /* Full duplex active */
#define MII_GSR_SI              (1 << 1) /* Speed 0-->10, 1 --> 100 */
#define MII_GSR_FORCE           (1 << 2) /* Force 0-->10, 1--> 100 */
#define MII_GSR_AN              (1 << 3) /* Autonegotiation enabled */

/*
 * Auxiliary Control/Status Register
 */
#define MII_AUX_LD          (1<<14) /* Link Integrity test disabled in PHY */

/*
 * Auxiliary Status Summary Register (ASSR - Broadcom BCM5400)
 */
#define MII_ASSR_PRTD           (1 << 0) /* Pause resolution/XMIT direction */
#define MII_ASSR_PRRD           (1 << 1) /* Pause resolution/RCV direction */
#define MII_ASSR_LS             (1 << 2) /* Link Status (1 == link up) */
#define MII_ASSR_LPNPA          (1 << 3) /* Link partner next page cap */
#define MII_ASSR_LPANA          (1 << 4) /* Link Partner AN capable */
#define MII_ASSR_ANPR           (1 << 5) /* Autoneg page received */
#define MII_ASSR_RF             (1 << 6) /* Remote Fault */
#define MII_ASSR_PDF            (1 << 7) /* Parallel detection fault */
#define MII_ASSR_HCD            (7 << 8) /* Current operating speed */
#define MII_ASSR_ANNPW          (1 << 11) /* Auto-neg next page wait */
#define MII_ASSR_ANABD          (1 << 12) /* Auto-neg Ability detected */
#define MII_ASSR_ANAD           (1 << 13) /* Auto-neg ACK detect */
#define MII_ASSR_ANCA           (1 << 14) /* Auto-neg complete ACK */
#define MII_ASSR_ANC            (1 << 15) /* AUto-neg complete */

#define MII_ASSR_HCD_FD_1000    (7 << 8)
#define MII_ASSR_HCD_HD_1000    (6 << 8)
#define MII_ASSR_HCD_FD_100     (5 << 8)
#define MII_ASSR_HCD_T4_100     (4 << 8)
#define MII_ASSR_HCD_HD_100     (3 << 8)
#define MII_ASSR_HCD_FD_10      (2 << 8)
#define MII_ASSR_HCD_HD_10      (1 << 8)
#define MII_ASSR_HCD_NC         (0 << 8) /* Not complete */

#define MII_TEST2_MS_SEL        (1 << 11)

/*
 * MII Broadcom Test Register(MII_SHA_CD_CTRL_REG)
 */
#define MII_SHA_CD_CTRL_PB_STAT (0x3 << 12) /* 13-0 pair B status */
#define MII_SHA_CD_CTRL_PA_STAT (0x3 << 10) /* 13-0 pair A status */
#define MII_SHA_CD_CTRL_START   (0x1 << 2)  /* 13-0 start bit */
#define MII_SHA_CD_CTRL_PB_LEN  (0xFF << 8)  /* 13-1 pair B length */
#define MII_SHA_CD_CTRL_PA_LEN  (0xFF)       /* 13-1 pair A length */

/*
 * MII Broadcom Test Register(MII_SHA_AUX_STAT2_REG)
 */
#define MII_SHA_AUX_STAT2_LEN   (0x7 << 12) /* Enable Shadow registers */

/*
 * MII Broadcom Test Register(MII_BRCM_TEST_REG): bit definitions
 */
#define MII_BRCM_TEST_ENSHA         (1 << 7)    /* Enable Shadow registers */
#define MII_BRCM_TEST_REG_DEFAULT   0x000B      /* Register's default value */

/*
 * MII Broadcom
 * Auxiliary Multiple PHY Register(MII_AUX_MULTI_PHY_REG): bit definitions
 */
#define MII_BRCM_AUX_MULTI_PHY_SUPER_ISO (1 << 3) /* Super Isolate registers */
#define MII_BRCM_AUX_MULTI_PHY_RAN       (1 << 8) /* Restart-AN registers */

#define GOOD_CABLE_LEN_TUNING   35

#define CABLE_FUZZY_LEN1        10      /* for established good link */
#define CABLE_FUZZY_LEN2        1

#define MII_MSSEED_SEED         0x3ff

#define PHY_MIN_REG             0
#define PHY_MAX_REG             0x1f

/* Clause 45 MIIM */
#define PHY_C45_DEV_RESERVED    0x00
#define PHY_C45_DEV_PMA_PMD     0x01
#define PHY_C45_DEV_WIS         0x02
#define PHY_C45_DEV_PCS         0x03
#define PHY_C45_DEV_PHYXS       0x04
#define PHY_C45_DEV_DTEXS       0x05
#define PHY_C45_DEV_AN          0x07

/* LRE/BroadR-Reach (Broadcom) Registers */

#define LRE_CTRL_REG            0x00    /* LRE Control Register (Addr 00h) */
#define LRE_STAT_REG            0x01    /* LRE Status Register (ADDR 01h) */
#define LRE_PHY_ID0_REG         0x02    /* LRE PHY Identifier Register (ADDR 02h) */
#define LRE_PHY_ID1_REG         0x03    /* LRE PHY Identifier Register (ADDR 03h) */
#define LDS_ADVA_REG            0x04    /* LDS Advertised Ability Register (ADDR 04h) */
#define LDS_ADVC_REG            0x05    /* LDS Advertised Control Register (ADDR 05h)  */
#define LDS_ABNP_REG            0x06    /* LDS Ability Next Page Register (ADDR 06h) */
#define LDS_LPAB_REG            0x07    /* LDS Link Partner Ability Register (ADDR 07h) */
#define LDS_LPNP_REG            0x08    /* LDS Link Partner Next Page Message Register (ADDR 08h) */
#define LDS_LPABNP_REG          0x09    /* LDS Ability Next Page Register (ADDR 09h) */
#define LDS_EXP_REG             0x0a    /* LDS Expansion Register (ADDR 0ah) */
#define LDS_ACC_REG             0x0e    /* LDS Access Register (ADDR 0eh) */
#define LRE_EXTEN_REG           0x0f    /* LRE Extended Status Register (ADDR 0fh) */

/* LRE Control Register: bit definitions */

#define LRE_CTRL_UE            (1 << 2)   /* Unidirectional Enable */
#define LRE_CTRL_MS            (1 << 3)   /* Master Selection */

#define LRE_CTRL_PS_MASK       (0x3 << 4) /* Pair Selection Mask */
#define LRE_CTRL_PS_1P         (0x0 << 4) /* 1 Pair */
#define LRE_CTRL_PS_2P         (0x1 << 4) /* 2 Pairs */
#define LRE_CTRL_PS_4P         (0x2 << 4) /* 4 Pairs */

#define LRE_CTRL_SS_MASK       (0xf << 6) /* Speed Selection Mask */
#define LRE_CTRL_SS_10         (0x0 << 6) /* 10Mbps */
#define LRE_CTRL_SS_50         (0x1 << 6) /* 50Mbps */
#define LRE_CTRL_SS_33         (0x2 << 6) /* 33Mbps */
#define LRE_CTRL_SS_25         (0x3 << 6) /* 25Mbps */
#define LRE_CTRL_SS_20         (0x4 << 6) /* 20Mbps */
#define LRE_CTRL_SS_100        (0x8 << 6) /* 100Mbps */

#define LRE_CTRL_IP            (1 << 10)  /* Isolate Phy */
#define LRE_CTRL_PD            (1 << 11)  /* Power Down */
#define LRE_CTRL_LDSE          (1 << 12)  /* LDS Enable */
#define LRE_CTRL_RLDS          (1 << 13)  /* Restart LDS */
#define LRE_CTRL_LE            (1 << 14)  /* Loopback Enable */
#define LRE_CTRL_RESET         (1 << 15)  /* Reset */


/* LRE Status Register: bit definitions */

#define LRE_STAT_EXT           (1 << 0)   /* Extended Capability */
#define LRE_STAT_JBBR          (1 << 1)   /* Jabber Detected */
#define LRE_STAT_LA            (1 << 2)   /* Link Active */
#define LRE_STAT_LDS_CAP       (1 << 3)   /* LDS capable */
#define LRE_STAT_IEEE_CAP      (1 << 4)   /* Supports 802.3 PHY */
#define LRE_STAT_LDS_DONE      (1 << 5)   /* LDS Done */
#define LRE_STAT_MF_PS         (1 << 6)   /* Preamble Suppression */
#define LRE_STAT_UA            (1 << 7)   /* Unidirectional Ability */
#define LRE_STAT_ES            (1 << 8)   /* Extended Status */

#define LRE_STAT_1_10          (1 << 9)   /* 1 pair 10Mbps capable */
#define LRE_STAT_2_10          (1 << 10)  /* 2 pair 10Mbps capable */
#define LRE_STAT_2_100         (1 << 11)  /* 2 pair 100Mbps capable */
#define LRE_STAT_4_100         (1 << 12)  /* 4 pair 100Mbps capable */
#define LRE_STAT_1_100         (1 << 13)  /* 1 pair 100Mbps capable */


/* LDS Advertised Ability Register: bit definitions */

#define LDS_ADVA_1_10          (1 << 1)   /* Supports 1 pair 10Mbps */
#define LDS_ADVA_2_10          (1 << 2)   /* Supports 2 pair 10Mbps */
#define LDS_ADVA_2_100         (1 << 3)   /* Supports 2 pair 100Mbps */
#define LDS_ADVA_4_100         (1 << 4)   /* Supports 4 pair 100Mbps */
#define LDS_ADVA_1_100         (1 << 5)   /* Supports 1 pair 100Mbps */
#define LDS_ADVA_1_20          (1 << 6)   /* Supports 1 pair 20Mbps */
#define LDS_ADVA_1_25          (1 << 7)   /* Supports 1 pair 25Mbps */
#define LDS_ADVA_1_33          (1 << 8)   /* Supports 1 pair 33Mbps */
#define LDS_ADVA_1_50          (1 << 9)   /* Supports 1 pair 33Mbps */
#define LDS_ADVA_2_20          (1 << 10)  /* Supports 2 pair 20Mbps */
#define LDS_ADVA_2_25          (1 << 11)  /* Supports 2 pair 25Mbps */
#define LDS_ADVA_2_33          (1 << 12)  /* Supports 2 pair 33Mbps */
#define LDS_ADVA_2_50          (1 << 13)  /* Supports 2 pair 33Mbps */

#define LDS_ADVA_PAUSE         (1 << 14)  /* Supports Pause */
#define LDS_ADVA_ASYM_PAUSE    (1 << 15)  /* Supports Asymetric Pause */

/* LDS Advertised Control Register: bit definitions */

#define LDS_ADVC_AFU            (1 << 8)   /* ADVA register updated */

/* LDS Link Partner Ability Register: bit definitions */

#define LDS_LPAB_1_10          (1 << 1)   /* Supports 1 pair 10Mbps */
#define LDS_LPAB_2_10          (1 << 2)   /* Supports 2 pair 10Mbps */
#define LDS_LPAB_2_100         (1 << 3)   /* Supports 2 pair 100Mbps */
#define LDS_LPAB_4_100         (1 << 4)   /* Supports 4 pair 100Mbps */
#define LDS_LPAB_1_100         (1 << 5)   /* Supports 1 pair 100Mbps */
#define LDS_LPAB_1_20          (1 << 6)   /* Supports 1 pair 20Mbps */
#define LDS_LPAB_1_25          (1 << 7)   /* Supports 1 pair 25Mbps */
#define LDS_LPAB_1_33          (1 << 8)   /* Supports 1 pair 33Mbps */
#define LDS_LPAB_1_50          (1 << 9)   /* Supports 1 pair 33Mbps */
#define LDS_LPAB_2_20          (1 << 10)  /* Supports 2 pair 20Mbps */
#define LDS_LPAB_2_25          (1 << 11)  /* Supports 2 pair 25Mbps */
#define LDS_LPAB_2_33          (1 << 12)  /* Supports 2 pair 33Mbps */
#define LDS_LPAB_2_50          (1 << 13)  /* Supports 2 pair 33Mbps */
#define LDS_LPAB_PAUSE         (1 << 14)  /* Supports Pause */
#define LDS_LPAB_ASYM_PAUSE    (1 << 15)  /* Supports Asymetric Pause */


/* LDS Access Register: bit definitions */

#define LDS_ACC_STAT           (1 << 0)   /* LRE Register Acress Status */
#define LDS_ACC_OVV            (1 << 1)   /* LRE Register Acress Override Value */
#define LDS_ACC_OVR            (1 << 2)   /* LRE Register Acress Override */


/* Initial BR mode settings */
#define LR_INITIAL_MODE_LR_ENABLE (1 << 0) /* Enable BroadReach at init() */

/* Initial BR ctrl settings */
#define LR_INITIAL_CTRL_LDS_ENABLE (1 << 0) /* Enable LDS at init(), valid only in BR in enabled */
#define LR_INITIAL_CTRL_LR_MS (1 << 1) /* Master Selection */
#define LR_INITIAL_CTRL_LR_UE (1 << 2) /* Unidirectional enable */
/* Initial BR pairs, valid only if LDS is disabled, but BR is enabled (3 bits) */
#define LR_INITIAL_CTRL_LR_PS_MASK (0x7 << 3)
#define LR_INITIAL_CTRL_LR_PS_1P (0x0 << 3) /* 1 Pair */
#define LR_INITIAL_CTRL_LR_PS_2P (0x1 << 3) /* 2 Pairs */
#define LR_INITIAL_CTRL_LR_PS_4P (0x2 << 3) /* 4 Pairs */
/* Initial BR speed, valid only if LDS is disabled, but BR is enabled (6 bits) */
#define LR_INITIAL_CTRL_LR_SS_MASK (0x3f << 6)
#define LR_INITIAL_CTRL_LR_SS_10 (0x0 << 6) /* 10Mbps */
#define LR_INITIAL_CTRL_LR_SS_20 (0x1 << 6) /* 20Mbps */
#define LR_INITIAL_CTRL_LR_SS_25 (0x2 << 6) /* 25Mbps */
#define LR_INITIAL_CTRL_LR_SS_33 (0x3 << 6) /* 33Mbps */
#define LR_INITIAL_CTRL_LR_SS_50 (0x4 << 6) /* 50Mbps */
#define LR_INITIAL_CTRL_LR_SS_100 (0x5 << 6) /* 100Mbps */

/* Initial advert ability, valid only if LDS is enabled */
#define LR_INITIAL_ADVA_1_10 (1 << 0) /* Advertise 1 pair 10Mbps */
#define LR_INITIAL_ADVA_2_10 (1 << 1) /* Advertise 2 pair 10Mbps */
#define LR_INITIAL_ADVA_1_20 (1 << 2) /* Advertise 1 pair 20Mbps */
#define LR_INITIAL_ADVA_2_20 (1 << 3) /* Advertise 2 pair 20Mbps */
#define LR_INITIAL_ADVA_1_25 (1 << 4) /* Advertise 1 pair 25Mbps */
#define LR_INITIAL_ADVA_2_25 (1 << 5) /* Advertise 2 pair 25Mbps */
#define LR_INITIAL_ADVA_1_33 (1 << 6) /* Advertise 1 pair 33Mbps */
#define LR_INITIAL_ADVA_2_33 (1 << 7) /* Advertise 2 pair 33Mbps */
#define LR_INITIAL_ADVA_1_50 (1 << 8) /* Advertise 1 pair 50Mbps */
#define LR_INITIAL_ADVA_2_50 (1 << 9) /* Advertise 2 pair 50Mbps */
#define LR_INITIAL_ADVA_1_100 (1 << 10) /* Advertise 1 pair 100Mbps */
#define LR_INITIAL_ADVA_2_100 (1 << 11) /* Advertise 2 pair 100Mbps */
#define LR_INITIAL_ADVA_4_100 (1 << 12) /* Advertise 4 pair 100Mbps */
#define LR_INITIAL_ADVA_PAUSE_RX (1 << 13) /* Advertise Pause RX */
#define LR_INITIAL_ADVA_PAUSE_TX (1 << 14) /* Advertise Pause TX */

/* PHY register access */

#define _SHR_PORT_PHY_INTERNAL        0x00000001
#define _SHR_PORT_PHY_NOMAP           0x00000002
#define _SHR_PORT_PHY_CLAUSE45        0x00000004
#define _SHR_PORT_PHY_I2C_DATA8       0x00000008
#define _SHR_PORT_PHY_I2C_DATA16      0x00000010

#define _SHR_PORT_PHY_CLAUSE45_ADDR(_devad, _regad)     \
            ((((_devad) & 0x3F) << 16) |                \
             ((_regad) & 0xFFFF))
#define _SHR_PORT_PHY_CLAUSE45_DEVAD(_reg_addr)         \
            (((_reg_addr) >> 16) & 0x3F)
#define _SHR_PORT_PHY_CLAUSE45_REGAD(_reg_addr)         \
            ((_reg_addr) & 0xFFFF)
#define _SHR_PORT_PHY_I2C_ADDR(_devad, _regad)          \
            ((((_devad) & 0xFF) << 16) |                \
             ((_regad) & 0xFFFF))
#define _SHR_PORT_PHY_I2C_DEVAD(_reg_addr)              \
            (((_reg_addr) >> 16) & 0xFF)
#define _SHR_PORT_PHY_I2C_REGAD(_reg_addr)              \
            ((_reg_addr) & 0xFFFF)

#define _SHR_PORT_PHY_REG_QSGMII            0x10000000
#define _SHR_PORT_PHY_REG_RESERVE_ACCESS    0x20000000
#define _SHR_PORT_PHY_REG_1000X             0x40000000
#define _SHR_PORT_PHY_REG_INDIRECT          0x80000000

#define _SHR_PORT_PHY_REG_FLAGS_MASK  0xFF000000
#define _SHR_PORT_PHY_REG_BANK_MASK   0x0000FFFF
#define _SHR_PORT_PHY_REG_ADDR_MASK   0x000000FF

#define _SHR_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_bank, _reg_addr) \
            ((((_flags) | _SHR_PORT_PHY_REG_INDIRECT) & \
              _SHR_PORT_PHY_REG_FLAGS_MASK) | \
             (((_reg_bank) & _SHR_PORT_PHY_REG_BANK_MASK) << 8) | \
             ((_reg_addr) & _SHR_PORT_PHY_REG_ADDR_MASK))
#define _SHR_PORT_PHY_REG_FLAGS(_reg_addr)                 \
            ((_reg_addr) & _SHR_PORT_PHY_REG_FLAGS_MASK)
#define _SHR_PORT_PHY_REG_BANK(_reg_addr)                 \
            (((_reg_addr) >> 8) & _SHR_PORT_PHY_REG_BANK_MASK)
#define _SHR_PORT_PHY_REG_ADDR(_reg_addr)                 \
            ((_reg_addr) & _SHR_PORT_PHY_REG_ADDR_MASK)

#define SOC_PHY_INTERNAL      _SHR_PORT_PHY_INTERNAL
#define SOC_PHY_NOMAP         _SHR_PORT_PHY_NOMAP
#define SOC_PHY_CLAUSE45      _SHR_PORT_PHY_CLAUSE45
#define SOC_PHY_I2C_DATA8     _SHR_PORT_PHY_I2C_DATA8
#define SOC_PHY_I2C_DATA16    _SHR_PORT_PHY_I2C_DATA16

#define SOC_PHY_CLAUSE45_ADDR(_devad, _regad) \
            _SHR_PORT_PHY_CLAUSE45_ADDR(_devad, _regad)
#define SOC_PHY_CLAUSE45_DEVAD(_reg_addr) \
            _SHR_PORT_PHY_CLAUSE45_DEVAD(_reg_addr)
#define SOC_PHY_CLAUSE45_REGAD(_reg_addr) \
            _SHR_PORT_PHY_CLAUSE45_REGAD(_reg_addr)
#define SOC_PHY_I2C_ADDR(_devad, _regad) \
	_SHR_PORT_PHY_I2C_ADDR(_devad, _regad)
#define SOC_PHY_I2C_DEVAD(_reg_addr) \
	_SHR_PORT_PHY_I2C_DEVAD(_reg_addr)
#define SOC_PHY_I2C_REGAD(_reg_addr) \
	_SHR_PORT_PHY_I2C_REGAD(_reg_addr)

/* Indirect PHY register address flags */
#define SOC_PHY_REG_RESERVE_ACCESS    _SHR_PORT_PHY_REG_RESERVE_ACCESS
#define SOC_PHY_REG_1000X             _SHR_PORT_PHY_REG_1000X
#define SOC_PHY_REG_INDIRECT          _SHR_PORT_PHY_REG_INDIRECT
#define SOC_PHY_REG_QSGMII            _SHR_PORT_PHY_REG_QSGMII

#define SOC_PHY_REG_INDIRECT_ADDR(_flags, _reg_type, _reg_selector) \
            _SHR_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_type, _reg_selector)
#define SOC_PHY_REG_BANK(_reg_addr)  _SHR_PORT_PHY_REG_BANK(_reg_addr)
#define SOC_PHY_REG_ADDR(_reg_addr)  _SHR_PORT_PHY_REG_ADDR(_reg_addr)
#define SOC_PHY_REG_FLAGS(_reg_addr) _SHR_PORT_PHY_REG_FLAGS(_reg_addr)


#define READ_PHY_REG(_unit, _pc,  _addr, _value) \
            (soc_phy_reg_read((_pc)->phy_id, (_addr), (_value)))
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
            (soc_phy_reg_write((_pc)->phy_id, (_addr), (_value)) )
#define MODIFY_PHY_REG(_unit, _pc, _addr, _data, _mask) \
            (soc_phy_reg_modify((_pc)->phy_id, (_addr), (_data), (_mask)))

#define SOC_E_NONE                (0)
#define SOC_E_PARAM              (-2)

#define SOC_SUCCESS(rv)          ((rv) ==  0)
#define SOC_FAILURE(rv)          ((rv) < 0)

#define PHY_BRCM_OUI1   0x001018        /* Broadcom original */
#define PHY_BRCM_OUI1R  0x000818        /* Broadcom reversed */
#define PHY_BRCM_OUI2   0x000af7        /* Broadcom new */
#define PHY_BRCM_OUI3   0x001be9        /* Broadcom new */
#define PHY_BRCM_OUI4   0x00c086        /* Broadcom OUI */
#define PHY_BRCM_OUI5   0x18c086        /* Broadcom OUI */

#define PHY_BCM54680_OUI        PHY_BRCM_OUI3
#define PHY_BCM54680_MODEL      0x1c

/* MSB of rev ID is used to distinguish between BCM54680E and BCM54640E */
#define PHY_BCM54680E_OUI       PHY_BRCM_OUI3
#define PHY_BCM54680E_MODEL     0x27

#define PHY_BCM54280_OUI        PHY_BRCM_OUI5
#define PHY_BCM54280_MODEL      0x04  /*Rev = 1*/

#define PHY_BCM54282_OUI        PHY_BRCM_OUI5
#define PHY_BCM54282_MODEL      0x05  /*Rev = 9*/

#define PHY_BCM54240_OUI        PHY_BRCM_OUI5
#define PHY_BCM54240_MODEL      0x06  /*Rev = 1*/

#define PHY_BCM54285_OUI        PHY_BRCM_OUI5
#define PHY_BCM54285_MODEL      0x05  /*Rev = 1*/

#define PHY_BCM54290_OUI        PHY_BRCM_OUI5
#define PHY_BCM54290_MODEL      0x12

#define PHY_BCM54292_OUI        PHY_BRCM_OUI5
#define PHY_BCM54292_MODEL      0x13

#define PHY_BCM54294_OUI        PHY_BRCM_OUI5
#define PHY_BCM54294_MODEL      0x12

#define PHY_BCM54295_OUI        PHY_BRCM_OUI5
#define PHY_BCM54295_MODEL      0x13

#define PHY_MODEL(id0, id1) ((id1) >> 4 & 0x3f)

#define PHY_REV(id0, id1)   ((id1) & 0xf)

typedef unsigned int _shr_port_mode_t;

/*
 * Defines:
 *      _SHR_PM_*
 * Purpose:
 *      Defines for port modes.
 */

#define _SHR_PM_10MB_HD         (1 << 0)
#define _SHR_PM_10MB_FD         (1 << 1)
#define _SHR_PM_100MB_HD        (1 << 2)
#define _SHR_PM_100MB_FD        (1 << 3)
#define _SHR_PM_1000MB_HD       (1 << 4)
#define _SHR_PM_1000MB_FD       (1 << 5)
#define _SHR_PM_2500MB_HD       (1 << 6)
#define _SHR_PM_2500MB_FD       (1 << 7)
#define _SHR_PM_10GB_HD         (1 << 8)
#define _SHR_PM_10GB_FD         (1 << 9)
#define _SHR_PM_PAUSE_TX        (1 << 10)       /* TX pause capable */
#define _SHR_PM_PAUSE_RX        (1 << 11)       /* RX pause capable */
#define _SHR_PM_PAUSE_ASYMM     (1 << 12)       /* Asymm pause capable (R/O) */
#define _SHR_PM_TBI             (1 << 13)       /* TBI mode supported */
#define _SHR_PM_MII             (1 << 14)       /* MII mode supported */
#define _SHR_PM_GMII            (1 << 15)       /* GMII mode supported */
#define _SHR_PM_SGMII           (1 << 16)       /* SGMII mode supported */
#define _SHR_PM_XGMII           (1 << 17)       /* XGMII mode supported */
#define _SHR_PM_LB_MAC          (1 << 18)       /* MAC loopback supported */
#define _SHR_PM_LB_NONE         (1 << 19)       /* Useful for automated test */
#define _SHR_PM_LB_PHY          (1 << 20)       /* PHY loopback supported */
#define _SHR_PM_AN              (1 << 21)       /* Auto-negotiation */
#define _SHR_PM_3000MB_HD       (1 << 29)
#define _SHR_PM_3000MB_FD       (1 << 30)
#define _SHR_PM_12GB_HD         (1 << 22)
#define _SHR_PM_12GB_FD         (1 << 23)
#define _SHR_PM_13GB_HD         (1 << 24)
#define _SHR_PM_13GB_FD         (1 << 25)
#define _SHR_PM_16GB_HD         (1 << 26)
#define _SHR_PM_16GB_FD         (1 << 27)
#define _SHR_PM_COMBO           (1 << 31)      /* COMBO ports support both
                                                * copper and fiber interfaces */

#define _SHR_PM_PAUSE           (_SHR_PM_PAUSE_TX  | _SHR_PM_PAUSE_RX)
#define _SHR_PM_10MB            (_SHR_PM_10MB_HD   | _SHR_PM_10MB_FD)
#define _SHR_PM_100MB           (_SHR_PM_100MB_HD  | _SHR_PM_100MB_FD)
#define _SHR_PM_1000MB          (_SHR_PM_1000MB_HD | _SHR_PM_1000MB_FD)
#define _SHR_PM_2500MB          (_SHR_PM_2500MB_HD | _SHR_PM_2500MB_FD)
#define _SHR_PM_3000MB          (_SHR_PM_3000MB_HD | _SHR_PM_3000MB_FD)
#define _SHR_PM_10GB            (_SHR_PM_10GB_HD   | _SHR_PM_10GB_FD)
#define _SHR_PM_12GB            (_SHR_PM_12GB_HD   | _SHR_PM_12GB_FD)
#define _SHR_PM_13GB            (_SHR_PM_13GB_HD   | _SHR_PM_13GB_FD)
#define _SHR_PM_16GB            (_SHR_PM_16GB_HD   | _SHR_PM_16GB_FD)

#define _SHR_PM_SPEED_ALL       (_SHR_PM_16GB |         \
                                 _SHR_PM_13GB |         \
                                 _SHR_PM_12GB |         \
                                 _SHR_PM_10GB |         \
                                 _SHR_PM_3000MB |       \
                                 _SHR_PM_2500MB |       \
                                 _SHR_PM_1000MB |       \
                                 _SHR_PM_100MB |        \
                                 _SHR_PM_10MB)

#define _SHR_PM_SPEED_MAX(m)    (((m) & _SHR_PM_16GB)   ? 16000 : \
                                 ((m) & _SHR_PM_13GB)   ? 13000 : \
                                 ((m) & _SHR_PM_12GB)   ? 12000 : \
                                 ((m) & _SHR_PM_10GB)   ? 10000 : \
                                 ((m) & _SHR_PM_3000MB) ? 3000 : \
                                 ((m) & _SHR_PM_2500MB) ? 2500 : \
                                 ((m) & _SHR_PM_1000MB) ? 1000 : \
                                 ((m) & _SHR_PM_100MB)  ? 100 : \
                                 ((m) & _SHR_PM_10MB)   ? 10 : 0)

#define _SHR_PM_SPEED(s)        ((16000 == (s)) ? _SHR_PM_16GB : \
                                 (13000 == (s)) ? _SHR_PM_13GB : \
                                 (12000 == (s)) ? _SHR_PM_12GB : \
                                 (10000 == (s)) ? _SHR_PM_10GB : \
                                 (3000  == (s)) ? _SHR_PM_3000MB : \
                                 (2500  == (s)) ? _SHR_PM_2500MB : \
                                 (1000  == (s)) ? _SHR_PM_1000MB : \
                                 (100   == (s)) ? _SHR_PM_100MB : \
                                 (10    == (s)) ? _SHR_PM_10MB : 0)

#define _SHR_PM_FD              (_SHR_PM_16GB_FD |    \
                                 _SHR_PM_13GB_FD |    \
                                 _SHR_PM_12GB_FD |    \
                                 _SHR_PM_10GB_FD |    \
                                 _SHR_PM_3000MB_FD |    \
                                 _SHR_PM_2500MB_FD |    \
                                 _SHR_PM_1000MB_FD |    \
                                 _SHR_PM_100MB_FD |     \
                                 _SHR_PM_10MB_FD)

#define _SHR_PM_HD              (_SHR_PM_16GB_HD |    \
                                 _SHR_PM_13GB_HD |    \
                                 _SHR_PM_12GB_HD |    \
                                 _SHR_PM_10GB_HD |    \
                                 _SHR_PM_3000MB_HD |    \
                                 _SHR_PM_2500MB_HD |    \
                                 _SHR_PM_1000MB_HD |    \
                                 _SHR_PM_100MB_HD |     \
                                 _SHR_PM_10MB_HD)

#define _SHR_PORT_CONTROL_FABRIC_HEADER_83200                            (1 << 0)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_88230                            (1 << 1)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_88020_83200_88230_INTEROP        (1 << 2)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_CUSTOM                           (1 << 3)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_88640_TM                         (1 << 4)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_88640_RAW                        (1 << 5)

typedef unsigned int    _shr_pa_encap_t;

typedef struct _shr_port_ability_s {
    _shr_port_mode_t speed_half_duplex;
    _shr_port_mode_t speed_full_duplex;
    _shr_port_mode_t pause;
    _shr_port_mode_t interface;
    _shr_port_mode_t medium;
    _shr_port_mode_t loopback;
    _shr_port_mode_t flags;
    _shr_port_mode_t eee;
    _shr_port_mode_t fcmap;
    _shr_pa_encap_t  encap;
} _shr_port_ability_t;

#define _SHR_PA_ABILITY_ALL     (0xffffffff)
#define _SHR_PA_ABILITY_NONE    (0)

/*
 * Defines:
 *      _SHR_PA_ENCAP_*
 * Purpose:
 *      Defines for port encapsulations.
 */

#define _SHR_PA_ENCAP_IEEE              (1 << 0)


/*
 * Defines:
 *      _SHR_PA_SPEED_*
 * Purpose:
 *      Defines for half duplex port speeds.
 */
#define _SHR_PA_SPEED_10MB         (1 << 0)
#define _SHR_PA_SPEED_100MB        (1 << 1)
#define _SHR_PA_SPEED_1000MB       (1 << 2)
#define _SHR_PA_SPEED_2500MB       (1 << 3)
#define _SHR_PA_SPEED_5000MB       (1 << 4)
#define _SHR_PA_SPEED_10GB         (1 << 5)

#define _SHR_PA_SPEED_STRING       {"10MB", \
                                 "100MB", \
                                 "1000MB", \
                                 "2500MB", \
                                 "5000MB", \
                                 "10GB"}
/*
 * Defines:
 *      _SHR_PA_PAUSE_*
 * Purpose:
 *      Defines for flow control abilities.
 */
#define _SHR_PA_PAUSE_TX        (1 << 0)       /* TX pause capable */
#define _SHR_PA_PAUSE_RX        (1 << 1)       /* RX pause capable */
#define _SHR_PA_PAUSE_ASYMM     (1 << 2)       /* Asymm pause capable (R/O) */

#define _SHR_PA_PAUSE_STRING    {"PAUSE_TX", \
                                 "PAUSE_RX", \
                                 "PAUSE_ASYMM"}

/*
 * Defines:
 *      _SHR_PA_INTF_*
 * Purpose:
 *      Defines for port interfaces supported.
 */
#define _SHR_PA_INTF_TBI        (1 << 0)       /* TBI mode supported */
#define _SHR_PA_INTF_MII        (1 << 1)       /* MII mode supported */
#define _SHR_PA_INTF_GMII       (1 << 2)       /* GMII mode supported */
#define _SHR_PA_INTF_RGMII      (1 << 3)       /* RGMII mode supported */
#define _SHR_PA_INTF_SGMII      (1 << 4)       /* SGMII mode supported */
#define _SHR_PA_INTF_XGMII      (1 << 5)       /* XGMII mode supported */
#define _SHR_PA_INTF_QSGMII     (1 << 6)       /* QSGMII mode supported */
#define _SHR_PA_INTF_CGMII      (1 << 7)       /* CGMII mode supported */

#define _SHR_PA_INTF_STRING     {"TBI", \
                                 "MII", \
                                 "GMII", \
                                 "RGMII", \
                                 "SGMII", \
                                 "XGMII", \
                                 "QSGMII", \
                                 "CGMII"}

/*
 * Defines:
 *      _SHR_PA_MEDIUM_*
 * Purpose:
 *      Defines for port medium modes.
 */
#define _SHR_PA_MEDIUM_COPPER   (1 << 0)
#define _SHR_PA_MEDIUM_FIBER    (1 << 1)

#define _SHR_PA_MEDIUM_STRING   {"COPPER", \
                                 "FIBER"}
/*
 * Defines:
 *      _SHR_PA_LOOPBACK_*
 * Purpose:
 *      Defines for port loopback modes.
 */
#define _SHR_PA_LB_NONE         (1 << 0)       /* Useful for automated test */
#define _SHR_PA_LB_MAC          (1 << 1)       /* MAC loopback supported */
#define _SHR_PA_LB_PHY          (1 << 2)       /* PHY loopback supported */
#define _SHR_PA_LB_LINE         (1 << 3)       /* PHY lineside loopback */

#define _SHR_PA_LB_STRING       {"LB_NONE", \
                                 "LB_MAC", \
                                 "LB_PHY", \
                                 "LB_LINE"}

/*
 * Defines:
 *      _SHR_PA_FLAGS_*
 * Purpose:
 *      Defines for the reest of port ability flags.
 */
#define _SHR_PA_AUTONEG         (1 << 0)       /* Auto-negotiation */
#define _SHR_PA_COMBO           (1 << 1)       /* COMBO ports support both
                                                * copper and fiber interfaces */
#define _SHR_PA_FLAGS_STRING    {"AUTONEG", \
                                 "COMBO"}

#define _SHR_PA_PAUSE           (_SHR_PA_PAUSE_TX  | _SHR_PA_PAUSE_RX)

#define _SHR_PA_SPEED_ALL    ( _SHR_PA_SPEED_10GB |        \
                              _SHR_PA_SPEED_5000MB |       \
                              _SHR_PA_SPEED_2500MB |       \
                              _SHR_PA_SPEED_1000MB |       \
                              _SHR_PA_SPEED_100MB |        \
                              _SHR_PA_SPEED_10MB)


#define _SHR_PA_SPEED_MAX(m) (((m) & _SHR_PA_SPEED_10GB)     ? 10000 : \
                              ((m) & _SHR_PA_SPEED_5000MB)   ? 5000 : \
                              ((m) & _SHR_PA_SPEED_2500MB)   ? 2500 : \
                              ((m) & _SHR_PA_SPEED_1000MB)   ? 1000 : \
                              ((m) & _SHR_PA_SPEED_100MB)    ? 100 : \
                              ((m) & _SHR_PA_SPEED_10MB)     ? 10 : 0)


#define _SHR_PA_SPEED(s)     ((10000 == (s)) ? _SHR_PA_SPEED_10GB : \
                              (5000  == (s)) ? _SHR_PA_SPEED_5000MB : \
                              (2500  == (s)) ? _SHR_PA_SPEED_2500MB : \
                              (1000  == (s)) ? _SHR_PA_SPEED_1000MB : \
                              (100   == (s)) ? _SHR_PA_SPEED_100MB : \
                              (10    == (s)) ? _SHR_PA_SPEED_10MB : 0)


/*
 * Defines:
 *      _SHR_PA_EEE_*
 * Purpose:
 *      Defines for EEE
 */
#define _SHR_PA_EEE_100MB_BASETX       (1 << 0)    /* EEE for 100M-BaseTX */
#define _SHR_PA_EEE_1GB_BASET          (1 << 1)    /* EEE for 1G-BaseT */
#define _SHR_PA_EEE_10GB_BASET         (1 << 2)    /* EEE for 10G-BaseT */
#define _SHR_PA_EEE_10GB_KX            (1 << 3)    /* EEE for 10G-KX */
#define _SHR_PA_EEE_10GB_KX4           (1 << 4)    /* EEE for 10G-KX4 */
#define _SHR_PA_EEE_10GB_KR            (1 << 5)    /* EEE for 10G-KR */


/*
 * Defines:
 *	_SHR_PORT_DUPLEX_*
 * Purpose:
 *	Defines duplexity of a port
 */

typedef enum _shr_port_duplex_e {
    _SHR_PORT_DUPLEX_HALF,
    _SHR_PORT_DUPLEX_FULL,
    _SHR_PORT_DUPLEX_COUNT	/* last, please */
} _shr_port_duplex_t;

/*
 * Defines:
 *      _SHR_PORT_MDIX_*
 * Purpose:
 *      Defines the MDI crossover (MDIX) modes for the port
 */
typedef enum _shr_port_mdix_e {
    _SHR_PORT_MDIX_AUTO,
    _SHR_PORT_MDIX_FORCE_AUTO,
    _SHR_PORT_MDIX_NORMAL,
    _SHR_PORT_MDIX_XOVER,
    _SHR_PORT_MDIX_COUNT    /* last, please */
} _shr_port_mdix_t;

/*
 * Defines:
 *      _SHR_PORT_MEDIUM_*
 * Purpose:
 *      Supported physical mediums
 */
typedef enum _shr_port_medium_e {
    _SHR_PORT_MEDIUM_NONE              = 0,
    _SHR_PORT_MEDIUM_COPPER            = 1,
    _SHR_PORT_MEDIUM_FIBER             = 2,
    _SHR_PORT_MEDIUM_COUNT             /* last, please */
} _shr_port_medium_t;

/*
 * Defines:
 *  _SHR_PORT_MS_*
 * Purpose:
 *  Defines master/slave mode of a port (PHY).
 */

typedef enum _shr_port_ms_e {
    _SHR_PORT_MS_SLAVE,
    _SHR_PORT_MS_MASTER,
    _SHR_PORT_MS_AUTO,
    _SHR_PORT_MS_NONE,
    _SHR_PORT_MS_COUNT  /* last, please */
} _shr_port_ms_t;


typedef _shr_port_ms_t soc_port_ms_t;
#define SOC_PORT_MS_SLAVE    _SHR_PORT_MS_SLAVE
#define SOC_PORT_MS_MASTER   _SHR_PORT_MS_MASTER
#define SOC_PORT_MS_AUTO     _SHR_PORT_MS_AUTO
#define SOC_PORT_MS_NONE     _SHR_PORT_MS_NONE
#define SOC_PORT_MS_COUNT    _SHR_PORT_MS_COUNT

typedef _shr_port_duplex_t soc_port_duplex_t;
#define SOC_PORT_DUPLEX_HALF   _SHR_PORT_DUPLEX_HALF
#define SOC_PORT_DUPLEX_FULL   _SHR_PORT_DUPLEX_FULL
#define SOC_PORT_DUPLEX_COUNT  _SHR_PORT_DUPLEX_COUNT

typedef _shr_port_mdix_t soc_port_mdix_t;
#define SOC_PORT_MDIX_AUTO          _SHR_PORT_MDIX_AUTO
#define SOC_PORT_MDIX_FORCE_AUTO    _SHR_PORT_MDIX_FORCE_AUTO
#define SOC_PORT_MDIX_NORMAL        _SHR_PORT_MDIX_NORMAL
#define SOC_PORT_MDIX_XOVER         _SHR_PORT_MDIX_XOVER
#define SOC_PORT_MDIX_COUNT         _SHR_PORT_MDIX_COUNT

typedef _shr_port_medium_t soc_port_medium_t;
#define SOC_PORT_MEDIUM_NONE        _SHR_PORT_MEDIUM_NONE
#define SOC_PORT_MEDIUM_COPPER      _SHR_PORT_MEDIUM_COPPER
#define SOC_PORT_MEDIUM_FIBER       _SHR_PORT_MEDIUM_FIBER
#define SOC_PORT_MEDIUM_COUNT       _SHR_PORT_MEDIUM_COUNT

/*
 * Defines:
 *      SOC_PM_*
 * Purpose:
 *      Defines for port modes.
 */
/* Port Speeds */
#define SOC_PM_10MB_HD          _SHR_PM_10MB_HD
#define SOC_PM_10MB_FD          _SHR_PM_10MB_FD
#define SOC_PM_100MB_HD         _SHR_PM_100MB_HD
#define SOC_PM_100MB_FD         _SHR_PM_100MB_FD
#define SOC_PM_1000MB_HD        _SHR_PM_1000MB_HD
#define SOC_PM_1000MB_FD        _SHR_PM_1000MB_FD
#define SOC_PM_2500MB_HD        _SHR_PM_2500MB_HD
#define SOC_PM_2500MB_FD        _SHR_PM_2500MB_FD
#define SOC_PM_10GB_HD          _SHR_PM_10GB_HD
#define SOC_PM_10GB_FD          _SHR_PM_10GB_FD
#define SOC_PM_SPEED_ALL        _SHR_PM_SPEED_ALL

/* Pause Modes */
#define SOC_PM_PAUSE_TX         _SHR_PM_PAUSE_TX
#define SOC_PM_PAUSE_RX         _SHR_PM_PAUSE_RX
#define SOC_PM_PAUSE_ASYMM      _SHR_PM_PAUSE_ASYMM

/* Interface Types */
#define SOC_PM_TBI              _SHR_PM_TBI
#define SOC_PM_MII              _SHR_PM_MII
#define SOC_PM_GMII             _SHR_PM_GMII
#define SOC_PM_SGMII            _SHR_PM_SGMII
#define SOC_PM_XGMII            _SHR_PM_XGMII

/* Loopback Mode */
#define SOC_PM_LB_MAC           _SHR_PM_LB_MAC
#define SOC_PM_LB_PHY           _SHR_PM_LB_PHY
#define SOC_PM_LB_NONE          _SHR_PM_LB_NONE

/* Autonegotiation */
#define SOC_PM_AN               _SHR_PM_AN

/* Combo port */
#define SOC_PM_COMBO            _SHR_PM_COMBO

#define SOC_PM_PAUSE            _SHR_PM_PAUSE

#define SOC_PM_10MB             _SHR_PM_10MB
#define SOC_PM_100MB            _SHR_PM_100MB
#define SOC_PM_1000MB           _SHR_PM_1000MB
#define SOC_PM_2500MB           _SHR_PM_2500MB
#define SOC_PM_3000MB           _SHR_PM_3000MB
#define SOC_PM_10GB             _SHR_PM_10GB

#define SOC_PM_SPEED_MAX(m)     _SHR_PM_SPEED_MAX(m)
#define SOC_PM_SPEED(s)         _SHR_PM_SPEED(s)

#define SOC_PM_FD               _SHR_PM_FD
#define SOC_PM_HD               _SHR_PM_HD

/*
 * Structure:
 *	_shr_phy_config_t
 * Purpose:
 *	Defines the operating mode of a PHY.
 */

typedef struct _shr_phy_config_s {
    int			enable;
    int			preferred;
    int			autoneg_enable;
    _shr_port_mode_t	autoneg_advert;
    _shr_port_ability_t advert_ability;
    int			force_speed;
    int			force_duplex;
    int			master;
    _shr_port_mdix_t    mdix;
} _shr_phy_config_t;

/*
 * Defines:
 *      SOC_PM_ABILITY_[MAC/PHY]
 * Purpose:
 *      Defines a mask of modes that are exclusive to the MAC or
 *      PHY drivers. To determine the ability of a port, the
 *      ability masks for the MAC and PHY are ANDed together, and then
 *      ORed with the MAC/PHY ability bits selected by these masks.
 */

#define SOC_PM_ABILITY_PHY      (SOC_PM_LB_PHY|SOC_PM_AN|SOC_PM_COMBO)
#define SOC_PM_ABILITY_MAC      (SOC_PM_LB_MAC)

typedef _shr_port_mode_t soc_port_mode_t;

typedef _shr_phy_config_t soc_phy_config_t;

typedef _shr_port_ability_t soc_port_ability_t;

#define SOC_PA_ABILITY_ALL     _SHR_PA_ABILITY_ALL
#define SOC_PA_ABILITY_NONE    _SHR_PA_ABILITY_NONE


typedef _shr_pa_encap_t soc_pa_encap_t;

#define SOC_PA_ENCAP_IEEE             _SHR_PA_ENCAP_IEEE

/*
 * Defines:
 *      SOC_PA_SPEED_*
 * Purpose:
 *      Defines for port speeds.
 */
#define SOC_PA_SPEED_10MB         _SHR_PA_SPEED_10MB
#define SOC_PA_SPEED_100MB        _SHR_PA_SPEED_100MB
#define SOC_PA_SPEED_1000MB       _SHR_PA_SPEED_1000MB
#define SOC_PA_SPEED_2500MB       _SHR_PA_SPEED_2500MB
#define SOC_PA_SPEED_10GB         _SHR_PA_SPEED_10GB

#define SOC_PA_SPEED_STRING       _SHR_PA_SPEED_STRING

/*
 * Defines:
 *      SOC_PA_PAUSE_*
 * Purpose:
 *      Defines for flow control abilities.
 */
#define SOC_PA_PAUSE_TX        _SHR_PA_PAUSE_TX
#define SOC_PA_PAUSE_RX        _SHR_PA_PAUSE_RX
#define SOC_PA_PAUSE_ASYMM     _SHR_PA_PAUSE_ASYMM

#define SOC_PA_PAUSE_STRING    _SHR_PA_PAUSE_STRING

/*
 * Defines:
 *      SOC_PA_INTF_*
 * Purpose:
 *      Defines for port interfaces supported.
 */
#define SOC_PA_INTF_TBI        _SHR_PA_INTF_TBI
#define SOC_PA_INTF_MII        _SHR_PA_INTF_MII
#define SOC_PA_INTF_GMII       _SHR_PA_INTF_GMII
#define SOC_PA_INTF_RGMII      _SHR_PA_INTF_RGMII
#define SOC_PA_INTF_SGMII      _SHR_PA_INTF_SGMII
#define SOC_PA_INTF_XGMII      _SHR_PA_INTF_XGMII
#define SOC_PA_INTF_QSGMII     _SHR_PA_INTF_QSGMII
#define SOC_PA_INTF_CGMII      _SHR_PA_INTF_CGMII

#define SOC_PA_INTF_STRING     _SHR_PA_INTF_STRING

/*
 * Defines:
 *      SOC_PA_MEDIUM_*
 * Purpose:
 *      Defines for port medium modes.
 */
#define SOC_PA_MEDIUM_COPPER   _SHR_PA_MEDIUM_COPPER
#define SOC_PA_MEDIUM_FIBER    _SHR_PA_MEDIUM_FIBER

#define SOC_PA_MEDIUM_STRING   _SHR_PA_MEDIUM_STRING

/*
 * Defines:
 *      SOC_PA_LOOPBACK_*
 * Purpose:
 *      Defines for port loopback modes.
 */
#define SOC_PA_LB_NONE         _SHR_PA_LB_NONE
#define SOC_PA_LB_MAC          _SHR_PA_LB_MAC
#define SOC_PA_LB_PHY          _SHR_PA_LB_PHY
#define SOC_PA_LB_LINE         _SHR_PA_LB_LINE

#define SOC_PA_LB_STRING       _SHR_PA_LB_STRING

/*
 * Defines:
 *      SOC_PA_FLAGS_*
 * Purpose:
 *      Defines for the reest of port ability flags.
 */
#define SOC_PA_AUTONEG         _SHR_PA_AUTONEG
#define SOC_PA_COMBO           _SHR_PA_COMBO


#define SOC_PA_FLAGS_STRING    _SHR_PA_FLAGS_STRING

/* Ability filters  */

#define SOC_PA_PAUSE           _SHR_PA_PAUSE

#define SOC_PA_SPEED_ALL    _SHR_PA_SPEED_ALL

#define SOC_PA_SPEED_MAX(m) _SHR_PA_SPEED_MAX(m)

#define SOC_PA_SPEED(s)     _SHR_PA_SPEED(s)


#define SOC_PORT_ABILITY_SFI      _SHR_PORT_ABILITY_SFI
#define SOC_PORT_ABILITY_DUAL_SFI _SHR_PORT_ABILITY_DUAL_SFI
#define SOC_PORT_ABILITY_SCI      _SHR_PORT_ABILITY_SCI
#define SOC_PORT_ABILITY_SFI_SCI  _SHR_PORT_ABILITY_SFI_SCI


/* EEE Abilities */
#define SOC_PA_EEE_100MB_BASETX         _SHR_PA_EEE_100MB_BASETX
#define SOC_PA_EEE_1GB_BASET            _SHR_PA_EEE_1GB_BASET
#define SOC_PA_EEE_10GB_BASET           _SHR_PA_EEE_10GB_BASET
#define SOC_PA_EEE_10GB_KX              _SHR_PA_EEE_10GB_KX
#define SOC_PA_EEE_10GB_KX4             _SHR_PA_EEE_10GB_KX4
#define SOC_PA_EEE_10GB_KR              _SHR_PA_EEE_10GB_KR

/*
 * Defines:
 *     _SHR_PORT_PHY_CONTROL_POWER_*
 * Purpose:
 *     PHY specific values for _SHR_PORT_PHY_CONTROL_POWER type
 */
typedef enum _shr_port_phy_control_power_e {
    _SHR_PORT_PHY_CONTROL_POWER_FULL,
    _SHR_PORT_PHY_CONTROL_POWER_LOW,
    _SHR_PORT_PHY_CONTROL_POWER_AUTO,
    /*
       Power mode added(SDK-30895). Requirement is to disable Auto Power Down
       without changing Tx bias current
    */
    _SHR_PORT_PHY_CONTROL_POWER_AUTO_DISABLE
} _shr_port_phy_control_power_t;

typedef enum soc_phy_control_power_e {
    SOC_PHY_CONTROL_POWER_FULL = _SHR_PORT_PHY_CONTROL_POWER_FULL,
    SOC_PHY_CONTROL_POWER_LOW  = _SHR_PORT_PHY_CONTROL_POWER_LOW,
    SOC_PHY_CONTROL_POWER_AUTO = _SHR_PORT_PHY_CONTROL_POWER_AUTO,
    /*
       Power mode added(SDK-30895). Requirement is to disable Auto Power Down
       without changing Tx bias current
    */
    SOC_PHY_CONTROL_POWER_AUTO_DISABLE= _SHR_PORT_PHY_CONTROL_POWER_AUTO_DISABLE

} soc_phy_control_power_t;


/*
 * Defines:
 *  _SHR_PORT_CABLE_STATE_*
 * Purpose:
 *  Cable diag state (per pair and overall)
 */
typedef enum {
    _SHR_PORT_CABLE_STATE_OK,
    _SHR_PORT_CABLE_STATE_OPEN,
    _SHR_PORT_CABLE_STATE_SHORT,
    _SHR_PORT_CABLE_STATE_OPENSHORT,
    _SHR_PORT_CABLE_STATE_CROSSTALK,
    _SHR_PORT_CABLE_STATE_UNKNOWN,
    _SHR_PORT_CABLE_STATE_COUNT /* last, as always */
} _shr_port_cable_state_t;

/*
 * Defines:
 *  _shr_port_cable_diag_t
 * Purpose:
 *  Shared definition of port (phy) cable diag status structure.
 *  Used both in soc/phy layer and in bcm layer.
 */
typedef struct _shr_port_cable_diag_s {
    _shr_port_cable_state_t state;  /* state of all pairs */
    int             npairs;    /* pair_* elements valid */
    _shr_port_cable_state_t pair_state[4];  /* pair state */
    int             pair_len[4];    /* pair length in metres */
    int             fuzz_len;   /* len values +/- this */
} _shr_port_cable_diag_t;

typedef _shr_port_cable_state_t soc_port_cable_state_t;
#define SOC_PORT_CABLE_STATE_OK        _SHR_PORT_CABLE_STATE_OK
#define SOC_PORT_CABLE_STATE_OPEN      _SHR_PORT_CABLE_STATE_OPEN
#define SOC_PORT_CABLE_STATE_SHORT     _SHR_PORT_CABLE_STATE_SHORT
#define SOC_PORT_CABLE_STATE_OPENSHORT _SHR_PORT_CABLE_STATE_OPENSHORT
#define SOC_PORT_CABLE_STATE_CROSSTALK _SHR_PORT_CABLE_STATE_CROSSTALK
#define SOC_PORT_CABLE_STATE_UNKNOWN   _SHR_PORT_CABLE_STATE_UNKNOWN
#define SOC_PORT_CABLE_STATE_COUNT     _SHR_PORT_CABLE_STATE_COUNT

typedef _shr_port_cable_diag_t soc_port_cable_diag_t;

/* 1000X Control #1 Register: Controls 10B/SGMII mode */
#define LP_1000X_FIBER_MODE    (1 << 0)     /* Enable SGMII fiber mode */
#define LP_1000X_EN10B_MODE    (1 << 1)     /* Enable TBI 10B interface */
#define LP_1000X_INVERT_SD     (1 << 3)     /* Invert Signal Detect */
#define LP_1000X_AUTO_DETECT   (1 << 4)     /* Auto-detect SGMII and 1000X */
#define LP_1000X_SGMII_MASTER  (1 << 5)
#define LP_1000X_PLL_PWRDWN    (1 << 6)
#define LP_1000X_RX_PKTS_CNTR_SEL (1 << 11) /* Select receive counter for 17h*/
#define LP_1000X_TX_AMPLITUDE_OVRD    (1 << 12)

/* 1000X Control #2 Register Fields */
#define LP_1000X_PAR_DET_EN        (1 << 0)    /* Enable Parallel Detect */
#define LP_1000X_FALSE_LNK_DIS     (1 << 1)    /* Disable false link */
#define LP_1000X_FILTER_FORCE_EN   (1 << 2)    /* Enable filter force link */
/* Allow packet to be transmitted regardless of the condition of the link */
#define LP_1000X_FORCE_XMIT_DATA_ON_TXSIDE (1 << 5)
#define LP_1000X_CLRAR_BER_CNTR    (1 << 14)   /* Clear bit-err-rate counter */

/* 1000X Control #3 Register Fields */
#define LP_1000X_TX_FIFO_RST           (1 << 0)    /* Reset TX FIFO */
#define LP_1000X_FIFO_ELASTICITY_MASK  (0x3 << 1)  /* Fifo Elasticity */
#define LP_1000X_FIFO_ELASTICITY_5K    (0x0 << 1)  /* 5 Kbytes */
#define LP_1000X_FIFO_ELASTICITY_10K   (0x1 << 1)  /* 10 Kbytes */
#define LP_1000X_FIFO_ELASTICITY_13_5K (0x2 << 1)  /* 13.5 Kbytes */
#define LP_1000X_FREQ_LOCK_ELASTICITY_RX (1 << 6)
#define LP_1000X_FREQ_LOCK_ELASTICITY_TX (1 << 5)
#define LP_1000X_RX_FIFO_RST           (1 << 14)   /* Reset RX FIFO */

/* 1000X Control #4 Register Fields */
#define LP_1000X_DIG_RESET             (1 << 6)    /* Reset Datapath */

/* 1000X Status #1 Register Fields */
#define LP_1000X_STATUS1_SGMII_MODE    (1 << 0)
#define LP_1000X_STATUS1_SPEED         (3 << 3)
#define LP_1000X_STATUS1_SPEED_10      (0)
#define LP_1000X_STATUS1_SPEED_100     (1 << 3)
#define LP_1000X_STATUS1_SPEED_1000    (1 << 4)

#define PHY_FLAGS(unit, port)        (gpc[port].flags)

#define PHY_FLAGS_COPPER           (1 << 0)  /* copper medium */
#define PHY_FLAGS_FIBER            (1 << 1)  /* fiber medium */
#define PHY_FLAGS_PASSTHRU         (1 << 2)  /* serdes passthru (5690) */
#define PHY_FLAGS_10B              (1 << 3)  /* ten bit interface (TBI) */
#define PHY_FLAGS_5421S            (1 << 4)  /* True if PHY is a 5421S */
#define PHY_FLAGS_DISABLE          (1 << 5)  /* True if PHY is disabled */
#define PHY_FLAGS_C45              (1 << 6)  /* True if PHY uses clause 45
                                              * MIIM */
#define PHY_FLAGS_100FX            (1 << 7)  /* True if PHY at 100FX
                                              * (for 5482S) */
#define PHY_FLAGS_MEDIUM_CHANGE    (1 << 8)  /* True if PHY medium changed
                                              * between copper and fiber */
#define PHY_FLAGS_SERDES_FIBER     (1 << 9)  /* True if use internal serdes
                                              * phy */
#define PHY_FLAGS_WAN              (1 << 10) /* WAN mode */
#define PHY_FLAGS_SGMII_AUTONEG    (1 << 11) /* Use SGMII autoneg between
                                              * internal SerDes and external
                                              * PHY */
#define PHY_FLAGS_EXTERNAL_PHY     (1 << 12) /* Port has external PHY */
#define PHY_FLAGS_FORCED_SGMII     (1 << 13) /* Interface between internal and
                                              * external PHY is always SGMII */
#define PHY_FLAGS_FORCED_COPPER    (1 << 14) /* Forced media to copper */
#define PHY_FLAGS_C73              (1 << 15) /* Auto-negotiation for Backplane
                                              * Ethernet (clause 73) */
#define PHY_FLAGS_INDEPENDENT_LANE (1 << 16) /* Treat each XGXS lane as
                                              * independent lane. */
#define PHY_FLAGS_SINGLE_LANE      (1 << 17) /* Use only lane 0 of XGXS core. */

#define PHY_FLAGS_PRIMARY_SERDES    (1<<18)

#define PHY_FLAGS_SECONDARY_SERDES (1<<19)

#define PHY_FLAGS_INIT_DONE        (1<<20)

#define PHY_FLAGS_HC65_FABRIC      (1<<21)   /* True if PHY used for SBX */
                                             /* fabric links */

#define PHY_FLAGS_EEE_CAPABLE      (1<<22)

#define PHY_FLAGS_EEE_ENABLED      (1<<23)

#define PHY_FLAGS_EEE_MODE         (1<<24)

#define PHY_FLAGS_BR               (1<<25)

#define PHY_FLAGS_HS_CAPABLE       (1<<26)

#define PHY_FLAGS_CHAINED          (1<<27)

#define PHY_FLAGS_REPEATER		   (1<<28)		/* PHY is a repeater */

#define PHY_FLAGS_SUPPORT_DUAL_RATE (1<<29)

#define PHY_FLAGS_SERVICE_INT_PHY_LINK_GET  (1<<30)

#define PHY_FLAGS_SET(unit, port, flags) \
            (PHY_FLAGS(unit, port) |= (flags))
#define PHY_FLAGS_CLR(unit, port, flags) \
            (PHY_FLAGS(unit, port) &= ~(flags))
#define PHY_FLAGS_TST(unit, port, flags) \
            ((PHY_FLAGS(unit, port) & (flags)) == (flags))
#define PHY_FLAGS_CLR_ALL(unit, port) \
            (PHY_FLAGS(unit, port) = 0)
#define PHY_COPPER_MODE(unit, port)        PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_COPPER)
#define PHY_FIBER_MODE(unit, port)         PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_FIBER)
#define PHY_PASSTHRU_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_PASSTHRU)
#define PHY_TBI_MODE(unit, port)           PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_10B)
#define PHY_5421S_MODE(unit, port)         PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_5421S)
#define PHY_DISABLED_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_DISABLE)
#define PHY_CLAUSE45_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_C45)
#define PHY_FIBER_100FX_MODE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_100FX)
#define PHY_MEDIUM_CHANGED(unit, port)     PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_MEDIUM_CHANGE)
#define PHY_SERDES_FIBER_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_SERDES_FIBER)
#define PHY_WAN_MODE(unit, port)           PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_WAN)
#define PHY_SGMII_AUTONEG_MODE(unit, port) PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SGMII_AUTONEG)
#define PHY_EXTERNAL_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_EXTERNAL_PHY)
#define PHY_FORCED_SGMII_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_FORCED_SGMII)
#define PHY_FORCED_COPPER_MODE(unit, port) PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_FORCED_COPPER)
#define PHY_CLAUSE73_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_C73)
#define PHY_INDEPENDENT_LANE_MODE(unit, port) PHY_FLAGS_TST(unit, port, \
                                                     PHY_FLAGS_INDEPENDENT_LANE)
#define PHY_SINGLE_LANE_MODE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SINGLE_LANE)
#define PHY_PRIMARY_SERDES_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_PRIMARY_SERDES)
#define PHY_SECONDARY_SERDES_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SECONDARY_SERDES)
#define PHY_EEE_CAPABLE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_EEE_CAPABLE)
#define PHY_BR_MODE(unit, port)       (PHY_FLAGS_TST(unit, port, PHY_FLAGS_COPPER) && \
                                        PHY_FLAGS_TST(unit, port, PHY_FLAGS_BR))
#define PHY_HS_CAPABLE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_HS_CAPABLE)
#define PHY_IS_CHAINED(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_CHAINED)
#define PHY_REPEATER(unit, port)   PHY_FLAGS_TST(unit, port, \
													   PHY_FLAGS_REPEATER)
#define PHY_IS_SUPPORT_DUAL_RATE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SUPPORT_DUAL_RATE)

/* device specific PHYCTRL flags */
#define PHYCTRL_MDIO_ADDR_SHARE (1 << 0) /*use base address of phy to access lanes */
#define PHYCTRL_MDIO_CL45       (1 << 1) /*use MDIO CL45 to access device */
#define PHYCTRL_INIT_DONE       (1 << 2) /*lane initialization is done */
#define PHYCTRL_MDIO_BCST       (1 << 3) /* capable of MDIO ucode broadcast */
#define PHYCTRL_UCODE_BCST_DONE (1 << 4) /* ucode broadcast done */
#define PHYCTRL_ANEG_INIT_DONE  (1 << 5) /* autoneg initialization is done */
#define PHYCTRL_SYS_SIDE_CTRL    (1 << 6) /* Select system side for control set/get operations */
#define PHYCTRL_CHAINED_PHY_CTRL (1 << 7) /* Select the chained phy for control set/get operations */
#define PHYCTRL_IS_PORT0         (1 << 8) /* Indicates is 1st port of chip */


/* bit 31:29 used for init state */
#define PHYCTRL_INIT_STATE_DEFAULT 0
#define PHYCTRL_INIT_STATE_PASS1   1
#define PHYCTRL_INIT_STATE_PASS2   2
#define PHYCTRL_INIT_STATE_PASS3   3
#define PHYCTRL_INIT_MASK          0x7
#define PHYCTRL_INIT_SHFT          29
#define PHYCTRL_INIT_STATE(_pc)     ((((_pc)->flags) >> PHYCTRL_INIT_SHFT) & PHYCTRL_INIT_MASK)
#define PHYCTRL_INIT_STATE_SET(_pc,_st) ((_pc)->flags = ((_pc)->flags & \
    (~(PHYCTRL_INIT_MASK << PHYCTRL_INIT_SHFT))) | \
    (((_st) & PHYCTRL_INIT_MASK) << PHYCTRL_INIT_SHFT))

/* control commands used by firmware_set for ucode broadcast. Internal use only */
/* Command should be executed once. Assume all devices in bcst mode */
#define PHYCTRL_UCODE_BCST_ONEDEV  0x10000

/* Command is executed on each device. Assume all devices in non-bcst mode */
#define PHYCTRL_UCODE_BCST_ALLDEV  0x20000

/* setup the broadcast mode for firmware download */
#define PHYCTRL_UCODE_BCST_SETUP   (0 | PHYCTRL_UCODE_BCST_ALLDEV)

/* Prepare micro-controller ready for firmware broadcast */
#define PHYCTRL_UCODE_BCST_uC_SETUP  (1 | PHYCTRL_UCODE_BCST_ONEDEV)

/* Re-Enable MDIO broadcast mode */
#define PHYCTRL_UCODE_BCST_ENABLE  (2 | PHYCTRL_UCODE_BCST_ALLDEV)

/* Load firmware */
#define PHYCTRL_UCODE_BCST_LOAD    (3 | PHYCTRL_UCODE_BCST_ONEDEV)

/* Verify firmware download */
#define PHYCTRL_UCODE_BCST_END     (4 | PHYCTRL_UCODE_BCST_ALLDEV)

/* device specific phy_mode values */
#define PHYCTRL_QSGMII_CORE_PORT  8
#define PHYCTRL_MULTI_CORE_PORT   7
#define PHYCTRL_LANE_MODE_CUSTOM_3p125MHZ  6
#define PHYCTRL_LANE_MODE_CUSTOM1 5
#define PHYCTRL_LANE_MODE_CUSTOM  4
#define PHYCTRL_ONE_LANE_PORT     3
#define PHYCTRL_DUAL_LANE_PORT    2
#define PHYCTRL_QUAD_LANE_PORT    1

#define SOC_MAX_NUM_PHYS_PER_CHIP 8

#define PHY_OFFSET_VALID(x)    (((x) >= 0) && ((x) < SOC_MAX_NUM_PHYS_PER_CHIP))

#define SOC_PHYCTRL_PHYN_MAX_LIMIT  (_SHR_PHY_GPORT_PHYN_PORT_PHYN_MASK)

/*
 * phy_addr encoding
 * bit7, 1: internal MDIO bus, 0: external MDIO bus
 * bit9,8,6,5, mdio bus number
 * bit4-0,   mdio addresses
 */
#define PHY_ID_BUS_UPPER_MASK     0x300
#define PHY_ID_BUS_UPPER_SHIFT    0x6
#define PHY_ID_BUS_LOWER_MASK     0x60
#define PHY_ID_BUS_LOWER_SHIFT    5
#define PHY_ID_BUS_NUM(_id)   ((((_id) & PHY_ID_BUS_UPPER_MASK) >> \
        PHY_ID_BUS_UPPER_SHIFT) | (((_id) & PHY_ID_BUS_LOWER_MASK) >> \
        PHY_ID_BUS_LOWER_SHIFT))

extern phy_ctrl_t gpc[32];

extern int soc_phy_init(uint8 unit, uint8 port);

extern int phy_ecd_cable_diag_init(uint8 unit, uint8 port);
extern int phy_ecd_cable_diag_init_40nm(uint8 unit, uint8 port);

extern int
phy_ecd_cable_diag_40nm(uint8 unit, uint8 port,
            soc_port_cable_diag_t *status, uint16 ecd_ctrl);
extern int
phy_ecd_cable_diag(uint8 unit, uint8 port, soc_port_cable_diag_t *status);

extern int
phy_reg_ge_read(uint8 unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 reg_addr, uint16 *data);
extern int
phy_reg_ge_write(uint8 unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 reg_addr, uint16 data);
extern int
phy_reg_ge_modify(uint8 unit, phy_ctrl_t *pct, uint32 flags, uint16 reg_bank,
                uint8 reg_addr, uint16 data, uint16 mask);

extern int
soc_phy_reg_read(uint16 phy_id, uint32 reg_addr, uint16 *p_value);

extern int
soc_phy_reg_write(uint16 phy_id, uint32 reg_addr, uint16 value);

extern int
soc_phy_reg_modify(uint16 phy_id, uint32 reg_addr,
                   uint16 reg_data, uint16 reg_mask);

extern sys_error_t phy_link_get(uint8 unit, uint8 port, uint8 *link);
extern sys_error_t
phy_speed_duplex_get(uint8 unit, uint8 port, uint16 *speed, BOOL *duplex);
extern int
phy_pause_get(uint8 unit, uint8 port, BOOL *tx_pause, BOOL *rx_pause);

extern int soc_phy_cable_diag(uint8 unit, uint8 port, int *length);

extern int
tsce_reg_wr(phy_ctrl_t *pc, uint32 addr, uint16 data);
extern int
tsce_reg_rd(phy_ctrl_t *pc, uint32 addr, uint16 *data);
extern int
tsce_reg_modify(phy_ctrl_t *pc, uint32 addr, uint16 data, uint16 mask);

#endif  /* !_SOC_PHY_H */

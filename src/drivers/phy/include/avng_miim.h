/*
 * $Id: avng_miim.h,v 1.0 2016/03/10 03:06:57 mohanm  $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */
#ifndef __AVNG_MIIM_H__
#define __AVNG_MIIM_H__

#include <sal_types.h>
#include <cdk/cdk_types.h>
/*
 * Definitions for 'phy_addr' parameter.
 *
 */
#define CDK_XGSM_MIIM_INTERNAL   0x00000200 /* Select internal SerDes MII bus */
#define CDK_XGSM_MIIM_CLAUSE45   0x00000020 /* Select clause 45 access method */
#define CDK_XGSM_MIIM_PHY_ID     0x0000001f /* PHY address in MII bus */
#define CDK_XGSM_MIIM_IBUS(_b)   (((_b) << 6) | 0x200)
#define CDK_XGSM_MIIM_EBUS(_b)   ((_b) << 6)
#define CDK_XGSM_MIIM_IBUS_NUM(_a)  ((_a & ~0x200) >> 6)

/*
 * Definitions for 'reg' parameter.
 *
 * Note 1:
 * For clause 22 access, the register address is 5 bits.
 *
 * Note 2:
 * For clause 45 access, the register address is 16 bits and
 * the device address is 5 bits.
 *
 * Note 3:
 * For internal SerDes registers, bits [23:8] are used for selecting
 * the block number for registers 0x10-0x1f. The block select value
 * will be written unmodified to register 0x1f.
 */
#define CDK_XGSM_MIIM_REG_ADDR   0x0000001f /* Clause 22 register address */
#define CDK_XGSM_MIIM_R45_ADDR   0x0000ffff /* Clause 45 register address */
#define CDK_XGSM_MIIM_DEV_ADDR   0x001f0000 /* Clause 45 device address */
#define CDK_XGSM_MIIM_BLK_ADDR   0x00ffff00 /* SerDes block mapping (reg 0x1f) */

/* Transform register address from software API to datasheet format */
#define CDK_XGSM_IBLK_TO_C45(_a) \
    (((_a) & 0xf) | (((_a) >> 8) & 0x7ff0) | (((_a) << 11) & 0x8000));

/* Transform register address from datasheet to software API format */
#define CDK_XGSM_C45_TO_IBLK(_a) \
    ((((_a) & 0x7ff0) << 8) | (((_a) & 0x8000) >> 11) | ((_a) & 0xf))


/* CMIC_CMCx_MIIM_STAT(x) */
#define CMIC_MIIM_OPN_DONE              (0x00000001)


/*  CMIC_CMCx_MIIM_CTRL(x) */
#define CMIC_MIIM_WR_START              (0x00000001)
#define CMIC_MIIM_RD_START              (0x00000002)

/* MII write access. */
extern int
cdk_avng_miim_write(int unit, uint32_t phy_addr, uint32_t reg, uint8 ext, uint32_t val);

/* PMI write access. */
extern int
cdk_avng_pmi_write(int unit, uint32_t phy_addr, uint32_t reg, uint32_t val);

/* MII read access. */
extern int
cdk_avng_miim_read(int unit, uint32_t phy_addr, uint32_t reg, uint8 ext, uint32_t *val);

/* PMI read access. */
extern int
cdk_avng_pmi_read(int unit, uint32_t phy_addr, uint32_t reg, uint32_t *val);

extern int
cdk_avng_miim_ext_mdio_write(int unit, uint32_t phy_addr, uint32_t reg, uint32_t val);

extern int
cdk_avng_miim_ext_mdio_read(int unit, uint32_t phy_addr, uint32_t reg, uint32_t *val);
#endif /* __AVNG_MIIM_H__ */


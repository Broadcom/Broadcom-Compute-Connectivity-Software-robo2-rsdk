/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *    chip.h
 * Description:
 *    Contains the defintion for various chip features 
 */

#ifndef __CHIP_H
#define __CHIP_H

#include <soc/types.h>
#include <soc/memory.h>
#include <soc/register.h>
#include <soc/robo2/common/robo2_types.h>


/*
 * Arrays of soc_robo2_block_info_t [soc_block_info_t] are built by the registers program.
 * Each array is indexed by block number (per chip).
 * The last entry has type -1.
 * Used in bcm*.i files.
 */
typedef struct {
    int    type;    /* SOC_BLK_* */
    int    number;  /* instance of type */
    int    base;    /* Base address of the block */
} soc_robo2_block_info_t;

/*
 * Arrays of soc_port_info_t are built by the registers program.
 * Each array is indexed by port number (per chip).
 * Unused ports have blk -1 and bindex 0
 * The last entry has blk -1 and bindex -1
 * Used in bcm*.i files.
 */
typedef struct {
    int     blk;    /* index into soc_block_info array */
    int     bindex; /* index of port within block */
} soc_port_info_t;

/*
 * Block types
 * Derived from SDK, Merge this to SDK if required in future
 */
typedef enum soc_robo2_block_type_e {
    SOC_BLOCK_TYPE_INVALID = -1,
    SOC_BLK_NONE = 0,
    SOC_BLK_CB,
    SOC_BLK_CMIC,
    SOC_BLK_CRU_AVS,
    SOC_BLK_CRU_CRU,
    SOC_BLK_CRU_EEPROM,
    SOC_BLK_CRU_OTP,
    SOC_BLK_CRU_ROM,
    SOC_BLK_CRU_TS_CORE,
    SOC_BLK_CRU_TS_TOP,
    SOC_BLK_DTCM,
    SOC_BLK_GPHY_DIG,
    SOC_BLK_GPIO,
    SOC_BLK_HPA,
    SOC_BLK_IDM_CB,
    SOC_BLK_IDM_DS0,
    SOC_BLK_IDM_SPIM,
    SOC_BLK_ITCM,
    SOC_BLK_LED,
    SOC_BLK_M7SC,
    SOC_BLK_MDIO,
    SOC_BLK_NPA,
    SOC_BLK_PACKET_RAM,
    SOC_BLK_PORT_MACRO,
    SOC_BLK_PWM,
    SOC_BLK_QSGMII_COMBO,
    SOC_BLK_QSPI,
    SOC_BLK_QSPI_FLASH,
    SOC_BLK_REMOTE_DTCM,
    SOC_BLK_REMOTE_ITCM,
    SOC_BLK_REMOTE_PERIPHERAL,
    SOC_BLK_REMOTE_RAM,
    SOC_BLK_RNG,
    SOC_BLK_SMBUS,
    SOC_BLK_SPI,
    SOC_BLK_TIM,
    SOC_BLK_UART,
    SOC_BLK_UNIMAC,
    SOC_BLK_WDT,
    SOC_BLK_XLPORT,

} soc_block_type_t;

extern int soc_robo2_max_blocks_per_entry_get(void);

#if 0 /* [VKC] initial compile */
/*
 * soc_feature_fun_t: boolean function indicating if feature is supported
 * Used in bcm*.i files.
 */
typedef int (*soc_feature_fun_t) (int unit, soc_feature_t feature);
#endif /* [VKC] initial compile */

/*
 * soc_init_chip_fun_t: chip initialization function
 * Used in bcm*.i files.
 */
typedef void (*soc_init_chip_fun_t) (void);


#endif /* __CHIP_H */

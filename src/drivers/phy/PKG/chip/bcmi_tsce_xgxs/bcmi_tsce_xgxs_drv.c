/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: Copyright 2020 Broadcom Inc.
 * This program is the proprietary software of Broadcom Inc.
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * PHY driver for Broadcom TSC/Eagle internal SerDes.
 *
 */
/*
 *   LAYERING.
 *   This driver has a layered architecture, including:
 *
 *      Phy Driver           - this module
 *      Eagle "Tier2"        - Eagle specific driver code.  Phymod uses
 *                              a "dispatch layer", similar to bcm dispatch
 *                              to call Tier2 functions.  This code contains
 *                              device specific logic, that typically consists
 *                              of a sequence of lower level "Tier1" functions.
 *      Eagle "Tier1"        - Eagle specific low level driver code.
 *                              Tier1 code is shared by the DV environment and
 *                              by Phymod, whether it is built as part of a
 *                              standalone PHYMOD driver, or included as part
 *                              of an SDK driver.
 */

#include <phy/phy.h>
#include <phy/phy_drvlist.h>
#include <phymod/phymod.h>
#include <phy/chip/bcmi_tsce_xgxs_defs.h>

#include "tsce/tsce.h"

#if PHY_CONFIG_PRIVATE_DATA_WORDS > 0

#define PRIV_DATA(_pc) ((_pc)->priv[0])

#define LANE_MAP_COMPRESS(_umap) \
    ((((_umap) & 0x3000) >> 6) | \
     (((_umap) & 0x0300) >> 4) | \
     (((_umap) & 0x0030) >> 2) | \
     (((_umap) & 0x0003) >> 0))
#define LANE_MAP_DECOMPRESS(_cmap) \
    ((((_cmap) << 6) & 0x3000) | \
     (((_cmap) << 4) & 0x0300) | \
     (((_cmap) << 2) & 0x0030) | \
     (((_cmap) << 0) & 0x0003))

#define TX_LANE_MAP_GET(_pc) (LANE_MAP_DECOMPRESS(PRIV_DATA(_pc) & 0xff))
#define TX_LANE_MAP_SET(_pc, _val) \
do { \
    PRIV_DATA(_pc) &= ~0xff; \
    PRIV_DATA(_pc) |= LANE_MAP_COMPRESS(_val); \
} while(0)

#define RX_LANE_MAP_GET(_pc) \
    (LANE_MAP_DECOMPRESS((PRIV_DATA(_pc) & 0xff00) >> 8))
#define RX_LANE_MAP_SET(_pc, _val) \
do { \
    PRIV_DATA(_pc) &= ~0xff00; \
    PRIV_DATA(_pc) |= (LANE_MAP_COMPRESS(_val) << 8); \
} while(0)

#define TX_POLARITY_GET(_pc)       ((PRIV_DATA(_pc) & 0xf0000) >> 16)
#define TX_POLARITY_SET(_pc, _val) \
do { \
    PRIV_DATA(_pc) &= ~0xf0000; \
    PRIV_DATA(_pc) |= (((_val) & 0xf) << 16); \
} while(0)

#define RX_POLARITY_GET(_pc)       ((PRIV_DATA(_pc) & 0xf00000) >> 20)
#define RX_POLARITY_SET(_pc, _val) \
do { \
    PRIV_DATA(_pc) &= ~0xf00000; \
    PRIV_DATA(_pc) |= (((_val) & 0xf) << 20); \
} while(0)

#define MULTI_CORE_LANE_MAP_GET(_pc) ((PRIV_DATA(_pc) & 0xf000000) >> 24)
#define MULTI_CORE_LANE_MAP_SET(_pc, _val) \
do { \
    PRIV_DATA(_pc) &= ~0xf000000; \
    PRIV_DATA(_pc) |= (((_val) & 0xf) << 24); \
} while(0)

#else

#define TX_LANE_MAP_GET(_pc) (0)
#define TX_LANE_MAP_SET(_pc, _val)
#define RX_LANE_MAP_GET(_pc) (0)
#define RX_LANE_MAP_SET(_pc, _val)
#define TX_POLARITY_GET(_pc) (0)
#define TX_POLARITY_SET(_pc, _val)
#define RX_POLARITY_GET(_pc) (0)
#define RX_POLARITY_SET(_pc, _val)
#define MULTI_CORE_LANE_MAP_GET(_pc)
#define MULTI_CORE_LANE_MAP_SET(_pc, _val)

#endif /* PHY_CONFIG_PRIVATE_DATA_WORDS */

/* Low level debugging (off by default) */
#ifdef PHY_DEBUG_ENABLE
#define _PHY_DBG(_pc, _stuff) \
    PHY_VERB(_pc, _stuff)
#else
#define _PHY_DBG(_pc, _stuff)
#endif

#define IS_1LANE_PORT(_pc) \
    ((PHY_CTRL_FLAGS(_pc) & (PHY_F_SERDES_MODE | PHY_F_2LANE_MODE)) == PHY_F_SERDES_MODE)
#define IS_2LANE_PORT(_pc) \
    (PHY_CTRL_FLAGS(_pc) & PHY_F_2LANE_MODE)
#define IS_4LANE_PORT(_pc) \
    ((PHY_CTRL_FLAGS(_pc) & PHY_F_SERDES_MODE) == 0)
#define IS_MULTI_CORE_PORT(_pc) \
    (PHY_CTRL_FLAGS(_pc) & PHY_F_MULTI_CORE)

#if PHY_CONFIG_INCLUDE_CHIP_SYMBOLS == 1
extern cdk_symbols_t bcmi_tsce_xgxs_symbols;
#define SET_SYMBOL_TABLE(_pc) \
    PHY_CTRL_SYMBOLS(_pc) = &bcmi_tsce_xgxs_symbols
#else
#define SET_SYMBOL_TABLE(_pc)
#endif

#define NUM_LANES                       4   /* num of lanes per core */
#define MAX_NUM_LANES                   4   /* max num of lanes per port */
#define LANE_NUM_MASK                   0x3 /* Lane from PHY control instance */

#define TSCE_CL73_CL37                 0x5
#define TSCE_CL73_HPAM                 0x4
#define TSCE_CL73_HPAM_VS_SW           0x8
#define TSCE_CL73_WO_BAM               0x2
#define TSCE_CL73_W_BAM                0x1
#define TSCE_CL73_DISABLE              0x0

#define TSCE_CL37_HR2SPM_W_10G         0x5
#define TSCE_CL37_HR2SPM               0x4
#define TSCE_CL37_W_10G                0x3
#define TSCE_CL37_WO_BAM               0x2
#define TSCE_CL37_W_BAM                0x1
#define TSCE_CL37_DISABLE              0x0

/* Private PHY flag is used to indicate that firmware is running */
#define PHY_F_FW_RUNNING                PHY_F_PRIVATE

#define PORT_REFCLK_INT                156

/*
 * Function:
 *      _tsce_serdes_lane
 * Purpose:
 *      Retrieve eagle lane number for this PHY instance.
 * Parameters:
 *      pc - PHY control structure
 * Returns:
 *      Lane number or -1 if lane is unknown
 */
static int
_tsce_serdes_lane(phy_ctrl_t *pc)
{
    uint32_t inst = PHY_CTRL_INST(pc);

    if (inst & PHY_INST_VALID) {
        return inst & LANE_NUM_MASK;
    }
    return -1;
}

/*
 * Function:
 *      _tsce_primary_lane
 * Purpose:
 *      Ensure that each tsce is initialized only once.
 * Parameters:
 *      pc - PHY control structure
 * Returns:
 *      TRUE/FALSE
 */
static int
_tsce_primary_lane(phy_ctrl_t *pc)
{
    uint32_t inst = PHY_CTRL_INST(pc);

    return (inst & LANE_NUM_MASK) ? FALSE : TRUE;
}

/*
 * Function:
 *      _tsce_phymod_core_t_init
 * Purpose:
 *      Init phymod_core_access structure.
 * Parameters:
 *      pc - PHY control structure
 *      core - PHYMOD core access structure
 * Returns:
 *      CDK_E_xxx
 */
static int
_tsce_phymod_core_t_init(phy_ctrl_t *pc, phymod_core_access_t *core)
{
    if (pc == NULL || core == NULL) {
        return CDK_E_PARAM;
    }

    CDK_MEMSET(core,0, sizeof(*core));

    /* Use user data pointer to keep whole phy ctrl data */
    core->access.user_acc = (void *)pc;

    /* Setting the flags for phymod */
    if (PHY_CTRL_FLAGS(pc) & PHY_F_CLAUSE45) {
        PHYMOD_ACC_F_CLAUSE45_CLR(&core->access);
    }

    return CDK_E_NONE;
}

/*
 * Function:
 *      _tsce_phymod_phy_t_init
 * Purpose:
 *      Init phymod_phy_access structure.
 * Parameters:
 *      pc - PHY control structure
 *      core - PHYMOD phy access structure
 * Returns:
 *      CDK_E_xxx
 */
static int
_tsce_phymod_phy_t_init(phy_ctrl_t *pc, phymod_phy_access_t *phy)
{
    uint32_t lane_map;
    int      shift = 0;
    if (pc == NULL || phy == NULL) {
        return CDK_E_PARAM;
    }

    CDK_MEMSET(phy, 0, sizeof(*phy));

    /* Use user data pointer to keep whole phy ctrl data */
    phy->access.user_acc = (void *)pc;

    lane_map = 0xf;
    if (IS_2LANE_PORT(pc)) {
        lane_map = 0x3;
    } else if (IS_1LANE_PORT(pc)) {
        lane_map = 0x1;
    }
    shift = _tsce_serdes_lane(pc);
    if (shift >= 0) {
        phy->access.lane_mask = lane_map << shift;
    }
    /* Setting the flags for phymod */
    if (PHY_CTRL_FLAGS(pc) & PHY_F_CLAUSE45) {
        PHYMOD_ACC_F_CLAUSE45_CLR(&phy->access);
    }

    return CDK_E_NONE;
}

/*
 * Function:
 *      _tsce_firmware_loader
 * Purpose:
 *      Download PHY firmware using fast interface such as S-channel.
 * Parameters:
 *      pa              - (IN)  PHYMOD access oject
 *      fw_len          - (IN)  Number of bytes to download
 *      fw_data         - (IN)  Firmware as byte array
 * Returns:
 *      PHYMOD_E_xxx
 */
static int
_tsce_firmware_loader(const phymod_core_access_t *pm_core,
                      uint32_t fw_size, const uint8_t *fw_data)
{
    phy_ctrl_t *pc;
    int rv = CDK_E_NONE;

    if (pm_core == NULL) {
        return PHYMOD_E_PARAM;
    }

    /* Retrieve PHY control information from PHYMOD object */
    pc = (phy_ctrl_t *)(pm_core->access.user_acc);
    if (pc == NULL) {
        return PHYMOD_E_PARAM;
    }

    /* Invoke external loader */
    rv = PHY_CTRL_FW_HELPER(pc)(pc, 0, fw_size, (uint8_t *)fw_data);

    return (rv == CDK_E_NONE) ? PHYMOD_E_NONE : PHYMOD_E_IO;
}

/*
 * Function:
 *      _tsce_serdes_stop
 * Purpose:
 *      Put PHY in or out of reset depending on conditions.
 * Parameters:
 *      pc - PHY control structure
 * Returns:
 *      CDK_E_xxx
 */
static int
_tsce_serdes_stop(phy_ctrl_t *pc)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phymod_phy_tx_lane_control_t tx_control;
    phymod_phy_rx_lane_control_t rx_control;
    uint32_t f_any = PHY_F_PHY_DISABLE | PHY_F_PORT_DRAIN;
    uint32_t f_copper = PHY_F_MAC_DISABLE | PHY_F_SPEED_CHG | PHY_F_DUPLEX_CHG;
    int stop;

    stop = 0;
    if ((PHY_CTRL_FLAGS(pc) & f_any) ||
        ((PHY_CTRL_FLAGS(pc) & PHY_F_FIBER_MODE) == 0 &&
         (PHY_CTRL_FLAGS(pc) & f_copper))) {
        stop = 1;
    }

    _PHY_DBG(pc, ("_tsce_serdes_stop: stop = %d\n", stop));

    if (!stop) { /* Enable */
        tx_control = phymodTxSquelchOff;
        rx_control = phymodRxSquelchOff;
    } else {
        tx_control = phymodTxSquelchOn;
        rx_control = phymodRxSquelchOn;
    }

    _tsce_phymod_phy_t_init(pc, &pm_phy);
    if (CDK_SUCCESS(rv)) {
        rv = tsce_phy_tx_lane_control_set(&pm_phy, tx_control);
    }
    if (CDK_SUCCESS(rv)) {
        rv = tsce_phy_rx_lane_control_set(&pm_phy, rx_control);
    }

    return rv;
}

/*
 * Function:
 *      _tsce_ability_set
 * Purpose:
 *      configure phy ability.
 * Parameters:
 *      pc - PHY control structure
 *      ability - ability value
 * Returns:
 *      CDK_E_xxx
 */
static int
_tsce_ability_set(phy_ctrl_t *pc, uint32_t ability)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phymod_autoneg_ability_t autoneg_ability;
    phy_ctrl_t *lpc;
    uint32_t an_cap, an_bam37_ability, an_bam73_ability;
    int idx;

    an_cap = 0;
    an_bam73_ability = 0;
    an_bam37_ability = 0;

    /* Set an_cap */
    if (IS_4LANE_PORT(pc)) {
        if (ability & PHY_ABIL_100GB) {
            PHYMOD_AN_CAP_100G_CR10_SET(an_cap);
        }
        if (ability & PHY_ABIL_40GB) {
            if (PHY_CTRL_LINE_INTF(pc) == PHY_IF_CR) {
                PHYMOD_AN_CAP_40G_CR4_SET(an_cap);
            } else {
                PHYMOD_AN_CAP_40G_KR4_SET(an_cap);
            }
        }
        if (ability & PHY_ABIL_20GB) {
            if (PHY_CTRL_LINE_INTF(pc) == PHY_IF_CR) {
                PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
            } else {
                PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
            }
        }
        if (ability & PHY_ABIL_10GB) {
            PHYMOD_AN_CAP_10G_KX4_SET(an_cap);
        }
        if (ability & PHY_ABIL_1000MB_FD) {
            PHYMOD_AN_CAP_1G_KX_SET(an_cap);
        }
    } else if (IS_2LANE_PORT(pc)) {
        if (ability & PHY_ABIL_20GB) {
            if (PHY_CTRL_LINE_INTF(pc) == PHY_IF_CR) {
                PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
            } else {
                PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
            }
        }
        if (ability & PHY_ABIL_10GB) {
            PHYMOD_AN_CAP_10G_KR_SET(an_cap);
        }
        if (ability & PHY_ABIL_1000MB_FD) {
            PHYMOD_AN_CAP_1G_KX_SET(an_cap);
        }
    } else {
        if (ability & PHY_ABIL_10GB) {
            PHYMOD_AN_CAP_10G_KR_SET(an_cap);
        }
        if (ability & PHY_ABIL_1000MB_FD) {
            PHYMOD_AN_CAP_1G_KX_SET(an_cap);
        }
    }

    /* Set an_bam37_ability */
    if (IS_4LANE_PORT(pc)) {
        if (ability & PHY_ABIL_40GB) {
            PHYMOD_BAM_CL37_CAP_40G_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_30GB) {
            PHYMOD_BAM_CL37_CAP_31P5G_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_25GB) {
            PHYMOD_BAM_CL37_CAP_25P455G_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_21GB) {
            PHYMOD_BAM_CL37_CAP_21G_X4_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_20GB) {
            PHYMOD_BAM_CL37_CAP_20G_X4_SET(an_bam37_ability);
            PHYMOD_BAM_CL37_CAP_20G_X4_CX4_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_16GB) {
            PHYMOD_BAM_CL37_CAP_16G_X4_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_13GB) {
            PHYMOD_BAM_CL37_CAP_13G_X4_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_10GB) {
            PHYMOD_BAM_CL37_CAP_10G_CX4_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_2500MB) {
            PHYMOD_BAM_CL37_CAP_2P5G_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_1000MB_FD) {
            PHYMOD_AN_CAP_1G_KX_SET(an_cap);
        }
    } else if (IS_2LANE_PORT(pc)) {
        if (ability & PHY_ABIL_20GB) {
            PHYMOD_BAM_CL37_CAP_20G_X2_SET(an_bam37_ability);
            PHYMOD_BAM_CL37_CAP_20G_X2_CX4_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_16GB) {
            PHYMOD_BAM_CL37_CAP_15P75G_R2_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_13GB) {
            PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_10GB) {
            PHYMOD_BAM_CL37_CAP_10G_X2_CX4_SET(an_bam37_ability);
            PHYMOD_BAM_CL37_CAP_10G_DXGXS_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_2500MB) {
            PHYMOD_BAM_CL37_CAP_2P5G_SET(an_bam37_ability);
        }
        if (ability & PHY_ABIL_1000MB_FD) {
            PHYMOD_AN_CAP_1G_KX_SET(an_cap);
        }
    } else {
        if (ability & PHY_ABIL_2500MB) {
            PHYMOD_BAM_CL37_CAP_2P5G_SET(an_bam37_ability);
        }
    }
    CDK_MEMSET(&autoneg_ability, 0, sizeof(phymod_autoneg_ability_t));

    autoneg_ability.an_cap = an_cap;
    autoneg_ability.cl73bam_cap = an_bam73_ability;
    autoneg_ability.cl37bam_cap = an_bam37_ability;

    if (ability & PHY_ABIL_PAUSE_TX) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&autoneg_ability);
    }
    if (ability & PHY_ABIL_PAUSE_RX) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&autoneg_ability);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&autoneg_ability);
    }

    /* Set the sgmii speed */
    if(ability & PHY_ABIL_1000MB_FD) {
        PHYMOD_AN_CAP_SGMII_SET(&autoneg_ability);
        autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;
    } else if(ability & PHY_ABIL_100MB_FD) {
        PHYMOD_AN_CAP_SGMII_SET(&autoneg_ability);
        autoneg_ability.sgmii_speed = phymod_CL37_SGMII_100M;
    } else if(ability & PHY_ABIL_10MB_FD) {
        PHYMOD_AN_CAP_SGMII_SET(&autoneg_ability);
        autoneg_ability.sgmii_speed = phymod_CL37_SGMII_10M;
    } else {
        PHYMOD_AN_CAP_SGMII_SET(&autoneg_ability);
        autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;
    }

    /* Check if we need to set cl37 attribute */
    autoneg_ability.an_cl72 = 1;
    if (PHY_CTRL_LINE_INTF(pc) == PHY_IF_HIGIG) {
        autoneg_ability.an_hg2 = 1;
    }

    lpc = pc;
    for (idx = 0; idx < pc->num_phys; idx++) {
        if (pc->num_phys == 3 && idx > 0) {
            lpc = pc->slave[idx - 1];
            if (lpc == NULL) {
                continue;
            }
        }
        _tsce_phymod_phy_t_init(lpc, &pm_phy);
        rv = tsce_phy_autoneg_ability_set(&pm_phy, &autoneg_ability);
    }

    return rv;
}

/*
 * Function:
 *      _tsce_ability_get
 * Purpose:
 *      Get phy ability.
 * Parameters:
 *      pc - PHY control structure
 *      ability - ability value
 * Returns:
 *      CDK_E_xxx
 */
static int
_tsce_ability_get(phy_ctrl_t *pc, uint32_t *ability)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phymod_autoneg_ability_t autoneg_ability;
    int reg37_ability, reg73_ability, reg_ability;

    PHY_CTRL_CHECK(pc);

    _tsce_phymod_phy_t_init(pc, &pm_phy);
    rv = tsce_phy_autoneg_ability_get(&pm_phy, &autoneg_ability);

    *ability = 0;

    /* retrieve CL73 abilities */
    reg73_ability = autoneg_ability.an_cap;
    *ability |= PHYMOD_AN_CAP_100G_CR10_GET(reg73_ability) ? PHY_ABIL_100GB : 0;
    *ability |= PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability) ? PHY_ABIL_40GB : 0;
    *ability |= PHYMOD_AN_CAP_40G_KR4_GET(reg73_ability) ? PHY_ABIL_40GB : 0;
    *ability |= PHYMOD_AN_CAP_10G_KR_GET(reg73_ability) ? PHY_ABIL_10GB : 0;
    *ability |= PHYMOD_AN_CAP_10G_KX4_GET(reg73_ability) ? PHY_ABIL_10GB : 0;
    *ability |= PHYMOD_AN_CAP_1G_KX_GET(reg73_ability) ? PHY_ABIL_1000MB_FD : 0;

    /* retrieve CL73bam abilities */
    reg73_ability = autoneg_ability.cl73bam_cap;
    *ability |= PHYMOD_BAM_CL73_CAP_20G_CR2_GET(reg73_ability) ? PHY_ABIL_20GB : 0;
    *ability |= PHYMOD_BAM_CL73_CAP_20G_KR2_GET(reg73_ability) ? PHY_ABIL_20GB : 0;

    /* retrieve CL37 abilities */
    reg37_ability = autoneg_ability.cl37bam_cap;
    *ability |= PHYMOD_BAM_CL37_CAP_40G_GET(reg37_ability) ? PHY_ABIL_40GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_31P5G_GET(reg37_ability) ? PHY_ABIL_30GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_25P455G_GET(reg37_ability) ? PHY_ABIL_25GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_21G_X4_GET(reg37_ability) ? PHY_ABIL_21GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_20G_X2_CX4_GET(reg37_ability) ? PHY_ABIL_20GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_20G_X2_GET(reg37_ability) ? PHY_ABIL_20GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_20G_X4_GET(reg37_ability) ? PHY_ABIL_20GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_20G_X4_CX4_GET(reg37_ability) ? PHY_ABIL_20GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_16G_X4_GET(reg37_ability) ? PHY_ABIL_16GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_15P75G_R2_GET(reg37_ability )? PHY_ABIL_16GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_13G_X4_GET(reg37_ability) ? PHY_ABIL_13GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_12P7_DXGXS_GET(reg37_ability)? PHY_ABIL_13GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_10G_X2_CX4_GET(reg37_ability) ? PHY_ABIL_10GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_10G_DXGXS_GET(reg37_ability) ? PHY_ABIL_10GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_10G_CX4_GET(reg37_ability) ? PHY_ABIL_10GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_10G_HIGIG_GET(reg37_ability) ? PHY_ABIL_10GB : 0;
    *ability |= PHYMOD_BAM_CL37_CAP_2P5G_GET(reg37_ability) ? PHY_ABIL_2500MB : 0;
    *ability |= PHY_ABIL_1000MB_FD ;

    /* retrieve "pause" abilities */
    reg_ability = autoneg_ability.capabilities;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        *ability |= PHY_ABIL_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE | PHYMOD_AN_CAP_ASYM_PAUSE)) {
        *ability |= PHY_ABIL_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        *ability |= PHY_ABIL_PAUSE;
    }

    return rv;
}

static int
_tsce_ref_clk_convert(int port_refclk_int, phymod_ref_clk_t *ref_clk)
{
    switch (port_refclk_int)
    {
        case 156:
            *ref_clk = phymodRefClk156Mhz;
            break;
        case 125:
            *ref_clk = phymodRefClk125Mhz;
            break;
        default:
            return CDK_E_PARAM;
            break;
    }

    return CDK_E_NONE;
}

static int
_tsce_intf_cfg_set(phy_ctrl_t *pc, uint32_t flags,
                   phymod_phy_inf_config_t *if_cfg)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phy_ctrl_t *lpc;
    uint32_t vco_12p5 = 0;
    lpc = pc;
    phymod_core_access_t pm_core;
    _tsce_phymod_core_t_init(lpc, &pm_core);
    pm_core.access.lane_mask = 0x1;

    if (if_cfg->data_rate == 2500) {
        vco_12p5 = 1;
    }
    {
        rv =  tsce_core_vco_change(&pm_core, vco_12p5);
    }
    _tsce_phymod_phy_t_init(lpc, &pm_phy);
    if ((lpc->media == phymodFirmwareMediaTypeCopperCable) || (if_cfg->data_rate == 10001)) {
        if_cfg->interface_modes  = PHYMOD_INTF_MODES_COPPER;
    }
    if ((if_cfg->data_rate == 1001) && (if_cfg->interface_type == phymodInterfaceXFI))  {
        if_cfg->interface_modes = 1;   /* AN */
        if_cfg->data_rate = 1000;
    }
    if (CDK_SUCCESS(rv)) {
        rv = tsce_phy_interface_config_set(&pm_phy, flags, if_cfg);
    }
    return rv;
}

/***********************************************************************
 *
 * PHY DRIVER FUNCTIONS
 *
 ***********************************************************************/
/*
 * Function:
 *      bcmi_tsce_xgxs_probe
 * Purpose:
 *      Probe for PHY.
 * Parameters:
 *      pc - PHY control structure
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_probe(phy_ctrl_t *pc)
{
    int rv = CDK_E_NONE;
    phymod_core_access_t pm_core;
    uint32_t found;
    PHY_CTRL_CHECK(pc);

    PHY_CTRL_FLAGS(pc) |= PHY_F_LANE_CTRL;
    _tsce_phymod_core_t_init(pc, &pm_core);

    found = 0;
    rv = tsce_core_identify(&pm_core, 0, &found);
    if (CDK_SUCCESS(rv) && found) {
        SET_SYMBOL_TABLE(pc);
        return CDK_E_NONE;
    }

    PHY_CTRL_FLAGS(pc) &= ~PHY_F_LANE_CTRL;
    return CDK_E_NOT_FOUND;
}


/*
 * Function:
 *      bcmi_tsce_xgxs_notify
 * Purpose:
 *      Handle PHY notifications.
 * Parameters:
 *      pc - PHY control structure
 *      event - PHY event
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_notify(phy_ctrl_t *pc, phy_event_t event)
{
    int ioerr = 0;
    int rv = CDK_E_NONE;

    PHY_CTRL_CHECK(pc);

    switch (event) {
    case PhyEvent_ChangeToPassthru:
        PHY_CTRL_FLAGS(pc) &= ~PHY_F_FIBER_MODE;
        PHY_CTRL_FLAGS(pc) |= PHY_F_PASSTHRU;
        return CDK_E_NONE;
    case PhyEvent_ChangeToFiber:
        PHY_CTRL_FLAGS(pc) |= PHY_F_FIBER_MODE;
        PHY_CTRL_FLAGS(pc) &= ~PHY_F_PASSTHRU;
        return CDK_E_NONE;
    case PhyEvent_MacDisable:
        PHY_CTRL_FLAGS(pc) |=  PHY_F_MAC_DISABLE;
        return CDK_E_NONE;
    case PhyEvent_MacEnable:
        PHY_CTRL_FLAGS(pc) &= ~PHY_F_MAC_DISABLE;
        return CDK_E_NONE;
    case PhyEvent_PhyDisable:
        PHY_CTRL_FLAGS(pc) |=  PHY_F_PHY_DISABLE;
        break;
    case PhyEvent_PhyEnable:
        PHY_CTRL_FLAGS(pc) &= ~PHY_F_PHY_DISABLE;
        break;
    case PhyEvent_PortDrainStart:
        PHY_CTRL_FLAGS(pc) |=  PHY_F_PORT_DRAIN;
        break;
    case PhyEvent_PortDrainStop:
        PHY_CTRL_FLAGS(pc) &= ~PHY_F_PORT_DRAIN;
        break;
    default:
        return CDK_E_NONE;
    }

    /* Update power-down state */
    ioerr = _tsce_serdes_stop(pc);

    return ioerr ? CDK_E_IO : rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_reset
 * Purpose:
 *      Reset PHY.
 * Parameters:
 *      pc - PHY control structure
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_reset(phy_ctrl_t *pc)
{
    PHY_CTRL_CHECK(pc);

    return CDK_E_NONE;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_init
 * Purpose:
 *      Initialize PHY driver.
 * Parameters:
 *      pc - PHY control structure
 * Returns:
 *      CDK_E_NONE
 */
static int
bcmi_tsce_xgxs_init(phy_ctrl_t *pc)
{
    int rv = CDK_E_NONE;
    phymod_core_access_t pm_core;
    phymod_phy_access_t pm_phy;
    phymod_core_init_config_t core_init_config;
    phymod_core_init_config_t *cic;
    phymod_core_status_t core_status;
    phymod_phy_inf_config_t *if_cfg;
    phymod_phy_init_config_t pm_phy_init_config;
    phy_ctrl_t *lpc;
    int idx;

    PHY_CTRL_CHECK(pc);

    pc->num_phys = 1;

    cic = &core_init_config;
    if_cfg = &cic->interface;
    if_cfg->interface_type = pc->interface;

    lpc = pc;
    for (idx = 0; idx < pc->num_phys; idx++) {

        _tsce_phymod_core_t_init(lpc, &pm_core);
        _tsce_phymod_phy_t_init(lpc, &pm_phy);
        pm_phy.access.lane_mask = 0x1;

        pm_phy_init_config.polarity.rx_polarity = 0;
        pm_phy_init_config.polarity.tx_polarity = 0;
        pm_phy_init_config.tx[0].pre = 0;
        pm_phy_init_config.tx[0].main = 112;
        pm_phy_init_config.tx[0].post = 0;
        pm_phy_init_config.tx[0].post2 = 0;
        pm_phy_init_config.tx[0].post3 = 0;
        pm_phy_init_config.tx[0].amp = 5;
        pm_phy_init_config.an_en = 0;
        pm_phy_init_config.cl72_en = 0;

        pm_core.access.lane_mask = 0x1;

        if (_tsce_primary_lane(lpc)) {
            CDK_MEMSET(cic, 0, sizeof(*cic));
            /* CORE configuration */
            cic->firmware_load_method = phymodFirmwareLoadMethodInternal;

            /* Check for external loader */
            if (PHY_CTRL_FW_HELPER(lpc)) {
                cic->firmware_loader = _tsce_firmware_loader;
                cic->firmware_load_method = phymodFirmwareLoadMethodExternal;
            }

            cic->lane_map.num_of_lanes = 1;
            PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(cic);

            cic->lane_map.lane_map_rx[0] = 0;
            cic->lane_map.lane_map_tx[0] = 0;
            cic->polarity_map.rx_polarity = 0;
            cic->polarity_map.tx_polarity = 0;
            core_status.pmd_active = 0;
#if (CONFIG_XFI_SPEED == 2500)
            cic->interface.data_rate = 2500;
#endif
#if (CONFIG_XFI_SPEED == 1000)
            cic->interface.data_rate = 1000;
#endif
            if (pc->speed != 0xFFFFFFFF) {
                cic->interface.data_rate = pc->speed;
            }
            if (CDK_SUCCESS(rv)) {
                rv = tsce_core_init(&pm_core, cic, &core_status);
            }
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_init(&pm_phy, &pm_phy_init_config);
            }
        }
        if (CDK_SUCCESS(rv))
        {
            phymod_phy_inf_config_t interface_config;
            phymod_interface_t if_type = 0;


            if_cfg = &interface_config;
            CDK_MEMSET(if_cfg, 0, sizeof(*if_cfg));

            if_cfg = &interface_config;
            if_type = pc->interface;
            /* Default setting the ref_clock to 156. */
            if_cfg->ref_clock = phymodRefClk156Mhz;
            if_cfg->interface_type = if_type;
#if defined(CONFIG_10G_INTF_SGMII)
            if_cfg->interface_type  = phymodInterfaceSGMII;
            if_cfg->interface_modes = 1;
#endif
#if defined(CONFIG_10G_INTF_KR)
            if_cfg->interface_type  = phymodInterfaceKR;
            if_cfg->interface_modes = 2;
#endif
#if defined(CONFIG_10G_INTF_KX)
            if_cfg->interface_type = phymodInterfaceKX;
            if_cfg->interface_modes = 3;
#endif
            if_cfg->data_rate = 10312;
#if (CONFIG_XFI_SPEED == 2500)
            if_cfg->data_rate = 2500;
#endif
#if (CONFIG_XFI_SPEED == 1000)
            if_cfg->data_rate = 1000;
#endif
            if ((pc->speed != 0xFFFFFFFF) && (pc->speed != 0)) {
                if_cfg->data_rate = pc->speed;
            }
            rv = tsce_phy_interface_config_set(&pm_phy, 0, if_cfg);
        }

    }
    /* Default mode is fiber */
    PHY_NOTIFY(pc, PhyEvent_ChangeToFiber);

    return rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_link_get
 * Purpose:
 *      Determine the current link up/down status.
 * Parameters:
 *      pc - PHY control structure
 *      link - (OUT) non-zero indicates link established.
 *      autoneg_done - (OUT) if true, auto-negotiation is complete
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_link_get(phy_ctrl_t *pc, int *link, int *autoneg_done)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phymod_autoneg_control_t an;
    int an_done;

    PHY_CTRL_CHECK(pc);

    CDK_MEMSET(&an, 0x0, sizeof(an));
    _tsce_phymod_phy_t_init(pc, &pm_phy);
    if (CDK_SUCCESS(rv)) {
        rv = tsce_phy_autoneg_get(&pm_phy, &an, (uint32_t *)&an_done);
    }
    *autoneg_done = 0;
    if (an.enable) {
        *autoneg_done = an_done;
    }

    if (CDK_SUCCESS(rv)) {
        _tsce_phymod_phy_t_init(pc, &pm_phy);
        rv = tsce_phy_link_status_get(&pm_phy, (uint32_t *)link);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      pc - PHY control structure
 *      duplex - non-zero indicates full duplex, zero indicates half
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_duplex_set(phy_ctrl_t *pc, int duplex)
{
    PHY_CTRL_CHECK(pc);

    return CDK_E_NONE;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      pc - PHY control structure
 *      duplex - (OUT) non-zero indicates full duplex, zero indicates half
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_duplex_get(phy_ctrl_t *pc, int *duplex)
{
    PHY_CTRL_CHECK(pc);

    *duplex = 1;

    return CDK_E_NONE;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_speed_set
 * Purpose:
 *      Set the current operating speed (forced).
 * Parameters:
 *      pc - PHY control structure
 *      speed - new link speed
 * Returns:
 *      CDK_E_xxx
 * Notes:
 *      The actual speed is controlled elsewhere, so we accept any value.
 */
static int
bcmi_tsce_xgxs_speed_set(phy_ctrl_t *pc, uint32_t speed)
{
    int rv = CDK_E_NONE;
    phymod_phy_inf_config_t interface_config;
    phymod_phy_inf_config_t *if_cfg;
    PHY_CTRL_CHECK(pc);

    if_cfg = &interface_config;

    CDK_MEMSET(if_cfg, 0, sizeof(*if_cfg));

    if_cfg->data_rate = speed;
    if_cfg->interface_modes = 0;

    /* Default setting the ref_clock to 156. */
    if_cfg->ref_clock = phymodRefClk156Mhz;

    switch (speed) {
        case 10:
        case 100:
        case 1000:
        case 1001:
        case 2500:
        case 5000:
        case 10000:
        case 10001:
            break;
        default:
            return CDK_E_PARAM;
    }
    if_cfg->interface_type = phymodInterfaceXFI;
#if defined(CONFIG_10G_INTF_XFI)
    if_cfg->interface_type = phymodInterfaceXFI;
    if_cfg->interface_modes = 1;
#endif
#if defined(CONFIG_10G_INTF_SGMII)
    if_cfg->interface_type = phymodInterfaceSGMII;
    if_cfg->interface_modes = 1;
#endif
#if defined(CONFIG_10G_INTF_KR)
    if_cfg->interface_type = phymodInterfaceKR;
#endif
#if defined(CONFIG_10G_INTF_KX)
    if_cfg->interface_type = phymodInterfaceKX;
#endif
    rv = _tsce_intf_cfg_set(pc, 0, if_cfg);
    return rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_speed_get
 * Purpose:
 *      Get the current operating speed.
 * Parameters:
 *      pc - PHY control structure
 *      speed - (OUT) current link speed
 * Returns:
 *      CDK_E_xxx
 * Notes:
 *      The actual speed is controlled elsewhere, so always return 10000
 *      for sanity purposes.
 */

static int
bcmi_tsce_xgxs_speed_get(phy_ctrl_t *pc, uint32_t *speed)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phymod_phy_inf_config_t interface_config;
    phymod_ref_clk_t ref_clock;

    PHY_CTRL_CHECK(pc);

    _tsce_phymod_phy_t_init(pc, &pm_phy);

    /* Initialize the data structure */
    interface_config.data_rate = 0;
    interface_config.interface_modes = 0;

    /* Note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    rv = _tsce_ref_clk_convert(PORT_REFCLK_INT, &ref_clock);
    if (CDK_SUCCESS(rv)) {
        rv = tsce_phy_interface_config_get(&pm_phy, 0, ref_clock,
                                           &interface_config);
    }

    *speed = interface_config.data_rate;

    return rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_autoneg_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      pc - PHY control structure
 *      autoneg - non-zero enables autoneg, zero disables autoneg
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_autoneg_set(phy_ctrl_t *pc, int autoneg)
{
    if (autoneg == 1) {
        return bcmi_tsce_xgxs_speed_set(pc, 1001);
    } else {
        return bcmi_tsce_xgxs_speed_set(pc, 10000);
    }
}

/*
 * Function:
 *      bcmi_tsce_xgxs_autoneg_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      pc - PHY control structure
 *      autoneg - (OUT) non-zero indicates autoneg enabled
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_autoneg_get(phy_ctrl_t *pc, int *autoneg)
{
    phymod_autoneg_control_t an;
    phymod_phy_access_t pm_phy;

    PHY_CTRL_CHECK(pc);

    _tsce_phymod_phy_t_init(pc, &pm_phy);

    return tsce_phy_autoneg_get(&pm_phy, &an, (uint32_t *)autoneg);
}

/*
 * Function:
 *      bcmi_tsce_xgxs_loopback_set
 * Purpose:
 *      Set PHY loopback mode.
 * Parameters:
 *      pc - PHY control structure
 *      enable - non-zero enables PHY loopback
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_loopback_set(phy_ctrl_t *pc, int enable)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phy_ctrl_t *lpc;

    PHY_CTRL_CHECK(pc);

    lpc = pc;
    _tsce_phymod_phy_t_init(lpc, &pm_phy);

    if (CDK_SUCCESS(rv)) {
        rv = tsce_phy_loopback_set(&pm_phy, phymodLoopbackGlobalPMD, enable);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_loopback_get
 * Purpose:
 *      Get the current PHY loopback mode.
 * Parameters:
 *      pc - PHY control structure
 *      enable - (OUT) non-zero indicates PHY loopback enabled
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_loopback_get(phy_ctrl_t *pc, int *enable)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    uint32_t lb_enable;

    PHY_CTRL_CHECK(pc);

    *enable = 0;

    _tsce_phymod_phy_t_init(pc, &pm_phy);
    rv = tsce_phy_loopback_get(&pm_phy, phymodLoopbackGlobalPMD, &lb_enable);
    if (lb_enable) {
        *enable = 1;
    }

    return rv;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_ability_get
 * Purpose:
 *      Get the abilities of the PHY.
 * Parameters:
 *      pc - PHY control structure
 *      abil - (OUT) ability mask indicating supported options/speeds.
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_ability_get(phy_ctrl_t *pc, uint32_t *abil)
{
    PHY_CTRL_CHECK(pc);

    *abil = PHY_ABIL_10GB | PHY_ABIL_LOOPBACK;
    if (IS_4LANE_PORT(pc)) {
        *abil = (PHY_ABIL_100GB | PHY_ABIL_40GB | PHY_ABIL_25GB |
                 PHY_ABIL_21GB | PHY_ABIL_20GB |
                 PHY_ABIL_16GB | PHY_ABIL_13GB | PHY_ABIL_10GB |
                 PHY_ABIL_PAUSE | PHY_ABIL_XAUI | PHY_ABIL_XGMII |
                 PHY_ABIL_AN | PHY_ABIL_LOOPBACK);
    } else if (IS_2LANE_PORT(pc)) {
        *abil = (PHY_ABIL_21GB | PHY_ABIL_21GB | PHY_ABIL_13GB | PHY_ABIL_10GB |
                 PHY_ABIL_XAUI | PHY_ABIL_XGMII | PHY_ABIL_LOOPBACK);
    } else {
        *abil = (PHY_ABIL_10GB | PHY_ABIL_2500MB | PHY_ABIL_1000MB |
                 PHY_ABIL_100MB | PHY_ABIL_10MB | PHY_ABIL_SERDES |
                 PHY_ABIL_PAUSE | PHY_ABIL_SGMII | PHY_ABIL_GMII |
                 PHY_ABIL_AN | PHY_ABIL_LOOPBACK);
    }
    return CDK_E_NONE;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_config_set
 * Purpose:
 *      Modify PHY configuration value.
 * Parameters:
 *      pc - PHY control structure
 *      cfg - Configuration parameter
 *      val - Configuration value
 *      cd - Additional configuration data (if any)
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_config_set(phy_ctrl_t *pc, phy_config_t cfg, uint32_t val, void *cd)
{
    int rv = CDK_E_NONE;
    phymod_core_access_t pm_core;
    phymod_phy_access_t pm_phy;
    phy_ctrl_t *lpc;
    int idx;

    PHY_CTRL_CHECK(pc);

    lpc = pc;

    switch (cfg) {
    case PhyConfig_Enable:
        return CDK_E_NONE;
    case PhyConfig_RemoteLoopback:
        {
            for (idx = 0; idx < pc->num_phys; idx++) {
                if (pc->num_phys == 3 && idx > 0) {
                    lpc = pc->slave[idx - 1];
                    if (lpc == NULL) {
                        continue;
                    }
                }

                _tsce_phymod_phy_t_init(lpc, &pm_phy);

                if (CDK_SUCCESS(rv)) {
                    rv = tsce_phy_loopback_set(&pm_phy, phymodLoopbackRemotePMD, val);
                }
            }

            return rv;
        }
#if PHY_CONFIG_INCLUDE_XAUI_TX_LANE_MAP_SET
    case PhyConfig_XauiTxLaneRemap:
        {
            phymod_lane_map_t lane_map;

            TX_LANE_MAP_SET(pc, val);

            _tsce_phymod_core_t_init(pc, &pm_core);
            rv = tsce_core_lane_map_get(&pm_core, &lane_map);
            if (CDK_SUCCESS(rv)) {
                for (idx = 0; idx < NUM_LANES; idx++) {
                    /*4 bit per lane*/
                    lane_map.lane_map_tx[idx] = (val >> (idx * 4)) & 0xf;
                }
                rv = tsce_core_lane_map_set(&pm_core, &lane_map);
            }

            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_XAUI_RX_LANE_MAP_SET
    case PhyConfig_XauiRxLaneRemap:
        {
            phymod_lane_map_t lane_map;

            RX_LANE_MAP_SET(pc, val);

            _tsce_phymod_core_t_init(pc, &pm_core);
            rv = tsce_core_lane_map_get(&pm_core, &lane_map);
            if (CDK_SUCCESS(rv)) {
                for (idx = 0; idx < NUM_LANES; idx++) {
                    /*4 bit per lane*/
                    lane_map.lane_map_rx[idx] = (val >> (idx * 4)) & 0xf;
                }
                rv = tsce_core_lane_map_set(&pm_core, &lane_map);
            }

            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_XAUI_TX_POLARITY_SET
    case PhyConfig_XauiTxPolInvert:
        {
            phymod_polarity_t polarity;

            TX_POLARITY_SET(pc, val);

            _tsce_phymod_phy_t_init(pc, &pm_phy);
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_polarity_get(&pm_phy, &polarity);
            }

            polarity.tx_polarity = val;
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_polarity_set(&pm_phy, &polarity);
            }

            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_XAUI_RX_POLARITY_SET
    case PhyConfig_XauiRxPolInvert:
        {
            phymod_polarity_t polarity;

            RX_POLARITY_SET(pc, val);

            _tsce_phymod_phy_t_init(pc, &pm_phy);
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_polarity_get(&pm_phy, &polarity);
            }

            polarity.rx_polarity = val;
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_polarity_set(&pm_phy, &polarity);
            }

            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_LINK_ABILITIES
    case PhyConfig_AdvLocal:
        rv = _tsce_ability_set(pc, val);
        return rv;
#endif
    case PhyConfig_EEE:
        {
            for (idx = 0; idx < pc->num_phys; idx++) {
                if (pc->num_phys == 3 && idx > 0) {
                    lpc = pc->slave[idx - 1];
                    if (lpc == NULL) {
                        continue;
                    }
                }
                _tsce_phymod_phy_t_init(lpc, &pm_phy);
                if (CDK_SUCCESS(rv)) {
                    rv = tsce_phy_eee_set(&pm_phy, val);
                }
            }

            return rv;
        }
    case PhyConfig_TxPreemp:
    case PhyConfig_TxPost2:
    case PhyConfig_TxIDrv:
    case PhyConfig_TxPreIDrv:
        return CDK_E_NONE;
    case PhyConfig_MultiCoreLaneMap:
        MULTI_CORE_LANE_MAP_SET(pc, val);
        return rv;
    default:
        break;
    }

    return CDK_E_UNAVAIL;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_config_get
 * Purpose:
 *      Get PHY configuration value.
 * Parameters:
 *      pc - PHY control structure
 *      cfg - Configuration parameter
 *      val - (OUT) Configuration value
 *      cd - (OUT) Additional configuration data (if any)
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_config_get(phy_ctrl_t *pc, phy_config_t cfg, uint32_t *val, void *cd)
{
    int rv = CDK_E_NONE;
    phymod_core_access_t pm_core;
    phymod_phy_access_t pm_phy;
    int idx;

    PHY_CTRL_CHECK(pc);

    switch (cfg) {
    case PhyConfig_Enable:
        return CDK_E_NONE;
    case PhyConfig_RemoteLoopback:
        {
            _tsce_phymod_phy_t_init(pc, &pm_phy);
            rv = tsce_phy_loopback_get(&pm_phy, phymodLoopbackRemotePMD, val);
            return rv;
        }
#if PHY_CONFIG_INCLUDE_XAUI_TX_LANE_MAP_SET
    case PhyConfig_XauiTxLaneRemap:
        {
            phymod_lane_map_t lane_map;

            _tsce_phymod_core_t_init(pc, &pm_core);
            rv = tsce_core_lane_map_get(&pm_core, &lane_map);
            if (CDK_SUCCESS(rv)) {
                *val = 0;
                for (idx = 0; idx < NUM_LANES; idx++) {
                    *val += (lane_map.lane_map_tx[idx] << (idx * 4 + 16));
                }
            }
            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_XAUI_RX_LANE_MAP_SET
    case PhyConfig_XauiRxLaneRemap:
        {
            phymod_lane_map_t lane_map;

            _tsce_phymod_core_t_init(pc, &pm_core);
            rv = tsce_core_lane_map_get(&pm_core, &lane_map);
            if (CDK_SUCCESS(rv)) {
                *val = 0;
                for (idx = 0; idx < NUM_LANES; idx++) {
                    *val += (lane_map.lane_map_rx[idx] << (idx * 4));
                }
            }
            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_XAUI_TX_POLARITY_SET
    case PhyConfig_XauiTxPolInvert:
        {
            phymod_polarity_t polarity;

            _tsce_phymod_phy_t_init(pc, &pm_phy);
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_polarity_get(&pm_phy, &polarity);
            }
            *val = polarity.tx_polarity;

            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_XAUI_RX_POLARITY_SET
    case PhyConfig_XauiRxPolInvert:
        {
            phymod_polarity_t polarity;

            _tsce_phymod_phy_t_init(pc, &pm_phy);
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_polarity_get(&pm_phy, &polarity);
            }
            *val = polarity.rx_polarity;

            return rv;
        }
#endif
#if PHY_CONFIG_INCLUDE_LINK_ABILITIES
    case PhyConfig_AdvLocal:
        rv = _tsce_ability_get(pc, val);
        return rv;
#endif
    case PhyConfig_EEE:
        {
            _tsce_phymod_phy_t_init(pc, &pm_phy);
            if (CDK_SUCCESS(rv)) {
                rv = tsce_phy_eee_get(&pm_phy, val);
            }
            return rv;
        }
    case PhyConfig_TxPreemp:
    case PhyConfig_TxPost2:
    case PhyConfig_TxIDrv:
    case PhyConfig_TxPreIDrv:
    case PhyConfig_InitStage:
        return CDK_E_NONE;
    case PhyConfig_MultiCoreLaneMap:
        *val = MULTI_CORE_LANE_MAP_GET(pc);
        return rv;
    default:
        break;
    }

    return CDK_E_UNAVAIL;
}

/*
 * Function:
 *      bcmi_tsce_xgxs_status_get
 * Purpose:
 *      Get PHY status value.
 * Parameters:
 *      pc - PHY control structure
 *      stat - status parameter
 *      val - (OUT) status value
 * Returns:
 *      CDK_E_xxx
 */
static int
bcmi_tsce_xgxs_status_get(phy_ctrl_t *pc, phy_status_t stat, uint32_t *val)
{
    int ioerr = 0;
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;
    phymod_ref_clk_t ref_clock;

    PHY_CTRL_CHECK(pc);

    switch (stat) {
    case PhyStatus_LineInterface:
        {
            phymod_phy_inf_config_t interface_config;

            CDK_MEMSET(&interface_config, 0x0, sizeof(interface_config));

            _tsce_phymod_phy_t_init(pc, &pm_phy);
            ioerr = _tsce_ref_clk_convert(PORT_REFCLK_INT, &ref_clock);
            if (CDK_SUCCESS(ioerr)) {
                ioerr = tsce_phy_interface_config_get(&pm_phy, 0, ref_clock,
                                                   &interface_config);
            }
            if (CDK_FAILURE(ioerr)) {
                return CDK_E_IO;
            }
            switch (interface_config.interface_type) {
            case phymodInterfaceXFI:
                *val = PHY_IF_XFI;
                break;
            case phymodInterfaceSFI:
                *val = PHY_IF_SFI;
                break;
            case phymodInterfaceXLAUI:
                *val = PHY_IF_XLAUI;
                break;
            case phymodInterface1000X:
                *val = PHY_IF_GMII;
                break;
            case phymodInterfaceKX:
                *val = PHY_IF_KX;
                break;
            case phymodInterfaceSGMII:
                *val = PHY_IF_SGMII;
                break;
            case phymodInterfaceRXAUI:
                *val = PHY_IF_RXAUI;
                break;
            case phymodInterfaceXGMII:
                *val = PHY_IF_XGMII;
                break;
            case phymodInterfaceKR4:
                *val = PHY_IF_KR;
                break;
            case phymodInterfaceKR:
            {
                int an = 0;
                bcmi_tsce_xgxs_autoneg_get(pc, &an);
                if (an) {
                    *val = PHY_IF_KR;
                } else {
                    if (PHY_CTRL_FLAGS(pc) & PHY_F_FIBER_MODE) {
                        *val = PHY_IF_SFI;
                    } else {
                        *val = PHY_IF_XFI;
                    }
                }
                break;
            }
            case phymodInterfaceCR4:
            case phymodInterfaceCR10:
                *val = PHY_IF_CR;
                break;
            default:
                *val = PHY_IF_XGMII;
                break;
            }
            return rv;
        }
    default:
        break;
    }
    return rv;
}

static int
bcmi_tsce_xgxs_reg_read(phy_ctrl_t *pc, uint32_t addr,  uint32_t *val)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;

    _tsce_phymod_phy_t_init(pc, &pm_phy);
    rv = tsce_phy_reg_read(&pm_phy, addr,  val);
    return rv;
}

static int
bcmi_tsce_xgxs_reg_write(phy_ctrl_t *pc, uint32_t addr, uint32_t val)
{
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;

    _tsce_phymod_phy_t_init(pc, &pm_phy);
    rv = tsce_phy_reg_write(&pm_phy, addr, val);
    return rv;
}

static int
bcmi_tsce_display_diag(phy_ctrl_t *pc, uint32_t diag_level) {
    int rv = CDK_E_NONE;
    phymod_phy_access_t pm_phy;

    _tsce_phymod_phy_t_init(pc, &pm_phy);
    rv = tsce_phy_display_diag(&pm_phy, diag_level);
    return rv;
}

/* Public PHY Driver Structure */
phy_driver_t bcmi_tsce_xgxs_drv = {
    "eagle",
    "Internal TSC Eagle 40G SerDes PHY Driver",
    PHY_DRIVER_F_INTERNAL,
    bcmi_tsce_xgxs_probe,                /* pd_probe */
    bcmi_tsce_xgxs_notify,               /* pd_notify */
    bcmi_tsce_xgxs_reset,                /* pd_reset */
    bcmi_tsce_xgxs_init,                 /* pd_init */
    bcmi_tsce_xgxs_link_get,             /* pd_link_get */
    bcmi_tsce_xgxs_duplex_set,           /* pd_duplex_set */
    bcmi_tsce_xgxs_duplex_get,           /* pd_duplex_get */
    bcmi_tsce_xgxs_speed_set,            /* pd_speed_set */
    bcmi_tsce_xgxs_speed_get,            /* pd_speed_get */
    bcmi_tsce_xgxs_autoneg_set,          /* pd_autoneg_set */
    bcmi_tsce_xgxs_autoneg_get,          /* pd_autoneg_get */
    bcmi_tsce_xgxs_loopback_set,         /* pd_loopback_set */
    bcmi_tsce_xgxs_loopback_get,         /* pd_loopback_get */
    bcmi_tsce_xgxs_ability_get,          /* pd_ability_get */
    bcmi_tsce_xgxs_config_set,           /* pd_config_set */
    bcmi_tsce_xgxs_config_get,           /* pd_config_get */
    bcmi_tsce_xgxs_status_get,           /* pd_status_get */
    NULL,                                /* pd_cable_diag */
    bcmi_tsce_xgxs_reg_read,             /* pd_reg_read */
    bcmi_tsce_xgxs_reg_write,            /* pd_reg_write */
    bcmi_tsce_display_diag               /* pd_display_diag */
};

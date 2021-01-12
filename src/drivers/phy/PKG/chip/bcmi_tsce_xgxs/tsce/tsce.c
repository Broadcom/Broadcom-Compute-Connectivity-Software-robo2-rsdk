/*
 *
 * $Id: tsce.c,v 1.2.2.26 2013/09/16 12:23:15 nirf Exp $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright:
 * All Rights Reserved.$
 *
 *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#include "tsce.h"
#include "eagle_common.h"
#include "temod_enum_defines.h"
#include "temod.h"
#include "temod_device.h"
#include "temod_sc_lkup_table.h"
#include "tePCSRegEnums.h"
#include "eagle_cfg_seq.h"
#include "eagle_tsc_common.h"
#include "eagle_tsc_fields.h"
#include "eagle_tsc_interface.h"
#include "eagle_tsc_dependencies.h"
#include "eagle_tsc_debug_functions.h"

#define EAGLE_PHY_ID0        0
#define EAGLE_PHY_ID1        0
#define EAGLE_PHY_PHY_ALL_LANES 0x1
#define EAGLE_PHY_PMD_CRC_UCODE 1
#define TSCE4_MODEL     0x12
#define TSCE12_MODEL    0x13

#define EAGLE_MODEL     0x1a
#define TSCE_REV_MASK   0x0
#define TSCE_NOF_DFES (5)
#define TSCE_NOF_LANES_IN_CORE (4)
#define TSCE_LANE_SWAP_LANE_MASK (0x3)
#define TSCE_PHY_ALL_LANES (0xf)

#define TSCE_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCE_PHY_ALL_LANES; \
    }while(0)

#define TSCE_MAX_FIRMWARES (5)

#define TSCE_USERSPEED_SELECT(_phy, _config, _type)  \
    ((_config)->mode==phymodPcsUserSpeedModeST) ?                       \
      temod_st_control_field_set(&(_phy)->access, (_config)->current_entry, _type, (_config)->value) : \
        temod_override_set(&(_phy)->access, _type, (_config)->value)

#define TSCE_USERSPEED_CREDIT_SELECT(_phy, _config, _type)  \
    ((_config)->mode==phymodPcsUserSpeedModeST) ?                       \
      temod_st_credit_field_set(&(_phy)->access, (_config)->current_entry, _type, (_config)->value) : \
        temod_credit_override_set(&(_phy)->access, _type, (_config)->value)

#define TSCE_PMD_CRC_UCODE  1
/* uController's firmware */
extern unsigned char tsce_ucode[];
extern unsigned short tsce_ucode_ver;
extern unsigned short tsce_ucode_crc;
extern unsigned short tsce_ucode_len;

typedef int (*sequncer_control_f)(const phymod_access_t* core, uint32_t enable);
typedef int (*rx_DFE_tap_control_set_f)(const phymod_access_t* phy, uint32_t val);

STATIC
int _tsce_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    const phymod_access_t *pm_acc = &phy->access;

    serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
    serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
    serdes_firmware_config.field.dfe_on            = fw_config.DfeOn;
    serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
    serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
    serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
    serdes_firmware_config.field.media_type        = fw_config.MediaType;
    serdes_firmware_config.field.cl72_auto_polarity_en   = fw_config.Cl72AutoPolEn;
    serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;

    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_lane_cfg(pm_acc, serdes_firmware_config));
    return PHYMOD_E_NONE;
}

int tsce_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    PHYID2r_t id2;
    PHYID3r_t id3;
    DIG_REVID0r_t revid;
    int rv ;
    *is_identified = 0;

    if(core_id == 0){
        ioerr += READ_PHYID2r(pm_acc, &id2);
        ioerr += READ_PHYID3r(pm_acc, &id3);
    } else {
        PHYID2r_SET(id2, ((core_id >> 16) & 0xffff));
        PHYID3r_SET(id3, core_id & 0xffff);
    }
    if ((PHYID2r_GET(id2) == EAGLE_PHY_ID0) &&
        (PHYID3r_GET(id3) &= ~TSCE_REV_MASK) == EAGLE_PHY_ID1) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_DIG_REVID0r(pm_acc, &revid);
        if (DIG_REVID0r_REVID_MODELf_GET(revid) == EAGLE_MODEL) {
            *is_identified = 1;
        }
    }
    rv = ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
    return rv ;
}


int tsce_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t serdes_id;
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &phy->access;
    PHYMOD_IF_ERR_RETURN
        (temod_revid_read(&phy->access, &serdes_id));
    info->serdes_id = serdes_id;
    if ((serdes_id & 0x3f) == TSCE4_MODEL) {
        info->core_version = phymodCoreVersionTsce4A0;
        PHYMOD_STRNCPY(info->name, "Tsce4A0", PHYMOD_STRLEN("Tsce4A0")+1);
    } else {
        info->core_version = phymodCoreVersionTsce12A0;
        PHYMOD_STRNCPY(info->name, "Tsce4A0", PHYMOD_STRLEN("Tsce12A0")+1);
    }
    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];
    return PHYMOD_E_NONE;
}


/*
 * set lane swapping for core
 * The tx swap is composed of PCS swap and after that the PMD swap.
 * The rx swap is composed just by PCS swap
 *
 * lane_map_tx and lane_map_rx[lane=logic_lane] are logic-lane base.
 * pcs_swap and register is logic_lane base.
 * but pmd_tx_map and addr_index_swap (and registers) are physical lane base
 */

int tsce_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    phymod_phy_access_t phy_access;
    uint32_t lane, pmd_tx_map =0;

    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        pmd_tx_map |= ((lane_map->lane_map_tx[lane]) & 3) << (lane*4);
    }

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_IF_ERR_RETURN
            (temod_pmd_lane_swap(&phy_access.access, pmd_tx_map));
    return PHYMOD_E_NONE;
}


int tsce_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    uint32_t pmd_swap = 0 , pcs_swap = 0, lane;
    PHYMOD_IF_ERR_RETURN(temod_pcs_lane_swap_get(&core->access, &pcs_swap));
    PHYMOD_IF_ERR_RETURN(temod_pmd_lane_swap_tx_get(&core->access, &pmd_swap));
    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        /* deccode each lane from four bits */
        lane_map->lane_map_rx[lane] = (pcs_swap>>(lane*4)) & TSCE_LANE_SWAP_LANE_MASK;
        /* considering the pcs lane swap: tx_map[lane] = pmd_map[pcs_map[lane]] */
        lane_map->lane_map_tx[lane] = (pmd_swap>>(lane_map->lane_map_rx[lane]*4)) & TSCE_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = TSCE_NOF_LANES_IN_CORE;
    return PHYMOD_E_NONE;
}


int tsce_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{

    /* Place your code here */


    return PHYMOD_E_NONE;
}


int tsce_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;
}

int tsce_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    /* FIXME: read from HW instead */
    /*
     * It's O.K to use this code as is since the firmware CRC is already checked at
     * at the time we load it.
     */
    fw_info->fw_crc = tsce_ucode_crc;
    fw_info->fw_version = tsce_ucode_ver;
    return PHYMOD_E_NONE;
}

/* load tsce fw. the fw_loader parameter is valid just for external fw load */
STATIC
int _tsce_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{
#ifndef TSCE_PMD_CRC_UCODE
    phymod_core_firmware_info_t actual_fw;
#endif

#ifdef VENDOR_BROADCOM
    const phymod_access_t *pm_acc;
    pm_acc = &core->access;
    PHYMOD_VDBG(DBG_CFG, pm_acc,
       ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32" load_meth=%0d",
        __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask, (int)load_method)) ;
#endif

    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_mdio_load(&core->access, tsce_ucode, tsce_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader);

        eagle_pram_flop_set(&core->access, 0x0);

        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_init(&core->access));

        PHYMOD_IF_ERR_RETURN
            (temod_pram_abl_enable_set(&core->access,1));
        PHYMOD_IF_ERR_RETURN
            (eagle_pram_firmware_enable(&core->access, 1));

        PHYMOD_IF_ERR_RETURN(fw_loader(core, tsce_ucode_len, tsce_ucode));

        PHYMOD_IF_ERR_RETURN
            (eagle_pram_firmware_enable(&core->access, 0));
        PHYMOD_IF_ERR_RETURN
            (temod_pram_abl_enable_set(&core->access,0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}


int tsce_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE;
}


int tsce_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_IF_ERR_RETURN
         (_tsce_phy_firmware_lane_config_set(phy, fw_config));
    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
        (temod_trigger_speed_change(&phy->access));

    return PHYMOD_E_NONE;
}


int tsce_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;
    fw_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_config->MediaType         = serdes_firmware_config.field.media_type;
    fw_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;

    return PHYMOD_E_NONE;
}

/* reset pll sequencer
 * flags - unused parameter
 */
int tsce_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    switch (operation) {
        case phymodSeqOpStop:
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 0));
        break;
        case phymodSeqOpStart:
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 1));
            /* FIXME, need to fill out the timeout value properly */
            /* PHYMOD_IF_ERR_RETURN
                (temod_pll_lock_wait(&core->access, 250000)); */
        break;
        case phymodSeqOpRestart:
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 0));
            /* FIXME delay is needed */
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 1));
            /* FIXME, need to fill out the timeout value properly */
            /* PHYMOD_IF_ERR_RETURN
                (temod_pll_lock_wait(&core->access, 250000)); */
        break;
        default:
            return PHYMOD_E_UNAVAIL;
        break;
    }
    return PHYMOD_E_NONE;
}

int tsce_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
    switch(event){
    case phymodCoreEventPllLock:
        /* PHYMOD_IF_ERR_RETURN(temod_pll_lock_wait(&core->access, timeout)); */
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal wait event %u"), event));
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_pll_multiplier_get(const phymod_phy_access_t* core, uint32_t* pll_multiplier)
{
    return PHYMOD_E_NONE;
}


/* reset rx sequencer
 * flags - unused parameter
 */
int tsce_phy_rx_restart(const phymod_phy_access_t* phy)
{
    PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_restart(&phy->access, 1));
    return PHYMOD_E_NONE;
}


int tsce_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    PHYMOD_IF_ERR_RETURN
        (temod_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity));
    return PHYMOD_E_NONE;
}


int tsce_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
   PHYMOD_IF_ERR_RETURN
        (temod_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));
    return PHYMOD_E_NONE;
}


int tsce_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    /* FIXME first need to call the validate routine to check */
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_PRE, tx->pre));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_MAIN, tx->main));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST1, tx->post));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST2, tx->post2));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST3, tx->post3));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_AMP,  tx->amp));

    return PHYMOD_E_NONE;
}

int tsce_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_PRE, &tx->pre));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_MAIN, &tx->main));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST1, &tx->post));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST2, &tx->post2));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST3, &tx->post3));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_AMP, &tx->amp));
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_tx_pi_freq_override(&phy->access,
                                    tx_override->phase_interpolator.enable,
                                    tx_override->phase_interpolator.value));
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
/*
    PHYMOD_IF_ERR_RETURN
        (temod_tx_pi_control_get(&phy->access, &tx_override->phase_interpolator.value));
*/
    return PHYMOD_E_NONE;
}


int tsce_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint32_t i;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, k;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* params check */
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps < TSCE_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), (unsigned int)rx->num_of_dfe_taps));
    }

    for (k = 0; k < num_lane; k++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + k);

        /*vga set*/
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_VGA, rx->vga.value));
        /* dfe set */
        for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
            switch (i) {
                case 0:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE1, rx->dfe[i].value));
                    break;
                case 1:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE2, rx->dfe[i].value));
                    break;
                case 2:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE3, rx->dfe[i].value));
                    break;
                case 3:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE4, rx->dfe[i].value));
                    break;
                case 4:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE5, rx->dfe[i].value));
                    break;
                default:
                    return PHYMOD_E_PARAM;
            }
        }
        /*peaking filter set*/
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF, rx->peaking_filter.value));

        /* low freq peak filter */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF2, (int8_t)rx->low_freq_peaking_filter.value));

    }
    return PHYMOD_E_NONE;
}


int tsce_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    int8_t tmpData;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_PF, &tmpData));
    rx->peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_PF2, &tmpData));
    rx->low_freq_peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_VGA,&tmpData));
    rx->vga.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE1,&tmpData));
    rx->dfe[0].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE2,&tmpData));
    rx->dfe[1].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE3, &tmpData));
    rx->dfe[2].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE4,&tmpData));
    rx->dfe[3].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE5,&tmpData));
    rx->dfe[4].value = tmpData;

    rx->num_of_dfe_taps = 5;
    rx->dfe[0].enable = 1;
    rx->dfe[1].enable = 1;
    rx->dfe[2].enable = 1;
    rx->dfe[3].enable = 1;
    rx->dfe[4].enable = 1;
    rx->vga.enable = 1;
    rx->low_freq_peaking_filter.enable = 1;
    rx->peaking_filter.enable = 1;

    return PHYMOD_E_NONE;
}


int tsce_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{



    /* Place your code here */


    return PHYMOD_E_NONE;


}


int tsce_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{



    /* Place your code here */


    return PHYMOD_E_NONE;


}


int tsce_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_port_enable_set(&pm_phy_copy.access, 0));
        }
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_port_enable_set(&pm_phy_copy.access, 1));
        }
    }
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
            /* disable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&phy->access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
            /* enable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&phy->access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
            /* disable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&phy->access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
            /* enable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&phy->access, 0));
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    /* Commented the following line. Because if in PMD loopback mode, we squelch the
           xmit, and we should still see the correct port status */
    /* PHYMOD_IF_ERR_RETURN(temod_tx_squelch_get(&pm_phy_copy.access, &enable)); */
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(temod_tx_squelch_set(&phy->access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(temod_tx_squelch_set(&phy->access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    int enable, reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod_tx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    if(enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
        if (!reset) {
            *tx_control = phymodTxReset;
        } else if (!tx_lane) {
            *tx_control = phymodTxTrafficDisable;
        } else {
            *tx_control = phymodTxTrafficEnable;
        }
    }
    return PHYMOD_E_NONE;
}


int tsce_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    switch (rx_control) {
    case phymodRxReset:
        PHYMOD_IF_ERR_RETURN(temod_rx_lane_control_set(&phy->access, 1));
        break;
    case phymodRxSquelchOn:
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodRxSquelchOff:
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&pm_phy_copy.access, 0));
        }
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int enable, reset;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod_rx_squelch_get(&pm_phy_copy.access, &enable));
    /* next check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if(enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(temod_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    PHYMOD_IF_ERR_RETURN(temod_fecmode_set(&phy->access, enable));
    return PHYMOD_E_NONE;

}

int tsce_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(temod_fecmode_get(&phy->access, enable));
    return PHYMOD_E_NONE;

}

#ifdef SPEED_FROM_REFCLK
STATIC
int _tsce_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
    case EAGLE_TSC_pll_div_46x:
        *pll_multiplier = 460;
        break;
    case EAGLE_TSC_pll_div_72x:
        *pll_multiplier = 720;
        break;
    case EAGLE_TSC_pll_div_40x:
        *pll_multiplier = 400;
        break;
    case EAGLE_TSC_pll_div_42x:
        *pll_multiplier = 420;
        break;
/*
    case 0x4:
        *pll_multiplier = 580;
        break;
*/
    case EAGLE_TSC_pll_div_50x:
        *pll_multiplier = 500;
        break;
    case EAGLE_TSC_pll_div_52x:
        *pll_multiplier = 520;
        break;
/*
    case 0x7:
        *pll_multiplier = 540;
        break;
*/
    case EAGLE_TSC_pll_div_60x:
        *pll_multiplier = 600;
        break;
    case EAGLE_TSC_pll_div_64x:
        *pll_multiplier = 640;
        break;
    case EAGLE_TSC_pll_div_66x:
        *pll_multiplier = 660;
        break;
    case EAGLE_TSC_pll_div_68x:
        *pll_multiplier = 680;
        break;
    case EAGLE_TSC_pll_div_70x:
        *pll_multiplier = 700;
        break;
    case EAGLE_TSC_pll_div_80x:
        *pll_multiplier = 800;
        break;
    case EAGLE_TSC_pll_div_92x:
        *pll_multiplier = 920;
        break;
    case EAGLE_TSC_pll_div_100x:
        *pll_multiplier = 1000;
        break;
    case EAGLE_TSC_pll_div_82p5x:
        *pll_multiplier = 825;
        break;
    case EAGLE_TSC_pll_div_87p5x:
        *pll_multiplier = 875;
        break;
    case EAGLE_TSC_pll_div_73p6x:
        *pll_multiplier = 736;
        break;
    case EAGLE_TSC_pll_div_36p8x:
        *pll_multiplier = 368;
        break;
/*
    case EAGLE_TSC_pll_div_199p04x:
        *pll_multiplier = 1990.4;
        break;
*/
    default:
        *pll_multiplier = 660;
        break;
    }
    return PHYMOD_E_NONE;
}
#endif
#if INCLUDE_UNUSED
STATIC
int _tsce_st_hto_interface_config_set(const temod_device_aux_modes_t *device, int lane_num, int speed_vec, uint32_t *new_pll_div, int16_t *os_mode)
{
    int i, max_lane=4 ;
    *os_mode = -1 ;
    /* ST */
    for(i=0;i<max_lane; i++) {
        if(device->st_hcd[i]==speed_vec){
            *new_pll_div = device->st_pll_div[i];
            *os_mode     = device->st_os[i] ;
            break ;
        }
    }
    /* HTO */
    if(device->hto_enable[lane_num]){
        *new_pll_div = device->hto_pll_div[lane_num];
        *os_mode     = device->hto_os[lane_num] ;
    }
    return PHYMOD_E_NONE;
}
#endif

/*FIXME : Take care of HiGig settings for HiGig Speeds, currently just fixed for 5000_XFI and 12.773G */
int tsce_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
     phymod_phy_access_t pm_phy_copy;
     temod_spd_intfc_type spd_intf = TEMOD_SPD_ILLEGAL;

     /*  Core and Lane Configuration for Firmware */
     phymod_firmware_lane_config_t firmware_lane_config;
     PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
     PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
     pm_phy_copy.access.lane_mask = 0x1 ;

     /* Keep the lane in reset */
    PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_release(&pm_phy_copy.access, 0));
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    firmware_lane_config.DfeOn = 0;
#if 0
    if( (config->data_rate >= 10000) ) {
        firmware_lane_config.DfeOn = 0;
    } else {
        firmware_lane_config.DfeOn = 0;
    }
#endif
    firmware_lane_config.Cl72RestTO = 0;
    firmware_lane_config.MediaType = 0;
    if (config->interface_modes == PHYMOD_INTF_MODES_COPPER) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
        firmware_lane_config.DfeOn = 1;
    }
    spd_intf = TEMOD_SPD_10G_FEC_OFF;

    if  (config->interface_type == phymodInterfaceSGMII) {
        if ( config->interface_modes == 1 ) {
            spd_intf = TEMOD_SPD_37AN_SGMII_S_10M_100M_1G;
        } else {
            switch (config->data_rate) {
                case 10:
                    spd_intf = TEMOD_SPD_37AN_SGMII_M_10M;
                    break;
                case 100:
                    spd_intf = TEMOD_SPD_37AN_SGMII_M_100M;
                    break;
                case 1000:
                    spd_intf = TEMOD_SPD_37AN_SGMII_M_1G;
                    break;
                default:
                    spd_intf = TEMOD_SPD_37AN_SGMII_M_1G;
                    break;
           }
       }
   } else if (config->interface_type == phymodInterfaceKR) {
       firmware_lane_config.LaneConfigFromPCS = 1;
       switch (config->interface_modes) {
           case 0:
               spd_intf = TEMOD_SPD_73AN_IEEE_KR_10G_FEC_ABL;
               break;
           case 1:
               spd_intf = TEMOD_SPD_73AN_IEEE_KR_10G_FEC_REQ;
               break;
           case 2:
               spd_intf = TEMOD_SPD_73AN_USER_KR_10G_FEC_ABL;
               break;
           case 3:
               spd_intf = TEMOD_SPD_73AN_USER_KR_10G_FEC_REQ;
               break;
           default:
               spd_intf = TEMOD_SPD_73AN_IEEE_KR_10G_FEC_ABL;
               break;
       }
   } else if (config->interface_type == phymodInterfaceKX) {
       switch (config->interface_modes) {
           case 0:
               spd_intf = TEMOD_SPD_73AN_IEEE_KX_1G;
               break;
           case 1:
               spd_intf = TEMOD_SPD_73AN_USER_KX_1G;
               break;
           case 2:
               spd_intf = TEMOD_SPD_37AN_IEEE_KX_1G;
               break;
           case 3:
               spd_intf = TEMOD_SPD_37AN_USER_KX_1G;
               break;
           default:
               spd_intf = TEMOD_SPD_73AN_IEEE_KX_1G;
               break;
        }

    } else if (config->interface_type == phymodInterfaceXFI) {
        switch (config->data_rate) {
            case 10:
                spd_intf = TEMOD_SPD_10M;
                break;
            case 100:
                spd_intf = TEMOD_SPD_100M;
                break;
            case 1000:
                spd_intf = TEMOD_SPD_1G;
                break;
            case 2500:
                spd_intf = TEMOD_SPD_2p5G;
                break;
            case 5000:
                spd_intf = TEMOD_SPD_5G;
                break;
            case 10312:
            case 10000:
                if (config->interface_modes > 0 ) {
                  spd_intf = TEMOD_SPD_10G_FEC_ON;
                } else {
                  spd_intf = TEMOD_SPD_10G_FEC_OFF;
                }
                break;
            default:
                spd_intf = TEMOD_SPD_10G_FEC_OFF;
                break;

        }
        if (config->interface_modes == 1) {
           spd_intf = TEMOD_SPD_37AN_USER_KX_1G;
        }
    }

    PHYMOD_IF_ERR_RETURN(_tsce_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    PHYMOD_IF_ERR_RETURN(eagle_phy_change_speed_reset(&pm_phy_copy.access));
    PHYMOD_IF_ERR_RETURN(tsce_phy_pcs_speed_set(&pm_phy_copy.access,spd_intf));
#ifdef DEBUG
    PHYMOD_IF_ERR_RETURN (eagle_tsc_display_lane_config(&pm_phy_copy.access));
#endif
    PHYMOD_IF_ERR_RETURN(eagle_phy_data_path_reset_release(&pm_phy_copy.access, 1));
    return PHYMOD_E_NONE;
}

/* flags- unused parameter */
int tsce_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    /* uint32_t pll_div; */
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t   data_rate = 0;
#ifdef SPEED_FROM_REFCLK
    int osr_mode;
    enum eagle_tsc_pll_enum pll_div;
    uint32_t pll_multiplier, refclk;
    uint32_t actual_osr, actual_osr_rem;
    phymod_osr_mode_t osr_mode_enum;
#endif
    config->ref_clock = ref_clock;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << start_lane;

#ifdef SPEED_FROM_REFCLK
    PHYMOD_IF_ERR_RETURN
        (eagle_osr_mode_get(&phy->access, &osr_mode));

    PHYMOD_IF_ERR_RETURN
        (eagle_pll_config_get(&phy_copy.access, &pll_div));
    PHYMOD_IF_ERR_RETURN
        (_tsce_pll_multiplier_get(pll_div, &pll_multiplier));


    if (ref_clock == phymodRefClk156Mhz) {
        refclk = 15625;
    } else if (ref_clock == phymodRefClk125Mhz) {
        refclk = 12500;
    } else {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL,  (_PHYMOD_MSG("Unknown refclk")));
    }

    PHYMOD_IF_ERR_RETURN(eagle_osr_mode_to_enum(osr_mode, &osr_mode_enum));
    PHYMOD_IF_ERR_RETURN(phymod_osr_mode_to_actual_os(osr_mode_enum, &actual_osr, &actual_osr_rem));

  /*  Divide by 1000 is take care of pll_mulitple(by 10) and ref_clk (by 100) */
    data_rate = (refclk*pll_multiplier)/(1000*actual_osr);
#else
    PHYMOD_IF_ERR_RETURN(tsce_phy_pcs_speed_get(&phy_copy.access, &data_rate));
#endif

    switch(data_rate) {
        case 10312:
        case 0xf :
            config->data_rate = 10000;
            break;
        case 3437:
        case 3:
            config->data_rate = 2500;
            break;
        case 1289:
        case 2:
            config->data_rate = 1000;
            break;
        case 100:
        case 1:
            config->data_rate = 100;
            break;
        case 10:
        case 0:
            config->data_rate = 10;
            break;
        default:
            break;
    }
    config->interface_type = phymodInterfaceBypass;

#ifdef SPEED_FROM_REFCLK
    if(osr_mode_enum == phymodOversampleMode2) {
        PHYMOD_INTF_MODES_OS2_SET(config);
    }
#endif
    return PHYMOD_E_NONE;
}


int tsce_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
#ifdef VENDOR_BROADCOM
    const phymod_access_t *pm_acc;
    pm_acc = &phy->access;
    PHYMOD_VDBG(DBG_CL72, pm_acc, ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32" cl72_en=%d\n",
                __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask, (int)cl72_en));
#endif

    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));

    if(serdes_firmware_config.field.dfe_on == 0) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (temod_clause72_control(&phy->access, cl72_en));
    return PHYMOD_E_NONE;
}

int tsce_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    uint32_t local_en;;
    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_cl72_enable_get(&phy->access, &local_en));
    *cl72_en = local_en;
    return PHYMOD_E_NONE;
}


int tsce_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint32_t local_status;
    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_cl72_receiver_status(&phy->access, &local_status));
    status->locked = local_status;
    return PHYMOD_E_NONE;
}

int tsce_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (temod_autoneg_set) \n"));
    return PHYMOD_E_NONE;

}

int tsce_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type){
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (temod_autoneg_get) \n"));
    return PHYMOD_E_NONE;
}
int tsce_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
#if 0

    temod_an_ability_t value;
    temod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int an_complete = 0;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));

    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_remote_ability_get(&phy_copy.access, &value));
    an_ability_get_type->an_cl72 = value.cl37_adv.an_cl72 | value.cl73_adv.an_cl72;
    an_ability_get_type->an_hg2 = value.cl37_adv.an_hg2;
    an_ability_get_type->an_fec = value.cl37_adv.an_fec | value.cl73_adv.an_fec;
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    if (an_control.an_type == TEMOD_AN_MODE_CL73 || an_control.an_type == TEMOD_AN_MODE_CL73BAM) {
      if (value.cl73_adv.an_pause == TEMOD_ASYM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl73_adv.an_pause == TEMOD_SYMM_PAUSE) {
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl73_adv.an_pause == TEMOD_ASYM_SYMM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      }
    } else {
      if (value.cl37_adv.an_pause == TEMOD_ASYM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl37_adv.an_pause == TEMOD_SYMM_PAUSE) {
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl37_adv.an_pause == TEMOD_ASYM_SYMM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      }
    }

    /* get the cl37 sgmii speed */
    switch (value.cl37_adv.cl37_sgmii_speed) {
    case TEMOD_CL37_SGMII_10M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
        break;
    case TEMOD_CL37_SGMII_100M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
        break;
    case TEMOD_CL37_SGMII_1000M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
        break;
    default:
        break;
    }

    /* first check cl73 ability */
    if (value.cl73_adv.an_base_speed &  1 << TEMOD_CL73_100GBASE_CR10)
        PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_40GBASE_CR4)
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_40GBASE_KR4)
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_10GBASE_KR)
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_10GBASE_KX4)
        PHYMOD_AN_CAP_10G_KX4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_1000BASE_KX)
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);

    /* next check cl73 bam ability */
    if (value.cl73_adv.an_bam_speed & 1 << TEMOD_CL73_BAM_20GBASE_KR2)
        PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.cl73_adv.an_bam_speed & 1 << TEMOD_CL73_BAM_20GBASE_CR2)
        PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_ability_get_type->cl73bam_cap);

    /* check cl37 bam ability */
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_2p5GBASE_X)
        PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_5GBASE_X4)
        PHYMOD_BAM_CL37_CAP_5G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_6GBASE_X4)
        PHYMOD_BAM_CL37_CAP_6G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X4)
        PHYMOD_BAM_CL37_CAP_10G_HIGIG_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X4_CX4)
        PHYMOD_BAM_CL37_CAP_10G_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X2)
        PHYMOD_BAM_CL37_CAP_10G_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X2_CX4)
        PHYMOD_BAM_CL37_CAP_10G_X2_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_BAM_10p5GBASE_X2)
        PHYMOD_BAM_CL37_CAP_10P5G_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12GBASE_X4)
        PHYMOD_BAM_CL37_CAP_12G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12p5GBASE_X4)
        PHYMOD_BAM_CL37_CAP_12P5_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12p7GBASE_X2)
        PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_ability_get_type->cl37bam_cap);

    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_13GBASE_X4)
        PHYMOD_BAM_CL37_CAP_13G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_15GBASE_X4)
        PHYMOD_BAM_CL37_CAP_15G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_15p75GBASE_X2)
        PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_16GBASE_X4)
        PHYMOD_BAM_CL37_CAP_16G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X4_CX4)
        PHYMOD_BAM_CL37_CAP_20G_X4_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X4)
        PHYMOD_BAM_CL37_CAP_20G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X2)
        PHYMOD_BAM_CL37_CAP_20G_X2_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X2_CX4)
        PHYMOD_BAM_CL37_CAP_20G_X2_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_21GBASE_X4)
        PHYMOD_BAM_CL37_CAP_21G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_25p455GBASE_X4)
        PHYMOD_BAM_CL37_CAP_25P455G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_31p5GBASE_X4)
        PHYMOD_BAM_CL37_CAP_31P5G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_32p7GBASE_X4)
        PHYMOD_BAM_CL37_CAP_32P7G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_40GBASE_X4)
        PHYMOD_BAM_CL37_CAP_40G_SET(an_ability_get_type->cl37bam_cap);

#endif
    return PHYMOD_E_NONE;
}



int tsce_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{

    return PHYMOD_E_NONE;

}

int tsce_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return eagle_pmd_an_get(&(phy->access), an, an_done);
}


int tsce_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (temod_autoneg_get) \n"));
    return PHYMOD_E_NONE;
}


STATIC
int _tsce_core_init_pass1(const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    uint32_t  uc_active = 0 ;

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN(eagle_uc_active_get(&core_copy.access, &uc_active)) ;
    if(uc_active) {
        return(PHYMOD_E_NONE);
    }
    /* Isolate Core and Lane Control Pins */
    PHYMOD_IF_ERR_RETURN(eagle_tsc_isolate_core_ctrl_pins(&core_copy.access, 1));
    PHYMOD_IF_ERR_RETURN(eagle_tsc_isolate_lane_ctrl_pins(&core_copy.access, 1));
    /* Soft Reset */
    PHYMOD_IF_ERR_RETURN(eagle_core_soft_reset_release(&core_copy.access, 0));
    PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_release(&core_copy.access, 0));
    PHYMOD_IF_ERR_RETURN(eagle_core_soft_reset_release(&core_copy.access, 1));
    PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_release(&core_copy.access, 1));


    /* Data Path Reset */
    PHYMOD_IF_ERR_RETURN(eagle_phy_data_path_reset_release(&core_copy.access, 0));

    /* need to set the heart beat default is for 156.25M */
    PHYMOD_IF_ERR_RETURN (temod_refclk_set(&core_copy.access,
                (init_config->interface.ref_clock == phymodRefClk125Mhz) ?
                              TEMODREFCLK125MHZ : TEMODREFCLK156MHZ));

    if (_tsce_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader)) {
            PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
            PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
    return (PHYMOD_E_NONE);
}

STATIC
int _tsce_core_init_pass2(const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

#ifndef TSCE_PMD_CRC_UCODE
    /* next we need to check if the load is correct or not */
    if (eagle_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) &tsce_ucode, tsce_ucode_len)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
#endif /* TSCE_PMD_CRC_UCODE */
    /* next we need to set the uc active and release uc */
    PHYMOD_IF_ERR_RETURN
        (eagle_uc_active_set(&core_copy.access ,1));

    /* release the uc reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_uc_reset(&core_copy.access ,0));
    /* we need to wait at least 10ms for the uc to settle */
    /*  PHYMOD_USLEEP(10000); */

    if(PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef TSCE_PMD_CRC_UCODE
        /* poll the ready bit in 10 ms */
        eagle_tsc_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 1);
#else
#ifndef EMULATION_BUILD
#if 0
        PHYMOD_IF_ERR_RETURN(
                eagle_tsc_ucode_crc_verify(&core_copy.access, tsce_ucode_len,tsce_ucode_crc));
#endif
#endif
#endif /* TSCE_PMD_CRC_UCODE */
    }
#if 0
    PHYMOD_IF_ERR_RETURN(
        eagle_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));
#endif
       /* plldiv CONFIG */
    if (init_config->interface.data_rate  == 2500) {
         PHYMOD_IF_ERR_RETURN(eagle_phy_pcs_12p5_vco_setup(&core_copy.access, 1));
         PHYMOD_IF_ERR_RETURN(
             eagle_tsce_vco_setup(&core_copy.access, init_config->interface.ref_clock, VCO_12p5));
    } else {
         PHYMOD_IF_ERR_RETURN(
             eagle_tsce_vco_setup(&core_copy.access, init_config->interface.ref_clock, VCO_10p3125));
    }

    /*now config the lane mapping and polarity */
    PHYMOD_IF_ERR_RETURN
             (tsce_core_lane_map_set(core, &init_config->lane_map));
    ///*don't overide the fw that set in config set if not specified*/
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /*set the vco rate to be default at 10.3125G */
    if (init_config->interface.data_rate  == 2500) {
        firmware_core_config_tmp.VcoRate = 28;
    } else {
        firmware_core_config_tmp.VcoRate = 19;
    }
    PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));

    PHYMOD_IF_ERR_RETURN(eagle_phy_pmd_setup(&core_copy.access, 0, init_config ));
    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset(&core_copy.access));
#ifdef DEBUG
    PHYMOD_IF_ERR_RETURN (eagle_tsc_display_core_config(&core_copy.access));
#endif
    return PHYMOD_E_NONE;
}

int tsce_core_vco_change(const phymod_core_access_t* core, uint32_t vco_12p5) {

    phymod_core_access_t  core_copy;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    /* 20.11 HW reset/Activate sw reset */
    PHYMOD_IF_ERR_RETURN(eagle_core_soft_reset_release(&core_copy.access, 0));

    /* Initialize  for 12.5G */
    PHYMOD_IF_ERR_RETURN(eagle_core_init_vco(&core_copy.access, vco_12p5));

    return PHYMOD_E_NONE;
}


int tsce_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tsce_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tsce_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;

}

int tsce_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    phymod_polarity_t tmp_pol;
#ifdef VENDOR_BROADCOM
    PHYMOD_VDBG(DBG_CFG, pm_acc, ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32"\n",
                __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask));
#endif
    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
#if 0
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (temod_pmd_x4_reset(pm_acc));
#endif

    PHYMOD_IF_ERR_RETURN(eagle_lane_hard_soft_reset_release(&pm_phy_copy.access, 0));
    PHYMOD_IF_ERR_RETURN(eagle_lane_hard_soft_reset_release(&pm_phy_copy.access, 1));
    /* poll for per lane uc_dsc_ready */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
    pm_phy_copy.access.lane_mask = 0x1;

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    pm_phy_copy.access.lane_mask = 0x1;
    return PHYMOD_E_NONE;
}


int tsce_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int start_lane, num_lane;
    int rv = PHYMOD_E_NONE;
    int i = 0;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        break;
    case phymodLoopbackGlobalPMD :
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(eagle_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
            PHYMOD_IF_ERR_RETURN(eagle_pmd_force_signal_detect(&phy_copy.access, (int) enable));
        }
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk(&phy->access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }
    return rv;
}

int tsce_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int start_lane, num_lane;

    *enable = 0;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }
    return PHYMOD_E_NONE;
}




int tsce_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    unsigned char rx_lock;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_pmd_lock_status(&phy->access, &rx_lock));
    *link_status = (uint32_t ) rx_lock;
    return PHYMOD_E_NONE;
}

int tsce_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock){
    PHYMOD_IF_ERR_RETURN(tsce_phy_link_status_get(phy, pmd_lock));
    return PHYMOD_E_NONE;
}


int tsce_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    int rv ;
    rv = PHYMOD_E_UNAVAIL ;

    switch(config->param){
    case phymodPcsUserSpeedParamEntry:
        /* no need */
        break ;
    case phymodPcsUserSpeedParamHCD:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_SPDID) ;
        break ;
    case phymodPcsUserSpeedParamClear:
        if(config->mode==phymodPcsUserSpeedModeST) {
            rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_SPDID) ;
        }
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_RESET) ;
        break ;
    case phymodPcsUserSpeedParamPllDiv:
        /* not required */
        break ;
    case phymodPcsUserSpeedParamPmaOS:
        /* not required */
        break ;
    case phymodPcsUserSpeedParamScramble:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_SCR_MODE) ;
        break ;
    case phymodPcsUserSpeedParamEncode:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_ENCODE_MODE) ;
        break ;
    case phymodPcsUserSpeedParamCl48CheckEnd:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CHKEND_EN) ;
        break ;
    case phymodPcsUserSpeedParamBlkSync:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_BLKSYNC_MODE) ;
        break ;
    case phymodPcsUserSpeedParamReorder:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_REORDER_EN) ;
        break ;
    case phymodPcsUserSpeedParamCl36Enable:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CL36_EN) ;
        break ;
    case phymodPcsUserSpeedParamDescr1:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DESCR_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDecode1:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DECODE_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDeskew:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DESKEW_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDescr2:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DESC2_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDescr2ByteDel:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CL36BYTEDEL_MODE) ;
        break ;
    case phymodPcsUserSpeedParamBrcm64B66:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_BRCM64B66_DESCR_MODE) ;
        break ;
    case phymodPcsUserSpeedParamSgmii:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_SGMII_SPD) ;
        break ;
    case phymodPcsUserSpeedParamClkcnt0:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_CLOCK_COUNT_0);
        break ;
    case phymodPcsUserSpeedParamClkcnt1:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_CLOCK_COUNT_1);
        break ;
    case phymodPcsUserSpeedParamLpcnt0:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_LOOP_COUNT_0);
        break ;
    case phymodPcsUserSpeedParamLpcnt1:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_LOOP_COUNT_1);
        break ;
    case phymodPcsUserSpeedParamMacCGC:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_MAC);
        break ;
    case phymodPcsUserSpeedParamRepcnt:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_PCS_REPCNT);
        break ;
    case phymodPcsUserSpeedParamCrdtEn:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_EN) ;
        break ;
    case phymodPcsUserSpeedParamPcsClkcnt:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_PCS_CLOCK_COUNT_0);
        break ;
    case phymodPcsUserSpeedParamPcsCGC:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_PCS_GEN_COUNT);
        break ;
    case phymodPcsUserSpeedParamCl72En:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CL72) ;
        break ;
    case phymodPcsUserSpeedParamNumOfLanes:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_NUM_LANES) ;
        break ;
    default:
        break ;
    }
    return rv;
}

int tsce_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}

int tsce_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t *val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int tsce_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}

int tsce_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    PHYMOD_IF_ERR_RETURN(temod_eee_control_set(&phy->access,enable));
    return PHYMOD_E_NONE;
}

int tsce_phy_eee_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    uint32_t enable_local;
    PHYMOD_IF_ERR_RETURN(temod_eee_control_get(&phy->access, &enable_local));
    *enable = enable_local;
    return PHYMOD_E_NONE;
}

int tsce_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 0));
    return PHYMOD_E_NONE;
}

int tsce_phy_display_diag(const phymod_phy_access_t* phy, uint32_t level)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = 0x1 ;

    PHYMOD_IF_ERR_RETURN (eagle_tsc_display_diag_data(&pm_phy_copy.access, level));
    return PHYMOD_E_NONE;
}

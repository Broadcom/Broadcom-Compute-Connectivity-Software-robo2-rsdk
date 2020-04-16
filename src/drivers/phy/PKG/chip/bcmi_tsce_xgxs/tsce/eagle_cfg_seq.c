/*
 *
 * $Id: eagle.c,v 1.2.2.26 2013/09/16 12:23:15 hoangngo Exp $
 *
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
 *
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 *  $Copyright: (c) 2020 Broadcom Inc.All Rights Reserved.$
 *  $Id$
*/


#include <phymod/phymod.h>
#include "eagle_cfg_seq.h"
#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#include "eagle_tsc_fields.h"
#include "eagle_tsc_dependencies.h"
#include "eagle_tsc_field_access.h"
#include "temod_enum_defines.h"
#include "temod.h"
/** Macro to execute a statement with automatic return of error LESTM(Local ESTM) to fix coverity error */
#define LESTM(statement) do {err_code_t __err = ERR_CODE_NONE; statement; if (__err) return(__err); } while(0)
/** Macro to invoke a function with automatic return of error */
#define LEFUN(fun) do {err_code_t __err = (fun); if (__err) return(__err); } while(0)

extern err_code_t eagle_tsc_configure_pll (const phymod_access_t *pa, enum eagle_tsc_pll_enum pll_cfg);
err_code_t eagle_uc_active_set(const phymod_access_t *pa, uint32_t enable)           /* set microcontroller active or not  */
{
    DIG_TOP_USER_CTL0r_t uc_active_reg;

    DIG_TOP_USER_CTL0r_CLR(uc_active_reg);
    DIG_TOP_USER_CTL0r_UC_ACTIVEf_SET(uc_active_reg, enable);
    MODIFY_DIG_TOP_USER_CTL0r(pa, uc_active_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_uc_active_get(const phymod_access_t *pa, uint32_t *enable)           /* get microcontroller active or not  */
{
    DIG_TOP_USER_CTL0r_t uc_active_reg;

    DIG_TOP_USER_CTL0r_CLR(uc_active_reg);
    READ_DIG_TOP_USER_CTL0r(pa, &uc_active_reg);
    *enable = DIG_TOP_USER_CTL0r_UC_ACTIVEf_GET(uc_active_reg);

    return ERR_CODE_NONE;
}

/* Enable or Disable the uC reset */
err_code_t eagle_uc_reset(const phymod_access_t *pa, uint32_t enable)
{
    if (enable) {
        /* Assert micro reset and reset all micro registers (all non-status registers written to default value) */
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD200, 0x0000));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD201, 0x0000));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD202, 0x0000));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD203, 0x0000));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD207, 0x0000));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD208, 0x0000));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD20A, 0x080f));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD20C, 0x0002));
        LEFUN(eagle_tsc_pmd_wr_reg(pa,0xD20D, 0x0000));
    } else {

        LEFUN(wrc_micro_system_clk_en(0x1));                   /* Enable clock to micro  */
        LEFUN(wrc_micro_system_reset_n(0x1));                  /* De-assert reset to micro */
        LEFUN(wrc_micro_mdio_dw8051_reset_n(0x1));
    }
    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_tx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data)
{
    TLB_TX_PRBS_GEN_CFGr_t tmp_reg;
    TLB_TX_PRBS_GEN_CFGr_CLR(tmp_reg);
    READ_TLB_TX_PRBS_GEN_CFGr(pa, &tmp_reg);
    *inv_data = TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_INVf_GET(tmp_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_rx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data)
{
    TLB_RX_PRBS_CHK_CFGr_t tmp_reg;
    TLB_RX_PRBS_CHK_CFGr_CLR(tmp_reg);
    READ_TLB_RX_PRBS_CHK_CFGr(pa, &tmp_reg);
    *inv_data = TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_INVf_GET(tmp_reg);
    return ERR_CODE_NONE;
}


err_code_t eagle_prbs_tx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly)
{
    TLB_TX_PRBS_GEN_CFGr_t tmp_reg;
    TLB_TX_PRBS_GEN_CFGr_CLR(tmp_reg);
    READ_TLB_TX_PRBS_GEN_CFGr(pa, &tmp_reg);
    *prbs_poly = TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_MODE_SELf_GET(tmp_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_rx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly)
{
    TLB_RX_PRBS_CHK_CFGr_t tmp_reg;
    TLB_RX_PRBS_CHK_CFGr_CLR(tmp_reg);
    READ_TLB_RX_PRBS_CHK_CFGr(pa, &tmp_reg);
    *prbs_poly = TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_MODE_SELf_GET(tmp_reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_tx_enable_get(const phymod_access_t *pa, uint32_t *enable)
{
    TLB_TX_PRBS_GEN_CFGr_t tmp_reg;
    TLB_TX_PRBS_GEN_CFGr_CLR(tmp_reg);
    READ_TLB_TX_PRBS_GEN_CFGr(pa, &tmp_reg);
    *enable = TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_ENf_GET(tmp_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_rx_enable_get(const phymod_access_t *pa, uint32_t *enable)
{
    TLB_RX_PRBS_CHK_CFGr_t tmp_reg;
    TLB_RX_PRBS_CHK_CFGr_CLR(tmp_reg);
    READ_TLB_RX_PRBS_CHK_CFGr(pa, &tmp_reg);
    *enable = TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_ENf_GET(tmp_reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pll_mode_set(const phymod_access_t *pa, int pll_mode)   /* PLL divider set */
{
    PLL_CAL_CTL7r_t eagle_xgxs_ctl_7_reg;

    PLL_CAL_CTL7r_CLR(eagle_xgxs_ctl_7_reg);
    PLL_CAL_CTL7r_PLL_MODEf_SET(eagle_xgxs_ctl_7_reg, pll_mode);
    MODIFY_PLL_CAL_CTL7r(pa, eagle_xgxs_ctl_7_reg);
    return ERR_CODE_NONE;
}
err_code_t eagle_core_init_vco(phymod_access_t *pa, uint32_t vco_12p5) {
    if(vco_12p5) {
        LEFUN(eagle_phy_pcs_12p5_vco_setup(pa));
    }
    /* 20.2 - PMD setup */
    LEFUN(wrc_refclk_sel(0x3));
    /* Bypass PMD Tx Oversampling */
    LEFUN(wr_tx_pcs_native_ana_frmt_en(1));
    LEFUN(wrc_core_dp_s_rstb(0));
    if(vco_12p5) {
        LEFUN(wrc_pll_mode(0xd));
    } else {
        LEFUN(wrc_pll_mode(0xa));
    }
    LEFUN(wrc_core_dp_s_rstb(1));
#if 0
    /* 20.9 - PRBS Generator */
    LEFUN(wr_prbs_gen_en(0x1));
    /* 20.3 - Force speed 2.5G */
    LEFUN(wr_credit_sw_en(0x0));
    LEFUN(wr_SW_actual_speed_force_en(0x1));
    LEFUN(wr_SW_actual_speed(0x3));
    LEFUN(wr_rstb_lane(1));
    LEFUN(wr_mac_creditenable(1));
    LEFUN(wr_rstb_tx_lane(1));
    LEFUN(wr_enable_tx_lane(1));

    /* wait for PLL lock */
    PHYMOD_USLEEP(2000);

    /* 20.6 - Datapath reset - core */
    LEFUN(wrc_core_s_rstb(1));
    PHYMOD_USLEEP(2000);

    LEFUN(wr_ln_dp_s_rstb(1));
#endif
    return ERR_CODE_NONE;
}

err_code_t eagle_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable)   /* release the pmd core soft reset */
{
    uint32_t rstb;
    err_code_t __err;

    rstb = rdc_core_s_rstb();
    if (rstb == 0) {
        enable = 1;
    } else {
        enable = 0;
    }
    LEFUN(wrc_core_s_rstb(enable));
    return ERR_CODE_NONE;
#if 0
    DIG_TOP_USER_CTL0r_t reg;

    DIG_TOP_USER_CTL0r_CLR(reg);
    DIG_TOP_USER_CTL0r_CORE_DP_S_RSTBf_SET(reg, enable);
    MODIFY_DIG_TOP_USER_CTL0r(pa, reg);
    return ERR_CODE_NONE;
#endif
}

err_code_t eagle_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable)   /* release the pmd core soft reset */
{
#if 0
    CKRST_LN_CLK_RST_N_PWRDWN_CTLr_t    reg_pwrdwn_ctrl;

    CKRST_LN_CLK_RST_N_PWRDWN_CTLr_CLR(reg_pwrdwn_ctrl);
    CKRST_LN_CLK_RST_N_PWRDWN_CTLr_LN_DP_S_RSTBf_SET(reg_pwrdwn_ctrl, enable);
    MODIFY_CKRST_LN_CLK_RST_N_PWRDWN_CTLr(pa,  reg_pwrdwn_ctrl);
#endif
    LEFUN(wr_ln_dp_s_rstb(enable));
    return ERR_CODE_NONE;
}

err_code_t eagle_pram_flop_set(const phymod_access_t *pa, int value)   /* release the pmd core soft reset */
{
    UC_COMMAND3r_t uc_reg;
    UC_COMMAND3r_CLR(uc_reg);
    UC_COMMAND3r_MICRO_PRAM_IF_FLOP_BYPASSf_SET(uc_reg, value);
    MODIFY_UC_COMMAND3r(pa, uc_reg);
    return ERR_CODE_NONE;
}


err_code_t eagle_pram_firmware_enable(const phymod_access_t *pa, int enable)   /* release the pmd core soft reset */
{
    UC_COMMAND3r_t uc_reg;
    /* UC_COMMAND4r_t reg; */
    UC_COMMAND3r_CLR(uc_reg);
    /* UC_COMMAND4r_CLR(reg); */
    if(enable == 1) {
    UC_COMMAND3r_MICRO_PRAM_IF_RSTBf_SET(uc_reg, 1);
    UC_COMMAND3r_MICRO_PRAM_IF_ENf_SET(uc_reg, 1);
    UC_COMMAND3r_MICRO_PRAM_IF_FLOP_BYPASSf_SET(uc_reg, 0);
    /*
    UC_COMMAND4r_MICRO_SYSTEM_CLK_ENf_SET(reg, 1);
    UC_COMMAND4r_MICRO_SYSTEM_RESET_Nf_SET(reg, 1);
    MODIFY_UC_COMMAND4r(pa, reg);
    */
    } else {
    UC_COMMAND3r_MICRO_PRAM_IF_RSTBf_SET(uc_reg, 0);
    UC_COMMAND3r_MICRO_PRAM_IF_ENf_SET(uc_reg, 0);
    UC_COMMAND3r_MICRO_PRAM_IF_FLOP_BYPASSf_SET(uc_reg, 1);
    }
    MODIFY_UC_COMMAND3r(pa, uc_reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_force_signal_detect(const phymod_access_t *pa, int enable)
{
    SIGDET_CTL1r_t reg;
    SIGDET_CTL1r_CLR(reg);
    SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(reg, enable);
    SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(reg, enable);
    MODIFY_SIGDET_CTL1r(pa, reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable)
{
    TLB_RX_DIG_LPBK_CFGr_t reg;
    READ_TLB_RX_DIG_LPBK_CFGr(pa, &reg);
    *enable = TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_cl72_enable_get(const phymod_access_t *pa, uint32_t *enable)
{
    CL72_TXBASE_R_PMD_CTLr_t reg;
    READ_CL72_TXBASE_R_PMD_CTLr(pa, &reg);
    *enable = CL72_TXBASE_R_PMD_CTLr_CL72_IEEE_TRAINING_ENABLEf_GET(reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_cl72_receiver_status(const phymod_access_t *pa, uint32_t *status)
{
    CL72_TXBASE_R_PMD_STSr_t reg;
    READ_CL72_TXBASE_R_PMD_STSr(pa, &reg);
    *status = CL72_TXBASE_R_PMD_STSr_CL72_IEEE_RECEIVER_STATUSf_GET(reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_uc_init(const phymod_access_t *pa)
{
  uint8_t result;
  err_code_t rc  = 0;

  LEFUN(wrc_micro_system_clk_en(0x1));                         /* Enable clock to micro  */
  LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */
  LEFUN(wrc_micro_system_reset_n(0x0));                        /* Assert reset to micro - Toggling micro reset*/
  LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */

  eagle_pram_flop_set(pa, 0x0);
  LEFUN(wrc_micro_mdio_ram_access_mode(0x0));                  /* Select Program Memory access mode - Program RAM load when Serdes DW8051 in reset */

  LEFUN(wrc_micro_ram_address(0x0));                           /* RAM start address */
  LEFUN(wrc_micro_init_cmd(0x0));                              /* Clear initialization command */
  LEFUN(wrc_micro_init_cmd(0x1));                              /* Set initialization command */

  rc = eagle_tsc_delay_us(4000);
  if (rc != ERR_CODE_NONE) {
      return rc;
  }

  LESTM(result = !rdc_micro_init_done());
	if (result) {                                        /* Check if initialization done within 500us time interval */
		PHYMOD_DEBUG_ERROR(("ERR_CODE_MICRO_INIT_NOT_DONE\n"));
		return (ERR_CODE_MICRO_INIT_NOT_DONE);    /* Else issue error code */
	}
  LEFUN(wrc_micro_init_cmd(0x0));                              /* Clear initialization command */
  return ( ERR_CODE_NONE );

}


/***********************************************/
/*  Microcode Init into Program RAM Functions  */
/***********************************************/

  /* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t eagle_tsc_ucode_init( const phymod_access_t *pa ) {

    uint8_t result;
    err_code_t rc  = 0;

	LEFUN(wrc_micro_system_clk_en(0x1));                         /* Enable clock to micro  */
	LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */
	LEFUN(wrc_micro_system_reset_n(0x0));                        /* Assert reset to micro - Toggling micro reset*/
	LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */

	LEFUN(wrc_micro_mdio_ram_access_mode(0x0));                  /* Select Program Memory access mode - Program RAM load when Serdes DW8051 in reset */

	LEFUN(wrc_micro_ram_address(0x0));                           /* RAM start address */
	LEFUN(wrc_micro_init_cmd(0x0));                              /* Clear initialization command */
	LEFUN(wrc_micro_init_cmd(0x1));                              /* Set initialization command */

    /*
     * COVERITY
     *
     * Delay doesn't check return value.
     */
    /* coverity[check_return] */
	rc = eagle_tsc_delay_us(500);
    if (rc != ERR_CODE_NONE) {
        return rc;
    }
	LEFUN(wrc_micro_init_cmd(0x0));                              /* Set initialization command */

    LESTM(result = !rdc_micro_init_done());

	if (result) {                                        /* Check if initialization done within 500us time interval */
		PHYMOD_DEBUG_ERROR(("ERR_CODE_MICRO_INIT_NOT_DONE\n"));
		return (ERR_CODE_MICRO_INIT_NOT_DONE);    /* Else issue error code */
	}

	return (ERR_CODE_NONE);
}

err_code_t eagle_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val)
{
	CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr_t pin_ctl;

	/*
	* Work around per Magesh/Justin
	* override input from PCS to allow uc_dsc_ready_for_cmd
	* reg get written by UC
	*/
	READ_CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr(pa, &pin_ctl);
	CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr_PMD_LN_H_RSTB_PKILLf_SET(pin_ctl, val);
	WRITE_CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr(pa, pin_ctl);
	return (ERR_CODE_NONE);
}

err_code_t eagle_tsc_identify(const phymod_access_t *pa, eagle_rev_id0_t *rev_id0, eagle_rev_id1_t *rev_id1)
{
  err_code_t __err;

  rev_id0->revid_rev_letter =0;
  rev_id0->revid_rev_number =0;
  rev_id0->revid_bonding    =0;
  rev_id0->revid_process    =0;
  rev_id0->revid_model      =0;

  rev_id1->revid_multiplicity =0;
  rev_id1->revid_mdio         =0;
  rev_id1->revid_micro        =0;
  rev_id1->revid_cl72         =0;
  rev_id1->revid_pir          =0;
  rev_id1->revid_llp          =0;
  rev_id1->revid_eee          =0;

  __err=ERR_CODE_NONE;

  rev_id0->revid_rev_letter =rdc_revid_rev_letter(); if(__err) return(__err);
  rev_id0->revid_rev_number =rdc_revid_rev_number(); if(__err) return(__err);
  rev_id0->revid_bonding    =rdc_revid_bonding();    if(__err) return(__err);
  rev_id0->revid_process    =rdc_revid_process();    if(__err) return(__err);
  rev_id0->revid_model      =rdc_revid_model();      if(__err) return(__err);

  rev_id1->revid_multiplicity =rdc_revid_multiplicity(); if(__err) return(__err);
  rev_id1->revid_mdio         =rdc_revid_mdio();         if(__err) return(__err);
  rev_id1->revid_micro        =rdc_revid_micro();        if(__err) return(__err);
  rev_id1->revid_cl72         =rdc_revid_cl72();         if(__err) return(__err);
  rev_id1->revid_pir          =rdc_revid_pir();          if(__err) return(__err);
  rev_id1->revid_llp          =rdc_revid_llp();          if(__err) return(__err);
  rev_id1->revid_eee          =rdc_revid_eee();          if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err = (uint32_t) wr_tx_pmd_dp_invert(tx_pol);
  if(__err) return(__err);
  __err = (uint32_t) wr_rx_pmd_dp_invert(rx_pol);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *tx_pol = (uint32_t) rd_tx_pmd_dp_invert();
  if(__err) return(__err);
  *rx_pol = (uint32_t) rd_rx_pmd_dp_invert();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_force_tx_set_rst (const phymod_access_t *pa, uint32_t rst)
{
  LEFUN(wr_afe_tx_reset_frc_val(rst));
  LEFUN(wr_afe_tx_reset_frc(0x1));
  return ERR_CODE_NONE;
}

err_code_t eagle_force_tx_get_rst (const phymod_access_t *pa, uint32_t *rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rst=(uint32_t) rd_afe_tx_reset_frc_val();
  return ERR_CODE_NONE;
}

err_code_t eagle_force_rx_set_rst (const phymod_access_t *pa, uint32_t rst)
{
  LEFUN(wr_afe_rx_reset_frc_val(rst));
  LEFUN(wr_afe_rx_reset_frc(0x1));
  return ERR_CODE_NONE;
}

err_code_t eagle_force_rx_get_rst (const phymod_access_t *pa, uint32_t *rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rst=(uint32_t) rd_afe_rx_reset_frc_val();
  return ERR_CODE_NONE;
}


err_code_t eagle_pll_mode_get(const phymod_access_t *pa, uint32_t *pll_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_mode=rdc_pll_mode();
  *pll_mode=rdc_ams_pll_fracn_sel();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_osr_mode_set(const phymod_access_t *pa, int osr_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_osr_mode_frc_val(osr_mode);
  if(__err) return(__err);
  __err=wr_osr_mode_frc(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_osr_mode_get(const phymod_access_t *pa, int *osr_mode)
{
  int osr_forced;
  err_code_t __err;
  __err=ERR_CODE_NONE;
  osr_forced = rd_osr_mode_frc();
  if(osr_forced) {
    *osr_mode = rd_osr_mode_frc_val();
    if(__err) return(__err);
  } else {
    *osr_mode = rd_osr_mode_pin();
    if(__err) return(__err);
  }

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_dig_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *lpbk=rd_dig_lpbk_en();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *lpbk=rd_rmt_lpbk_en();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_core_soft_reset_set(const phymod_access_t *pa)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_core_dp_s_rstb(0);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}
err_code_t eagle_core_soft_reset(const phymod_access_t *pa)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_core_dp_s_rstb(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

/* set powerdown for tx or rx */
/* tx_rx == 1 => disable (enable) power for Tx */
/* tx_rx != 0 => disable (enable) power for Rx */
/* pwrdn == 0 => enable power */
/* pwrdn == 1 => disable power */
err_code_t eagle_tsc_pwrdn_set(const phymod_access_t *pa, int tx_rx, int pwrdn)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  if(tx_rx) {
    __err = (uint32_t) wr_ln_tx_s_pwrdn(pwrdn);
  } else {
    __err = (uint32_t) wr_ln_rx_s_pwrdn(pwrdn);
  }
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_pwrdn_get(const phymod_access_t *pa, power_status_t *pwrdn)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  pwrdn->pll_pwrdn  = 0;
  pwrdn->tx_s_pwrdn = 0;
  pwrdn->rx_s_pwrdn = 0;
  pwrdn->pll_pwrdn  = rdc_afe_s_pll_pwrdn(); if(__err) return(__err);
  pwrdn->tx_s_pwrdn = rd_ln_tx_s_pwrdn();    if(__err) return(__err);
  pwrdn->rx_s_pwrdn = rd_ln_rx_s_pwrdn();    if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pcs_lane_swap_tx(const phymod_access_t *pa, uint32_t tx_lane_map) {
  err_code_t __err;
  uint32_t lane_addr_0, lane_addr_1, lane_addr_2, lane_addr_3;

  __err=ERR_CODE_NONE;
  lane_addr_0 = (tx_lane_map >> 16) & 0xf;
  lane_addr_1 = (tx_lane_map >> 20) & 0xf;
  lane_addr_2 = (tx_lane_map >> 24) & 0xf;
  lane_addr_3 = (tx_lane_map >> 28) & 0xf;

  __err=wrc_lane_addr_0(lane_addr_0); if(__err) return(__err);
  __err=wrc_lane_addr_1(lane_addr_1); if(__err) return(__err);
  __err=wrc_lane_addr_2(lane_addr_2); if(__err) return(__err);
  __err=wrc_lane_addr_3(lane_addr_3); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pmd_lane_swap (const phymod_access_t *pa, uint32_t lane_map) {
  err_code_t __err;
  uint32_t lane_0, lane_1, lane_2, lane_3;

  __err=ERR_CODE_NONE;
  lane_0 = ((lane_map >> 0)  & 0x3);
  lane_1 = ((lane_map >> 4)  & 0x3);
  lane_2 = ((lane_map >> 8)  & 0x3);
  lane_3 = ((lane_map >> 12) & 0x3);

  __err=wrc_tx_lane_map_0(lane_0); if(__err) return(__err);
  __err=wrc_tx_lane_map_1(lane_1); if(__err) return(__err);
  __err=wrc_tx_lane_map_2(lane_2); if(__err) return(__err);
  __err=wrc_tx_lane_map_3(lane_3); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pmd_lane_swap_tx_get (const phymod_access_t *pa, uint32_t *lane_map) {
  err_code_t __err;
  uint32_t lane_0, lane_1, lane_2, lane_3;

  __err=ERR_CODE_NONE;

  lane_0=rdc_tx_lane_map_0(); if(__err) return(__err);
  lane_1=rdc_tx_lane_map_1(); if(__err) return(__err);
  lane_2=rdc_tx_lane_map_2(); if(__err) return(__err);
  lane_3=rdc_tx_lane_map_3(); if(__err) return(__err);

  *lane_map = ((lane_3 & 0x3) <<12)|((lane_2 & 0x3) <<8)|((lane_1 & 0x3) <<4)|((lane_0 & 0x3) <<0);

  return ERR_CODE_NONE;
}

err_code_t eagle_lane_hard_soft_reset_release(const phymod_access_t *pa, uint32_t enable)   /* release the pmd core soft reset */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_ln_s_rstb(enable); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t eagle_clause72_control(const phymod_access_t *pa, uint32_t cl_72_en)                /* CLAUSE_72_CONTROL */
{
  err_code_t __err;

  __err=ERR_CODE_NONE;
  if (cl_72_en) {
    __err=wr_cl72_ieee_training_enable(1);  if(__err) return(__err);
#if 0
    __err=wr_cl72_ieee_restart_training(1); if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(0);                  if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(1);                  if(__err) return(__err);
#endif
  } else {
    __err=wr_cl72_ieee_training_enable(0);        if(__err) return(__err);
    __err=wr_cl72_ieee_restart_training(0); if(__err) return(__err);
#if 0
    __err=wr_ln_dp_s_rstb(0);                  if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(1);                  if(__err) return(__err);
#endif
  }
  return ERR_CODE_NONE;
}

err_code_t eagle_clause72_control_get(const phymod_access_t *pa, uint32_t *cl_72_en)                /* CLAUSE_72_CONTROL */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *cl_72_en = rd_cl72_ieee_training_enable(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

/**
@brief   Init the PMD
@param   pmd_touched If the PMD is already initialized
@returns The value ERR_CODE_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
err_code_t eagle_pmd_reset_seq(const phymod_access_t *pa, int pmd_touched)
{
  if (pmd_touched == 0) {
    LEFUN(wrc_core_s_rstb(1));
    LEFUN(wrc_core_dp_s_rstb(1));
  }
    return (ERR_CODE_NONE);
}

/**
@brief   Enable the pll reset bit
@param   enable Controls whether to reset PLL
@returns The value ERR_CODE_NONE upon successful completion
@details
Resets the PLL
*/
err_code_t eagle_pll_reset_enable_set (const phymod_access_t *pa, int enable)
{
  LEFUN(wrc_afe_s_pll_reset_frc_val(enable));
  LEFUN(wrc_afe_s_pll_reset_frc(1));
    return (ERR_CODE_NONE);
}

/**
@brief   Read PLL range
*/
err_code_t eagle_tsc_read_pll_range (const phymod_access_t *pa, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = rdc_ams_pll_range();
  return (ERR_CODE_NONE);
}

/**
@brief   Reag signal detect
*/
err_code_t eagle_tsc_signal_detect (const phymod_access_t *pa, uint32_t *signal_detect)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *signal_detect = rd_signal_detect();
  return (ERR_CODE_NONE);
}


err_code_t eagle_tsc_ladder_setting_to_mV(const phymod_access_t *pa, int8_t y, int16_t* level) {

  /* *level = _ladder_setting_to_mV(y,0); */
   *level = (y*300)/127;

  return(ERR_CODE_NONE);
}

err_code_t eagle_pll_config_get(const phymod_access_t *pa,enum  eagle_tsc_pll_enum *pll_mode)
{
  err_code_t __err;
  uint32_t fracn_sel;
  uint8_t fracn_ndiv;
  uint32_t pll_div;
  uint32_t pll_vco_div2;
  __err=ERR_CODE_NONE;

  pll_div=rdc_pll_mode();
  fracn_sel=rdc_ams_pll_fracn_sel();
  fracn_ndiv=rdc_ams_pll_fracn_ndiv_int();
  pll_vco_div2=rdc_ams_pll_vco_div2();

  if(fracn_sel == 1) {
     switch(fracn_ndiv) {
       case 0x52: *pll_mode = EAGLE_TSC_pll_div_82p5x;
                  break;
       case 0x57: *pll_mode = EAGLE_TSC_pll_div_87p5x;
                  break;
       case 0x49: if(pll_vco_div2 == 0x1)
                   *pll_mode = EAGLE_TSC_pll_div_36p8x;
                  else
                   *pll_mode = EAGLE_TSC_pll_div_73p6x;
                  break;
       case 0xc7: *pll_mode = EAGLE_TSC_pll_div_199p04x;
                  break;
       default: return ((ERR_CODE_INVALID_PLL_CFG));
                break;

    }
  } else {
     switch(pll_div){
       case 0x1: *pll_mode = EAGLE_TSC_pll_div_72x;
                 break;
       case 0xc: *pll_mode = EAGLE_TSC_pll_div_70x;
                 break;
       case 0xa: *pll_mode = EAGLE_TSC_pll_div_66x;
                 break;
       case 0x9: *pll_mode = EAGLE_TSC_pll_div_64x;
                 break;
       case 0x8: *pll_mode = EAGLE_TSC_pll_div_60x;
                 break;
       case 0x6: *pll_mode = EAGLE_TSC_pll_div_52x;
                 break;
       case 0x3: *pll_mode = EAGLE_TSC_pll_div_42x;
                 break;
       case 0x2: *pll_mode = EAGLE_TSC_pll_div_40x;
                 break;
       case 0xf: *pll_mode = EAGLE_TSC_pll_div_100x;
                 break;
       case 0xe: *pll_mode = EAGLE_TSC_pll_div_92x;
                 break;
       case 0xb: *pll_mode = EAGLE_TSC_pll_div_68x;
                 break;
       case 0x5: *pll_mode = EAGLE_TSC_pll_div_50x;
                 break;
       case 0x0: *pll_mode = EAGLE_TSC_pll_div_46x;
                 break;
       case 0xd: *pll_mode = EAGLE_TSC_pll_div_80x;
                 break;
       default: return ((ERR_CODE_INVALID_PLL_CFG));
                break;
     }
  }

  if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tx_lane_control_set(const phymod_access_t *pa, uint32_t en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = wr_sdk_tx_disable(en); if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tx_lane_control_get(const phymod_access_t *pa, uint32_t* en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  *en = rd_sdk_tx_disable(); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_rx_lane_control_set(const phymod_access_t *pa, uint32_t en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  if(en) {
  __err = wr_signal_detect_frc(1); if(__err) return(__err);
  __err = wr_signal_detect_frc_val(0); if(__err) return(__err);
  } else {
  __err = wr_signal_detect_frc(0); if(__err) return(__err);
  __err = wr_signal_detect_frc_val(0); if(__err) return(__err);
  }

  return ERR_CODE_NONE;
}


err_code_t eagle_rx_lane_control_get(const phymod_access_t *pa, uint32_t* en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  *en = rd_signal_detect_frc(); if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tsc_get_vco (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode) {
  *vco_rate = 0;
  *new_pll_div=0;
  *new_os_mode = 0;
  if(config->ref_clock == phymodRefClk156Mhz) {
    switch (config->data_rate) {
      case 5750 :   /* speed 5.75G ; fracn pll= 73.6    */
        *new_pll_div = EAGLE_TSC_pll_div_36p8x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 5750; /* vco_rate = 24 */
        break;

      case 6250:   /* speed 6.25G: pll_div=40  */
        *new_pll_div = EAGLE_TSC_pll_div_40x;  *new_os_mode = 0 /*= OS1 */; *vco_rate = 6250; /* vco_rate= 3 */
        break;

      case 10312:   /* speed 10.3G; pll_div = 66  */
        *new_pll_div = EAGLE_TSC_pll_div_66x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 103125; /* vco_rate = 19 */
        break;

      case 11500:   /* speed 11.5G ; fracn pll= 73.6    */
        *new_pll_div = EAGLE_TSC_pll_div_73p6x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 11500; /* vco_rate = 24 */
        break;

      case 12500:   /* speed 12.5G ; pll_div = 80    */
        *new_pll_div = EAGLE_TSC_pll_div_80x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 12500; /* vco_rate =  */
        break;

      default:
        PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d", (int)config->data_rate, config->ref_clock));
        return ERR_CODE_DIAG;
        break;
    }
  } else if(config->ref_clock == phymodRefClk125Mhz) {
    switch (config->data_rate) {
      case 5750 :   /* speed 5.75G ; fracn pll= 46    */
        *new_pll_div = EAGLE_TSC_pll_div_46x; *new_os_mode = 0 /*= OS1 */; *vco_rate =5750; /* vco_rate =  */
        break;

      case 6250:   /* speed 6.25G: pll_div=50  */
        *new_pll_div = EAGLE_TSC_pll_div_50x;  *new_os_mode = 0 /*= OS1 */; *vco_rate = 6250; /* vco_rate= 3 */
        break;

      case 10312:   /* speed 10.3G; pll_div = 82.5  */
        *new_pll_div = EAGLE_TSC_pll_div_82p5x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 10312; /* vco_rate = 19 */
        break;

      case 11500:   /* speed 11.5G ; fracn pll= 92    */
        *new_pll_div = EAGLE_TSC_pll_div_92x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 11500; /* vco_rate = 24 */
        break;

      default:
        PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d", (int)config->data_rate, config->ref_clock));
        return ERR_CODE_DIAG;
        break;
    }
  } else {
    PHYMOD_DEBUG_ERROR(("Unsupported ref clk :: %d\n", config->ref_clock));
    return ERR_CODE_DIAG;
  }
  return(ERR_CODE_NONE);
}

err_code_t eagle_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_pmd_tx_disable_pkill(enable); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pmd_tx_disable_pin_dis_get(const phymod_access_t *pa, uint32_t *enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rd_pmd_tx_disable_pkill(); if(__err) return(__err);

  return ERR_CODE_NONE;
}

/* Get Enable/Disable Shared TX pattern generator */
err_code_t eagle_tsc_tx_shared_patt_gen_en_get( const phymod_access_t *pa, uint8_t *enable) {
  err_code_t __err;

  *enable = rd_patt_gen_en(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_config_shared_tx_pattern_idx_set( const phymod_access_t *pa, uint32_t *pattern) {
  err_code_t __err;
  uint16_t lsb, msb;
  int pattern_idx;

  __err=ERR_CODE_NONE;
  for (pattern_idx=0;pattern_idx<PATTERN_MAX_SIZE;pattern_idx++) {

      lsb = (uint16_t) (pattern[pattern_idx] & 0xffff);
      msb = (uint16_t) ((pattern[pattern_idx]>>16) & 0xffff);

      switch (pattern_idx) {
        case 0:
          __err=wrc_patt_gen_seq_14(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_13(lsb); if(__err) return(__err);
          break;
        case 1:
          __err=wrc_patt_gen_seq_12(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_11(lsb); if(__err) return(__err);
          break;
        case 2:
          __err=wrc_patt_gen_seq_10(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_9(lsb); if(__err) return(__err);
          break;
        case 3:
          __err=wrc_patt_gen_seq_8(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_7(lsb); if(__err) return(__err);
          break;
        case 4:
          __err=wrc_patt_gen_seq_6(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_5(lsb); if(__err) return(__err);
          break;
        case 5:
          __err=wrc_patt_gen_seq_4(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_3(lsb); if(__err) return(__err);
          break;
        case 6:
          __err=wrc_patt_gen_seq_2(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_1(lsb); if(__err) return(__err);
          break;
        case 7:
          __err=wrc_patt_gen_seq_0(msb); if(__err) return(__err);
          break;
        /* Its a dead default and cause coverity defect
        default:
              PHYMOD_DEBUG_ERROR(("Wrong index value : Should be between 0 and 7\n"));
          return ERR_CODE_DIAG;
          break;*/
      }
  }
  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_config_shared_tx_pattern_idx_get( const phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern) {
  err_code_t __err;
  uint8_t mode_sel;
  uint32_t lsb = 0, msb = 0;
  int pattern_idx;

  __err=ERR_CODE_NONE;

  mode_sel = rd_patt_gen_start_pos(); if(__err) return(__err);

  mode_sel = 12 - mode_sel;

  if(mode_sel == 6) {
    *pattern_len = 140;
  } else if (mode_sel == 5) {
    *pattern_len = 160;
  } else if (mode_sel == 4) {
    *pattern_len = 180;
  } else if (mode_sel == 3) {
    *pattern_len = 200;
  } else if (mode_sel == 2) {
    *pattern_len = 220;
  } else if (mode_sel == 1) {
    *pattern_len = 240;
  } else {
    *pattern_len = 0;
  }

  for (pattern_idx=0;pattern_idx<PATTERN_MAX_SIZE;pattern_idx++) {
      switch (pattern_idx) {
        case 0:
          msb=rdc_patt_gen_seq_14(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_13(); if(__err) return(__err);
          break;
        case 1:
          msb=rdc_patt_gen_seq_12(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_11(); if(__err) return(__err);
          break;
        case 2:
          msb=rdc_patt_gen_seq_10(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_9(); if(__err) return(__err);
          break;
        case 3:
          msb=rdc_patt_gen_seq_8(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_7(); if(__err) return(__err);
          break;
        case 4:
          msb=rdc_patt_gen_seq_6(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_5(); if(__err) return(__err);
          break;
        case 5:
          msb=rdc_patt_gen_seq_4(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_3(); if(__err) return(__err);
          break;
        case 6:
          msb=rdc_patt_gen_seq_2(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_1(); if(__err) return(__err);
          break;
        case 7:
          msb=rdc_patt_gen_seq_0(); if(__err) return(__err);
          lsb=0;
          break;
        /* Its a dead default and cause coverity defect
        default:
              PHYMOD_DEBUG_ERROR(("Wrong index value : Should be between 0 and 7\n"));
          return ERR_CODE_DIAG;
          break;*/
      }

      pattern[pattern_idx] = (msb <<16) | lsb;
  }

  return ERR_CODE_NONE;
}

int eagle_tsce_vco_setup(const phymod_access_t *pa, phymod_ref_clk_t ref_clk, eagle_tsce_vco_enum_t vco)
{
    switch (ref_clk) {
         case phymodRefClk156Mhz :
              LEFUN(wrc_refclk_sel       (0x3));
              switch (vco){
                  case VCO_10p3125 :
                      LEFUN(wr_ln_dp_s_rstb              (0x0));
                      LEFUN(wrc_pll_mode                 (0xA));
                      //LEFUN(wr_ln_dp_s_rstb              (0x1));
                      break;
                  case VCO_12p5   :
                      LEFUN(wr_ln_dp_s_rstb              (0x0));
                      LEFUN(wrc_pll_mode (0xD));
                      //LEFUN(wr_ln_dp_s_rstb(0x1));
                      break;
                  default :
                      break;
               }
               break;
         case phymodRefClk50Mhz :
              LEFUN(wrc_refclk_sel (0x6));
              switch (vco){
                  case VCO_10p3125 :
                      LEFUN(wr_ln_dp_s_rstb (0x0));
                      LEFUN(wrc_ams_pll_fracn_sel (0x1));
                      LEFUN(wrc_ams_pll_fracn_ndiv_int (0x0ce));
                      LEFUN(wrc_ams_pll_fracn_div_h (0x1));
                      LEFUN(wrc_ams_pll_fracn_div_l (0x0000));
                      //LEFUN(wr_ln_dp_s_rstb //(0x1));
                      break;
                  case VCO_12p5 :
                      LEFUN(wr_ln_dp_s_rstb (0x0));
                      LEFUN(wrc_ams_pll_fracn_sel (0x1));
                      LEFUN(wrc_ams_pll_fracn_ndiv_int (0x0FA));
                      LEFUN(wrc_ams_pll_fracn_div_h (0x0));
                      LEFUN(wrc_ams_pll_fracn_div_l (0x0000));
                      //LEFUN(wr_ln_dp_s_rstb (0x1));
                      break;
                  default:
                      break;
              }
              break;
         default:
              break;
    }
    LEFUN(wr_tx_pcs_native_ana_frmt_en (0x1));
    return (ERR_CODE_NONE);
}


err_code_t tsce_phy_pcs_forced_speed_setup(const phymod_access_t *pa, temod_spd_intfc_type  speed)
{
    LEFUN(wr_credit_sw_en(0x0));
    LEFUN(wr_SW_actual_speed_force_en(0x1));

    switch(speed){
         case TEMOD_SPD_10M                       :
             LEFUN(wr_SW_actual_speed(0x0));
             LEFUN(wr_replication_cnt(0x1));
             break;
         case TEMOD_SPD_100M                      :
             LEFUN(wr_SW_actual_speed(0x1));
             break;
         case TEMOD_SPD_1G                        :
             LEFUN(wr_SW_actual_speed(0x2));
             break;
         case TEMOD_SPD_2p5G                      :
             LEFUN(wr_spd_2p5G_sel(1));
             LEFUN(wr_SW_actual_speed(0x3));
             break;
         case TEMOD_SPD_5G :
             LEFUN(wr_SW_actual_speed(0x10));
             break;
         case TEMOD_SPD_10G_FEC_OFF :
             LEFUN(wr_SW_actual_speed (0xf));
             LEFUN(wr_fec_enable (0x0));
             break;
         case TEMOD_SPD_10G_FEC_ON :
             LEFUN(wr_SW_actual_speed (0xf));
             LEFUN(wr_fec_enable (0x1));
             LEFUN(wr_block_sync_mode (0x4));
             break;
         default : PHYMOD_DEBUG_ERROR(("ERROR: tsce_phy_pcs_force_speed_setup(..) called for unsupported speed\n"));
             break;
    }
    LEFUN(wr_rstb_lane(0x1));
    LEFUN(wr_mac_creditenable(0x1));
    LEFUN(wr_rstb_tx_lane(0x1));
    LEFUN(wr_enable_tx_lane(0x1));
    return (ERR_CODE_NONE);
}


err_code_t tsce_phy_pcs_an_timer_speed_up(const phymod_access_t *pc)
{
    AN_X1_CL37_RESTARTr_t                reg_cl37_restart;
    AN_X1_CL37_ACKr_t                    reg_cl37_ack;
    AN_X1_CL37_ERRr_t                    reg_cl37_err;
    AN_X1_CL73_BRK_LNKr_t                reg_cl73_break_link;
    AN_X1_CL73_ERRr_t                    reg_cl73_err;
    AN_X1_CL73_DME_LOCKr_t               reg_cl73_dme_lock;
    AN_X1_LNK_UPr_t                      reg_link_up;
    AN_X1_LNK_FAIL_INHBT_TMR_CL72r_t     reg_inhibit_timer;
    AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_t reg_inhibit_not_timer;
    AN_X1_CL37_SYNC_STS_FILTER_TMRr_t    reg_sync_status_filter_timer;
    AN_X1_IGNORE_LNK_TMRr_t              reg_ignore_link_timer;

    /*0x9250 AN_X1_TIMERS_cl37_restart */
    AN_X1_CL37_RESTARTr_CLR(reg_cl37_restart);
    AN_X1_CL37_RESTARTr_SET(reg_cl37_restart, 0x29a );
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_RESTARTr(pc, reg_cl37_restart));

    /*0x9251 AN_X1_TIMERS_cl37_ack */
    AN_X1_CL37_ACKr_CLR(reg_cl37_ack);
    AN_X1_CL37_ACKr_SET(reg_cl37_ack,  0x29a);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_ACKr(pc, reg_cl37_ack));

    /*0x9252 AN_X1_CL37_ERR */
    AN_X1_CL37_ERRr_CLR(reg_cl37_err);
    AN_X1_CL37_ERRr_SET(reg_cl37_err,  0xA000);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_ERRr(pc, reg_cl37_err));

    /*0x9253 AN_X1_TIMERS_cl73_break_link */
    AN_X1_CL73_BRK_LNKr_CLR(reg_cl73_break_link);
    AN_X1_CL73_BRK_LNKr_SET(reg_cl73_break_link, 0x10ee);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_BRK_LNKr(pc, reg_cl73_break_link));

    /*0x9254 AN_X1_CL73_ERR */
    AN_X1_CL73_ERRr_CLR(reg_cl73_err);
    AN_X1_CL73_ERRr_SET(reg_cl73_err,  0xA000);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_ERRr(pc, reg_cl73_err));

    /*0x9255 AN_X1_TIMERS_cl73_dme_lock*/
    AN_X1_CL73_DME_LOCKr_CLR(reg_cl73_dme_lock);
    AN_X1_CL73_DME_LOCKr_SET(reg_cl73_dme_lock, 0x0BB8);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_DME_LOCKr(pc, reg_cl73_dme_lock));

    /*0x9256 AN_X1_TIMERS_link_up*/
    AN_X1_LNK_UPr_CLR(reg_link_up);
    AN_X1_LNK_UPr_SET(reg_link_up, 0x1B00);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_UPr(pc, reg_link_up));

    /*0x9257 AN_X1_TIMERS_link_fail_inhibit_timer_cl72*/
    AN_X1_LNK_FAIL_INHBT_TMR_CL72r_CLR(reg_inhibit_timer);
    AN_X1_LNK_FAIL_INHBT_TMR_CL72r_SET(reg_inhibit_timer, 0x8235);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, reg_inhibit_timer));

    /*0x9258 AN_X1_TIMERS_link_fail_inhibit_timer_not_cl72*/
    AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_CLR(reg_inhibit_not_timer);
    AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_SET(reg_inhibit_not_timer, 0x8235);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, reg_inhibit_not_timer));

    /*0x925a AN_X1_TIMERS_cl72_max_wait_timer*/
    AN_X1_CL37_SYNC_STS_FILTER_TMRr_CLR(reg_sync_status_filter_timer);
    AN_X1_CL37_SYNC_STS_FILTER_TMRr_SET(reg_sync_status_filter_timer, 0x8235);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_SYNC_STS_FILTER_TMRr(pc, reg_sync_status_filter_timer));

    /*0x925c AN_X1_TIMERS_ignore_link_timer*/
    AN_X1_IGNORE_LNK_TMRr_CLR(reg_ignore_link_timer);
    AN_X1_IGNORE_LNK_TMRr_SET(reg_ignore_link_timer, 0x000F);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_IGNORE_LNK_TMRr(pc, reg_ignore_link_timer));
    return PHYMOD_E_NONE;
}


/*err_code_t eagle_phy_1_phy_pcs_devad_setup(enum pcs_devad devad)  */
/*{                                                                  */
/*    uint16_t aer_value;                                            */
/*    uint8_t  devad;                                   */
/*    aer_value = rdc_mdio_aer();                       */
/*    switch(devad){                                    */
/*       case DEVAD_CL22    : aer_value[15:11] = 0x0;   */
/*       case DEVAD_PMA_PMD : aer_value[15:11] = 0x1;   */
/*       case DEVAD_CL73    : aer_value[15:11] = 0x7;   */
/*       case DEVAD_PHY     : aer_value[15:11] = 0x4;   */
/*       case DEVAD_DTE     : aer_value[15:11] = 0x5;   */
/*       case DEVAD_PCS     : aer_value[15:11] = 0x3;   */
/*    }                                                 */
/*    LEFUN(wrc_mdio_aer(aer_value));                    */
/*                                                      */
/*    return (ERR_CODE_NONE);                           */
/*}                                                     */

err_code_t tsce_phy_pcs_an_speed_setup(const phymod_access_t *pa, temod_spd_intfc_type speed)
{

    switch(speed){
        case TEMOD_SPD_73AN_IEEE_KR_10G_FEC_ABL  :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x0));
            LEFUN(wr_cl45_BASE_10GBASE_KR          (0x1));
            LEFUN(wr_selector                      (0x1));
            LEFUN(wr_fec_requested                 (0x1));
            LEFUN(wr_AN_enable                     (0x1));
            break;
        case TEMOD_SPD_73AN_IEEE_KR_10G_FEC_REQ  :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x0));
            LEFUN(wr_cl45_BASE_10GBASE_KR          (0x1));
            LEFUN(wr_selector                      (0x1));
            LEFUN(wr_fec_requested                 (0x3));
            LEFUN(wr_AN_enable                     (0x1));
            break;
        case TEMOD_SPD_73AN_USER_KR_10G_FEC_ABL  :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_BASE_10GBASE_KR               (0x1));
            LEFUN(wr_base_selector                 (0x1));
            LEFUN(wr_fec_requested                 (0x1));
            LEFUN(wr_cl73_enable                   (0x1));
            break;
        case TEMOD_SPD_73AN_USER_KR_10G_FEC_REQ  :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_BASE_10GBASE_KR               (0x1));
            LEFUN(wr_base_selector                 (0x1));
            LEFUN(wr_fec_requested                 (0x3));
            LEFUN(wr_cl73_enable                   (0x1));
            break;
        case TEMOD_SPD_73AN_IEEE_KX_1G           :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x0));
            LEFUN(wr_cl45_BASE_1000BASE_KX         (0x1));
            LEFUN(wr_selector                      (0x1));
            LEFUN(wr_AN_enable                     (0x1));
            break;
        case TEMOD_SPD_73AN_USER_KX_1G           :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_BASE_1000BASE_KX              (0x1));
            LEFUN(wr_base_selector                 (0x1));
            LEFUN(wr_cl37_enable                   (0x1));
            break;
        case TEMOD_SPD_37AN_IEEE_KX_1G           :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x1));
            LEFUN(wr_full_duplex                   (0x1));
            LEFUN(wr_AN_enable                     (0x1));
            break;
        case TEMOD_SPD_37AN_USER_KX_1G           :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_cl37_full_duplex              (0x1));
            LEFUN(wr_cl37_enable                   (0x1));
            break;
        case TEMOD_SPD_37AN_SGMII_M_1G           :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_sgmii_full_duplex             (0x1));
            LEFUN(wr_sgmii_speed                   (0x2));
            LEFUN(wr_sgmii_master_mode             (0x1));
            LEFUN(wr_cl37_sgmii_enable             (0x1));
            LEFUN(wr_cl37_enable                   (0x1));
            break;
        case TEMOD_SPD_37AN_SGMII_M_100M         :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_sgmii_full_duplex             (0x1));
            LEFUN(wr_sgmii_speed                   (0x1));
            LEFUN(wr_sgmii_master_mode             (0x1));
            LEFUN(wr_cl37_sgmii_enable             (0x1));
            LEFUN(wr_cl37_enable                   (0x1));
            break;
        case TEMOD_SPD_37AN_SGMII_M_10M          :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_sgmii_full_duplex             (0x1));
            LEFUN(wr_sgmii_speed                   (0x0));
            LEFUN(wr_sgmii_master_mode             (0x1));
            LEFUN(wr_cl37_sgmii_enable             (0x1));
            LEFUN(wr_cl37_enable                   (0x1));
            break;
        case TEMOD_SPD_37AN_SGMII_S_10M_100M_1G  :
            LEFUN(wr_use_ieee_reg_ctrl_sel         (0x2));
            LEFUN(wr_cl37_sgmii_enable             (0x1));
            LEFUN(wr_cl37_enable                   (0x1));
            break;
        default :
            PHYMOD_DEBUG_ERROR(("ERROR: tsce_phy_pcs_an_speed_setup(..) called for unsupported speed\n"));
            break;

    }

    LEFUN(wr_credit_sw_en                     (0x0));
    LEFUN(wr_SW_actual_speed_force_en         (0x0));
    LEFUN(wr_rstb_lane                        (0x1));
    LEFUN(wr_mac_creditenable                 (0x1));
    LEFUN(wr_rstb_tx_lane                     (0x1));
    LEFUN(wr_enable_tx_lane                   (0x1));

    return (ERR_CODE_NONE);
}


int tsce_phy_pcs_speed_get(const phymod_access_t *pa, uint32_t *speed)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    *speed = rd_SW_actual_speed();
    return __err;
}

int tsce_phy_pcs_speed_set(const phymod_access_t *pa, temod_spd_intfc_type speed)
{

    err_code_t __err = ERR_CODE_NONE; /* PHY Forced/AN Speed Configuration */
    switch(speed) {
        case TEMOD_SPD_10M                      :
        case TEMOD_SPD_100M                     :
        case TEMOD_SPD_1G                       :
        case TEMOD_SPD_2p5G                     :
        case TEMOD_SPD_5G                       :
        case TEMOD_SPD_10G_FEC_OFF              :
        case TEMOD_SPD_10G_FEC_ON               :
            __err = tsce_phy_pcs_forced_speed_setup(pa,speed);
            break;

         case TEMOD_SPD_73AN_IEEE_KR_10G_FEC_ABL :
         case TEMOD_SPD_73AN_IEEE_KR_10G_FEC_REQ :
         case TEMOD_SPD_73AN_USER_KR_10G_FEC_ABL :
         case TEMOD_SPD_73AN_USER_KR_10G_FEC_REQ :
         case TEMOD_SPD_73AN_IEEE_KX_1G          :
         case TEMOD_SPD_73AN_USER_KX_1G          :
         case TEMOD_SPD_37AN_IEEE_KX_1G          :
         case TEMOD_SPD_37AN_USER_KX_1G          :
         case TEMOD_SPD_37AN_SGMII_M_1G          :
         case TEMOD_SPD_37AN_SGMII_M_100M        :
         case TEMOD_SPD_37AN_SGMII_M_10M         :
         case TEMOD_SPD_37AN_SGMII_S_10M_100M_1G :
             __err = tsce_phy_pcs_an_timer_speed_up(pa);
             __err = tsce_phy_pcs_an_speed_setup(pa,speed);
                 break;
        default :
            PHYMOD_DEBUG_ERROR(("ERROR: eagle_phy_1_phy_pcs_speed_setup(..) called for unsupported speed\n"));
            break;
    }
    return (__err);
}

err_code_t eagle_phy_data_path_reset_release(const phymod_access_t *pa, uint32_t enable)
{
    LEFUN(wrc_core_dp_s_rstb(enable));
    LEFUN(wr_ln_dp_s_rstb(enable));
    return ERR_CODE_NONE;
}

err_code_t eagle_phy_change_speed_reset(const phymod_access_t *pa )
{
    uint32_t  reg = 0;
    uint32_t addr = 0x0000;
    phymod_tsc_iblk_read(pa, addr, &reg);
    reg |= 0x8000;
    phymod_tsc_iblk_write(pa, addr, reg);
#if 0
    LEFUN(wr_rst_hw(0x1));
#endif
    LEFUN(wr_ln_dp_s_rstb(0x0));
    LEFUN(wr_tx_pcs_native_ana_frmt_en(0x1));
    return (ERR_CODE_NONE);
}

/*
@brief   Init the PMD
@param   pmd_touched If the PMD is already initialized
@returns The value ERR_CODE_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
err_code_t eagle_phy_pmd_setup (const phymod_access_t *pa, int pmd_touched, const phymod_core_init_config_t* init_config)
{
    uint8_t refclk_sel_val;
    enum eagle_tsc_pll_enum pll_cfg;

    pll_cfg = EAGLE_TSC_pll_div_66x;
    /* Identify the Reference Clock sel value to be used based on the the core_init_config */
    switch (init_config->interface.ref_clock) {
      case phymodRefClk125Mhz:
           refclk_sel_val = 0x2;
           break;
      case phymodRefClk156Mhz:
           refclk_sel_val = 0x3;
           if (init_config->interface.data_rate == 2500) {
               pll_cfg = EAGLE_TSC_pll_div_80x_refc156;
           } else {
               pll_cfg = EAGLE_TSC_pll_div_66x;
           }
           break;
      case phymodRefClk161Mhz:
           refclk_sel_val = 0x5;
           break;
      case phymodRefClk50Mhz:
          refclk_sel_val = 0x6;
          if (init_config->interface.data_rate == 2500) {
              pll_cfg = EAGLE_TSC_pll_div_80x_refc156;
          } else {
              pll_cfg = EAGLE_TSC_pll_div_66x;
          }
          break;
      case phymodRefClk106Mhz:
          refclk_sel_val = 0x7;
          break;
      default:
          refclk_sel_val = 0x3;
          pll_cfg = EAGLE_TSC_pll_div_66x;
          break;
    }

    if (pmd_touched == 0) {
        /* Programm Refclk_sel */
        LEFUN(wrc_refclk_sel(refclk_sel_val));
       /* Bypass PMD Tx Oversampling */
        LEFUN(wr_tx_pcs_native_ana_frmt_en(1));
        LEFUN(wrc_core_dp_s_rstb(0));
        LEFUN(eagle_tsc_configure_pll(pa,pll_cfg));
#if 0
        // Do not release the reset of core dp - until core configurations are written into ram for 8051
        //sal_printf ("Out of reset wrc_core_dp_s_rstb \n");
        //LEFUN(wrc_core_dp_s_rstb(1));
        ////LEFUN(wrc_pmd_core_dp_s_rstb(1));
        //sal_printf ("Done eagle_phy_1_pmd_reset_seq... \n");
#endif
    }
    return (ERR_CODE_NONE);
}

err_code_t eagle_phy_pcs_12p5_vco_setup(const phymod_access_t *pa)
{
   LEFUN(wrc_os_mode_cl36_2500m                        (0x4));
   LEFUN(wrc_os_mode_cl36_1000m                        (0x8));
   LEFUN(wrc_os_mode_cl36_100m                         (0x8));
   LEFUN(wrc_os_mode_cl36_10m                          (0x8));

   LEFUN(wrc_osr_mode_cl73                             (0x9));
   LEFUN(wrc_osr_mode_cl36                             (0x9));
   LEFUN(wrc_osr_mode_cl36_2p5                         (0x4));
   LEFUN(wrc_osr_mode_cl49                             (0x0));

   LEFUN(wrc_reg1G_ClockCount0                         (0x19));
   LEFUN(wrc_reg1G_CGC                                 (0x6));
   LEFUN(wrc_reg1G_loopcnt0                            (0x1));
   LEFUN(wrc_reg1G_ClockCount1                         (0x0));
   LEFUN(wrc_reg1G_loopcnt1                            (0x0));
   LEFUN(wrc_reg1G_modulo                              (0x5));

   LEFUN(wrc_reg100M_ClockCount0                       (0x7d));
   LEFUN(wrc_reg100M_CGC                               (0x1f));
   LEFUN(wrc_reg100M_loopcnt0                          (0x1));
   LEFUN(wrc_reg100M_ClockCount1                       (0x0));
   LEFUN(wrc_reg100M_loopcnt1_Hi                       (0x0));
   LEFUN(wrc_reg100M_loopcnt1_Lo                       (0x0));
   LEFUN(wrc_reg100M_PCS_ClockCount0                   (0x32));
   LEFUN(wrc_reg100M_PCS_CGC                           (0x31));
   LEFUN(wrc_reg100M_modulo                            (0x1f));

   LEFUN(wrc_reg10M_ClockCount0                        (0x271));
   LEFUN(wrc_reg10M_CGC                                (0x138));
   LEFUN(wrc_reg10M_loopcnt0                           (0x1));
   LEFUN(wrc_reg10M_ClockCount1                        (0x0));
   LEFUN(wrc_reg10M_loopcnt1_Hi                        (0x0));
   LEFUN(wrc_reg10M_loopcnt1_Lo                        (0x0));
   LEFUN(wrc_reg10M_PCS_ClockCount0                    (0x32));
   LEFUN(wrc_reg10M_PCS_CGC                            (0x31));
   LEFUN(wrc_reg10M_modulo                             (0x138));

   return (ERR_CODE_NONE);
}
err_code_t eagle_pmd_an_get(const phymod_access_t *pa, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    uint16_t an_en = 0;
    uint32_t  reg = 0;
    *an_done = 0;
    MII_STATUSr_t mii_status;
    LEFUN(READ_MII_STATUSr(pa, &mii_status));
    phymod_tsc_iblk_read(pa, 0x7003c480, &reg);
    an_en = ((reg >> 6) & 0x1);
    if (an_en) {
        *an_done = MII_STATUSr_AN_COMPLETEf_GET(mii_status);
    }
    return PHYMOD_E_NONE;
}

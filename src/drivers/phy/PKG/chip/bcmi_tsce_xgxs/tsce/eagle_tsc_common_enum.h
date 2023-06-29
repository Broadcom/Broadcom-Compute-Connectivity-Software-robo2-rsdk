/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: srds_api_enum.h 898 2015-02-04 23:22:17Z eroes $ *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 *                                                                            *
 *  $Copyright: (c) 2017 Broadcom Corporation                                 *
 *  All Rights Reserved$                                                      *
 *  No portions of this material may be reproduced in any form without        *
 *  the written permission of:                                                *
 *      Broadcom Corporation                                                  *
 *      5300 California Avenue                                                *
 *      Irvine, CA  92617                                                     *
 *                                                                            *
 *  All information contained in this document is Broadcom Corporation        *
 *  company private proprietary, and trade secret.                            *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

/** @file srds_api_enum.h
 * Enum types used by Serdes API functions
   This file contains Enum's which are common to all serdes cores
 */

#ifndef SRDS_API_ENUM_H
#define SRDS_API_ENUM_H

/** PRBS Polynomial Enum */
enum srds_prbs_polynomial_enum {
  PRBS_7  = 0,
  PRBS_9  = 1,
  PRBS_11 = 2,
  PRBS_15 = 3,
  PRBS_23 = 4,
  PRBS_31 = 5,
  PRBS_58 = 6
};


/** PRBS Checker Mode Enum */
enum srds_prbs_checker_mode_enum {
  PRBS_SELF_SYNC_HYSTERESIS       = 0,
  PRBS_INITIAL_SEED_HYSTERESIS    = 1,
  PRBS_INITIAL_SEED_NO_HYSTERESIS = 2
};


/** TX_PI Fixed Frequency and Jitter Generation Modes Enum */
enum srds_tx_pi_freq_jit_gen_enum {
  TX_PI_FIXED_FREQ      = 0,
  TX_PI_SSC_LOW_FREQ    = 1,
  TX_PI_SSC_HIGH_FREQ   = 2,
  TX_PI_SJ              = 3
};

/** RX AFE Settings Enum */
enum srds_rx_afe_settings_enum {
    RX_AFE_PF,
    RX_AFE_PF2,
    RX_AFE_VGA,
    RX_AFE_DFE1,
    RX_AFE_DFE2,
    RX_AFE_DFE3,
    RX_AFE_DFE4,
    RX_AFE_DFE5,
    RX_AFE_DFE6,
    RX_AFE_DFE7,
    RX_AFE_DFE8,
    RX_AFE_DFE9,
    RX_AFE_DFE10,
    RX_AFE_DFE11,
    RX_AFE_DFE12,
    RX_AFE_DFE13,
    RX_AFE_DFE14
};

/** TX AFE Settings Enum */
enum srds_tx_afe_settings_enum {
    TX_AFE_PRE,
    TX_AFE_MAIN,
    TX_AFE_POST1,
    TX_AFE_POST2,
    TX_AFE_POST3,
    TX_AFE_AMP,
    TX_AFE_DRIVERMODE
};

/** AFE Drivermode Enum */
enum srds_afe_drivermode_enum {
    DM_DEFAULT = 0,
    DM_NOT_SUPPORTED,
    DM_HALF_AMPLITUDE,
    DM_HALF_AMPLITUDE_HI_IMPED
};

/** Repeater Mode Enum */
enum srds_rptr_mode_enum {
    DATA_IN_SIDE,
    DIG_LPBK_SIDE
};

/** Core Powerdown Modes Enum */
enum srds_core_pwrdn_mode_enum {
    PWR_ON,
    PWRDN,
    PWRDN_DEEP,
    PWRDN_TX,
    PWRDN_RX
};

/** Evenet Log Display Mode Enum */
enum srds_event_log_display_mode_enum {
    EVENT_LOG_HEX,
    EVENT_LOG_DECODED,
    EVENT_LOG_HEX_AND_DECODED,
    EVENT_LOG_MODE_MAX
};

/** Eye Scan Direction Enum */
enum srds_eye_scan_dir_enum {
      EYE_SCAN_HORIZ = 0,
      EYE_SCAN_VERTICAL = 1
};

#ifdef OLT_MAPLE
/** XPON Lane Mode Enum */
enum xpon_lane_mode_enum {
    XPON_10G_EPON,
    XPON_1G_EPON,
    XPON_1G_GPON,
    XPON_2G_XGPON,
    XPON_XFI
};

/** XPON RX Termination Enum */
enum xpon_rx_term_enum {
    XPON_DC_10G_EPON,
    XPON_DC_10G_EPON_BW,
    XPON_XGPON,
    XPON_MODE_4,
    XPON_DC_1G_EPON,
    XPON_AC_10G_EPON,
    XPON_AC_10G_EPON_BW,
    XPON_AC_XGPON
};
#endif
#endif

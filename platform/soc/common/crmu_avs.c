/*
 * Copyright 2017 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 */
#include "shared/system.h"
#include "soc/avs.h"
#include "soc/error.h"
#include "soc/crmu_avs.h"
#include <soc/robo2/common/memregs.h>
#include <soc/drv.h>

/**********************************************************************************************************************
 Constants & macros
**********************************************************************************************************************/
/* Robo Avenger has 36 cent rosc, so 2 should be enough for BitMap
 * Robo Avenger has 10 rmt rosc, so 1 should be enough for BitMap
 */
#ifdef BCM_AVENGER_SUPPORT
#define AVS_NUM_CENT_XBMP   2
#define AVS_NUM_RMT_XBMP    1
#else
#define AVS_NUM_CENT_XBMP   4
#define AVS_NUM_RMT_XBMP    8
#endif

/* This creates an unsigned integer from a float with the precision
 * defined above
 */
#define AVS_UINT(_x_) ((uint32_t)((_x_)*AVS_S1))
/* This creates a signed integer from a float with the precision
 * defined above
 */
#define AVS_INT(_x_)  ((int32_t)((_x_)*AVS_S1))
/* Transfer a micro voltage integer to 2 parts:
 * ignore the least significant 2 digits, then divide 10000,
 * the 1st is the integer part, 2nd is fractional part
 * 1123456uv => 11234 => 1, 1234 => 1.1234v
 * 976000uv =>   9760 => 0, 9760 => 0.9760v
 */
#define AVS_INT_FRAC_2PT_4(x)   (x)/1000000, ((x)/100)%10000

#define AVS_MAX(a,b)  ((a) > (b)  ? (a) : (b))
#define AVS_MIN(a,b)  ((a) < (b)  ? (a) : (b))
#define AVS_ABS(x)    (((x) >= 0) ? (x) : -(x))

#define AVS_ASSERT(x) do{if(!(x)) { sal_printf("Oops!\n"); while(1);}}while(0)

#define AVS_OSC_EXCLUDED(i, xbmp) (((((xbmp) >> ((i)%32)) & 0x1)==0)? AVS_FALSE : AVS_TRUE)

#define AVS_SOC_IF_NULL_RETURN(arg) do{ if((arg) == NULL) \
                   {sal_printf("NULL arg at %d\n", __LINE__); return (SOC_E_PARAM);} \
                   }while(0)

#define AVS_ABS_MIN_CVOLT                 AVS_UINT(0.80)  /* 0.8 V, absolute min core voltage in 0.1mv */
#define AVS_ABS_MAX_CVOLT                 AVS_UINT(1.1)   /* 1.10 V, absolute max core voltage in 0.1mv */
#define AVS_DEFAULT_CVOLT_INC_STEP_SIZE   AVS_UINT(0.005) /* 0.01 V = 10 mV, inc step voltage in 0.1mv */
#define AVS_DEFAULT_CVOLT_DEC_STEP_SIZE   AVS_UINT(0.005) /* 0.01 V = 10 mV, dec step voltage in 0.1mv */

#define AVS_TRACK_VINC_STEP               AVS_INT(0.006)  /* 6 mV */
#define AVS_TRACK_VDEC_STEP               AVS_INT(-0.002) /* -3 mV */

#define AVS_ROSC_CONTROL_INIT_VALUE       0x7FFF00CF
/* Assumes a total of 40mV for dynamic IR drop at the local memories
 * and voltage measurement accuracy
 */
#define AVS_VMIN_LVM                      AVS_UINT(0.94)
/* 10 mV, if  (Vhigh - Vlow) < this value, don't do AVS on this part */
#define AVS_VHI_VLO_MIN_DIFF              AVS_UINT(.010)
/* to determine v1s, v2s - local_fit
 * across all splits, voltage is less than 1V,
 * so lower AVS_VHIGH for better local fit, was 1.02V
 */
#define AVS_VHIGH                         AVS_UINT(0.98)
#define AVS_V1_MINUS_V2                   AVS_UINT(0.03)  /* 30 mV */
#define AVS_VLOW_SS                       AVS_INT(0.94)   /* lowest slow-slow part voltage */
#define AVS_V2S_INC_FOR_2ND_PASS          AVS_UINT(0.028) /* 28 mV */
/* 16*0.88  = 14.08 mV, used by soc_avs_set_new_thresholds */
/*#define AVS_VINC_FOR_SET_NEW_THR        AVS_UINT(16*0.088)*/
#define AVS_VINC_FOR_SET_NEW_THR          AVS_UINT(0.01408) /* 14.08 mV, used by soc_avs_set_new_thresholds */

/* Use a place holder of 15mV for now. We have measured a few parts
 * and the Vmon voltage varies by less than 10mV between IDLE and
 * traffic running
 */
#define AVS_VINC_FOR_IR_DROP_AVENGER_A0              AVS_UINT(0.015) /* additional margin, IR drop */
#define AVS_VINC_FOR_IR_DROP_AVENGER_B0              AVS_UINT(0.009) /* additional margin, IR drop */

/* AVS_DAC_WRITE_DELAY + extra_delay? */
#define AVS_SEQUENCER_DELAY               (32U)

/* Convert voltage in V to 0.1mV, i.e. 1.1V => 11000 mV
 * make this larger to get more precision in the numbers
 * (but be careful of overflow)
 */
#define AVS_SCALING_FACTOR                (10000)
/* WARNING: DO NOT CHANGE THIS -- it MUST match the definition used
 * to build the library
 */
#define AVS_S1                          AVS_SCALING_FACTOR
#define AVS_S2                          AVS_SCALING_FACTOR

#define AVS_ALL_ONES                    0xFFFFFFFF
#define AVS_SKIP                        0xfafafafa

#define AVS_ROSC_COUNT_MODE_1EDGE       1 /* 1 is for one-edge (only rise edge) */
#define AVS_ROSC_COUNT_MODE_2EDGES      0 /* 0 is for 2 edges (both rise, fall) */
#define AVS_ROSC_COUNT_MODE             AVS_ROSC_COUNT_MODE_1EDGE
#define AVS_ROSC_EDGE_COUNT             ((AVS_ROSC_COUNT_MODE == AVS_ROSC_COUNT_MODE_1EDGE)? 1U : 2U)

#define AVS_MEASUREMENT_TIME_CONTROL    (127U)      /* 0x7F, MSB of measure time control */
/* freq in MHz for reference_clock that establishes the measurement_interval */
#define AVS_REF_CLK_COUNTER_MAX         (AVS_MEASUREMENT_TIME_CONTROL*256U + 255U)
#define AVS_REF_CLK_FREQ                (25U)
/* there is a divide_by_2 for cent_rosc_counter in NTSW chips,
 * so real_count_value = 2*value_that_we_read
 * dominic: from data collected, divide_by_2 is removed,
 * confirmed by avs_monitor2 spec
 */
#define AVS_CENT_OSC_COUNT_MULTIPLIER   (1U)

/* We need the frequency units to be in 10K but multiplying by 10K will
 * cause the overflow in frequency calculation.
 * So use 1K before division and multiply by 10 after.
 */
#define AVS_FREQ_DIVIDER                (1000U)
#define AVS_FREQ_DIVIDER_ADJUSTMENT     (10U)

#define AVS_REMOTE_DELAY                (0U)

/* Number of bits in exclude bit map */
#define NUM_BITS_PER_XBMP               (32U)
/* if don't see done/valid then re-read the results this many times */
#define AVS_MAX_READ_RETRYS             (10U)
/* micro volts per volt */
#define UV_PER_V                        (1000000U)
/* milli volts per volt */
#define MV_PER_V                        (1000U)
#define SWREG_BASE_0P45V_UV             ((uint32_t)(UV_PER_V*(0.45)))
#define SWREG_ADJ_STEP_UV               ((uint32_t)(MV_PER_V*(3.125)))
#define SWREG_MIN_VOLTAGE_UV            (SWREG_BASE_0P45V_UV)
#define SWREG_MAX_VOLTAGE_UV            (SWREG_BASE_0P45V_UV + 0xff * SWREG_ADJ_STEP_UV)

#define AVS_LEN_VOLT_STR                (4 + 2 + 1)
#define AVS_WAIT_MSEC_AFTER_SCV_BB      10

/* Feature / Option FLAGs */
/* Avenger pvtmon is not mature, reading cent osc count return 0 at init, so disable it */
#define AVS_DISABLE_PVTMON_AT_INIT
/* find and set the thresholds (moved from AVS Track) */
#define AVS_SET_THRESHOLDS
/* #undef AVS_SET_THRESHOLDS */

/* enable to set the LVM flag */
#define AVS_USE_LVM_FLAG
/*#undef AVS_USE_LVM_FLAG*/

#define AVS_ENABLE_CLI
/* do not read & write regs for disabled/exclued osc */
#define DO_NOT_ACCESS_XOSC

/* Macros for Register Operations */
#define dsb() __asm__ __volatile__("dsb" : : : "memory")
#define cpu_reg32_rd(addr)         (*((volatile uint32_t*)(addr)))
#define cpu_reg32_wr(addr, data)   \
        do {\
                *((volatile uint32_t *)(addr)) = (uint32_t) (data); \
                /*dsb();*/\
        } \
        while(0)

/* Calculation of Oscillator Frequency is same for both Central & Remote
 * Oscillators No Idea why Cygnus code has two different functions.
 */
#define soc_avs_calc_cent_osc_freq  soc_avs_calc_osc_freq
#define soc_avs_calc_rmt_osc_freq   soc_avs_calc_osc_freq

/* all based on 250 MHz clk */
#ifndef CONFIG_EXTERNAL_HOST
#define ONE_MICRO_SEC       (2*250)
#else /* CONFIG_EXTERNAL_HOST  */
#define ONE_MICRO_SEC       (1) /* Input to sal_usleep */
#endif
#define ONE_MILLI_SEC       (ONE_MICRO_SEC*1000)
#define ONE_SECOND          (ONE_MILLI_SEC*1000)

/**********************************************************************************************************************
 Private Type Definitions
**********************************************************************************************************************/
typedef enum {
    AVS_FALSE = 0,
    AVS_TRUE = 1
} avs_bool_t;

typedef enum {
    AVS_ROSC_RMT = 0,
    AVS_ROSC_CENT = 1
} avs_rosc_type_t;

typedef struct {
    /* soc_chip_e unit_type; */
    int first_cent;
    int num_centrals;
    uint32_t cent_xbmp[AVS_NUM_CENT_XBMP];
    int first_rmt;
    int num_remotes;
    uint32_t rmt_xbmp[AVS_NUM_RMT_XBMP];
    int32_t vmin_avs;
    int32_t vmax_avs;
    int32_t vmargin_low;
    int32_t vmargin_high;
    int32_t vmargin_110C;
    int32_t vpred;
} avs_info_t;

/**********************************************************************************************************************
  Set & Get Voltage Functions brought over from avs.c - TBD
**********************************************************************************************************************/
extern uint32 SetAvsVoltage(int unit, uint32 nominal_voltage, uint32 dac_code);
extern uint32 AvsReadVoltage(int unit, uint32 count);
/**********************************************************************************************************************
 Private Functions Declarations & Globals
**********************************************************************************************************************/
static avs_info_t soc_avs_info;
/* Private Global to remember last_set_voltage in 0.1mV Units */
static uint32_t soc_last_set_voltage;

static uint32_t cli_rmt_xbmp[AVS_NUM_RMT_XBMP];
static uint32_t cli_cent_xbmp[AVS_NUM_CENT_XBMP];
#if 0
/* Table of Frequency Thresholds:
 * Currently these are not used, but just in case we will use them in future
 * after obtaining new measurements
 */
static uint32_t reffreq[36] = {
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_0), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_1),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_2), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_3),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_4), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_5),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_6), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_7),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_8), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_9),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_10), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_11),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_12), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_13),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_14), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_15),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_16), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_17),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_18), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_19),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_20), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_21),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_22), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_23),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_24), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_25),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_26), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_27),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_28), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_29),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_30), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_31),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_32), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_33),
    AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_34), AVS_UINT(AVS_AVENGER_CENT_FREQ_THRESHOLD_35)
};
#endif
/* Global: Avoidig use of dynamic kmalloc, this global serves as the
 * placeholder to keep the frequency counts read off the ROs during
 * the AVS process.
 * Used in Function:
 * (i) soc_avs_predict_vpred(),
 * (ii) soc_avs_converge_process()
 * (iii) soc_avs_set_new_thr() in lieu of stack storage
 * Assuming maximum number of central and remote oscillators
 * static uint32_t alloc_ptr1[2 * AVS_AVENGER_NUMBER_OF_CENTRALS + 4 * AVS_AVENGER_NUMBER_OF_REMOTES];
 * static uint32_t alloc_ptr2[6 * 32]; <-- This alone should suffice
 * since the array is re-initialized before every use.
 */
static uint32_t alloc_ptr[6 * 32];

/**********************************************************************************************************************
 Private Functions Definitions
**********************************************************************************************************************/
static void avs_fill_uint32_array(uint32_t *ptr, int value, size_t num)
{
    int i;
    for (i = 0; i < num; i++) ptr[i] = value;
}

/* Wait time in milliSeconds
 * Adopted from ROM code
 */
static void soc_avs_msleep(uint32_t msec)
{
    if (msec > 0) {
        uint32 tcount = 0;
        /* one loop will take on average 8 instructions.
         * Note it is 8bit processor
         */
        tcount = (msec * ONE_MILLI_SEC) >> 3 ;
        do {
            tcount = tcount - 1;
        } while (tcount);
    }
}
/* Wait time in microSeconds
 */
static void soc_avs_usleep(uint32_t usec)
{
    if (usec > 0) {
        uint32 tcount = 0;
        /* one loop will take on average 8 instructions.
         * Note it is 8bit processor
         */
        tcount = (usec * ONE_MICRO_SEC) >> 3 ;
        do {
            tcount = tcount - 1;
        } while (tcount);
    }
}

static avs_bool_t soc_avs_check_osc_excluded(int oscid,
                                             avs_rosc_type_t osc_type,
                                             avs_info_t *avs_info)
{
    switch (osc_type) {
        case AVS_ROSC_CENT:
            return AVS_OSC_EXCLUDED(oscid,
                        (cli_cent_xbmp[oscid / NUM_BITS_PER_XBMP] | avs_info->cent_xbmp[oscid / NUM_BITS_PER_XBMP]));
        case AVS_ROSC_RMT:
            return AVS_OSC_EXCLUDED(oscid,
                        (cli_rmt_xbmp[oscid / NUM_BITS_PER_XBMP] | avs_info->rmt_xbmp[oscid / NUM_BITS_PER_XBMP]));
        default:
            iproc_err("Unknow ROSC type:%d\n", osc_type);
            return (AVS_FALSE);
    }
}

static int soc_avs_set_osc_excluded(int oscid, avs_rosc_type_t osc_type)
{
    switch (osc_type) {
    case AVS_ROSC_CENT:
        iproc_prt("cent rosc %d excluded!\n", oscid);
        cli_cent_xbmp[oscid / NUM_BITS_PER_XBMP] |= (1U << (oscid % NUM_BITS_PER_XBMP)); //_cmd_avs_set_cent_xbmp(unit, oscid, 1, 1);
        return 0;
    case AVS_ROSC_RMT:
        iproc_prt("rmt rosc %d excluded!\n", oscid);
        cli_rmt_xbmp[oscid / NUM_BITS_PER_XBMP] |= (1U << (oscid % NUM_BITS_PER_XBMP)); //_cmd_avs_set_rmt_xbmp(unit, oscid, 1, 1);
        return 0;
    default:
        iproc_err("unknow rosc type:%d\n", osc_type);
        return -EINVAL;
    }
}

/*
 * Initialize central, remote osc (set enables, threshold_enables).\n
 *
 * AVS_REG_PMB_SLAVE_AVS_ROSC_CONTROL = 0x7FFF50CF means:
 *      bit    field = value
 *      0      RO_EN_S = 1
 *      1      RO_EN_H = 1
 *      2      ECTR_EN_S = 1
 *      3      ECTR_EN_H = 1
 *      4      THRSH_EN_S = 0
 *      5      THRSH_EN_H = 0
 *      6      CONTINUOUS_S = 1
 *      7      CONTINUOUS_H = 1
 *      11:8   reserved = 0
 *      12     VALID_S = 1
 *      13     ALERT_S = 0
 *      14     VALID_H = 1
 *      15     ALERT_H = 0
 *      31:16  TEST_INTERVAL = 0x7FFF
 *
 */
static void soc_avs_initialize_oscs(int unit)
{
    int i = 0;
    avs_info_t *avs_info = &soc_avs_info;
    uint32_t val;

    /* step0: seq init (stop)
     * Initialize the sensor sequencer
     * 1 = The sequencer will be initialized to the reset state.
     * 0 = Initialization is not triggered
     * Initializing the Sequencer is basically the initializing the
     * collection of counters that eventually measure frequency.
     */
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_INIT, 1);
    val = 1;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_INITr(unit, &val);
#ifdef AVS_DISABLE_PVTMON_AT_INIT
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR, 0xffffffff);  /* mask */
    val = 0xffffffff;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, &val);
#endif

    /* step1: measure time limit
     * Control the time taken for a rosc/pwd measurement:
     * Represents 8 MSBs of reference counter saturation limitFor example:
     * 1f = Reference counter will saturate when it reaches 16\'h1FFF
     */
    REG_READ_CRU_AVS_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROLr(unit, &val);
    val = ((val & ~0xff) | AVS_MEASUREMENT_TIME_CONTROL);
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROLr(unit, &val);

    /* step2: counting mode */
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_ROSC_COUNTING_MODE, AVS_ROSC_COUNT_MODE);
    val = AVS_ROSC_COUNT_MODE;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_ROSC_COUNTING_MODEr(unit, &val);

    /* step3.1: always enable all cent oscillators at start-up */
    /* TODO assume < 32 centrals */
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0, AVS_ALL_ONES);
    val = AVS_ALL_ONES;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r(unit, &val);

    /* step4: set up the cent ROSC Threshold enables
     * CRU_AVS_AVS_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0:
     * For central ring oscillators, from 32nd(bit 31) to 1st(bit 0)
     * CRU_AVS_AVS_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_1:
     * For central ring oscillators, from 36th(bit 3) to 33rd(bit 0)
     * 1 = Enable threshold1 test on the rosc
     * 0 = No threshold1 test on the rosc
     * Since we are using only the first 32 central ring oscillators, we are not programming
     * the second register.
     */
    /* TODO assume < 32 centrals */
    //iproc_reg32_write(CRU_AVS_AVS_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0, AVS_ALL_ONES);
    val = AVS_ALL_ONES;
    REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0r(unit, &val);

    /* TODO: Why are we programming all 1's into the threshold value register? Because we are
     * trying to program the max values here: i.e. 0x7fff because there are only 15 bits in
     * this register and data is 32-bit.
     * why is it done only for ROSC 0?
     * Is it just because we are initializing?
     */
    //iproc_reg32_write(CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_0, AVS_ALL_ONES);
    val = AVS_ALL_ONES;
    REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_0r(unit, &val);

    /* step5: remote oscillators are on PMB bus on newer parts */
    val = AVS_ROSC_CONTROL_INIT_VALUE;
    for (i = 0; i < avs_info->num_remotes; i++) {
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info))
            continue;
        //iproc_reg32_write(CRU_AVS_AVS_PMB_S_000_AVS_ROSC_CONTROL + i * 0x40, AVS_ROSC_CONTROL_INIT_VALUE);
        switch (i) {
        case 0: REG_WRITE_CRU_AVS_AVS_PMB_S_000_AVS_ROSC_CONTROLr(unit, &val); break;
        case 1: REG_WRITE_CRU_AVS_AVS_PMB_S_001_AVS_ROSC_CONTROLr(unit, &val); break;
        case 2: REG_WRITE_CRU_AVS_AVS_PMB_S_002_AVS_ROSC_CONTROLr(unit, &val); break;
        case 3: REG_WRITE_CRU_AVS_AVS_PMB_S_003_AVS_ROSC_CONTROLr(unit, &val); break;
        case 4: REG_WRITE_CRU_AVS_AVS_PMB_S_004_AVS_ROSC_CONTROLr(unit, &val); break;
        case 5: REG_WRITE_CRU_AVS_AVS_PMB_S_005_AVS_ROSC_CONTROLr(unit, &val); break;
        case 6: REG_WRITE_CRU_AVS_AVS_PMB_S_006_AVS_ROSC_CONTROLr(unit, &val); break;
        case 7: REG_WRITE_CRU_AVS_AVS_PMB_S_007_AVS_ROSC_CONTROLr(unit, &val); break;
        case 8: REG_WRITE_CRU_AVS_AVS_PMB_S_008_AVS_ROSC_CONTROLr(unit, &val); break;
        case 9: REG_WRITE_CRU_AVS_AVS_PMB_S_009_AVS_ROSC_CONTROLr(unit, &val); break;
        default: break; /* Error!! Invalid Remote Osc. */
        }
        /* TODO need delay after each remote??? */
        soc_avs_msleep(AVS_REMOTE_DELAY);
    }

    /*step6: seq init (start) */
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_INIT, 0);
    val = 0;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_INITr(unit, &val);

    // enable montiors and OSC to run.
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR, AVS_AVENGER_PVT_MNTR_XBMP1);
    val = AVS_AVENGER_PVT_MNTR_XBMP1;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, &val);
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0, AVS_AVENGER_CENTRAL_XBMP0);
    val = AVS_AVENGER_CENTRAL_XBMP0;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0r(unit, &val);
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1, AVS_AVENGER_CENTRAL_XBMP1);
    val = AVS_AVENGER_CENTRAL_XBMP1;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1r(unit, &val);
    soc_avs_msleep(10);
    iproc_dbg("AVS cent & remote OSC initialized\n");
}


/*
 *  Returns predefined ref_freq_h, ref_freq_s values
 *        Different  'unit' types can have unique set of values.
 *        All remote osc within same 'unit' have same h, s values.(In other
 *        words 'osc_num' argument for this function is unused)\n
 *
 * //xsm: ???input param osc_num is unused, so same value for all rmt_osc?
 * //xrl: This is correct and ok.
 * //xck: All ING AVS3 will also have only two values for rmt_osc because
 * //     there are only two types of rmt_osc
 *
 * [in] osc_num specifies one of the remote_osc. UNUSED
 * [out] *ref_freq_h is used to return freq_h based on unit_type.
 * [out] *ref_freq_s is used to return freq_s based on unit_type.
 */
static int soc_avs_get_rmt_osc_ref_freq(int osc_num, uint32_t *ref_freq_h,
                                        uint32_t *ref_freq_s)
{
    SOC_IF_NULL_RETURN(ref_freq_h);
    SOC_IF_NULL_RETURN(ref_freq_s);

    *ref_freq_h = 0;
    *ref_freq_s = 0;

#if defined(BCM_AVENGER_SUPPORT)
    *ref_freq_h = AVS_UINT(AVS_AVENGER_RMT_FREQ_THRESHOLD_GH);
    *ref_freq_s = AVS_UINT(AVS_AVENGER_RMT_FREQ_THRESHOLD_GS);
    return (SOC_E_NONE);
#else
    return (SOC_E_UNAVAIL);
#endif
}

/*
 * Computes the cent rosc_freq from rosc_count (0.1KHz units)
 *
 * freq = ( (REF_CLK_FREQ*rosc_count*1000)/ (REF_CLK_COUNTER_MAX*ROSC_EDGE_COUNT) )*10;
 * where
 *    REF_CLK_FREQ = 25;
 *    REF_CLK_COUNTER_MAX = (32K-1);
 *    ROSC_EDGE_COUNT = 1;
 * 
 * How did we come with above formula?
 *
 * If Reference clock of 54MHz is used to create obs_interval.
 * full_clock_period_ref_clk = 1000/54 nS
 *
 * Width of interval is specifed by counter
 * (see definition of AVS_MEASUREMENT_TIME_CONTROL = 0x7F)
 * as 0x7F_FF = 32K-1
 *
 * Assuming, AVS_MEASUREMENT_TIME_CONTROL = 0x7F,
 *     obs_interval = full_clock_period * 0x7F_FF;
 *                  = 1000 * 32767 / 54; //nS
 *
 * If we have a ring_oscillator count during above reference obs_interval to
 * 'rosc_count' then what is rosc_freq?
 *
 * if rosc is configured to count only rise_edges,
 *     rosc_period = obs_interval/(rosc_count) = (1000 * 32767)/(54 * rosc_count) in nS
 * else
 *     rosc_period = obs_interval/(rosc_count/2) = (1000 * 32767 * 2)/(54 * rosc_count) in nS
 *
 * Thus, rosc_freq = 1000/rosc_period MHz;
 *                 = (54 * rosc_count) /32767 MHz; //count_rise_edges_only
 *                 = (54 * rosc_count) /(32767*2) MHz; //count_both_edges
 *
 * If we multiply freq_MHz by 1000 we get freq_KHz
 * If we multiply freq_MHz by 10,000 we get freq_0p1KHz = freq_100Hz
 * We thus have:
 *       rosc_freq = 10000 * (54 * rosc_count) /(32767*ROSC_EDGE_COUNT);
 *              //0p1_KHz units
 * Because multiplying by 10,000 can cause overflow so multiply by 1000
 * before division and then multiply by 10 after division:
 *       rosc_freq = ( 1000 * (54 * rosc_count) /(32767*ROSC_EDGE_COUNT) )* 10;
 *       in 0p1_KHz units, where ROSC_EDGE_COUNT = 1 or 2 for 'rise_edge_only' or 'both_edges' respectively
 * 
 * Remote OSCs
 * Computes rosc_freq from rosc_count (0.1KHz units).\n
 *
 * freq = ( (REF_CLK_FREQ*rosc_count*1000)/
 *          (REF_CLK_COUNTER_MAX*ROSC_EDGE_COUNT) )*10;
 * where
 *    REF_CLK_FREQ = 25;
 *    REF_CLK_COUNTER_COUNTER_MAX = (32K-1);
 *    ROSC_EDGE_COUNT = 1;
 * 
 * @rosc_count : input specified rosc_count
 * @return        : calcauted freq (units of 0.1KHz)
 */
static uint32_t soc_avs_calc_osc_freq(uint32_t rosc_count)
{
    uint32_t freq;
    freq = (((AVS_REF_CLK_FREQ * rosc_count) * AVS_FREQ_DIVIDER)
            / (AVS_REF_CLK_COUNTER_MAX * AVS_ROSC_EDGE_COUNT))
            * AVS_FREQ_DIVIDER_ADJUSTMENT;
    return (freq);
}

static void soc_avs_enable_block(int unit, int en)
{
    uint32 val;
    REG_READ_CRU_CRU_IP_RESET_REG_CRUr(unit, &val);
    if (en) {
        //IPROC_REG32_BIT_SET(CRU_CRU_ip_reset_reg_cru, 7);
        val = (val | (0x1 << 7));
    } else {
        //IPROC_REG32_BIT_CLEAR(CRU_CRU_ip_reset_reg_cru, 7);
        val = (val & ~(0x1 << 7));
    }
    REG_WRITE_CRU_CRU_IP_RESET_REG_CRUr(unit, &val);
}

/*
 * Computes voltage v3 corresponding to ref_freq f3.
 *
 * //Given v2, f2, slope:
 * //    find predicted voltage corresponding to
 * //    reference_freq 'f3' ASSUMING linearity.
 *
 * //m = (f2-f3)/(v2-v3), so
 * //v3 = v2 - (f2-f3)/m;
 * //'v3' thus found corresponds to 'f3' frequency
 *
 * [in] f2 Frequency at low voltage
 * [in] v2 Low voltage (units of 0.1mV)
 * [in] m Slope
 * [in] f3 Frequency at high voltage
 * \return Voltage v3 (units of 0.1mV)
 */
static uint32_t soc_avs_calc_v3_for_ref_f3(uint32_t f2, uint32_t v2,
                                           uint32_t m, uint32_t f3)
{
    uint32_t v3;
    uint32_t vdelta;
    //iproc_dbg("soc_avs_calc_v3_for_ref_f3 %u %u %u %u\n", f2, v2, m, f3);
    /* find v3 corresponding to f3 */
    if (f2 >= f3) {
        vdelta = ((f2 - f3) * AVS_S2) / m;
        if (v2 <= vdelta) {
            iproc_err("f2=%d, v2=%d, m=%d, f3=%d, vdelta=%d\n",
                      f2, v2, m, f3, vdelta);
            return AVS_SKIP;
        }

        AVS_ASSERT(v2 > vdelta);
        v3 = v2 - vdelta;
    } else {
        vdelta = ((f3 - f2) * AVS_S2) / m;
        v3 = v2 + vdelta;
    }

    return (v3);
}

/*Unified method to get core voltage, can via i2c, bsti, pwi,...
 * @core_voltage: read current core voltage in 0.1mV(100uV) unit
 */
static int soc_avs_get_core_voltage(int unit, uint32_t *core_voltage)
{
    *core_voltage = AvsReadVoltage(unit, 3);
    return 0;
}

/* Unified method to set core voltage directly, can via i2c, bsti, pwi,
 * @core_voltage: target core voltage to set in 0.1mV(100uV) unit
 *         i.e. core_voltage =12345 means 1234.5mV, 1.2345 V
 */
static int soc_avs_set_core_voltage(int unit, uint32_t core_voltage)
{
    uint32_t dac_code;
    /* Cannot use AvsSetVoltage() because its input parameter is DAC code.
     * This would require a conversion of voltage to DAC code which is
     * affected by PVT.
     * Use new function SetAvsVoltage() which gradually changes to
     * desired voltage.
     */
    dac_code = SetAvsVoltage(unit, core_voltage, currentDAC);
    iproc_dbg(".......currentDAC = %d, new dac_code=%d\n", currentDAC, dac_code);
    currentDAC = dac_code;

    return 0;
}

/* Unified method to set core voltage step by step
 * @req_cvolt: desired target core voltage to set in 0.1mV(100uV) unit
 *         i.e. req_cvolt =12345 means 1234.5mv, 1.2345 V
 * @svolt:  step_size_voltage in units of 0.1mV(100uv) unit
 *         i.e. svolt = 100 means 10 mV
 */
static int avs_scv_step(int unit, uint32_t req_cvolt, uint32_t svolt)
{
    uint32_t cur_cvolt;
    //uint32_t inc_svolt, dec_svolt;
    //uint16_t numsteps = 0;
    //uint16_t maxnumsteps;

    //iproc_dbg("\ndwl........avs_scv_step with req_cvolt=%d, svolt=%d start\n", req_cvolt, svolt);
    if ((req_cvolt < AVS_ABS_MIN_CVOLT) || (req_cvolt > AVS_ABS_MAX_CVOLT)) {
        iproc_err("Desired core voltage %d(0.1mV) overflow, must be within"
                  " [%d, %d]\n", req_cvolt, AVS_ABS_MIN_CVOLT,
                  AVS_ABS_MAX_CVOLT);
        return (SOC_E_FAIL);
    }

    //if (svolt == 0) {
    //    inc_svolt = AVS_DEFAULT_CVOLT_INC_STEP_SIZE;
    //    dec_svolt = AVS_DEFAULT_CVOLT_DEC_STEP_SIZE;
    //} else {
    //    inc_svolt = svolt;
    //    dec_svolt = svolt;
    //}
    /* iproc_dbg("Use inc/dec step of %d/%d(0.1mV) for voltage change\n", inc_svolt, dec_svolt); */
    SOC_IF_ERROR_RETURN(soc_avs_get_core_voltage(unit, &cur_cvolt));

    iproc_dbg("........req_cvolt=%d(0.1mV), cur_cvolt=%d(0.1mV)\n", req_cvolt,
              cur_cvolt);

    if (req_cvolt != cur_cvolt) {
        SOC_IF_ERROR_RETURN(soc_avs_set_core_voltage(unit, req_cvolt));
    }

    SOC_IF_ERROR_RETURN(soc_avs_get_core_voltage(unit, &cur_cvolt));
    iproc_dbg("........avs_scv_step with req_cvolt=%d(0.1mV) done, chk voltage"
              " above:\n\n", req_cvolt);
    return (SOC_E_NONE);
}

/*
 * Program DAC to get a desired voltage. This is the ONLY func that should be used to set the DAC to ensure that the
 * voltage is never read before it becomes stable.\n
 *
 * voltage in 0.1mV(100uV) unit
 */
int avs_set_voltage(int unit, uint32_t voltage)
{
    int rv;

    /*iproc_dbg("Starts avs_set_voltage with voltage=%d(0.1mV)\n", voltage);*/
    rv = avs_scv_step(unit, voltage, 0); /* interpret voltage as cvolt */

    if (SOC_FAILURE(rv)) {
        return (rv);
    } else {
        soc_last_set_voltage = voltage;        /* remember last_set_voltage */
        return (SOC_E_NONE);
    }
}

int avs_get_voltage(int unit, uint32_t *voltage)
{
    return soc_avs_get_core_voltage(unit, voltage);
}

/*
 *  Returns 15b data field (count) for specified range of central oscillators
 * //mult_by_2 is needed because in ING chips, there is div_by_2 for cent_rosc counter
 * @start_osc : specifies first central_osc
 * @num_osc : specifies number of central_osc
 * @count      : array to return oscillator counts
 *
 * Calling function must ensure (start_osc + num_osc - 1) is legal osc_num
 */
#define CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_0r__valid 16
static uint32_t soc_avs_get_cent_osc_count(int unit, int start_osc,
                                           int num_osc, uint32_t *count)
{
    int i, failed = 0, retry = 0;
#ifdef DO_NOT_ACCESS_XOSC
    avs_info_t *avs_info = &soc_avs_info;
#endif

    SOC_IF_NULL_RETURN(count);
    /*iproc_dbg("dwl........  soc_avs_get_cent_osc_count, from osc%d for %d osc's \n", start_osc, num_osc);*/
    avs_fill_uint32_array(count, 0, num_osc);

    /* read osc_counts */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#ifdef DO_NOT_ACCESS_XOSC
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info))
             continue;
#endif
        retry = 0;
        do {
            //count[i] = iproc_reg32_read(CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_0 + i * 4);
            switch (i) {
            case 0: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_0r(unit, &count[i]); break;
            case 1: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_1r(unit, &count[i]); break;
            case 2: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_2r(unit, &count[i]); break;
            case 3: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_3r(unit, &count[i]); break;
            case 4: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_4r(unit, &count[i]); break;
            case 5: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_5r(unit, &count[i]); break;
            case 6: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_6r(unit, &count[i]); break;
            case 7: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_7r(unit, &count[i]); break;
            case 8: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_8r(unit, &count[i]); break;
            case 9: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_9r(unit, &count[i]); break;
            case 10: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_10r(unit, &count[i]); break;
            case 11: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_11r(unit, &count[i]); break;
            case 12: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_12r(unit, &count[i]); break;
            case 13: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_13r(unit, &count[i]); break;
            case 14: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_14r(unit, &count[i]); break;
            case 15: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_15r(unit, &count[i]); break;
            case 16: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_16r(unit, &count[i]); break;
            case 17: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_17r(unit, &count[i]); break;
            case 18: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_18r(unit, &count[i]); break;
            case 19: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_19r(unit, &count[i]); break;
            case 20: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_20r(unit, &count[i]); break;
            case 21: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_21r(unit, &count[i]); break;
            case 22: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_22r(unit, &count[i]); break;
            case 23: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_23r(unit, &count[i]); break;
            case 24: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_24r(unit, &count[i]); break;
            case 25: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_25r(unit, &count[i]); break;
            case 26: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_26r(unit, &count[i]); break;
            case 27: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_27r(unit, &count[i]); break;
            case 28: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_28r(unit, &count[i]); break;
            case 29: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_29r(unit, &count[i]); break;
            case 30: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_30r(unit, &count[i]); break;
            case 31: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_31r(unit, &count[i]); break;
            case 32: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_32r(unit, &count[i]); break;
            case 33: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_33r(unit, &count[i]); break;
            case 34: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_34r(unit, &count[i]); break;
            case 35: REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_35r(unit, &count[i]); break;
            default: break;
            }
            if (((count[i] >> CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_0r__valid) & 0x1) == 1)
                break;
            soc_avs_usleep(100);
        } while (retry++ < 10);
    }

    /* process the counts */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#ifdef DO_NOT_ACCESS_XOSC
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            count[i] = 0;
            continue;
        }
#endif

        if (((count[i] >> CRU_AVS_AVS_RO_REGISTERS_0_CEN_ROSC_STATUS_0r__valid)
              & 0x1) == 0)
        {
            iproc_err("Failed to read valid count of cent rosc %2d, regval=0x%08x\n", i, count[i]);
#if 0
            failed = 1;
#else
            count[i] = 0;
            /* soc_avs_set_osc_excluded(i, AVS_ROSC_CENT); */
#endif
        }
        count[i] = AVS_CENT_OSC_COUNT_MULTIPLIER * (count[i] & 0xffff);
    }

    if (failed == 1) {
        return (SOC_E_FAIL);
    } else {
        return (SOC_E_NONE);
    }
}


/*
 * Program hi/lo threshold counter for specified range of central osc.
 * Unique values can be specified for each osc.
 *
 * [in] start_osc specifies 1st central_osc
 * [in] num_osc specifies number of central_osc
 * [in] sel_hi_thr when true means program hi_threshold
 * [in] *regval array of values to be written into threshold registers
 */
static int soc_avs_set_cent_osc_thr(int unit, int start_osc, int num_osc,
                                 avs_bool_t sel_hi_thr, const uint32_t *regval)
{
    int i;
#if defined(DO_NOT_ACCESS_XOSC)
    avs_info_t *avs_info = &soc_avs_info;
#endif

    SOC_IF_NULL_RETURN(regval);

    for (i = start_osc; i < start_osc + num_osc; i++) {
        uint32_t val = *(regval + i);
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            continue;
        }
#endif
        if (sel_hi_thr == AVS_TRUE) {
            //iproc_reg32_write(CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_0 + i * 4, regval[i]);
            switch (i) {
            case 0: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_0r(unit, &val); break;
            case 1: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_1r(unit, &val); break;
            case 2: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_2r(unit, &val); break;
            case 3: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_3r(unit, &val); break;
            case 4: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_4r(unit, &val); break;
            case 5: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_5r(unit, &val); break;
            case 6: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_6r(unit, &val); break;
            case 7: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_7r(unit, &val); break;
            case 8: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_8r(unit, &val); break;
            case 9: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_9r(unit, &val); break;
            case 10: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_10r(unit, &val); break;
            case 11: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_11r(unit, &val); break;
            case 12: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_12r(unit, &val); break;
            case 13: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_13r(unit, &val); break;
            case 14: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_14r(unit, &val); break;
            case 15: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_15r(unit, &val); break;
            case 16: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_16r(unit, &val); break;
            case 17: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_17r(unit, &val); break;
            case 18: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_18r(unit, &val); break;
            case 19: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_19r(unit, &val); break;
            case 20: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_20r(unit, &val); break;
            case 21: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_21r(unit, &val); break;
            case 22: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_22r(unit, &val); break;
            case 23: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_23r(unit, &val); break;
            case 24: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_24r(unit, &val); break;
            case 25: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_25r(unit, &val); break;
            case 26: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_26r(unit, &val); break;
            case 27: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_27r(unit, &val); break;
            case 28: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_28r(unit, &val); break;
            case 29: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_29r(unit, &val); break;
            case 30: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_30r(unit, &val); break;
            case 31: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_31r(unit, &val); break;
            case 32: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_32r(unit, &val); break;
            case 33: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_33r(unit, &val); break;
            case 34: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_34r(unit, &val); break;
            case 35: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_35r(unit, &val); break;
            default: break;
            }
        } else {
            //iproc_reg32_write(CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_0 + i * 4, regval[i]);
            switch (i) {
            case 0: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_0r(unit, &val); break;
            case 1: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_1r(unit, &val); break;
            case 2: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_2r(unit, &val); break;
            case 3: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_3r(unit, &val); break;
            case 4: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_4r(unit, &val); break;
            case 5: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_5r(unit, &val); break;
            case 6: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_6r(unit, &val); break;
            case 7: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_7r(unit, &val); break;
            case 8: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_8r(unit, &val); break;
            case 9: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_9r(unit, &val); break;
            case 10: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_10r(unit, &val); break;
            case 11: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_11r(unit, &val); break;
            case 12: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_12r(unit, &val); break;
            case 13: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_13r(unit, &val); break;
            case 14: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_14r(unit, &val); break;
            case 15: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_15r(unit, &val); break;
            case 16: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_16r(unit, &val); break;
            case 17: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_17r(unit, &val); break;
            case 18: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_18r(unit, &val); break;
            case 19: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_19r(unit, &val); break;
            case 20: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_20r(unit, &val); break;
            case 21: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_21r(unit, &val); break;
            case 22: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_22r(unit, &val); break;
            case 23: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_23r(unit, &val); break;
            case 24: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_24r(unit, &val); break;
            case 25: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_25r(unit, &val); break;
            case 26: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_26r(unit, &val); break;
            case 27: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_27r(unit, &val); break;
            case 28: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_28r(unit, &val); break;
            case 29: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_29r(unit, &val); break;
            case 30: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_30r(unit, &val); break;
            case 31: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_31r(unit, &val); break;
            case 32: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_32r(unit, &val); break;
            case 33: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_33r(unit, &val); break;
            case 34: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_34r(unit, &val); break;
            case 35: REG_WRITE_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_35r(unit, &val); break;
            default: break;
            }
        }
    }

    return (SOC_E_NONE);
}

/*
 * Reads 16bit COUNT_S, COUNT_H fields for specified range of remote_oscillators
 *
 * [in] start_osc specifies first remote_osc
 * [in] num_osc specifies number of remote_osc
 * [out] *count_h specifies array to return COUNT_H values
 * [out] *count_s specifies array to return COUNT_S values
 *
 * caller must ensure (start_osc + num_osc - 1) is legal osc_num
 */
static int soc_avs_get_rmt_osc_count(int unit, int start_osc, int num_osc,
                                     uint32_t *count_h, uint32_t *count_s)
{
    int i;
    uint32_t val;
#if defined(DO_NOT_ACCESS_XOSC)
    avs_info_t *avs_info = &soc_avs_info;
#endif

    SOC_IF_NULL_RETURN(count_h);
    SOC_IF_NULL_RETURN(count_s);

    /*iproc_dbg("dwl........  soc_avs_get_rmt_osc_count H,from osc%d for %d osc's \n", start_osc, num_osc);*/
    avs_fill_uint32_array(count_h, 0, num_osc);
    /*iproc_dbg("dwl........  soc_avs_get_rmt_osc_count S,from osc%d for %d osc's \n", start_osc, num_osc);*/
    avs_fill_uint32_array(count_s, 0, num_osc);

    /* read count registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#ifdef DO_NOT_ACCESS_XOSC
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }
#endif
        //count_s[i] = iproc_reg32_read(CRU_AVS_AVS_PMB_S_000_AVS_ROSC_COUNT + i * 0x40);
        switch (i) {
        case 0: REG_READ_CRU_AVS_AVS_PMB_S_000_AVS_ROSC_COUNTr(unit, &val); break;
        case 1: REG_READ_CRU_AVS_AVS_PMB_S_001_AVS_ROSC_COUNTr(unit, &val); break;
        case 2: REG_READ_CRU_AVS_AVS_PMB_S_002_AVS_ROSC_COUNTr(unit, &val); break;
        case 3: REG_READ_CRU_AVS_AVS_PMB_S_003_AVS_ROSC_COUNTr(unit, &val); break;
        case 4: REG_READ_CRU_AVS_AVS_PMB_S_004_AVS_ROSC_COUNTr(unit, &val); break;
        case 5: REG_READ_CRU_AVS_AVS_PMB_S_005_AVS_ROSC_COUNTr(unit, &val); break;
        case 6: REG_READ_CRU_AVS_AVS_PMB_S_006_AVS_ROSC_COUNTr(unit, &val); break;
        case 7: REG_READ_CRU_AVS_AVS_PMB_S_007_AVS_ROSC_COUNTr(unit, &val); break;
        case 8: REG_READ_CRU_AVS_AVS_PMB_S_008_AVS_ROSC_COUNTr(unit, &val); break;
        case 9: REG_READ_CRU_AVS_AVS_PMB_S_009_AVS_ROSC_COUNTr(unit, &val); break;
        default: break; /* Error!! Invalid Remote Osc. */
        }
        count_s[i] = val;
        soc_avs_msleep(AVS_REMOTE_DELAY);
    }

    /* extract fields from regval */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#ifdef DO_NOT_ACCESS_XOSC
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            count_h[i] = 0;
            count_s[i] = 0;
            continue;
        }
#endif
        count_h[i] = (count_s[i] & 0xffff0000) >> 16;
        count_s[i] = (count_s[i] & 0x0000ffff);
    }

    return (SOC_E_NONE);
}


/**
 * Fill hi/lo s_, h_ thresholds into specified range of remote osc.
 * SAME value is copied into all registers.
 *
 * [in] start_osc specifies 1st remote_osc
 * [in] num_osc specifies number of remote_osc
 * [in] regval_h value to be programmed in ROSC_H_THRESHOLD register
 * [in] regval_s value to be programmed in ROSC_S_THRESHOLD register
 *
 */
static int soc_avs_set_rmt_osc_thr(int unit, int start_osc, int num_osc,
                                   uint32_t regval_h, uint32_t regval_s)
{
    int i;

#if defined(DO_NOT_ACCESS_XOSC)
    avs_info_t *avs_info = &soc_avs_info;
#endif

    /* write (same) regval_h to all registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }
#endif
        //iproc_reg32_write(CRU_AVS_AVS_PMB_S_000_AVS_ROSC_H_THRESHOLD + i * 0x40, regval_h);
        switch (i) {
        case 0: REG_WRITE_CRU_AVS_AVS_PMB_S_000_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 1: REG_WRITE_CRU_AVS_AVS_PMB_S_001_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 2: REG_WRITE_CRU_AVS_AVS_PMB_S_002_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 3: REG_WRITE_CRU_AVS_AVS_PMB_S_003_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 4: REG_WRITE_CRU_AVS_AVS_PMB_S_004_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 5: REG_WRITE_CRU_AVS_AVS_PMB_S_005_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 6: REG_WRITE_CRU_AVS_AVS_PMB_S_006_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 7: REG_WRITE_CRU_AVS_AVS_PMB_S_007_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 8: REG_WRITE_CRU_AVS_AVS_PMB_S_008_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        case 9: REG_WRITE_CRU_AVS_AVS_PMB_S_009_AVS_ROSC_H_THRESHOLDr(unit, &regval_h); break;
        default: break; /* Error!! Invalid Remote Osc. */
        }
        soc_avs_msleep(AVS_REMOTE_DELAY);
    }

    /* write (same) regval_s to all registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }
#endif
        //iproc_reg32_write(CRU_AVS_AVS_PMB_S_000_AVS_ROSC_S_THRESHOLD + i * 0x40, regval_s);
        switch (i) {
        case 0: REG_WRITE_CRU_AVS_AVS_PMB_S_000_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 1: REG_WRITE_CRU_AVS_AVS_PMB_S_001_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 2: REG_WRITE_CRU_AVS_AVS_PMB_S_002_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 3: REG_WRITE_CRU_AVS_AVS_PMB_S_003_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 4: REG_WRITE_CRU_AVS_AVS_PMB_S_004_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 5: REG_WRITE_CRU_AVS_AVS_PMB_S_005_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 6: REG_WRITE_CRU_AVS_AVS_PMB_S_006_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 7: REG_WRITE_CRU_AVS_AVS_PMB_S_007_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 8: REG_WRITE_CRU_AVS_AVS_PMB_S_008_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        case 9: REG_WRITE_CRU_AVS_AVS_PMB_S_009_AVS_ROSC_S_THRESHOLDr(unit, &regval_s); break;
        default: break; /* Error!! Invalid Remote Osc. */
        }
        soc_avs_msleep(AVS_REMOTE_DELAY);
    }

    return (SOC_E_NONE);
}

#if defined(AVS_USE_LVM_FLAG)   /* LOW VOLTAGE MEASURMENT ? */
/*
 * Sets LVM_LL flag if voltage read is < AVS_VMIN_LVM
 */

#define CRU_CRU_Eth_interface_registers__gphy1_otp_en_lvm_regfile_ovrd_value 20
#define CRU_CRU_Eth_interface_registers__gphy1_otp_en_lvm_regfile_ovrd_en 19
#define CRU_CRU_Eth_interface_registers__gphy0_otp_en_lvm_regfile_ovrd_value 18
#define CRU_CRU_Eth_interface_registers__gphy0_otp_en_lvm_regfile_ovrd_en 17
static int soc_avs_check_lvm(int unit)
{
    uint32_t voltage;
    uint32_t regdata;

    SOC_IF_ERROR_RETURN(avs_get_voltage(unit, &voltage));

    if (voltage < AVS_VMIN_LVM) {
      //IPROC_REG32_BIT_SET(CRU_AVS_AVS_TOP_CTRL_MEMORY_ASSIST, CRU_AVS_AVS_TOP_CTRL_MEMORY_ASSIST__AVS_TOP_LVM_LL);
      //regdata = iproc_reg32_read(CRU_CRU_Eth_interface_registers);
      REG_READ_CRU_CRU_ETH_INTERFACE_REGISTERSr(unit, &regdata);

      //iproc_dbg(" CRU_CRU_Eth_interface_registers = 0x%x\n", regdata);
      regdata |= (0x1 << CRU_CRU_Eth_interface_registers__gphy1_otp_en_lvm_regfile_ovrd_en);
      regdata |= (0x1 << CRU_CRU_Eth_interface_registers__gphy1_otp_en_lvm_regfile_ovrd_value);
      regdata |= (0x1 << CRU_CRU_Eth_interface_registers__gphy0_otp_en_lvm_regfile_ovrd_en);
      regdata |= (0x1 << CRU_CRU_Eth_interface_registers__gphy0_otp_en_lvm_regfile_ovrd_value);
      //iproc_reg32_write(CRU_CRU_Eth_interface_registers, regdata);
      REG_WRITE_CRU_CRU_ETH_INTERFACE_REGISTERSr(unit, &regdata);
      //regdata = iproc_reg32_read(CRU_CRU_Eth_interface_registers);
      REG_READ_CRU_CRU_ETH_INTERFACE_REGISTERSr(unit, &regdata);
      iproc_dbg(" CRU_CRU_Eth_interface_registers = 0x%x\n", regdata);

      /* CRU_CRU_cru_mem_ctrl_0_register[9,10]   avr_portmacro_otp_lvm
       * CRU_CRU_cru_mem_ctrl_0_register[21,22]  m7ss_top_otp_lvm
       * CRU_CRU_cru_mem_ctrl_0_register[15,16]  cru_top_otp_lvm
       * CRU_CRU_cru_mem_ctrl_0_register[27,28]  sw_top_otp_lvm
       */
      //regdata = iproc_reg32_read(CRU_CRU_cru_mem_ctrl_0_register);
      REG_READ_CRU_CRU_CRU_MEM_CTRL_0_REGISTERr(unit, &regdata);
      /*iproc_dbg(" CRU_CRU_cru_mem_ctrl_0_register = 0x%x\n", regdata);*/
      regdata |= (0x1 << 9);
      regdata |= (0x1 << 10);
      regdata |= (0x1 << 15);
      regdata |= (0x1 << 16);
      regdata |= (0x1 << 21);
      regdata |= (0x1 << 22);
      regdata |= (0x1 << 27);
      regdata |= (0x1 << 28);
      //iproc_reg32_write(CRU_CRU_cru_mem_ctrl_0_register, regdata);
      REG_WRITE_CRU_CRU_CRU_MEM_CTRL_0_REGISTERr(unit, &regdata);
      //regdata = iproc_reg32_read(CRU_CRU_cru_mem_ctrl_0_register);
      REG_READ_CRU_CRU_CRU_MEM_CTRL_0_REGISTERr(unit, &regdata);
      iproc_dbg("CRU_CRU_cru_mem_ctrl_0_register = 0x%x\n", regdata);

      //CRU_CRU_cru_mem_ctrl_1_register[0,1]    all_srf_lvm override
      //CRU_CRU_cru_mem_ctrl_1_register[18,19]  timesync_otp_lvm_control
      //regdata = iproc_reg32_read(CRU_CRU_cru_mem_ctrl_1_register);
      REG_READ_CRU_CRU_CRU_MEM_CTRL_1_REGISTERr(unit, &regdata);
      //iproc_dbg("CRU_CRU_cru_mem_ctrl_1_register = 0x%x\n", regdata);
      regdata |= (0x1 << 0);
      regdata |= (0x1 << 1);
      regdata |= (0x1 << 18);
      regdata |= (0x1 << 19);
      //iproc_reg32_write(CRU_CRU_cru_mem_ctrl_1_register, regdata);
      REG_WRITE_CRU_CRU_CRU_MEM_CTRL_1_REGISTERr(unit, &regdata);
      //regdata = iproc_reg32_read(CRU_CRU_cru_mem_ctrl_1_register);
      REG_READ_CRU_CRU_CRU_MEM_CTRL_1_REGISTERr(unit, &regdata);
      iproc_dbg("CRU_CRU_cru_mem_ctrl_1_register = 0x%x\n", regdata);
    }
    return 0;
}
#endif

/* Get the raw max predict voltage without margin
 *    - Applies (high_v1, low_v2) once time
 *    - Read (high_count1, low_count2) from all ROSC
 *    - Calc (high_f1, low_f2) for all ROSC
 *    - Computes v3 for each pre-defined ref f3 of ROSC
 *    - Returns vpred = max(v3)
 *
 * [in] voltage_low   :   voltage correspoding to desired v1
 * [in] voltage_high  :   voltage correspoding to desired v2, voltage_high should > voltage_low!
 * [out] *vlow             :    real Voltage corresponding to voltage_low
 * [out] *vhigh            :    real Voltage corresponding to voltage_high
 * [out] *vpred            :    the converged voltage (main output)
 */
static int soc_avs_predict_vpred(int unit, int pass, uint32_t voltage_low,
                                 uint32_t voltage_high, uint32_t *vlow,
                                    uint32_t *vhigh, uint32_t *vpred)
{
    int i;
    int32_t v3_max_cent_osc = 0, v3_max_rmt_osc = 0;
    //int32_t v3_med_cent_osc = 0, v3_min_cent_osc = 0;
    int32_t v3_med_rmt_osc = 0, v3_min_rmt_osc = 0;
    //int32_t max_cent_osc = 0, med_cent_osc = 0, min_cent_osc = 0;
    int32_t max_rmt_osc = 0, med_rmt_osc = 0, min_rmt_osc = 0;

    uint32_t *pcount_cent_osc_at_vhigh = NULL, *pcount_cent_osc_at_vlow = NULL;
    uint32_t *pcount_rmt_osc_at_vhigh_h = NULL, *pcount_rmt_osc_at_vlow_h = NULL;
    uint32_t *pcount_rmt_osc_at_vhigh_s = NULL, *pcount_rmt_osc_at_vlow_s = NULL;
    avs_info_t *avs_info = &soc_avs_info;

    *vlow = 0;
    *vhigh = 0;
    *vpred = 0;
    AVS_ASSERT(voltage_high > voltage_low);

    /*iproc_dbg("Starts avs_predict_vpred with voltage_low=%d, voltage_high=%d\n", voltage_low, voltage_high);*/

    /* init pointers */
    pcount_cent_osc_at_vhigh = alloc_ptr;
    pcount_cent_osc_at_vlow = pcount_cent_osc_at_vhigh + avs_info->num_centrals;
    pcount_rmt_osc_at_vhigh_h = pcount_cent_osc_at_vlow + avs_info->num_centrals;
    pcount_rmt_osc_at_vlow_h = pcount_rmt_osc_at_vhigh_h + avs_info->num_remotes;
    pcount_rmt_osc_at_vhigh_s = pcount_rmt_osc_at_vlow_h + avs_info->num_remotes;
    pcount_rmt_osc_at_vlow_s = pcount_rmt_osc_at_vhigh_s + avs_info->num_remotes;
    avs_fill_uint32_array(&alloc_ptr[0], 0, sizeof(alloc_ptr) / sizeof(uint32_t));

    /*iproc_dbg("Ring Osc Buffer Initialized\n");*/

    /* step1.1: Apply first voltage (higher DAC is higher voltage),  for PVT_1V_0 */
    /*iproc_dbg("\nSetting Core Voltage to high at %d (0.1mV)\n", voltage_high);*/
    SOC_IF_ERROR_RETURN(avs_set_voltage(unit, voltage_high));    /* voltage_high = desired higher voltage */
    SOC_IF_ERROR_RETURN(avs_get_voltage(unit, vhigh));    /* vhigh = real voltage */

    /* step1.2: Read all cent,rmt osc counts (at vhigh) */
    /*iproc_dbg("\nReading ring osc at high voltage=%d\n", *vhigh); // */
    SOC_IF_ERROR_RETURN(soc_avs_get_cent_osc_count(unit, avs_info->first_cent,
                        avs_info->num_centrals, pcount_cent_osc_at_vhigh));
    SOC_IF_ERROR_RETURN(soc_avs_get_rmt_osc_count(unit, avs_info->first_rmt,
                        avs_info->num_remotes, pcount_rmt_osc_at_vhigh_h,
                        pcount_rmt_osc_at_vhigh_s));

    /* step2.1: Switch to the 2nd voltage and record the oscillator values (lower DAC is lower voltage), for PVT_1V_0 */
    /*iproc_dbg("\nSetting Core Voltage to low at %d (0.1mV)\n", voltage_low);*/
    SOC_IF_ERROR_RETURN(avs_set_voltage(unit, voltage_low));     /* voltage_low = desired lower voltage */
    SOC_IF_ERROR_RETURN(avs_get_voltage(unit, vlow));            /* vlow = real voltage */

    /* step2.2: Verify that AVS hardware is enabled on this board!If the difference in the two voltages is < 10mV, then
     * don't do AVS on this part!
     */
    /* if (pass == 0) */
    /*iproc_dbg("\navs_get_voltage getting vhigh=%d(0.1mV), vlow=%d(0.1mV)\n", *vhigh, *vlow);*/
    AVS_ASSERT(*vhigh > *vlow);
    if ((*vhigh - *vlow) < AVS_VHI_VLO_MIN_DIFF) {
        iproc_dbg("Voltage diff was < %d mV -- stopping AVS processing!\n", AVS_VHI_VLO_MIN_DIFF / 10);
        return (-ENXIO);
    }

    /* step2.3: Read all cent, rmt osc counts again(at vlow) */
    /*iproc_dbg("\nReading ring osc at low voltage=%d\n", *vlow); */
    SOC_IF_ERROR_RETURN(soc_avs_get_cent_osc_count(unit, avs_info->first_cent,
                             avs_info->num_centrals, pcount_cent_osc_at_vlow));
    SOC_IF_ERROR_RETURN(soc_avs_get_rmt_osc_count(unit, avs_info->first_rmt,
                        avs_info->num_remotes, pcount_rmt_osc_at_vlow_h,
                                              pcount_rmt_osc_at_vlow_s));

    /* step3:  Get largest predicted voltage across all valid central oscillators.
     * central osc's can be skipped as AVS in Avenger is dominated by the rmt osc.
     * However, in future it may be revived to improve the accuracy of the estimates.
     */
#if 0
    iproc_dbg("avs_predict_vpred: ........calculation using central osc\n");
    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        uint32_t freq_at_vhigh, freq_at_vlow;
        uint32_t f3, v3, slope;

        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            continue;
        }

        /* step3.1: Calc freqs & slop at vhigh & vlow for each cent rosc */
        freq_at_vhigh = soc_avs_calc_cent_osc_freq(pcount_cent_osc_at_vhigh[i]);
        freq_at_vlow = soc_avs_calc_cent_osc_freq(pcount_cent_osc_at_vlow[i]);

        if (freq_at_vhigh <= freq_at_vlow) {
            iproc_dbg("cent[%d]SKIP:freq_at_vhigh=%d <= freq_at_vlow=%d\n",
                      i, freq_at_vhigh, freq_at_vlow);
            /* TODO: At this point we realise that one RO has behaved
             * badly, so should we abandon Dynamic AVS using ROs and
             * revert to static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_CENT);
            continue;
        }

        /* AVS_ASSERT(freq_at_vhigh > freq_at_vlow);
         * m = (f1-f2)/(v1-v2) , scaling factor is added to improve accuracy.
         * Must remember slope is now in 10,000th
         */
        slope = (freq_at_vhigh - freq_at_vlow) * AVS_S2 / (*vhigh - *vlow);
        if (slope == 0) {
            iproc_dbg("cent[%d]SKIP: slop=0\n", i);
            /* TODO: At this point we realise that one RO has behaved
             * badly, so should we abandon Dynamic AVS using ROs and
             * revert to static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_CENT);
            continue;
        }

        /*step3.2: Read pre-defined freq_threshold (f3) for this rosc */
        SOC_IF_ERROR_RETURN(soc_avs_get_cent_osc_ref_freq(i, &f3));

        /*step3.3: Calc v3 corresponding to f3 */
        v3 = soc_avs_calc_v3_for_ref_f3(freq_at_vlow, *vlow, slope, f3);
        if (v3 == AVS_SKIP) {
            iproc_err("cent[%d]SKIP: v3=0\n", i);
            /* TODO: At this point we realise that one RO has behaved badly, so should we abandon
             * Dynamic AVS using ROs and revert to static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_CENT);
            continue;
        }

        /*step3.4: Get top 3 */
        //v3_max_cent_osc = AVS_MAX(v3, v3_max_cent_osc);
        if (v3 > v3_max_cent_osc) {
            v3_min_cent_osc = v3_med_cent_osc;
            min_cent_osc    = med_cent_osc;
            v3_med_cent_osc = v3_max_cent_osc;
            med_cent_osc    = max_cent_osc;
            v3_max_cent_osc = v3;
            max_cent_osc    = i;
        } else if (v3 > v3_med_cent_osc) {
            v3_min_cent_osc = v3_med_cent_osc;
            min_cent_osc    = med_cent_osc;
            v3_med_cent_osc = v3;
            med_cent_osc    = i;
        } else if (v3 > v3_min_cent_osc) {
            v3_min_cent_osc = v3;
            min_cent_osc    = i;
        }
        /* iproc_dbg("cent[%d]: v3=%d, v3_max_cent_osc=%d\n",i, v3, v3_max_cent_osc); */
    }

    iproc_dbg("   avs_predict_vpred:pass %d, Max voltage across central oscillator %d %d(0.1mV)\n", pass, max_cent_osc, v3_max_cent_osc);
    iproc_dbg("   avs_predict_vpred:pass %d, Med voltage across central oscillator %d %d(0.1mV)\n", pass, med_cent_osc, v3_med_cent_osc);
    iproc_dbg("   avs_predict_vpred:pass %d, Min voltage across central oscillator %d %d(0.1mV)\n\n", pass, min_cent_osc, v3_min_cent_osc);
#endif

    /* step4:  Get largest predicted voltage across all valid remote oscillators:
     * iproc_dbg("avs_predict_vpred: ........calculation using remote  osc\n");
     */
    for(i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        uint32_t freq_at_vhigh_s, freq_at_vhigh_h;
        uint32_t freq_at_vlow_s, freq_at_vlow_h;
        uint32_t f3_s, f3_h, v3_s, v3_h, slope;

        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }

        /*step4.1 */
        /*iproc_dbg("rmt_osc_count[%d]:%d,%d,%d,%d\n", i, pcount_rmt_osc_at_vhigh_h[i],
        pcount_rmt_osc_at_vhigh_s[i], pcount_rmt_osc_at_vlow_h[i], pcount_rmt_osc_at_vlow_s[i]); */
        freq_at_vhigh_h = soc_avs_calc_rmt_osc_freq(pcount_rmt_osc_at_vhigh_h[i]);
        freq_at_vhigh_s = soc_avs_calc_rmt_osc_freq(pcount_rmt_osc_at_vhigh_s[i]);
        freq_at_vlow_h = soc_avs_calc_rmt_osc_freq(pcount_rmt_osc_at_vlow_h[i]);
        freq_at_vlow_s = soc_avs_calc_rmt_osc_freq(pcount_rmt_osc_at_vlow_s[i]);

        if (freq_at_vhigh_h <= freq_at_vlow_h) {
            iproc_dbg("rmt[%d]SKIP:freq_at_vhigh_h=%d <= freq_at_vlow_h=%d\n", i, freq_at_vhigh_h, freq_at_vlow_h);
            /* TODO: At this point we realise that one RO has behaved badly,
             * so should we abandon Dynamic AVS using ROs and revert to
             * static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_RMT);
            continue;
        }

        if (freq_at_vhigh_s <= freq_at_vlow_s) {
            iproc_dbg("rmt[%d]SKIP:freq_at_vhigh_s=%d <= freq_at_vlow_s=%d\n", i, freq_at_vhigh_s, freq_at_vlow_s);
            /* TODO: At this point we realise that one RO has behaved badly,
             * so should we abandon Dynamic AVS using ROs and revert to static
             * AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_RMT);
            continue;
        }

        /*step4.2 */
        SOC_IF_ERROR_RETURN(soc_avs_get_rmt_osc_ref_freq(i, &f3_h, &f3_s));

        /*step4.3 */
        AVS_ASSERT(freq_at_vhigh_h > freq_at_vlow_h);
        slope = (freq_at_vhigh_h - freq_at_vlow_h) * AVS_S2 / (*vhigh - *vlow);
        if (slope == 0) {
            iproc_dbg("rmt[%d]SKIP: slop=0\n", i);
            /* TODO: At this point we realise that one RO has behaved badly,
             * so should we abandon Dynamic AVS using ROs and revert to
             * static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_RMT);
            continue;
        }

        v3_h = soc_avs_calc_v3_for_ref_f3(freq_at_vlow_h, *vlow, slope, f3_h);
        //iproc_dbg(".    calc_v3_for_ref_f3, remoteH osc=%d RO@vhigh=%u"
        //          " vhigh=%u RO@vlow=%u vlow=%u slope=%u ref_RO=%u ==> v3=%u\n", 
        //          i, freq_at_vhigh_h, *vhigh, freq_at_vlow_h, *vlow, slope, f3_h, v3_h);

        if (v3_h == AVS_SKIP) {
            iproc_err("rmt[%d]SKIP: v3_h=0\n", i);
            /* TODO: At this point we realise that one RO has behaved badly,
             * so should we abandon Dynamic AVS using ROs and revert to
             * static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_RMT);
            continue;
        }

        AVS_ASSERT(freq_at_vhigh_s > freq_at_vlow_s);
        slope = (freq_at_vhigh_s - freq_at_vlow_s) * AVS_S2 / (*vhigh - *vlow);
        v3_s = soc_avs_calc_v3_for_ref_f3(freq_at_vlow_s, *vlow, slope, f3_s);
        //iproc_dbg(".    calc_v3_for_ref_f3, remoteS osc=%d RO@vhigh=%u"
        //          " vhigh=%u RO@vlow=%u vlow=%u slope=%u ref_RO=%u ==> v3=%u\n",
        //          i, freq_at_vhigh_s, *vhigh, freq_at_vlow_s, *vlow, slope, f3_s, v3_s);
        if (v3_s == AVS_SKIP) {
            iproc_err("rmt[%d]SKIP: v3_s=0\n", i);
            /* TODO: At this point we realise that one RO has behaved badly,
             * so should we abandon Dynamic AVS using ROs and revert to
             * static AVS or Continue trying?
             */
            soc_avs_set_osc_excluded(i, AVS_ROSC_RMT);
            continue;
        }

        // remember top 3 OSC
        // v3_max_rmt_osc = AVS_MAX(v3_h, v3_max_rmt_osc);
        if (v3_h > v3_max_rmt_osc) {
            v3_min_rmt_osc = v3_med_rmt_osc;
            min_rmt_osc    = med_rmt_osc;
            v3_med_rmt_osc = v3_max_rmt_osc;
            med_rmt_osc    = max_rmt_osc;
            v3_max_rmt_osc = v3_h;
            max_rmt_osc    = i;
        } else if (v3_h > v3_med_rmt_osc) {
            v3_min_rmt_osc = v3_med_rmt_osc;
            min_rmt_osc    = med_rmt_osc;
            v3_med_rmt_osc = v3_h;
            med_rmt_osc    = i;
        } else if (v3_h > v3_min_rmt_osc) {
            v3_min_rmt_osc = v3_h;
            min_rmt_osc    = i;
        }

        // remember top 3 OSC, osc number <= 100 means _s
        // v3_max_rmt_osc = AVS_MAX(v3_s, v3_max_rmt_osc);
        if (v3_s > v3_max_rmt_osc) {
            v3_min_rmt_osc = v3_med_rmt_osc;
            min_rmt_osc    = med_rmt_osc;
            v3_med_rmt_osc = v3_max_rmt_osc;
            med_rmt_osc    = max_rmt_osc;
            v3_max_rmt_osc = v3_s;
            max_rmt_osc    = 100+i;
        } else if (v3_s > v3_med_rmt_osc) {
            v3_min_rmt_osc = v3_med_rmt_osc;
            min_rmt_osc    = med_rmt_osc;
            v3_med_rmt_osc = v3_s;
            med_rmt_osc    = 100+i;
        } else if (v3_s > v3_min_rmt_osc) {
            v3_min_rmt_osc = v3_s;
            min_rmt_osc    = 100+i;
        }
        /*iproc_dbg("rmt[%d]: v3_h=%d, v3_s=%d, v3_max_rmt_osc=%d\n", i, v3_h, v3_s, v3_max_rmt_osc); */
    }

    iproc_dbg("avs_predict_vpred:pass %d, Max voltage across remote"
              " oscillators %d %d(0.1mV)\n", pass, max_rmt_osc, v3_max_rmt_osc);
    iproc_dbg("avs_predict_vpred:pass %d, Med voltage across remote"
              " oscillators %d %d(0.1mV)\n", pass, med_rmt_osc, v3_med_rmt_osc);
    iproc_dbg("avs_predict_vpred:pass %d, Min voltage across remote"
              " oscillators %d %d(0.1mV)\n\n", pass, min_rmt_osc, v3_min_rmt_osc);

    /*step5: Get final max Vpred */
    *vpred = AVS_MAX(v3_max_cent_osc, v3_max_rmt_osc);

    /*iproc_dbg("Starts avs_predict_vpred with voltage_low=%d, voltage_high=%d\n", voltage_low, voltage_high);*/
    iproc_dbg("Avs_predict_vpred: v1s=%d, v2s=%d, v1r=%d, v2r=%d, predicted"
              " voltage=%d\n\n", voltage_high, voltage_low, *vhigh,*vlow, *vpred);
    return 0;
}

/*
 * Run avs_predict_vpred, add margins, get computed Vavs, then set voltage to Vavs.
 *
 * //1st pass uses rough values (local fit)
 * //xsm: For 1st pass, choose v2 that is smaller but very close to v1 and assuming linear
 * //     relationship between v1,f1 and v2,f2 - compute v3 (Vpred) corresponding to
 * //     reference_f3 for EACH osc (draw separate line for each osc).
 * //     Choose max out of these v3 and call it Vpred.
 * //     Add margins - call it Vavs.
 * //     When computing V3 and predcting Vavs, we must
 * //     ensure that chip can run both at selected V1 and V2.
 *
 * //2nd pass uses the more refined (wider range) values (global fit)
 * //xsm: if V3 and predicted Vavs in 1st pass is < 0.94V, then don't
 * //     trust the Vavs computed in pass_1 because linear relationship between
 * //     may not hold true for larger V2.
 * //     So try selecting new_V2 = old_V2 + some_fixed_value
 * //     and recompute new_V3 and thus new_Vavs
 * //     Use new_Vavs
 * 
 * [out] *vpred    : predicted voltage from soc_avs_predict_vpred()
 * [out] *vavs_sv : vpred + margins
 */
static int soc_avs_find_final_voltage(int unit, uint32_t *vpred, uint32_t *vavs_sv)
{
    int pass;
    //int result;
    uint32_t vlow, vhigh, ccv, v1s, v2s;
    int32_t new_vmin_avs, vsum, vavs;
    int32_t slope_avs, intercept_avs, vmargin;
    int32_t tmon, temperature, vmargin_temp;
    int32_t tmp1, tmp2;
    avs_info_t *avs_info = &soc_avs_info;
    *vpred = 0;

    new_vmin_avs = avs_info->vmin_avs;

    sal_printf("avs_find_final_voltage: Vmin_avs=%d\n", new_vmin_avs);

    /* Read cur_core_voltage
     * TODO: This function will never fail. It invokes static AVS function 
     * to obtain PVTMON measured core voltage.
     */
    SOC_IF_ERROR_RETURN(avs_get_voltage(unit, &ccv));

    /* Determine v1s, v2s (voltage points for local fit), make sure v1s >v2s */
    if (ccv > AVS_VHIGH) {
        v1s = ccv;
        AVS_ASSERT(ccv > AVS_V1_MINUS_V2);
        v2s = ccv - AVS_V1_MINUS_V2;
    } else {
        v1s = ccv + AVS_V1_MINUS_V2;
        v2s = ccv;
    }
    iproc_dbg("Starts avs_find_final_voltage with ccv=%d, v1s=%d, v2s=%d\n", ccv, v1s, v2s);

    /* First pass uses rough values (local fit)
     * Second pass uses the more refined (wider range - or narrower range??
     * TODO ) values (global fit), if needed.
     */
    for (pass = 0; pass < 2; pass++) {
        // get die temperature for adjustment of margin
        //tmon = iproc_reg32_read(CRU_AVS_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS);
        REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr(unit, &tmon);
        if ((tmon & 0x10400) == 0x10400) {
            // make sure valid data
            /* temperature conversion:
            413.35 - (tmon * 0.49055);
            scale up to avoid floating point
            */
            temperature = tmon & 0x3ff; // get monitor value
            temperature = 41335000 - (temperature * 49055);
            temperature = temperature/100000 + 5;
            sal_printf ("dwl.....pvt_mon=0x%x....die temperature"
                        " = %d (degrees C)\n", tmon, temperature);
        } else {
            sal_printf ("dwl........invalid temperature monitor"
                        " data = 0x%x, default to 40C\n", tmon);
            temperature = 45;
        }

        SOC_IF_ERROR_RETURN(soc_avs_predict_vpred(unit, pass, v2s, v1s, &vlow, &vhigh, vpred));
        if (*vpred == 0) {
            iproc_err("Failed to do avs prediction using ROs.\n");
            /* AVS not enabled */
            return -EFAIL;
        }

        /* Applying Margins If any. Because the RO doesn't reflect the exact critical path in the circuit.
         * The margin of error is expected to be some function of the estimated (converged) AVS voltage only.
         * For simplicity (bringing down the order of complexity), we assume a liner relationship between the
         * margin that need to be applied and the estimated voltage.
         * This H/W scheme will use only 2 programmable parameters A and B to perform: V_mrgn = A * V_est + B.
         * This scheme can provide 4 different margin modes:
         * 1. Zero margin mode: If A = B = 0, then V_mrgn = 0V.
         * 2. Constant margin mode: If A = 0 and B = a positive value, then V_mrgn = B (e.g., 50mV).
         * 3. Proportional margin mode: If A = a percentage number and B = 0, then V_mrgn = A * V_est.
         * 4. “Steep” margin mode: If A = a percentage number and B = a “negative” value, then V_mrgn = A* V_est + B.
         *
         * TODO??: Why can't we have B = a "positive" value in the 4th case??
         * 
         * The complete procedure for calculating V_fin is listed below:
         * 1.       V_mrgn = A * V_est + B;
         * 2.       V_sum = V_est + V_mrgn; and
         * 3.       V_fin = min{ max( V_sum, V_lb), V_ub}.
         * 
         * The V_lb and V_ub are programmable lower bound (e.g., 0.85V) and upper bond (e.g., 1.05V), respectively.
         * slope = (VmarginH - VmarginL)/( (Vmax_avs - Vmin_avs) + (VmrginL - VmarginH) )
         *
         * TODO: Why is (VmrginL - VmarginH) added?
         * Because: Vmax_avs = V_est + VmrginH; & Vmin_avs = V_est + VmrginL;
         *
         * intercept = VmarginL - slope*(Vmin_avs - VmarginL)
         * Vsum = Slope*Vpred + intercept + vpred
         *
         * Input Parameters: No_Margin  Positive_Constant_Margin Negative_Constant_Margin SS-Only_Margin FF-Only_Margin
         * FF, vmin          0.85       0.85                     0.85                     0.85           0.85
         * SS, vmin          1.035      1.035                    1.035                    1.035          1.035
         * VmarginL          0          0.035                    -0.025                   0              0.075
         * VmarginH          0          0.035                    -0.025                   0.075          0
         * slope             0          0                        0                        0.681818182    -0.288461538
         * Intercept         0          0.035                    -0.025                   -0.579545455   0.298557692
         */

        /* add margin as a function of temperature. */
        if (temperature > 0) {
            vmargin_temp = (avs_info->vmargin_110C * temperature) / 110;
            iproc_dbg("... margin added at temperature %d : %d, original"
                      " vpred = %d\n", temperature, vmargin_temp, (*vpred));
        } else {
            vmargin_temp = 0;
        }

        /* Calculate margin as a function of process:
         * slope_avs = ((avs_info->vmargin_high - avs_info->vmargin_low) * AVS_S2) /
         * ((avs_info->vmax_avs - new_vmin_avs) + (avs_info->vmargin_low - avs_info->vmargin_high));
         */
        tmp1 = (avs_info->vmargin_high - avs_info->vmargin_low) * AVS_S2;
        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
            tmp2 = (avs_info->vmax_avs - new_vmin_avs) +
                            (avs_info->vmargin_low - avs_info->vmargin_high);
            slope_avs = tmp1/tmp2;

            tmp1 = slope_avs * (new_vmin_avs - avs_info->vmargin_low);
            tmp2 = tmp1 / AVS_S2;
            intercept_avs = avs_info->vmargin_low - tmp2;

            vmargin = (slope_avs * (int32_t)(*vpred)) / AVS_S2 + intercept_avs;
        } else {
            tmp2 = (avs_info->vmax_avs - new_vmin_avs) +
                           ((avs_info->vmargin_low - avs_info->vmargin_high) / 10);
            slope_avs = tmp1/tmp2;

            tmp1 = slope_avs * (new_vmin_avs - (avs_info->vmargin_low / 10));
            tmp2 = tmp1 / AVS_S2;
            intercept_avs = avs_info->vmargin_low - tmp2;

            vmargin = (slope_avs * ((int32_t)(*vpred) + vmargin_temp)) / AVS_S2 + intercept_avs;
        }
        vsum = (int32_t)(*vpred) + vmargin + vmargin_temp;
        iproc_dbg("Margins added:pass%d for temperature(%d C): %d, for"
                  " process: %d; original Vavs=%d, Vavs with margin=%d\n",
                 pass, temperature, vmargin_temp, vmargin, (*vpred), vsum);

        /*
        iproc_dbg("dwl ... margin calculation: margin_high=%d, margin_low=%d"
                    " AVS_S2=%d\n", avs_info->vmargin_high, avs_info->vmargin_low,
                    AVS_S2);
        iproc_dbg("dwl ... margin calculation: slope_avs=%d, intercept_avs=%d,"
                  " vmargin=%d, vsum=%d, new_vmin_avs=%d, vmax=%d\n", slope_avs,
                   intercept_avs, vmargin, vsum, new_vmin_avs, avs_info->vmax_avs);
         */

        /* Make sure this is NEVER outside the allowable voltage min/max values */
        if (vsum < new_vmin_avs) {
            vavs = new_vmin_avs;
        } else if (vsum > avs_info->vmax_avs) {
            vavs = avs_info->vmax_avs;
        } else {
            vavs = vsum;
        }

        /*iproc_dbg("avs_find_final_voltage: pass=%d, vpred=%d, vavs=%d \n", pass, *vpred, vavs);*/

        /* On next pass, adjust low value (v2s) to get more precise
         * calculation on second pass.
         * This turns the process from a local fit to a global fit process
         */
        if (vavs < AVS_VLOW_SS) {
            AVS_ASSERT(vavs > 0);
            /* if vavs is below v2s by at least 2*AVS_V1_MINUS_V2, will use
             * v2s as the next v1s. This is done for two reasons:
             * (1) the core voltage has less tendency to swing up and down
             *     during the AVS process
             * (2) run time is reduced because the current core voltage should
             *     be at v2s.
             */
            if ((vavs + 2*AVS_V1_MINUS_V2) < v2s) {
                iproc_dbg("avs threshold=%d, v1s=%d, v2s=%d, adjust v1s for"
                          " next pass to %d\n", vavs + 2*AVS_V1_MINUS_V2,
                          v1s, v2s, v2s);
                v1s = v2s;
            }

            v2s = (uint32_t)vavs + AVS_V2S_INC_FOR_2ND_PASS;
            if (v2s < AVS_ABS_MIN_CVOLT)
                v2s = AVS_ABS_MIN_CVOLT;
            /* AVS_ASSERT(v1s > v2s); */
            if (v1s < v2s) {
                uint32_t temp;
                temp = v1s;
                v1s = v2s;
                v2s = temp;
            }
            /*iproc_dbg("dwl ... next pass, v1s=%d, v2s=%d(added %d)\n", v1s, v2s, AVS_V2S_INC_FOR_2ND_PASS);*/
        } else {
            /* no need to do 2nd pass (global fit) - because v1s, v2s
             * remain same as 1st pass
             */
            iproc_dbg(".... skip pass, v1s=%d, v2s=%d\n", v1s, v2s);
            break;
        }
    }

    AVS_ASSERT(vavs > 0);
    iproc_dbg("avs_find_final_voltage: set voltage:%d\n", vavs);
    *vavs_sv = (uint32_t)vavs;

#if defined(AVS_USE_LVM_FLAG)
    SOC_IF_ERROR_RETURN(soc_avs_check_lvm(unit));
#endif

    /* SOC_IF_ERROR_RETURN(avs_get_voltage(&vlow)); */
    /* iproc_dbg("Final Voltage =", vlow); */

    return 0;
}

#if defined(AVS_SET_THRESHOLDS)
/*
 *  Reads all osc_counts, determines lowest_osc_count and programs osc_thresholds.
 *  This is a helper function for soc_avs_set_new_thr.
 *
 * //xsm: ???Currently this function goes by comparing the counts of cent_osc, rmt_osc.
 * //     This is correct ONLY if 'windows' for measurement are same for cent, rmt osc.
 * //     If ref_clk used for window generation or window_width is different,
 * //     we will have to compare frequencies and not counts
 * //xck_AI: do we need changes?
 * [in] sel_hi_thr                 : For cent & rmt, 1 => update high_thr,   0 => update low_thr
 * [in] write_rmt_osc_thr    : 1 => update rmt_osc_thr, 0 => don't update rmt_osc_thr
 * [out] *alloc_ptr               : Points to area in mem to store osc_counts temporarily
 * [out] *rmt_osc_regval_h :
 * [out] *rmt_osc_regval_s :
 */
static int soc_avs_find_new_thr(int unit, avs_bool_t sel_hi_thr,
                                int write_rmt_osc_thr,
                                uint32_t *alloc_ptr,
                                uint32_t *rmt_osc_regval_h,
                                uint32_t *rmt_osc_regval_s)
{
    int i;
    uint32_t *rmt_osc_count_s = NULL, *rmt_osc_count_h = NULL;
    uint32_t *cent_osc_count = NULL, *cent_osc_thr = NULL;
    uint32_t lowest_count_s = AVS_ALL_ONES, lowest_count_h = AVS_ALL_ONES;
    avs_info_t *avs_info = &soc_avs_info;

    /*step1: Read H & S osc_count for remote oscillators and find lowest */
    rmt_osc_count_s = alloc_ptr;
    rmt_osc_count_h = rmt_osc_count_s + avs_info->num_remotes;
    SOC_IF_ERROR_RETURN(soc_avs_get_rmt_osc_count(unit, avs_info->first_rmt,
                    avs_info->num_remotes, rmt_osc_count_h, rmt_osc_count_s));

    for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }

        if (rmt_osc_count_s[i] != 0) {
            lowest_count_s = AVS_MIN(lowest_count_s, rmt_osc_count_s[i]);
        }

        if (rmt_osc_count_h[i] != 0) {
            lowest_count_h = AVS_MIN(lowest_count_h, rmt_osc_count_h[i]);
        }
    }

    /*step2: Read osc_count for central oscillators and find lowest */
    cent_osc_count = alloc_ptr;
    SOC_IF_ERROR_RETURN(soc_avs_get_cent_osc_count(unit, avs_info->first_cent,
                                     avs_info->num_centrals, cent_osc_count));

    for (i = 0; i < avs_info->num_centrals; i++) {
        /* In 28nm , only oscillators 1&3 are VHT and VST types,
         * so only include them in finding lowest oscillator count
         */
        if (i != 1 && i != 3) {
            continue;
        }
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            continue;
        }
        if (cent_osc_count[i] == 0) {
            continue;
        }
        /* Central #1 is SVT and #3 is HVT */
        if (i == 1) {
            lowest_count_s = AVS_MIN(lowest_count_s, cent_osc_count[i]);
        }
        if (i == 3) {
            lowest_count_h = AVS_MIN(lowest_count_h, cent_osc_count[i]);
        }
    }

    /*step3: Update thresholds for rmt_osc */
    if (sel_hi_thr == AVS_TRUE) {
        *rmt_osc_regval_h = ((*rmt_osc_regval_h) & 0x0000ffff) | (lowest_count_h << 16);
        *rmt_osc_regval_s = ((*rmt_osc_regval_s) & 0x0000ffff) | (lowest_count_s << 16);
    } else {
        *rmt_osc_regval_h = ((*rmt_osc_regval_h) & 0xffff0000) | lowest_count_h;
        *rmt_osc_regval_s = ((*rmt_osc_regval_s) & 0xffff0000) | lowest_count_s;
    }

    /* step4:  compute thresholds for cent_osc
     * SPECIAL CASE: in 28nm only centrals 1 and 3 are H&S types
     * (just use current for all others)
     */
    cent_osc_thr = cent_osc_count;
    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        /* Central #1 is SVT and #3 is HVT */
        if (i == 1) {
            cent_osc_thr[i] = lowest_count_s;
        } else if (i == 3) {
            cent_osc_thr[i] = lowest_count_h;
        } else {
            cent_osc_thr[i] = cent_osc_count[i];
        }

        /* anluo: duplicated ??? what does this mean ????? */
        if (sel_hi_thr == AVS_TRUE) {
            *(cent_osc_count + i) = (*(cent_osc_count + i) & 0xffff8000) | cent_osc_thr[i];
        } else {
            *(cent_osc_count + i) = (*(cent_osc_count + i) & 0xffff8000) | cent_osc_thr[i];
        }
    }

    /*step5:  write thresholds for cent_osc */
    SOC_IF_ERROR_RETURN(soc_avs_set_cent_osc_thr(unit, avs_info->first_cent,
                   avs_info->num_centrals, sel_hi_thr, cent_osc_count));

    /*step6:  write thresholds for rmt_osc */
    if (write_rmt_osc_thr) {
        SOC_IF_ERROR_RETURN(soc_avs_set_rmt_osc_thr(unit, avs_info->first_rmt,
         avs_info->num_remotes, *rmt_osc_regval_h, *rmt_osc_regval_s));
    }

    return 0;
}

/*
 * Finds lowest_count for cur_dac_code, and sets lo_thr to this value
 * for all osc.
 * Changes voltage to cur_dac_code+OFFSET, finds new lowest_count and
 * sets hi_thr for all osc to this value.
 *
 * [in] vavs_sv : Voltage at which lo_thr is determined.
 *
 */
static int soc_avs_set_new_thr(int unit, uint32_t vavs_sv)
{
    uint32_t rmt_osc_regval_h = 0, rmt_osc_regval_s = 0;
    uint32_t new_thr_voltage = vavs_sv + AVS_VINC_FOR_SET_NEW_THR;
    //avs_info_t *avs_info = &soc_avs_info;

    /* step1: Set lo_thr to the smallest count found at current (low) voltage,
     * Find & Set lo_thr for cent osc,
     * Find_only lo_thr for rmt osc. */
    /*iproc_dbg("avs_set_new_thr: find & set new low threshold at voltage %d(0.1mV)\n", vavs_sv);*/
    SOC_IF_ERROR_RETURN(soc_avs_find_new_thr(unit, AVS_FALSE, AVS_FALSE,
                  alloc_ptr, &rmt_osc_regval_h, &rmt_osc_regval_s));

    /* step2: Set voltage to Vasv_sv + Vinc_for_new_thr */
    /*iproc_dbg("avs_set_new_thr: set new thr voltage %d(0.1mV)\n", new_thr_voltage);*/
    SOC_IF_ERROR_RETURN(avs_set_voltage(unit, new_thr_voltage));

    /* step3: Set the hi_thr to the smallest count found at new (higher)
     * voltage,
     * Find & Set hi_thr for cent osc,
     * Find hi_thr for rmt, write hi_lo_thr for rmt */
    /*iproc_dbg("avs_set_new_thr: find & set new high threshold at voltage %d(0.1mV)\n", new_thr_voltage);*/
    SOC_IF_ERROR_RETURN(soc_avs_find_new_thr(unit, AVS_TRUE, AVS_TRUE,
                       alloc_ptr, &rmt_osc_regval_h, &rmt_osc_regval_s));

    return 0;
}

/* ///////////////////////////////////////////////////////////////////////////////////////////////// */
#if 0
/*
 Currently soc_avs_converge_process() (and therefore other functions invoked
 from within it) is NOT being used, possibly to reduce run-time and may be
 the "algorithm run-time vs. estimate accuracy improvement" tradeoff didn't
 justify using it. This portion of the code is retained under comment to
 facilitate future experimentation and possible improvements.
*/

/*
 * Returns reference_frequency value from array of defines.
 *        Oscillators for different  'unit' types can have unique set of values.
 *        Oscillators within same 'unit' can have different value.
 *
 * @osc_num : specifies one of the central_osc
 * @ref_freq  : returns reference_frequency_threshold (example: 107220) based on osc_num
 */
static int soc_avs_get_cent_osc_ref_freq(int osc_num, uint32_t *ref_freq)
{
    SOC_IF_NULL_RETURN(ref_freq);
    /* default: consider it un-implemented osc */
    *ref_freq = 0;
#if defined(BCM_AVENGER_SUPPORT)
    if (osc_num < AVS_AVENGER_NUMBER_OF_CENTRALS) {
        *ref_freq = reffreq[osc_num];
        return (SOC_E_NONE);
    }
#endif
    return (SOC_E_UNAVAIL);
}

/*
 * Reads 15bit high, low counter thresholds for specified range of central oscillators
 *
 * [in] start_osc specifies first central_osc
 * [in] num_osc specifies number of central_osc
 * [out] *lo_thr specifies array to return low threshold
 * [out] *hi_thr specifies array to return high threshold
 */
static int soc_avs_get_cent_osc_thr(int start_osc, int num_osc, uint32_t *lo_thr, uint32_t *hi_thr)
{
    int i;
    uint32_t val;
#if defined(DO_NOT_ACCESS_XOSC)
    avs_info_t *avs_info = &soc_avs_info;
#endif

    SOC_IF_NULL_RETURN(lo_thr);
    SOC_IF_NULL_RETURN(hi_thr);

    avs_fill_uint32_array(lo_thr, 0, num_osc);
    avs_fill_uint32_array(hi_thr, 0, num_osc);

    /* read lo_thr reg */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            continue;
        }
#endif
        //lo_thr[i] = iproc_reg32_read(CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_0 + i * 4);
        switch (i) {
        case 0: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_0r(0, &val); break;
        case 1: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_1r(0, &val); break;
        case 2: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_2r(0, &val); break;
        case 3: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_3r(0, &val); break;
        case 4: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_4r(0, &val); break;
        case 5: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_5r(0, &val); break;
        case 6: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_6r(0, &val); break;
        case 7: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_7r(0, &val); break;
        case 8: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_8r(0, &val); break;
        case 9: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_9r(0, &val); break;
        case 10: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_10r(0, &val); break;
        case 11: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_11r(0, &val); break;
        case 12: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_12r(0, &val); break;
        case 13: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_13r(0, &val); break;
        case 14: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_14r(0, &val); break;
        case 15: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_15r(0, &val); break;
        case 16: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_16r(0, &val); break;
        case 17: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_17r(0, &val); break;
        case 18: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_18r(0, &val); break;
        case 19: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_19r(0, &val); break;
        case 20: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_20r(0, &val); break;
        case 21: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_21r(0, &val); break;
        case 22: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_22r(0, &val); break;
        case 23: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_23r(0, &val); break;
        case 24: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_24r(0, &val); break;
        case 25: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_25r(0, &val); break;
        case 26: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_26r(0, &val); break;
        case 27: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_27r(0, &val); break;
        case 28: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_28r(0, &val); break;
        case 29: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_29r(0, &val); break;
        case 30: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_30r(0, &val); break;
        case 31: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_31r(0, &val); break;
        case 32: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_32r(0, &val); break;
        case 33: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_33r(0, &val); break;
        case 34: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_34r(0, &val); break;
        case 35: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSC_35r(0, &val); break;
        default: break;
        }
        lo_thr[i] = val;
    }

    /* read hi_thr reg */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            continue;
        }
#endif
        //hi_thr[i] = iproc_reg32_read(CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_0 + i * 4);
        switch (i) {
        case 0: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_0r(0, &val); break;
        case 1: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_1r(0, &val); break;
        case 2: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_2r(0, &val); break;
        case 3: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_3r(0, &val); break;
        case 4: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_4r(0, &val); break;
        case 5: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_5r(0, &val); break;
        case 6: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_6r(0, &val); break;
        case 7: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_7r(0, &val); break;
        case 8: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_8r(0, &val); break;
        case 9: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_9r(0, &val); break;
        case 10: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_10r(0, &val); break;
        case 11: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_11r(0, &val); break;
        case 12: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_12r(0, &val); break;
        case 13: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_13r(0, &val); break;
        case 14: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_14r(0, &val); break;
        case 15: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_15r(0, &val); break;
        case 16: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_16r(0, &val); break;
        case 17: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_17r(0, &val); break;
        case 18: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_18r(0, &val); break;
        case 19: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_19r(0, &val); break;
        case 20: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_20r(0, &val); break;
        case 21: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_21r(0, &val); break;
        case 22: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_22r(0, &val); break;
        case 23: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_23r(0, &val); break;
        case 24: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_24r(0, &val); break;
        case 25: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_25r(0, &val); break;
        case 26: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_26r(0, &val); break;
        case 27: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_27r(0, &val); break;
        case 28: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_28r(0, &val); break;
        case 29: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_29r(0, &val); break;
        case 30: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_30r(0, &val); break;
        case 31: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_31r(0, &val); break;
        case 32: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_32r(0, &val); break;
        case 33: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_33r(0, &val); break;
        case 34: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_34r(0, &val); break;
        case 35: REG_READ_CRU_AVS_AVS_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSC_35r(0, &val); break;
        default: break;
        }
        hi_thr[i] = val;
    }

    /* Extract threshold field from registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            lo_thr[i] = 0;
            hi_thr[i] = 0;
            continue;
        }
#endif
        lo_thr[i] = (lo_thr[i] & 0xffff);
        hi_thr[i] = (hi_thr[i] & 0xffff);
    }

    return (SOC_E_NONE);
}

/*
 * Reads 16b hi, lo h, s thresholds for specified range of remote_oscillators.
 *
 * [in] start_osc specifies first remote_osc
 * [in] num_osc specifies number of remote_osc
 * [out] *lo_thr_h Array to return low threshold_h
 * [out] *hi_thr_h Array to return high threshold_h
 * [out] *lo_thr_s Array to return low threshold_s
 * [out] *hi_thr_s Array to return high threshold_s
 *
 */
static int soc_avs_get_rmt_osc_thr(int start_osc, int num_osc,
                                   uint32_t *lo_thr_h, uint32_t *hi_thr_h,
                                   uint32_t *lo_thr_s, uint32_t *hi_thr_s)
{
    int i;
    uint32_t val;
#if defined(DO_NOT_ACCESS_XOSC)
    avs_info_t *avs_info = &soc_avs_info;
#endif

    if ((lo_thr_h == NULL) || (hi_thr_h == NULL) || (lo_thr_s == NULL) || (hi_thr_s == NULL)) {
        return (SOC_E_PARAM);
    }

    avs_fill_uint32_array(lo_thr_h, 0, num_osc);
    avs_fill_uint32_array(hi_thr_h, 0, num_osc);
    avs_fill_uint32_array(lo_thr_s, 0, num_osc);
    avs_fill_uint32_array(hi_thr_s, 0, num_osc);

    /* read thr_h registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }
#endif
        //lo_thr_h[i] = iproc_reg32_read(CRU_AVS_AVS_PMB_S_000_AVS_ROSC_H_THRESHOLD + i * 0x40);
        switch (i) {
        case 0: REG_READ_CRU_AVS_AVS_PMB_S_000_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 1: REG_READ_CRU_AVS_AVS_PMB_S_001_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 2: REG_READ_CRU_AVS_AVS_PMB_S_002_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 3: REG_READ_CRU_AVS_AVS_PMB_S_003_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 4: REG_READ_CRU_AVS_AVS_PMB_S_004_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 5: REG_READ_CRU_AVS_AVS_PMB_S_005_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 6: REG_READ_CRU_AVS_AVS_PMB_S_006_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 7: REG_READ_CRU_AVS_AVS_PMB_S_007_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 8: REG_READ_CRU_AVS_AVS_PMB_S_008_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        case 9: REG_READ_CRU_AVS_AVS_PMB_S_009_AVS_ROSC_H_THRESHOLDr(0, &val); break;
        default: break; /* Error!! Invalid Remote Osc. */
        }
        lo_thr_h[i] = val;
        soc_avs_msleep(AVS_REMOTE_DELAY);
    }

    /* read thr_s registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            continue;
        }
#endif
        //lo_thr_s[i] = iproc_reg32_read(CRU_AVS_AVS_PMB_S_000_AVS_ROSC_S_THRESHOLD + i * 0x40);
        switch (i) {
        case 0: REG_READ_CRU_AVS_AVS_PMB_S_000_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 1: REG_READ_CRU_AVS_AVS_PMB_S_001_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 2: REG_READ_CRU_AVS_AVS_PMB_S_002_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 3: REG_READ_CRU_AVS_AVS_PMB_S_003_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 4: REG_READ_CRU_AVS_AVS_PMB_S_004_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 5: REG_READ_CRU_AVS_AVS_PMB_S_005_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 6: REG_READ_CRU_AVS_AVS_PMB_S_006_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 7: REG_READ_CRU_AVS_AVS_PMB_S_007_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 8: REG_READ_CRU_AVS_AVS_PMB_S_008_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        case 9: REG_READ_CRU_AVS_AVS_PMB_S_009_AVS_ROSC_S_THRESHOLDr(0, &val); break;
        default: break; /* Error!! Invalid Remote Osc. */
        }
        lo_thr_s[i] = val;
        soc_avs_msleep(AVS_REMOTE_DELAY);
    }

    /* extract hi,lo_thr_h from registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            hi_thr_h[i] = 0;
            lo_thr_h[i] = 0;
            continue;
        }
#endif
        hi_thr_h[i] = (lo_thr_h[i] & 0xffff0000) >> 16;
        lo_thr_h[i] = (lo_thr_h[i] & 0x0000ffff);
    }

    /* extract hi,lo_thr_s from registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
#if defined(DO_NOT_ACCESS_XOSC)
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            hi_thr_s[i] = 0;
            lo_thr_s[i] = 0;
            continue;
        }
#endif
        hi_thr_s[i] = (lo_thr_s[i] & 0xffff0000) >> 16;
        lo_thr_s[i] = (lo_thr_s[i] & 0x0000ffff);
    }

    return (SOC_E_NONE);
}

/*
 * If current voltage is within [Vmin_avs,Vmax_avs], last_set_voltage is adjusted with adj_step.
 * If current voltage is outside the range, last_set_voltage is adjusted by adj_step to bring back within the range
 * (by ignoring sign of adj_step).
 *
 * [in] adjustment_step                 : Positive or Negative value to be added to last_set_voltage.
 * [out] *voltage_change_aborted : TRUE if we could change voltage as requested.
 */
static int soc_avs_adjust_voltage(int adjustment_step, avs_bool_t *voltage_change_aborted)
{
    uint32_t last_set_voltage, new_set_voltage;
    uint32_t read_voltage;
    int32_t new_vmin_avs, new_vmax_avs;
    avs_info_t *avs_info = &soc_avs_info;
#if defined(AVS_USE_OTP_VTRAP_DATA)
    uint32_t alt_vmin_avs;
#endif
    *voltage_change_aborted = AVS_FALSE;

    iproc_dbg("avs_adjust_voltage: adjustment_step=%d\n", adjustment_step);

    new_vmin_avs = avs_info->vmin_avs;
    new_vmax_avs = avs_info->vmax_avs;

    last_set_voltage = soc_last_set_voltage;
    AVS_ASSERT(last_set_voltage > 0);

    SOC_IF_ERROR_RETURN(avs_get_voltage(&read_voltage));

    iproc_dbg("avs_adjust_voltage: read_voltage=%d, vmin_avs=%d, vmax_avs=%d, last_set_voltage=%d\n", read_voltage,
                new_vmin_avs, new_vmax_avs, last_set_voltage);
    if ((new_vmin_avs <= (int32_t)read_voltage) && ((int32_t)read_voltage <= new_vmax_avs)) {
        /* current voltage is within range, normal change. */
        *voltage_change_aborted = AVS_FALSE;
        if (adjustment_step < 0) {
            iproc_dbg("normally decrease\n");
            AVS_ASSERT(last_set_voltage > AVS_ABS(adjustment_step));
            new_set_voltage = last_set_voltage - AVS_ABS(adjustment_step);
        } else {
            iproc_dbg("normally increase\n");
            new_set_voltage = last_set_voltage + AVS_ABS(adjustment_step);
        }
        SOC_IF_ERROR_RETURN(avs_set_voltage(new_set_voltage));
    } else {
        /* current voltage is out of range, normal change aborted, bring it back. */
        *voltage_change_aborted = AVS_TRUE;

        if ((int32_t)read_voltage > new_vmin_avs) {
            AVS_ASSERT(last_set_voltage > AVS_ABS(adjustment_step));
            new_set_voltage = last_set_voltage - AVS_ABS(adjustment_step);
            iproc_dbg("WARNING!! read_voltage=%d, over voltage, voltage will be decreased\n", read_voltage);
        } else {
            new_set_voltage = last_set_voltage + AVS_ABS(adjustment_step);
            iproc_dbg("WARNING!! read_voltage=%d, under voltage, voltage will be increased\n", read_voltage);
        }
        SOC_IF_ERROR_RETURN(avs_set_voltage(new_set_voltage));
    }

    return 0;
}

/*
 * If any osc_count < lo_thr, then increase the voltage.
 * Else if ALL osc_count > hi_thr, then decrease the voltage. If any osc count < its high threshold,
 * then no need to decrease voltage
 *
 * [out] *voltage_change_requested: tell caller that <0,+1,-1> = <NoNeedToChange, NeedToInc, NeedToDec>
 * [out] *voltage_change_aborted   :
 *
 * This is the amount we'll change the value written to the DAC.
 * The step is programmable and, if optimizations are enabled, changes once we converge.
 * This ensures that we should converge or recover quickly (or more quickly) and then adjust more slowly
 * after we converge.
 * With optimization enabled, the dac_step_size will change when required.
 */
static int soc_avs_converge_process(int *voltage_change_requested, avs_bool_t *voltage_change_aborted)
{
    int i;
    uint32_t *posc_count = NULL, *posc_lo_thr = NULL, *posc_hi_thr = NULL;
    uint32_t *posc_count_h = NULL, *posc_count_s = NULL, *posc_lo_thr_h = NULL;
    uint32_t *posc_hi_thr_h = NULL, *posc_lo_thr_s = NULL, *posc_hi_thr_s = NULL;

    avs_bool_t inc_voltage = AVS_FALSE, found_one_osc_below_hi_thr = AVS_FALSE;
    avs_info_t *avs_info = &soc_avs_info;

    /* init outputs */
    *voltage_change_aborted = AVS_FALSE;
    *voltage_change_requested = 0;

    /*step1: Read all cent_osc counts, thresholds */
    avs_fill_uint32_array(&alloc_ptr[0], 0, sizeof(alloc_ptr) / sizeof(uint32_t));
    posc_count = &alloc_ptr[0];
    posc_hi_thr = posc_count + avs_info->num_centrals;
    posc_lo_thr = posc_hi_thr + avs_info->num_centrals;
    iproc_dbg("soc_avs_converge_process: step1\n");
    SOC_IF_ERROR_RETURN(soc_avs_get_cent_osc_count(avs_info->first_cent, avs_info->num_centrals, posc_count));
    SOC_IF_ERROR_RETURN(soc_avs_get_cent_osc_thr(avs_info->first_cent, avs_info->num_centrals, posc_lo_thr,
                                                    posc_hi_thr));

    iproc_dbg("soc_avs_converge_process: step2\n");

    /*step2: Process cent_osc counts, thresholds */
    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_CENT, avs_info)) {
            iproc_dbg("soc_avs_converge_process cent_osc %d excluded\n", i);
            continue;
        }
        iproc_dbg("soc_avs_converge_process cent osc count  %x lo %x hi %x\n", posc_count[i], posc_lo_thr[i],
                            posc_hi_thr[i]);
        if (posc_count[i] == 0) {
            continue;
        }

        if ((posc_lo_thr[i] != 0) && (posc_count[i] < posc_lo_thr[i])) {  /* find one osc_count < lo_thr */
            iproc_dbg("cent_osc_count[%d] count=%d < lo_thr[i]=%d, need to inc voltage\n", i, posc_count[i],
                                posc_lo_thr[i]);
            inc_voltage = AVS_TRUE;
        } else if ((posc_hi_thr[i] != 0) && (posc_count[i] <= posc_hi_thr[i])) {
            /* find one osc_count < hi_thr */
            /* iproc_dbg("cent_osc_count[%d] count=%d < hi_thr[i]=%d, don't dec voltage\n", i, posc_count[i],
               posc_hi_thr[i]); */
            found_one_osc_below_hi_thr = AVS_TRUE;
        }
    }
    iproc_dbg("soc_avs_converge_process: step3\n");
    /*step3: Read all rmt_osc counts, thresholds */
    avs_fill_uint32_array(&alloc_ptr[0], 0, sizeof(alloc_ptr) / sizeof(uint32_t));
    posc_count_h = &alloc_ptr[0];
    posc_count_s = posc_count_h + avs_info->num_remotes;
    posc_hi_thr_h = posc_count_s + avs_info->num_remotes;
    posc_lo_thr_h = posc_hi_thr_h + avs_info->num_remotes;
    posc_hi_thr_s = posc_lo_thr_h + avs_info->num_remotes;
    posc_lo_thr_s = posc_hi_thr_s + avs_info->num_remotes;

    SOC_IF_ERROR_RETURN(soc_avs_get_rmt_osc_count(avs_info->first_rmt, avs_info->num_remotes, posc_count_h,
                                                    posc_count_s));
    SOC_IF_ERROR_RETURN(soc_avs_get_rmt_osc_thr(avs_info->first_rmt, avs_info->num_remotes, posc_lo_thr_h,
                                                            posc_hi_thr_h, posc_lo_thr_s, posc_hi_thr_s));

    iproc_dbg("soc_avs_converge_process: step4\n");
    /*step4: Process remote_osc counts, thresholds */
    for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        if (soc_avs_check_osc_excluded(i, AVS_ROSC_RMT, avs_info)) {
            iproc_dbg("soc_avs_converge_process remote_osc %d excluded\n", i);
            continue;
        }
        iproc_dbg("rmt_osc_s[%d]: count_s=0x%08x, lo_thr_s=0x%08x, hi_thr_s=0x%08x\n", i, posc_count_s[i],
                    posc_lo_thr_s[i], posc_lo_thr_h[i]);
        iproc_dbg("rmt_osc_h[%d]: count_h=0x%08x, lo_thr_h=0x%08x, hi_thr_h=0x%08x\n", i, posc_count_h[i],
                    posc_hi_thr_s[i], posc_hi_thr_h[i]);
        if ((posc_count_h[i] == 0) || (posc_count_s[i] == 0)) continue;
        if (((posc_lo_thr_s[i] != 0) && (posc_count_s[i] < posc_lo_thr_s[i])) ||
            ((posc_lo_thr_h[i] != 0) && (posc_count_h[i] < posc_lo_thr_h[i]))) {
            iproc_dbg("rmt_osc_count[%d] count_s=%d, count_h=%d < lo_thr_s=%d, lo_thr_h=%d, need to inc voltage\n", i,
                        posc_count_s[i], posc_count_h[i], posc_lo_thr_s[i], posc_lo_thr_h[i]);
            inc_voltage = AVS_TRUE;
        } else if (((posc_hi_thr_s[i] != 0) && (posc_count_s[i] <= posc_hi_thr_s[i])) ||
                   ((posc_hi_thr_h[i] != 0) && (posc_count_h[i] <= posc_hi_thr_h[i]))) {
            iproc_dbg("rmt_osc_count[%d] count_s=%d, count_h=%d < hi_thr_s=%d, hi_thr_h=%d, don't dec voltage\n", i,
                        posc_count_s[i], posc_count_h[i],  posc_hi_thr_s[i], posc_hi_thr_h[i]);
            found_one_osc_below_hi_thr = AVS_TRUE;
        }
    }
    iproc_dbg("soc_avs_converge_process: step5\n");
    /*step5: Adjust the core voltage */
    if (inc_voltage == AVS_TRUE) {
        *voltage_change_requested = +1;
        SOC_IF_ERROR_RETURN(soc_avs_adjust_voltage(AVS_TRACK_VINC_STEP, voltage_change_aborted));
    } else if (found_one_osc_below_hi_thr == AVS_FALSE) {
        if ((soc_last_set_voltage - AVS_ABS(AVS_TRACK_VDEC_STEP)) >= AVS_ABS_MIN_CVOLT) {
            *voltage_change_requested = -1;
            SOC_IF_ERROR_RETURN(soc_avs_adjust_voltage(AVS_TRACK_VDEC_STEP, voltage_change_aborted));
        }
    }
    iproc_dbg("soc_avs_converge_process: done\n");
    return 0;
}
#endif
#endif

/**********************************************************************************************************************
 Public APIs Definitions
**********************************************************************************************************************/

int avs_init(int unit)
{
    uint32_t val;
    avs_info_t *avs_info = &soc_avs_info;
    avs_info->num_centrals = AVS_AVENGER_NUMBER_OF_CENTRALS;
    avs_info->first_cent = AVS_AVENGER_FIRST_CENTRAL;
    avs_info->cent_xbmp[0] = AVS_AVENGER_CENTRAL_XBMP0;
    avs_info->cent_xbmp[1] = AVS_AVENGER_CENTRAL_XBMP1;

    avs_info->num_remotes = AVS_AVENGER_NUMBER_OF_REMOTES;
    avs_info->first_rmt = AVS_AVENGER_FIRST_REMOTE;
    avs_info->rmt_xbmp[0] = AVS_AVENGER_REMOTE_XBMP0;

    avs_info->vmin_avs = AVS_INT(AVS_AVENGER_VMIN_AVS);
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
        avs_info->vmax_avs = AVS_INT(AVS_AVENGER_VMAX_AVS_A0);
        avs_info->vmargin_low = AVS_INT(AVS_AVENGER_VMARGIN_LOW_A0);
        avs_info->vmargin_high = AVS_INT(AVS_AVENGER_VMARGIN_HIGH_A0);
    } else {
        avs_info->vmax_avs = AVS_INT(AVS_AVENGER_VMAX_AVS_B0);
        avs_info->vmargin_low = AVS_INT(AVS_AVENGER_VMARGIN_LOW_B0);
        avs_info->vmargin_high = AVS_INT(AVS_AVENGER_VMARGIN_HIGH_B0);
    }
    avs_info->vmargin_110C = AVS_INT(AVS_AVENGER_VMARGIN_110C);

    soc_avs_enable_block(unit, 1);
    /* unmask all sensors for PVT measurements */
    //iproc_reg32_write(CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR, AVS_AVENGER_PVT_MNTR_XBMP1);
    val = AVS_AVENGER_PVT_MNTR_XBMP1;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, &val);
    //soc_avs_msleep(3000);
    iproc_dbg("avs_init done\n");
    return 0;
}

/*
 * Runs initialize_oscs, find_final_voltage, set_new_thr.
 */
int avs_start(int unit)
{
    uint32_t vpred = 0, vavs_sv = 0;
    uint32_t final_avs_voltage;
    /* dump the excluded oscillators at compile time */
    //iproc_dbg("Statical excluded osc bitmap:\n");
    //avs_show_rmt_xbmp();
    //avs_show_cent_xbmp();
    /*iproc_dbg(" ... avs_start called...\n");*/
    /* TODO ??? the oscillator initialization is not necessary in 28nm */
    soc_avs_initialize_oscs(unit);
    currentDAC = 0;

    iproc_dbg("\navs_start1\n");
    /* find & set Vavs */
    SOC_IF_ERROR_RETURN(soc_avs_find_final_voltage(unit, &vpred, &vavs_sv));

    iproc_dbg("\navs_start2\n");
    if (vpred == 0) {
        return (-ENXIO);
    }
    soc_avs_info.vpred = vpred;

    /* cap the avs predicted final voltage including margin to min cvlot */
    if (vavs_sv < AVS_ABS_MIN_CVOLT) vavs_sv = AVS_ABS_MIN_CVOLT;

    if (0) {
      /* dominic: do not see the value of soc_avs_set_new_thr, other than
       * adding AVS_VINC_FOR_SET_NEW_THR, maybe this is done for dynamic
       * AVS. This is skipped to reduce run time, if we want to implement
       * truly dynamic AVS, the following code may needs to be revived.
       *
       * set avs voltage. low thresholds will be set at this voltage
       */
      SOC_IF_ERROR_RETURN(avs_set_voltage(unit, (uint32_t)vavs_sv));
      SOC_IF_ERROR_RETURN(soc_avs_set_new_thr(unit, vavs_sv));
      iproc_dbg("avs_start done\n\n");
      //iproc_dbg("avs_start3\n");
      //iproc_dbg("New excluded osc bitmap:\n");
      //avs_show_rmt_xbmp();
      //avs_show_cent_xbmp();
    } else {
        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
            final_avs_voltage = vavs_sv + AVS_VINC_FOR_SET_NEW_THR + AVS_VINC_FOR_IR_DROP_AVENGER_A0;
        } else {
            final_avs_voltage = vavs_sv + AVS_VINC_FOR_SET_NEW_THR + AVS_VINC_FOR_IR_DROP_AVENGER_B0;
        }
      iproc_dbg("avs done with voltage set to %d\n\n", final_avs_voltage);
      SOC_IF_ERROR_RETURN(avs_set_voltage(unit, (uint32_t)final_avs_voltage));
    }
    return (0);
}

#if 0
void avs_exit(void)
{
    soc_avs_enable_block(0);
}

int m0_avs_get_info(struct m0_ipc_m0_cmd_get_avsinfo *avsinfo) {
    int ret = 0;
    avsinfo->vcore = soc_last_set_voltage;
    avsinfo->vpred = soc_avs_info.vpred;
    return ret;
}
#endif


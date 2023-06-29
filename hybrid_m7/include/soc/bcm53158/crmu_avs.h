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

#ifndef __AVS_HDR__
#define __AVS_HDR__

#define iproc_err sal_printf
#define iproc_dbg sal_printf
#define iproc_prt sal_printf

extern void avs_reg_write(int unit, uint32 address, uint32 reg_data);
extern uint32 avs_reg_read(int unit, uint32 address);

#define AVS_SUPPORT_ENABLED

#define CONFIG_MACH_AVENGER
#define BCM_AVENGER_SUPPORT

/* ------------------------------------------------------------------------- */
#define AVS_DEFAULT_VMIN_AVS              0.81
#define AVS_DEFAULT_VMAX_AVS              1.03
#define AVS_DEFAULT_VMARGIN_HIGH          0.000 /*-0.020 */
#define AVS_DEFAULT_VMARGIN_LOW           0.025 /*-0.020 */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
#ifdef BCM_AVENGER_SUPPORT
/* there are 36 rosc in Avenger, but only 20(0~7, 12~23) used, others skipped */
#define AVS_AVENGER_NUMBER_OF_CENTRALS     24
#define AVS_AVENGER_FIRST_CENTRAL          0
/* #define AVS_AVENGER_CENTRAL_XBMP0          0x00000000 */
/* #define AVS_AVENGER_CENTRAL_XBMP1          0xFFFFFFF0 */
/* dwl skip central OSC's because remote OSC dominates across split-lots */
// #define AVS_AVENGER_CENTRAL_XBMP0          0xFF000F00   /* SKIP 8~11,24~31 */
#define AVS_AVENGER_CENTRAL_XBMP0          0xFFFFFFFF   /* SKIP 0~31 */
/* Don't know why, if AVS_AVENGER_CENTRAL_XBMP1=0xffffffff */
/* temperature monitor not working, have to leave one of them on */
/* SKIP 33~35 */
#define AVS_AVENGER_CENTRAL_XBMP1          0xFFFFFFFe
/* SKIP all except Temperature and 1V_0 */
#define AVS_AVENGER_PVT_MNTR_XBMP1         0xFFFFFFF6

#define AVS_AVENGER_NUMBER_OF_REMOTES      10
#define AVS_AVENGER_FIRST_REMOTE           0
#define AVS_AVENGER_REMOTE_XBMP0           0x00000000
/* #define AVS_AVENGER_REMOTE_XBMP0           0x000003E0   // SKIP 5~9 */

#define AVS_AVENGER_VMIN_AVS               AVS_DEFAULT_VMIN_AVS
#define AVS_AVENGER_VMAX_AVS               AVS_DEFAULT_VMAX_AVS
#define AVS_AVENGER_VMARGIN_HIGH           AVS_DEFAULT_VMARGIN_HIGH
#define AVS_AVENGER_VMARGIN_LOW            AVS_DEFAULT_VMARGIN_LOW

/* Temperature related margin, in units of V
     AVS_AVENGER_VMARGIN_HIGH_T is margin at 110C
     no margin to add at 0C and below
*/
#define AVS_AVENGER_VMARGIN_110C           0.025

/* These are the threshold values at 0.9V, SS, 125C condition */
/* Same with Greyhound threshold values - they are the same technology. */
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_0  2.74895
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_1  2.59525
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_2  2.12515
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_3  1.3756
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_4  3.72545
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_5  3.4551
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_6  2.8331
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_7  1.85085
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_8  10.722
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_9  9.8360
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_10 8.1625
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_11 5.2245
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_12 7.7715
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_13 6.8730
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_14 5.7190
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_15 3.59835
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_16 7.2590
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_17 6.3300
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_18 5.3105
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_19 3.4723
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_20 4.23225
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_21 3.6809
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_22 3.12725
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_23 1.99115
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_24 0.9911
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_25 0.9144
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_26 0.87855
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_27 0.89025
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_28 0.81955
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_29 0.90125
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_30 0.6245
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_31 0.62915
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_32 0.21924
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_33 0.16357
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_34 0.19974
#define AVS_AVENGER_CENT_FREQ_THRESHOLD_35 0.16873

#define AVS_AVENGER_RMT_FREQ_THRESHOLD_GS  2.59525
#define AVS_AVENGER_RMT_FREQ_THRESHOLD_GH  1.3756

#endif              /* BCM_AVENGER_SUPPORT */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
typedef enum {
    SOC_E_NONE = 0,
    SOC_E_INTERNAL = -1,
    SOC_E_MEMORY = -2,
    SOC_E_UNIT = -3,
    SOC_E_PARAM = -4,
    SOC_E_EMPTY = -5,
    SOC_E_FULL = -6,
    SOC_E_NOT_FOUND = -7,
    SOC_E_EXISTS = -8,
    SOC_E_TIMEOUT = -9,
    SOC_E_BUSY = -10,
    SOC_E_FAIL = -11,
    SOC_E_DISABLED = -12,
    SOC_E_BADID = -13,
    SOC_E_RESOURCE = -14,
    SOC_E_CONFIG = -15,
    SOC_E_UNAVAIL = -16,
    SOC_E_INIT = -17,
    SOC_E_PORT = -18,

    SOC_E_LIMIT = -19   /* Must be last */
} soc_error_t;

typedef char *parse_key_t;
#define ARGS_CNT        16  /* Max argv entries */
#define ARGS_BUFFER     128 /* # bytes total for arguments */
typedef struct args_s {
    parse_key_t a_cmd;  /* Initial string */
    char *a_argv[ARGS_CNT]; /* argv pointers */
    char a_buffer[ARGS_BUFFER]; /* Split up buffer */
    int a_argc;     /* Parsed arg counter */
    int a_arg;      /* Pointer to NEXT arg */
} args_t;

#define ARG_CMD(_a)     (_a)->a_cmd
#define _ARG_CUR(_a)    ((_a)->a_argv[(_a)->a_arg])
#define ARG_CUR(_a)     (((_a)->a_arg >= (_a)->a_argc) ? NULL : _ARG_CUR(_a))
#define _ARG_GET(_a)    ((_a)->a_argv[(_a)->a_arg++])
#define ARG_GET(_a)     (((_a)->a_arg >= (_a)->a_argc) ? NULL : _ARG_GET(_a))
#define ARG_NEXT(_a)    (_a)->a_arg++
#define ARG_PREV(_a)    (_a)->a_arg--
#define ARG_DISCARD(_a) ((_a)->a_arg = (_a)->a_argc)
#define ARG_CNT(_a)     ((_a)->a_argc - (_a)->a_arg)

typedef enum cmd_result_e {
    CMD_OK = 0,     /* Command completed successfully */
    CMD_FAIL = -1,      /* Command failed */
    CMD_USAGE = -2,     /* Command failed, print usage  */
    CMD_NFND = -3,      /* Command not found */
    CMD_EXIT = -4,      /* Exit current shell level */
    CMD_INTR = -5,      /* Command interrupted */
    CMD_NOTIMPL = -6    /* Command not implemented */
} cmd_result_t;

typedef cmd_result_t (*cmd_func_t)(int, args_t *);

/*
 * Typedef:    cmd_t
 * Purpose:    Table command match structure.
 */
typedef struct cmd_s {
    parse_key_t c_cmd;  /* Command string */
    cmd_func_t c_f;     /* Function to call */
    const char *c_usage;    /* Usage string */
    const char *c_help; /* Help string */
} cmd_t;

//#define SOC_IF_ERROR_RETURN(op)      do { int __rv__; if ((__rv__ = (op)) < 0) { iproc_err("Failed at %d, ret=%d\n", __LINE__, __rv__); return(__rv__); } } while(0)
#define SOC_IF_NULL_RETURN(arg)    do {   \
                                    if((arg)==NULL) \
                                        {   \
                                        iproc_err("NULL arg at %d\n", __LINE__);    \
                                        return (SOC_E_PARAM);   \
                                        }   \
                                    } while(0)
#define SOC_SUCCESS(rv)     ((rv) >= 0)
#define SOC_FAILURE(rv)     ((rv) < 0)

#define EPERM        2  /* Operation not permitted */
#define ENXIO        6  /* No such device or address */
#define EFAIL        7  /* The Operation failed */
#define EINVAL      22  /* Invalid argument */

/*
* vpred: avs predicted raw voltage in 0.1 milivolt
*   This will be useful in debug.
*   The final voltage set will include other margins
*   on top of the vpred and may be capped to
*   miinimum CVOLT.
*
* vcore: last set core voltage in 0.1 milivolt
*   this reflects the last set voltage on vcore.
*/
struct m0_ipc_m0_cmd_get_avsinfo {
    uint32_t vpred;
    uint32_t vcore;
};

/*
 * currentDAC is initialized to 0, it remembers that last DAC that
 * was used to set AVS voltage so that it is available for the
 * next iteration.
 */
uint32 currentDAC;

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* Global function prototypes */
//extern int soc_avs_start(int unit);
//extern int soc_avs_track(int unit);
/* ------------------------------------------------------------------------- */

#endif              /* __AVS_HDR__ */

/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_cfgx.h
 */

/* TRGTRG TODO - break this up a bit based on api's, defines and structures */

#ifndef DOT1AS_CFGX_H
#define DOT1AS_CFGX_H

#include "avb.h"
#include "avb_cfgx.h"
#include "dot1as_api.h"

#define CFGX_DOT1AS_PORT_ROLE_LEN   4
#define CFGX_AVNU_CLOCK_STATE_LEN   2
extern uint8_t dot1asCfgxPortRoles[CFGX_DOT1AS_PORT_ROLE_LEN];
extern uint8_t avnuCfgxClockState[CFGX_AVNU_CLOCK_STATE_LEN];

#define DOT1AS_CFG_KEY_STR                        "dot1asCfg"
#define DOT1AS_CFG_KEY_                            0x1000

#define DOT1AS_GLOBAL_ADMIN_KEY_STR               "globalAdmin"
#define DOT1AS_GLOBAL_ADMIN_KEY                    0x1001
#define DOT1AS_GLOBAL_ADMIN_DEFAULT                1
#define DOT1AS_PRIORITY1_KEY_STR                  "priority1"
#define DOT1AS_PRIORITY1_KEY                       0x1002
#define DOT1AS_PRIORITY1_MIN                       0
#define DOT1AS_PRIORITY1_MAX                       255
#define DOT1AS_PRIORITY1_DEFAULT                   246
#define DOT1AS_PRIORITY2_KEY_STR                  "priority2"
#define DOT1AS_PRIORITY2_KEY                       0x1003
#define DOT1AS_PRIORITY2_MIN                       0
#define DOT1AS_PRIORITY2_MAX                       255
#define DOT1AS_PRIORITY2_DEFAULT                   248
#define DOT1AS_AVNU_MODE_KEY_STR                  "avnuMode"
#define DOT1AS_AVNU_MODE_KEY                       0x1004
#define DOT1AS_AVNU_MODE_DEFAULT                   AVB_FALSE
#define DOT1AS_AVNU_CLOCK_STATE_KEY_STR            "avnuClockState"
#define DOT1AS_AVNU_CLOCK_STATE_KEY                0x1005
#define DOT1AS_AVNU_CLOCK_STATE_ENUM_LEN           CFGX_AVNU_CLOCK_STATE_LEN
#define DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_KEY_STR  "avnuSyncAbsenceTimeout"
#define DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_KEY       0x1006
#define DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_MIN       1
#define DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_MAX       20
#define DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_DEFAULT   10
#define DOT1AS_INTF_CFG_KEY_STR                    INTF_CFG_KEY_STR
#define DOT1AS_INTF_CFG_KEY                        0x1800
#define DOT1AS_INTF_KEY_STR                        INTF_KEY_STR
#define DOT1AS_INTF_KEY                            0x1801
#define DOT1AS_INTF_MODE_KEY_STR                   MODE_KEY_STR
#define DOT1AS_INTF_MODE_KEY                       0x1802
#define DOT1AS_INTF_MODE_DEFAULT                   AVB_TRUE
#define DOT1AS_IMP_INTF_MODE_DEFAULT               AVB_FALSE
#define DOT1AS_INTF_INIT_NBR_PDELAY_KEY_STR       "initNbrPdelay"
#define DOT1AS_INTF_INIT_NBR_PDELAY_KEY            0x1803
#define DOT1AS_INTF_INIT_NBR_PDELAY_MIN           -5
#define DOT1AS_INTF_INIT_NBR_PDELAY_MAX            5
#define DOT1AS_INTF_INIT_NBR_PDELAY_DEFAULT        0
#define DOT1AS_INTF_INIT_SYNC_KEY_STR             "initSync"
#define DOT1AS_INTF_INIT_SYNC_KEY                  0x1804
#define DOT1AS_INTF_INIT_SYNC_MIN                 -5
#define DOT1AS_INTF_INIT_SYNC_MAX                  5
#define DOT1AS_INTF_INIT_SYNC_DEFAULT              0  /* changed from -3 to 0, check later */
#define DOT1AS_INTF_INIT_ANN_KEY_STR              "initAnn"
#define DOT1AS_INTF_INIT_ANN_KEY                   0x1805
#define DOT1AS_INTF_INIT_ANN_MIN                  -5
#define DOT1AS_INTF_INIT_ANN_MAX                   5
#define DOT1AS_INTF_INIT_ANN_DEFAULT               0
#define DOT1AS_INTF_ANN_TIMEOUT_KEY_STR           "annTimeout"
#define DOT1AS_INTF_ANN_TIMEOUT_KEY                0x1806
#define DOT1AS_INTF_ANN_TIMEOUT_MIN                2
#define DOT1AS_INTF_ANN_TIMEOUT_MAX                255
#define DOT1AS_INTF_ANN_TIMEOUT_DEFAULT            3
#define DOT1AS_INTF_SYNC_TIMEOUT_KEY_STR          "syncTimeout"
#define DOT1AS_INTF_SYNC_TIMEOUT_KEY               0x1807
#define DOT1AS_INTF_SYNC_TIMEOUT_MIN               2
#define DOT1AS_INTF_SYNC_TIMEOUT_MAX               255
#define DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT           3
#define DOT1AS_INTF_PDELAY_THRESH_KEY_STR         "pdelayThresh"
#define DOT1AS_INTF_PDELAY_THRESH_KEY              0x1808
#define DOT1AS_INTF_PDELAY_THRESH_MIN              0
#define DOT1AS_INTF_PDELAY_THRESH_MAX              1000000000
#define DOT1AS_INTF_PDELAY_THRESH_DEFAULT          20000
#define DOT1AS_INTF_LOST_RESP_KEY_STR             "lostResp"
#define DOT1AS_INTF_LOST_RESP_KEY                  0x1809
#define DOT1AS_INTF_LOST_RESP_MIN                  0
#define DOT1AS_INTF_LOST_RESP_MAX                  65535
#define DOT1AS_INTF_LOST_RESP_DEFAULT              3
#define DOT1AS_INTF_AVNU_ROLE_KEY_STR             "avnuRole"
#define DOT1AS_INTF_AVNU_ROLE_KEY                  0x180A
#define DOT1AS_INTF_AVNU_ROLE_ENUM_LEN             CFGX_DOT1AS_PORT_ROLE_LEN
#define DOT1AS_INTF_AVNU_ROLE_DEFAULT              DOT1AS_ROLE_PASSIVE
#define DOT1AS_INTF_AVNU_AS_CAP_KEY_STR           "avnuAsCap"
#define DOT1AS_INTF_AVNU_AS_CAP_KEY                0x180B
#define DOT1AS_INTF_AVNU_AS_CAP_DEFAULT            AVB_FALSE
#define DOT1AS_INTF_AVNU_NBR_PDELAY_KEY_STR       "avnuNbrPdelay"
#define DOT1AS_INTF_AVNU_NBR_PDELAY_KEY            0x180C
#define DOT1AS_INTF_AVNU_NBR_PDELAY_MIN            0
#define DOT1AS_INTF_AVNU_NBR_PDELAY_MAX            1000000000
#define DOT1AS_INTF_AVNU_NBR_PDELAY_DEFAULT        2500
#define DOT1AS_INTF_AVNU_INIT_PDELAY_KEY_STR      "avnuInitPdelay"
#define DOT1AS_INTF_AVNU_INIT_PDELAY_KEY           0x180D
#define DOT1AS_INTF_AVNU_INIT_PDELAY_MIN          -5
#define DOT1AS_INTF_AVNU_INIT_PDELAY_MAX           127      /* Valid values are -5 to 5 and 127 to disable sending pDelay on master port*/
#define DOT1AS_INTF_AVNU_INIT_PDELAY_DEFAULT       0
#define DOT1AS_INTF_AVNU_INIT_SYNC_KEY_STR        "avnuInitSync"
#define DOT1AS_INTF_AVNU_INIT_SYNC_KEY             0x180E
#define DOT1AS_INTF_AVNU_INIT_SYNC_MIN            -5
#define DOT1AS_INTF_AVNU_INIT_SYNC_MAX             5
#define DOT1AS_INTF_AVNU_INIT_SYNC_DEFAULT        -5
#define DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_KEY_STR "avnuOperPdelay"
#define DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_KEY      0x180F
#define DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_MIN      0
#define DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_MAX      127 /* Valid values are 0 to 3 and 127 to disable sending pDelay on master port*/
#define DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_DEFAULT  0

typedef struct dot1as_intf_cfgx_s {
    uint16_t            intf;
    bool_cfgx_t         pttPortEnabled;
    int8_cfgx_t         initialLogNbrPDelayInterval;
    int8_cfgx_t         initialLogSyncInterval;
    int8_cfgx_t         initialLogAnnounceInterval;
    uint8_cfgx_t        announceReceiptTimeout;
    uint8_cfgx_t        syncReceiptTimeout;
    uint32_cfgx_t       neighborPropDelayThresh;
    uint32_cfgx_t       allowedLostResponses;
    enum8_len16_cfgx_t  avnuPortRole;
    bool_cfgx_t         avnuDot1asCapable;
    uint32_cfgx_t       avnuNeighborPropDelay;
    int8_cfgx_t         avnuInitialLogPdelayReqInterval;
    int8_cfgx_t         avnuInitialLogSyncInterval;
    int8_cfgx_t         avnuOperationalLogPdelayReqInterval;
} dot1as_intf_cfgx_t;


typedef struct dot1as_cfgx_s {
    cfgx_comp_hdr_t     hdr;
    bool_cfgx_t         globalAdmin;
    uint8_cfgx_t        priority1;
    uint8_cfgx_t        priority2;
    bool_cfgx_t         avnuMode;
    enum8_len16_cfgx_t  avnuClockState;
    uint8_cfgx_t        avnuSyncAbsenceTimeout;
    dot1as_intf_cfgx_t  *intfCfg;
} dot1as_cfgx_t;

extern AVB_RC_t dot1asCfgxInit(void);
extern void     dot1asCfgxShow();
extern void     avnuCfgxShow(void);
extern void     dot1asCfgxDump(void); /* Debug only */
extern AVB_RC_t dot1asCfgxApply(void);


extern AVB_RC_t  dot1asCfgxGlobalAdminGet(uint8_t *val);
extern AVB_RC_t  dot1asCfgxGlobalAdminSet(uint8_t  val);
extern AVB_RC_t  portCfgxAdminModeGet(uint16_t intfNum, uint8_t *val);
extern AVB_RC_t  dot1asCfgxPriority1Get(uint8_t *val);
extern AVB_RC_t  dot1asCfgxPriority1Set(uint8_t  val);
extern AVB_RC_t  dot1asCfgxPriority2Get(uint8_t *val);
extern AVB_RC_t  dot1asCfgxPriority2Set(uint8_t  val);
extern AVB_RC_t  dot1asCfgxAVNUModeGet(uint8_t *val);
extern AVB_RC_t  dot1asCfgxAVNUModeSet(uint8_t  val);
extern AVB_RC_t  dot1asCfgxAVNUClockStateGet(uint8_t *val);
extern AVB_RC_t  dot1asCfgxAVNUClockStateSet(uint8_t  val);
extern AVB_RC_t  dot1asCfgxAVNUSyncAbsenceTimeoutGet(uint8_t *val);
extern AVB_RC_t  dot1asCfgxAVNUSyncAbsenceTimeoutSet(uint8_t  val);
extern AVB_RC_t  dot1asCfgxIntfPttPortEnabledGet(uint16_t intfNum, uint8_t *val);
extern AVB_RC_t  dot1asCfgxIntfPttPortEnabledSet(uint16_t intfNum, uint8_t  val);
extern AVB_RC_t  dot1asCfgxIntfInitialLogNbrPDelayIntervalGet(uint16_t intfNum, int8_t *val);
extern AVB_RC_t  dot1asCfgxIntfInitialLogNbrPDelayIntervalSet(uint16_t intfNum, int8_t  val);
extern AVB_RC_t  dot1asCfgxIntfInitialLogSyncIntervalGet(uint16_t intfNum, int8_t *val);
extern AVB_RC_t  dot1asCfgxIntfInitialLogSyncIntervalSet(uint16_t intfNum, int8_t  val);
extern AVB_RC_t  dot1asCfgxIntfInitialLogAnnounceIntervalGet(uint16_t intfNum, int8_t *val);
extern AVB_RC_t  dot1asCfgxIntfInitialLogAnnounceIntervalSet(uint16_t intfNum, int8_t  val);
extern AVB_RC_t  dot1asCfgxIntfAnnounceReceiptTimeoutGet(uint16_t intfNum, uint8_t *val);
extern AVB_RC_t  dot1asCfgxIntfAnnounceReceiptTimeoutSet(uint16_t intfNum, uint8_t  val);
extern AVB_RC_t  dot1asCfgxIntfSyncReceiptTimeoutGet(uint16_t intfNum, uint8_t *val);
extern AVB_RC_t  dot1asCfgxIntfSyncReceiptTimeoutSet(uint16_t intfNum, uint8_t  val);
extern AVB_RC_t  dot1asCfgxIntfNeighborPropDelayThreshGet(uint16_t intfNum, uint32_t *val);
extern AVB_RC_t  dot1asCfgxIntfNeighborPropDelayThreshSet(uint16_t intfNum, uint32_t  val);
extern AVB_RC_t  dot1asCfgxIntfAllowedLostResponsesGet(uint16_t intfNum, uint32_t *val);
extern AVB_RC_t  dot1asCfgxIntfAllowedLostResponsesSet(uint16_t intfNum, uint32_t  val);
extern AVB_RC_t  dot1asCfgxIntfAVNUPortRoleGet(uint16_t intfNum, uint8_t *val);
extern AVB_RC_t  dot1asCfgxIntfAVNUPortRoleSet(uint16_t intfNum, uint8_t  val);
extern AVB_RC_t  dot1asCfgxIntfAVNUDot1asCapableGet(uint16_t intfNum, uint8_t *val);
extern AVB_RC_t  dot1asCfgxIntfAVNUDot1asCapableSet(uint16_t intfNum, uint8_t  val);
extern AVB_RC_t  dot1asCfgxIntfAVNUNeighborPropDelayGet(uint16_t intfNum, uint32_t *val);
extern AVB_RC_t  dot1asCfgxIntfAVNUNeighborPropDelaySet(uint16_t intfNum, uint32_t  val);
extern AVB_RC_t  dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet(uint16_t intfNum, int8_t *val);
extern AVB_RC_t  dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalSet(uint16_t intfNum, int8_t  val);
extern AVB_RC_t  dot1asCfgxIntfAVNUInitialLogSyncIntervalGet(uint16_t intfNum, int8_t *val);
extern AVB_RC_t  dot1asCfgxIntfAVNUInitialLogSyncIntervalSet(uint16_t intfNum, int8_t  val);
extern AVB_RC_t  dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalGet(uint16_t intfNum, int8_t *val);
extern AVB_RC_t  dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalSet(uint16_t intfNum, int8_t  val);

#endif /* DOT1AS_CFGX_H */

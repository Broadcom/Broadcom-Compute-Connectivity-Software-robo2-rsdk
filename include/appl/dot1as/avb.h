/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:    avb.h
 */

#ifndef AVB_H
#define AVB_H


#define AVB_VERSION 0x01050000       /* 1.5.0.0 */
#define AVB_VERSION_SUFFIX_STRING ""  /* AVB version suffic string, possibly "-ea" or "-beta". "" by default */
#define AVB_VERSION_SUFFIX_STRING_MAX_LEN 8

#ifdef TOOLCHAIN_arm
/* MUST USE THE FOLLOWING MEMCPY FOR ARM TOOLCHAIN, ELSE ARM CPU COULD THROW
 * DATA EXCEPTION
 */
#define memcpy __aeabi_memcpy
extern void *__aeabi_memcpy(void *, const void *, size_t);
#define memset __aeabi_memset
extern void *__aeabi_memset(void *, char, int);
#endif

extern uint8_t  avbMaxIntfaces_g;
extern uint16_t avbIntfMaskPhysP8_g;
extern uint8_t  avbMaxIntfPhysP8_g;

#define DOT1AS_TASK_STACK_SIZE   2048
#define DOT1AS_TASK_PRIO 90
#define DOT1AS_MAX_QUEUE_MSGS 10
#define DOT1AS_PKT_RECEIVE_PRIORITY 10

/* DOT1AS message structure */
struct dot1asMsg {
    uint8 port;
    uint32 ingTs;
    uint8 *buffer;
    uint16 bufferSize;
};

#if DEV_BUILD_DOT1AS
/* This structure is defined only for the testing purpose */
typedef struct {
  uint8_t  isTSValWrong;                   /* wrong transport specific in all messages */
  uint8_t  isStopSync;                     /* stop sending followup */
  uint8_t  isStopFollowup;                 /* stop sending followup */
  uint8_t  isFollowupSeqIdWrong;           /* wrong sequence id in Followup message */
  uint8_t  isStopPdelayResp;               /* stop sending pdelay response message */
  uint8_t  isStopPdelayRespFollowup;       /* stop sending pdelay response followup message */
  uint8_t  isPdelayRespSeqIdWrong;         /* wrong sequence id in pDelay response message */
  uint8_t  isPdelayRespFollowupSeqIdWrong; /* wrong sequence id in pDelay response followup  message */
}AVB_DOT1AS_TEST_t;

AVB_DOT1AS_TEST_t avbDot1asTest_g;
#endif

typedef enum {
  AVB_AVNU_DEVICE_STATE_NOT_APPLICABLE        = 0,
  AVB_AVNU_DEVICE_STATE_ETH_READY             = 1,
  AVB_AVNU_DEVICE_STATE_AVB_SYNC              = 2,
  AVB_AVNU_DEVICE_STATE_SYNC_RECEIPT_TIMEOUT  = 3,
} AVB_AVNU_DEVICE_STATE_t;

typedef enum {
  AVB_SUCCESS = 0,
  AVB_FAILURE = 1
} AVB_RC_t;

typedef enum {
  AVB_FALSE = 0,
  AVB_TRUE  = 1
} AVB_BOOL_t;

typedef enum {
  AVB_DISABLE = 0,
  AVB_ENABLE  = 1
} AVB_MODE_t;

typedef enum {
  AVB_LAG_OP_ADD      = 0,
  AVB_LAG_OP_REMOVE   = 1,
  AVB_LAG_OP_LAST     = 2
} AVB_LAG_OP_t;

#define AVB_MAC_ADDR_LEN             6   /* MAC address length */

/* AVB LAG related */
#define AVB_LAG_MEMBER_MASK   avb_lag_member_mask()    /* ports 0-4, port 4, if in BRMode */
#define AVB_LAG_INDEX_0       0
#define AVB_LAG_INDEX_1       1
#define AVB_LAG_INDEX_NONE    2

#define AVB_INTF_INVALID      0xFF    /* Invalid interface index */

#define AVB_PORT_LINK_UP        1
#define AVB_PORT_LINK_DOWN      0

#define AVB_TRACE(format, args...)                                  \
 do                                                                 \
 {                                                                  \
   sal_printf(format, ##args);                                      \
 } while(0)

#define AVB_TRACE_LINE(n)                                           \
  do {                                                              \
    uint32_t i;                                                     \
    for (i=0; i < (n); i++) { AVB_TRACE("-"); }                     \
    AVB_TRACE("\n");                                                \
  } while(0)                                                        \

extern AVB_RC_t   avb_intf_external_to_internal(uint32_t intf, uint32_t *intIntf);
extern uint32_t portLinkChangeCountGet(uint32_t port);
#endif /* AVB_H */

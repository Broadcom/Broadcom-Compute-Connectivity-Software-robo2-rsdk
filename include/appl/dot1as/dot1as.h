/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:    dot1as.h
 */

#ifndef DOT1AS_H
#define DOT1AS_H

#include "osal/sal_types.h"
#include "osal/sal_console.h"
#include "osal/sal_alloc.h"
#include "osal/sal_util.h"
#include "shared/types.h"
#include "fsal_int/port.h"
#include "fsal/ts.h"
#include "avb.h"
#include "dot1as_api.h"

typedef uint64_t dot1asTime_t;
extern uint8_t dot1asIsReady_g;
#define DOT1AS_IS_READY (dot1asIsReady_g == AVB_TRUE)

#define ETHER_HEADER_LEN 14
#define ETHER_ADDR_LEN 6
extern uint8_t dot1asEthSrcAddr[ETHER_ADDR_LEN];
extern uint8_t *avbCfgxIntfNum;

#define DOT1AS_IS_INTF_CPU(intf) (intf == CBX_PORT_ECPU)
#define ETHER_PROTO_DOT1AS  0x88F7

#define DOT1AS_DEBUG_TIMESYNC_LOG 1
#define DOT1AS_DEBUG_PDELAY_LOG 1

/* 802.1AS control thread priority */
#define DOT1AS_TIMER_THREAD_PRIO   HIGH_PRIO_APP
#define DOT1AS_RX_THREAD_PRIO      MED_PRIO_APP
#define DOT1AS_TX_THREAD_PRIO      LOW_PRIO_APP

/* 802.1AS maximum number of interfaces */
extern uint8_t dot1asMaxInterfaceCount_g;

/* 802.1AS maximum number of configurable interfaces */
extern uint8_t dot1asMaxInterfacesCfg_g;

/* Default 802.1AS admin mode */
#define DOT1AS_MODE_DEFAULT         DOT1AS_ENABLE

/* Maximum number of hops in the 802.1AS domain. Mainly used for limiting the
 * path trace TLV
 */
#define DOT1AS_MAX_HOPS_PER_DOMAIN   8

/* Maximum size of the 802.1AS PDU, includes raw and parsed PDU formats */
/* NOTE: This value must accomodate all 802.1AS PDU structures defined below */
#define DOT1AS_MAX_PDU_SIZE  256

/* Priority of the 802.1AS PDUs when transmitted from the switch */
#define DOT1AS_PDU_TX_PCP_PRIO  7

/* Maximum value of the stepsRemoved field to qualify */
#define DOT1AS_MAX_STEPS_REMOVED           255

/* Value of priority1 for GM not-capable system */
#define DOT1AS_PRIO1_NOT_GM_CAPABLE        255

/* 802.1AS priorities values and types */
#define DOT1AS_MIN_PRIORITY_VALUE          0
#define DOT1AS_MAX_PRIORITY_VALUE          255
#define DOT1AS_PRIO1_DEFAULT               246
#define DOT1AS_PRIO2_DEFAULT               248

/* 802.1AS transmit interval limits in millisec and log base 2.
 * Note: Although standard supports a large range of values, only a subset
 * of values are supported.
 */
#define DOT1AS_PDU_TX_INTL_LOG2_MAX    5       /* 127 as per standard */
#define DOT1AS_PDU_TX_INTL_LOG2_MIN   -5       /* -128 as per standard */

/* Macro to check for reserved log base 2 interval values */
#define DOT1AS_IS_INT_LOG2_RESERVED(n)                                      \
        ((((n) >= -128) && ((n) <= -125)) || (((n) >= 124) && ((n) <=126)))

/* Default SYNC interval corresponds to value -3, which is 8Hz/125ms */
#define DOT1AS_SYNC_INTL_DEFAULT            -3

/* Default ANNOUNCE interval corresponds to value 0, which is 1Hz/1000ms */
#define DOT1AS_ANNOUNCE_INTL_DEFAULT         0

/* Default PDELAY interval corresponds to value 0, which is 1Hz/1000ms */
#define DOT1AS_PDELAY_INTL_DEFAULT           0

#define DOT1AS_SYNC_RX_TIMEOUT_DEFAULT       3           /* Intervals */
#define DOT1AS_ANNOUNCE_RX_TIMEOUT_DEFAULT   3           /* Intervals */

/* 802.1AS receipt timeout limits in number of intervals */
#define DOT1AS_MIN_PDU_RX_TIMEOUT  2
#define DOT1AS_MAX_PDU_RX_TIMEOUT  255

/* 802.1AS propagation delay threshold valid range in nanoseconds */
#define DOT1AS_MIN_PDELAY_THRESHOLD  0           /* nanoseconds */
#define DOT1AS_MAX_PDELAY_THRESHOLD  1000000000  /* nanoseconds */

/* Maximum propagation delay allowed on a link for it to be considered
 * 802.1AS capable. Default value is chosen considering 100Mbps/1Gig
 * ethernet port speeds with an external phy. 10Mbps links have delay
 * over 10,000ns and not supported for EAV. Fiber links could extend
 * for a long distance (upto 2Km/5Km), which results in a larger
 * propagation delay.
 */
#define DOT1AS_PDELAY_THRESHOLD_COPPER    5000       /* Nanosec */
#define DOT1AS_PDELAY_THRESHOLD_SFP       8000       /* Nanosec */

/* Max limit for number of PDELAY lost responses allowed */
#define DOT1AS_MAX_ALLOWED_RESP_LOST 65535
#define DOT1AS_MIN_ALLOWED_RESP_LOST 0

/* Number of Pdelay_Req messages for which a valid response is not received,
 * above which a port is considered to not be exchanging peer delay messages
 * with its neighbor
 */
#define DOT1AS_PDELAY_ALLOWED_LOST_RESP_DEFAULT   3

#define DOT1AS_CLOCK_TIMESOURCE        0xA0 /* Internal oscillator */

/* Default value of the Clock Class */
#define DOT1AS_CLOCK_CLASS_DEFAULT         248

/* Default value of the Clock Accuracy */
#define DOT1AS_CLOCK_ACCURACY_DEFAULT      254

/* Default value of the Clock variance */
#define DOT1AS_CLOCK_VARIANCE_DEFAULT      0x4100 /* 16640 */

/* DOT1AS default rate ratio */
#define DOT1AS_RATE_RATIO_DEFAULT   (0x40000000)

#define DOT1AS_INTL_SEC_TO_NS(secs)       ((secs) * 1000 * 1000 * 1000)
#define DOT1AS_INTL_NS_TO_SEC(ns)         ((ns) / (1000 * 1000 * 1000))
#define DOT1AS_INTK_NS_TO_US(ns)          ((ns) / (1000))
#define DOT1AS_INTL_MS_TO_US(ms)    ((ms) * 1000)
#define DOT1AS_INTL_US_TO_MS(us)    ((us) / 1000)
#define DOT1AS_INTL_SEC_TO_US(secs) ((secs) * 1000 * 1000)
#define DOT1AS_TIMER_TICK_US        (250000) /* microseconds (250 ms) */

/* PDELAY Filter parameters */
#define PDELAY_K1 9         /* Pdelay filter */
#define PRATIO_K1 5         /* Neighbor Rate Ratio filter */

#define SHIFTROUND(x,n) (((x) + (1<<(n-1))) >> n)
#define UUMULT(x,y)     (((int64_t)x) * ((int64_t)y))
#define ONE_SEC  1000000000
#define HALF_SEC  500000000

#define AVNU_PDELAY_OPERATIONAL_CUTOVER_TIME 30000000 /* use operational pdelay interval after 30 seconds */
#define AVNU_NBR_RATE_RATIO_DEFAULT 0
#define AVNU_NBR_PDELAY_DEFAULT 0
#define AVNU_INITIAL_PDELAY_REQ_INTVAL_DEFAULT 0
#define AVNU_INITIAL_SYNC_INTVAL_DEFAULT 0
#define DOT1AS_AVNU_CLOCK_STATE_GM DOT1AS_CLOCK_UPDATE_GM
#define DOT1AS_AVNU_CLOCK_STATE_SLAVE DOT1AS_CLOCK_UPDATE_SLAVE
#define DOT1AS_AVNU_CLOCK_STATE_DEFAULT DOT1AS_CLOCK_UPDATE_SLAVE
#define DOT1AS_AVNU_PDELAY_DIFF_MAX 100

#define DOT1AS_AVNU_SIGNALING_TIMER_TIMEOUT     2000000  /* Wait 2 seconds after signal received before sending signal out slave port */
#define DOT1AS_AVNU_SIGNALING_TIMER_STOP        0
#define DOT1AS_AVNU_SIGNALING_TIMER_START       1
#define DOT1AS_AVNU_SIGNALING_SYNC_COUNT_MAX    5  /* If after 5 syncs the partner has not adjusted the interval forget it */

/******************************************************************************
*                              Protocol Data Structures
*******************************************************************************/

#define DOT1AS_ENET_HDR_SIZE    14
#define DOT1AS_ENET_ENCAPS_HDR_SIZE  2

/* Clock Identity */
#define DOT1AS_CLOCK_ID_LEN   8 /* Length of clock identify - 8 octets */
typedef struct
{
  uint8_t   id[DOT1AS_CLOCK_ID_LEN];
} DOT1AS_CLOCK_IDENTITY_t;

/* Clock Identity Macros */

/* Are CLOCK_IDENTITY 1 and 2 equal? */
#define DOT1AS_CLOCK_ID_IS_EQ(clk1, clk2)                                     \
        (sal_memcmp((clk1)->id, (clk2)->id, DOT1AS_CLOCK_ID_LEN) == 0)

/* Are CLOCK_IDENTITY 1 and 2 not equal? */
#define DOT1AS_CLOCK_ID_IS_NOT_EQ(clk1, clk2)                                 \
        (sal_memcmp((clk1)->id, (clk2)->id, DOT1AS_CLOCK_ID_LEN) != 0)

/* Copy CLOCK_IDENTITY 2 in to 1 */
#define DOT1AS_CLOCK_ID_ASSIGN(clk1, clk2)                                    \
        (sal_memcpy((clk1)->id, (clk2)->id, DOT1AS_CLOCK_ID_LEN))

/* Clock Quality */
typedef struct
{
  uint8_t    class;
  uint8_t    accuracy;
  uint16_t   variance;
} DOT1AS_CLOCK_QUALITY_t;

/* Timestamp value */
#define DOT1AS_TIMESTAMP_LEN   10

#define DOT1AS_RX_TIMESTAMP    0   /* Ingress timestamp */
#define DOT1AS_TX_TIMESTAMP    1   /* Egress timestamp */

typedef struct
{
  uint64_t  seconds;
  uint32_t  nanoseconds;
} DOT1AS_TIMESTAMP_t;

/* Port Identity */
typedef struct
{
  DOT1AS_CLOCK_IDENTITY_t clock;     /* Clock Identity */
  uint16_t                num;       /* Port number 1 to n */
} DOT1AS_PORT_IDENTITY_t;

/* Time aware system attributes */
typedef struct
{
  uint16_t   numPorts;
} DOT1AS_SYSTEM_ATTR_t;


/* 802.1AS PDU types. Note, that term 'PDU' is used instead of message to avoid
 * conflict with OSAPI message queues. The
 */
#define DOT1AS_PDU_SYNC                   0x0
#define DOT1AS_PDU_FOLLOWUP               0x8
#define DOT1AS_PDU_PDELAY_REQ             0x2
#define DOT1AS_PDU_PDELAY_RESP            0x3
#define DOT1AS_PDU_PDELAY_RESP_FOLLOWUP   0xA
#define DOT1AS_PDU_ANNOUNCE               0xB
#define DOT1AS_PDU_SIGNALING              0xC

/* 802.1AS control type */
#define DOT1AS_SYNC_CONTROL_TYPE          0x00  /* for SYNC PDU */
#define DOT1AS_FOLLOWUP_CONTROL_TYPE      0x02  /* for FOLLOWUP PDU */
#define DOT1AS_OTHER_CONTROL_TYPE         0x05  /* for ANNOUNCE/PDELAY msgs */

/* 802.1AS Version */
#define DOT1AS_PDU_VERSION                0x02

/* 802.1AS PDU masks */
#define DOT1AS_PDU_TYPE_MASK              0x0F
#define DOT1AS_PDU_TRANSPORT_MASK         0xF0
#define DOT1AS_PDU_TRANSPORT_SHIFT        0x04
#define DOT1AS_PDU_VER_MASK               0x0F

/* 802.1AS header length and field lengths */
#define DOT1AS_PDU_HEADER_LEN             34
#define DOT1AS_PDU_CORRECTION_FLD_LEN     8
#define DOT1AS_PDU_SEQ_ID_OFST            30
#define DOT1AS_PDU_INTERVAL_OFST          33
#define DOT1AS_PDU_CORRECTION_OFST        8

/* 802.1AS Clock/Time Property flags */
/* NOTE: The flags defined here as per the 802.1AS PDU header format.
 * !!!!!! DO NOT CHANGE THE ORDERING OF THE BITS !!!!!
 */
#define DOT1AS_CLK_TWO_STEP_FLAG         0x0200 /* Octet 0, bit 1 */
#define DOT1AS_CLK_FLG_LEAP61            0x0001 /* Octet 1, bit 0 */
#define DOT1AS_CLK_FLG_LEAP59            0x0002 /* Octet 1, bit 1 */
#define DOT1AS_CLK_FLG_UTC_OFST_VALID    0x0004 /* Octet 1, bit 2 */
#define DOT1AS_CLK_PTP_TIMESCALE_FLAG    0x0008 /* Octet 1, bit 3 */
#define DOT1AS_CLK_FLG_TIME_TRACEABLE    0x0010 /* Octet 1, bit 4 */
#define DOT1AS_CLK_FLG_FREQ_TRACEABLE    0x0020 /* Octet 1, bit 5 */


/* 802.1AS Vendor and Organization TLV header length */
#define DOT1AS_PDU_TLV_HDR_LEN            10 /* 2bytes of tlv type +
                                              * 2bytes of tlv len  +
                                              * 3bytes of orgId    +
                                              * 3bytes of orgSubType
                                              */

/* 802.1AS gPTP domain number */
#define DOT1AS_GPTP_DOMAIN_NUM            0

/* 802.1AS flag bits are defined in commdefs.h */

/* 802.1AS TLV types */
#define DOT1AS_TLV_TYPE_ORG_EXT           0x03
#define DOT1AS_TLV_TYPE_PATH_TRACE        0x08

/* 802.1AS PDU header format */
typedef struct
{
  uint8_t    pduType;                  /* PDU Type */
  uint8_t    transportSpecific;        /* Transport specific field */
  uint8_t    version;                  /* 802.1AS version */
  uint16_t   pduLen;                   /* PDU length (2 bytes) */
  uint8_t    domain;                   /* 802.1AS domain number (1 byte) */
  uint8_t    reserved1;                /* Reserved field (1 byte) */
  uint16_t   flags;                    /* 802.1AS flags (2 bytes) */
  int64_t    correctionFld;            /* Correction field (8 byte) */
  uint32_t   reserved2;                /* Reserved (4 bytes) */
  DOT1AS_PORT_IDENTITY_t portId;       /* Port identity (10 bytes) */
  uint16_t   sequenceId;               /* SequenceId (2 bytes) */
  uint8_t    control;                  /* Control Field (1 byte) */
  int8_t     pduInterval;              /* Log mean PDU interval */

} DOT1AS_PDU_HEADER_t;


/* 802.1AS ANNOUNCE PDU format */
#define DOT1AS_TIMESOURCE_ATOMIC     0x10   /* Sync'ed to Atomic clock */
#define DOT1AS_TIMESOURCE_GPS        0x20   /* Sync'ed to Satellite clock */
#define DOT1AS_TIMESOURCE_RADIO      0x30   /* Sync'ed to Radio systems */
#define DOT1AS_TIMESOURCE_PTP        0x40   /* Sync'ed to PTP-based source */
#define DOT1AS_TIMESOURCE_NTP        0x50   /* Sync'ed to NTP server */
#define DOT1AS_TIMESOURCE_HANDSET    0x60   /* Sync'ed manually */
#define DOT1AS_TIMESOURCE_OTHER      0x90   /* Sync'ed by other means */
#define DOT1AS_TIMESOURCE_INT_OSC    0xA0   /* Not Sync'ed, free running */

#define DOT1AS_ANNOUNCE_PDU_LEN        30   /* With out Path Trace TLV */
#define DOT1AS_PATH_TRACE_TLV_MIN_LEN  12   /* Atleast one hop trace */

/* 802.1AS priorities values and types */
#define DOT1AS_MIN_PRIORITY_VALUE 0
#define DOT1AS_MAX_PRIORITY_VALUE 255

/* avnu 8021as Status */
typedef enum
{
  DOT1AS_DISABLED = 0,
  DOT1AS_IEEE = 1,
  DOT1AS_AVNU_DISABLED = 2,
  DOT1AS_AVNU_AUTOMOTIVE =3
}DOT1AS_AVNU_STATUS_t;

/* avnu GM Status */
typedef enum
{
  AVNU_GM_STARTUP_ABSENT = 0,
  AVNU_GM_OPERATIONAL_ABSENT = 1,
  AVNU_GM_UNDETERMINED = 2,
  AVNU_GM_DISABLED = 3,
  AVNU_GM_NO_SLAVE_PORT = 4,
  AVNU_GM_PRESENT = 5
}AVNU_GM_STATUS_t;

typedef enum
{
  AVNU_ASCAPABLE_STATUS_TRUE = 0,
  AVNU_ASCAPABLE_STATUS_PORT_DOWN = 1,
  AVNU_ASCAPABLE_STATUS_CONFIG_AVNU = 2,
  AVNU_ASCAPABLE_STATUS_CONFIG_DOT1AS = 3
}AVNU_ASCAPABLE_STATUS_t;

/* 802.1AS Port priority vector info */
typedef enum
{
  DOT1AS_INFO_RECEIVED = 0,  /* Time out has not occured */
  DOT1AS_INFO_MINE,          /* Info is derived from current timeaware system */
  DOT1AS_INFO_AGED,          /* Announce RX or SYNC RX timeout occured */
  DOT1AS_INFO_DISABLED,      /* Port is not enabled for 802.1AS */
  DOT1AS_INFO_UNKNOWN        /* Info is unknown, default state to begin */
} DOT1AS_BMC_INFO_t;

/* Internal BMC events */
typedef enum
{
  DOT1AS_BMC_INTF_EVENT = 0,   /* An interface event (NIM/Admin) has occured */
                               /* 802.1AS capability has changed */
  DOT1AS_BMC_ANNOUNCE_RX,      /* Announce received */
  DOT1AS_BMC_SYNC_TIMEOUT,     /* SYNC receipt timed out */
  DOT1AS_BMC_ANNOUNCE_TIMEOUT  /* ANNOUNCE receipt timed out */
} DOT1AS_BMC_EVENT_t;

/* 802.1AS System Identity */
typedef struct
{
  uint16_t  priority1;           /* Clock's priority1 value */
  DOT1AS_CLOCK_QUALITY_t quality;   /* Clock's quality */
  uint16_t  priority2;           /* Clock's priority2 value */
  DOT1AS_CLOCK_IDENTITY_t clock;    /* Clock identity */
} DOT1AS_SYSTEM_IDENTITY_t;


/* 802.1AS priority vector */
typedef struct
{
  DOT1AS_SYSTEM_IDENTITY_t sysId;        /* System identity */
  uint16_t              steps;        /* Steps to GM */
  DOT1AS_PORT_IDENTITY_t   sourcePort;   /* Port id of transmitting system */
  uint16_t              rcvdPortNum;  /* Port number of receiving port */
} DOT1AS_PRIO_VECTOR_t;


/* Standard Announce Path Trace TLV */
typedef struct
{
  uint16_t        type;              /* TLV type */
  uint16_t        length;            /* Length of TLV data */
  /* Clock Identity of each of the hops traversed */
  DOT1AS_CLOCK_IDENTITY_t clk[DOT1AS_MAX_HOPS_PER_DOMAIN];
} DOT1AS_PATH_TRACE_TLV_t;

typedef struct
{
  uint8_t      reserved1[10];            /* Reserved   10bytes */
  uint16_t     currentUTCoffset;         /* UTC offset  2bytes */
  uint8_t      reserved2;                /* Reserved    1 byte */
  uint8_t      gmPriority1;              /* Grand Master Priority1 value */
  DOT1AS_CLOCK_QUALITY_t  gmClockQuality; /* Grand Master clock quality */
  uint8_t                 gmPriority2;    /* Grand Master Priority2 value */
  DOT1AS_CLOCK_IDENTITY_t gmClock;        /* Grand Master clock identity */
  uint16_t   stepsRemoved;             /* Steps from local clock to GM */
  uint8_t    timeSource;               /* Time source 1 byte */
  DOT1AS_PATH_TRACE_TLV_t  tlv;
} DOT1AS_ANNOUNCE_PDU_t;


/* 802.1AS SYNC PDU format */
#define DOT1AS_SYNC_PDU_LEN               10

/* SYNC receive states */
typedef enum
{
  DOT1AS_SYNC_RX_DISABLED=0,     /* All received SYNC are discarded */
  DOT1AS_SYNC_RX_WAIT,           /* Waiting for SYNC */
  DOT1AS_SYNC_RX_WAIT_FOLLOWUP,  /* SYNC received, waiting for Followup */
} DOT1AS_SYNC_RX_STATE_t;

/* SYNC Info */
typedef struct
{
  uint32_t               rxIntfIndex; /* Port number on which sync was received */
  uint16_t               lastSeqId;  /* Seq Id of the rx sync message */
  int64_t                followupCorrectionField; /* Incoming Correction field */
  int8_t                 pduInterval; /* Interval value of rx sync msg */
  DOT1AS_TIMESTAMP_t     preciseOriginTimestamp; /* GM timestamp */
  DOT1AS_TIMESTAMP_t     syncRxTimestamp; /* Sync ingress timestamp */
  uint16_t               gmTimeBaseIndicator;
  int8_t                 lastGmPhaseChange[12];
  int32_t                lastGmFreqChange;
} DOT1AS_SYNC_INFO_t;

typedef struct
{
  /* No specific msg fields. Message header has all the required info */
  uint8_t   reserved[10];
} DOT1AS_SYNC_PDU_t;


/* 802.1AS FOLLOWUP PDU format */
#define DOT1AS_FOLLOWUP_PDU_LEN           42   /* 10 timestamp + 32 TLV */
#define DOT1AS_FOLLOWUP_TLV_LEN           28   /* 2 type, 2 length, 28 TLV */
#define DOT1AS_FOLLOWUP_TIMESTAMP_OFST    0x00
#define DOT1AS_FOLLOWUP_RATE_RATIO_OFST   (DOT1AS_PDU_HEADER_LEN + 10 + 10)

/* Standard FOLLOWUP TLV */
typedef struct
{
  uint16_t      type;              /* TLV type */
  uint16_t      length;            /* Length of TLV data */
  uint8_t       orgId[3];          /* Organization ID */
  uint8_t       orgSubType[3];     /* Organization subtype */
  int32_t       cumulativeRateOfst;
  uint16_t      gmTimeBaseIndicator;
  int8_t        lastGmPhaseChange[12];
  int32_t       lastGmFreqChange;
} DOT1AS_FOLLOWUP_TLV_t;

typedef struct
{
  DOT1AS_TIMESTAMP_t preciseOriginTimestamp;  /* Timestamp of associated SYNC */
  DOT1AS_FOLLOWUP_TLV_t gmTlv;                /* Standard follow-up tlv (mandatory) */
} DOT1AS_FOLLOWUP_PDU_t;


/* 802.1AS PDELAY_REQ PDU format */
#define DOT1AS_PDELAY_REQ_PDU_LEN               20
#define DOT1AS_PDELAY_REQ_PDU_START_SEQUANCE_ID 0x1234  /* To randomize */

typedef struct
{
  /* No specific msg fields. Header has all the required info */
  uint8_t   reserved1[10];
  uint8_t   reserved2[10];
} DOT1AS_PDELAY_REQ_PDU_t;


/* 802.1AS PDELAY_RESP PDU format */
#define DOT1AS_PDELAY_RESP_PDU_LEN        20

typedef struct
{
  DOT1AS_TIMESTAMP_t reqReceiptTimestamp;  /* Timestamp of PDELAY_REQ PDU */
  DOT1AS_PORT_IDENTITY_t reqPortId;        /* PortIdentity of requesting port */
} DOT1AS_PDELAY_RESP_PDU_t;

/* 802.1AS PDELAY_RESP_FOLLOWUP PDU format */
#define DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_LEN     20

typedef struct
{
  DOT1AS_TIMESTAMP_t respOriginTimestamp;  /* Timestamp of PDELAY_RESP PDU */
  DOT1AS_PORT_IDENTITY_t reqPortId;        /* PortIdentity of requesting port */
} DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_t;


/* 802.1AS SIGNALING PDU format */
#define DOT1AS_SIGNALING_PDU_LEN               0x1A /* 10+16 bytes */
#define DOT1AS_SIGNALING_MSG_INT_TLV_LEN       0x0C /* 12 bytes */

/* 802.1AS Message interval request TLV flags */
#define DOT1AS_SIGNALING_NBR_RATE_RATIO_FLAG   0x01 /* Compute rate ratio */
#define DOT1AS_SIGNALING_NBR_PDELAY_FLAG       0x02 /* Compute PDELAY */

/* 802.1AS Message interval values in the TLV */
#define DOT1AS_SIGNALING_MSG_INT_REQ_STOP     127   /* Stop sending message */
#define DOT1AS_SIGNALING_MSG_INT_REQ_INIT     126   /* Set to initial value */
#define DOT1AS_SIGNALING_MSG_INT_REQ_NONE    -128   /* Do not change interval */

#define DOT1AS_SIGNALING_MSG_INT_INVALID      99    /* Invalid message interval */

/* Standard SIGNALING Message interval request TLV */
typedef struct
{
  uint16_t   type;              /* TLV type */
  uint16_t   length;            /* Length of TLV data */
  uint8_t    orgId[3];          /* Organization ID */
  uint8_t    orgSubType[3];     /* Organization subtype */
  int8_t     linkDelayInterval; /* Mean time interval for PDELAY_REQ */
  int8_t     timeSyncInterval;  /* Mean time interval for SYNC */
  int8_t     announceInterval;  /* Mean time interval for ANNOUNCE */
  uint8_t    flags;
  uint16_t   reserved;
} DOT1AS_SIGNALING_MSG_INT_REQ_TLV_t;

typedef struct
{
  DOT1AS_PORT_IDENTITY_t targetPortIdentity; /* portId of port at other end */
  DOT1AS_SIGNALING_MSG_INT_REQ_TLV_t msgIntReqTlv; /* Message interval req tlv */
} DOT1AS_SIGNALING_PDU_t;


/* Internal representation of 802.1AS PDU */
typedef struct
{
  DOT1AS_PDU_HEADER_t  hdr;                      /* Header */

  union                                          /* Body */
  {
    DOT1AS_ANNOUNCE_PDU_t             announcePdu;
    DOT1AS_SYNC_PDU_t                 syncPdu;
    DOT1AS_FOLLOWUP_PDU_t             followUpPdu;
    DOT1AS_PDELAY_REQ_PDU_t           pDelayReqPdu;
    DOT1AS_PDELAY_RESP_PDU_t          pDelayRespPdu;
    DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_t pDelayRespFollowupPdu;
    DOT1AS_SIGNALING_PDU_t            signalingPdu;
  } body;

} DOT1AS_PDU_t;


/*******************************************************************************
*                              Internal Data Structures
*******************************************************************************/


/* PDELAY initiator states */
typedef enum
{
  DOT1AS_PDELAY_DISABLED=0,     /* PDELAY mechanism is disabled */
  DOT1AS_PDELAY_REQ_SEND,       /* Send PDELAY_REQ (port is active) */
  DOT1AS_PDELAY_WAIT_FOR_RESP,  /* Wait for RESP */
  DOT1AS_PDELAY_WAIT_FOR_RESP_FOLLOWUP, /* Wait for RESP followup */

} DOT1AS_PDELAY_REQ_STATE_t;

/* Internal state for initializing and updating local time */
typedef enum
{
  DOT1AS_CLOCK_INIT_GM=0,
  DOT1AS_CLOCK_UPDATE_GM,
  DOT1AS_CLOCK_TRANSITION_TO_SLAVE,
  DOT1AS_CLOCK_UPDATE_SLAVE,
  DOT1AS_CLOCK_TRANSITION_TO_GM
} DOT1AS_CLOCK_STATE_t;

/* 802.1AS interface statistics */
typedef  struct
{
  uint32_t syncTxCount;
  uint32_t syncRxCount;
  uint32_t syncRxDiscards;
  uint32_t syncTooFast;
  uint32_t followUpTxCount;
  uint32_t followUpRxCount;
  uint32_t followupRxDiscards;
  uint32_t announceTxCount;
  uint32_t announceRxCount;
  uint32_t announceRxDiscards;
  uint32_t signalingRxCount;
  uint32_t signalingTxCount;
  uint32_t signalingRxDiscards;
  uint32_t pDelayReqTxCount;
  uint32_t pDelayReqRxCount;
  uint32_t pDelayRespTxCount;
  uint32_t pDelayRespRxCount;
  uint32_t pDelayRespFollowUpTxCount;
  uint32_t pDelayRespFollowUpRxCount;
  uint32_t pDelayReqRxDiscards;
  uint32_t pDelayRespRxDiscards;
  uint32_t syncTransmitTimeouts;
  uint32_t syncReceiptTimeouts;
  uint32_t followupReceiptTimeouts;
  uint32_t pDelayReceiptTimeouts;
  uint32_t announceReceiptTimeouts;
  uint32_t badHeaderCount;
  uint32_t badPdelayValues;
  uint32_t txErrors;
  uint32_t tsErrors;
  uint32_t ptpDiscardCount;
  uint32_t pDelayLostResponsesExceeded;
} DOT1AS_INTF_STATS_t;

/* 802.1AS Global State */
typedef struct
{
  uint8_t adminMode;  /* Global mode, Enabled or Disabled */
  uint8_t priority1;  /* priority1 value of local clock */
  uint8_t priority2;  /* priority2 value of local clock */
  DOT1AS_CLOCK_IDENTITY_t thisClock;    /* Clock identity of local clock */
  DOT1AS_CLOCK_QUALITY_t  thisClockQuality; /* Clock quality of local clock */
  uint16_t  thisClockFlags;      /* Time properties of local clock */

  uint8_t  isGmPresent; /* True is GM is present, else False */
  uint16_t gmStepsRemoved; /* Master steps removed for this system */
  uint8_t  gmPriority1;    /* GM priority1 value */
  uint8_t  gmPriority2;    /* GM priority2 value */
  uint16_t  gmFlags;             /* Leap59, Leap61, Time traceable etc */
  uint8_t   gmTimeSrc;           /* Timesource value of Best clock */
  uint16_t  gmUtcOfst;           /* UTC offset value of Best clock */

  DOT1AS_CLOCK_IDENTITY_t gmClock;   /* Best/Root Clock identity */
  DOT1AS_CLOCK_QUALITY_t  gmQuality; /* Clock quality of Best clock */
  DOT1AS_PORT_IDENTITY_t  parentId;  /* Parent Clock's Port identity */
  DOT1AS_PATH_TRACE_TLV_t gmPath;    /* Path trace to Best clock */

  /* GM tracking */
  uint32_t     gmChangeCount;          /* Number of GM changes */
  uint32_t     lastGmChangeEvent;      /* Timestamp of last GM change */
  uint32_t     lastGmPhaseChangeEvent; /* Timestamp of last phase change */
  uint32_t     lastGmFreqChangeEvent;  /* Timestamp of last freq change */

  /* If this system is GM, the local time is updated whenever SYNC interval
   * expires. If this system is NOT the GM, the local time is updated based
   * on the received SYNC information
   */
  DOT1AS_TIMESTAMP_t   bridgeLocalTime;           /* Local time on this bridge */
  DOT1AS_TIMESTAMP_t   prevRxTimestamp;           /* Previous value of SYNC rx ts */
  DOT1AS_CLOCK_STATE_t clkState;                  /* Clock state */
  int32_t              rateRatioIn;               /* DPLL variable */
  int32_t              rateRatioOut;              /* DPLL variable */
  uint32_t             avnuMode;                  /* TRUE incase of switch is operating in avnu mode */
  uint32_t             avnuSyncAbsenceTimeout;    /* sync absence timeout in micro seconds */
  uint32_t             avnuSyncAbsenceTimer;      /* Sync Absence timer*/
  uint32_t             avnuSignalingTimer;        /* Wait before signaling master ports */
  int32_t              avnuSigSyncLogInterval;    /* The signaling sync interval sent on the slave interface */
  uint32_t             avnuSigSyncRxCount;        /* The number of syncs received on the slave interface since signal sent */
  uint8_t              avnuSigSyncAdjustTimeout;  /* Indicates that the sync timeout interval still needs to be adjusted */
  uint8_t              isSyncReceived;            /* Indicates atleast one sync has been received*/
  uint8_t              avnuGMLost;                /* GM has been lost, either on startup or after a sync timeout */
  uint8_t              avnuSyncAbsenceOnStartUp;  /* Is the GM Absent on startup ? */
  uint8_t              avnuDeviceState;           /* AVnu device state */
} DOT1AS_BRIDGE_STATE_t;

/* 802.1AS Port State */
typedef struct
{
  uint8_t pttPortEnabled;        /* 802.1AS interface admin mode */
  uint8_t lagMemberDisabled;     /* Indicates this is a LAG interface with dot1as disabled */

  /* Peer Delay state information */
  DOT1AS_PDELAY_REQ_STATE_t pDelayReqState; /* State of PDELAY_REQ machine */
  uint8_t   isMeasuringPdelay;   /* If the port is receving pdelay PDUs */
  uint8_t   dot1asCapable;       /* True if the link delay is acceptable */
  uint16_t  pDelaySeqId;         /* Sequence ID for transmit pdelay PDUs */
  uint16_t  numPdelayRespLost;   /* Num of pdelay timeouts for REQs sent */
  uint16_t  allowedLostReponses; /* Num of pdelay timeouts allowed */
  uint32_t  neighborPropDelay;   /* Computed Propagation delay */
  /* The frequency ratio between local XTAL and the PDELAY Neighbor */
  int32_t   neighborRateRatio;   /* Neighbor Rate Ratio */
  uint8_t   pDelayFilterInit;    /* PDELAY filter variable */
  int64_t   delayEstimate;       /* PDELAY filter variable */

  uint64_t  pDelayIntervalTimer; /* Running timer for REQ interval */
  uint8_t   pDelayIntervalTimerReset;
  uint32_t  avnuOperationalPdelayTimer;          /*operational pdelay timer*/
  uint8_t   isAVnuPdelayConfigSaved; /* if configured and calculated pdelay values are differed by 100ns then update stored pdelay value
                                     with calculated pdelay value once in link up cycle */
  uint64_t  meanPdelayReqInterval; /* REQ TX interval or RX timeout */
  int32_t   currentLogPdelayReqInterval; /* Log REQ TX interval */
  uint32_t  neighborPropDelayThres;/* Max prop delay limit */

  DOT1AS_PORT_IDENTITY_t partnerId; /* Partner port identity */
  DOT1AS_TIMESTAMP_t  prevT1;    /* Previous REQ origin timestamp */
  DOT1AS_TIMESTAMP_t  prevT2;    /* Previous REQ receipt timestamp */
  DOT1AS_TIMESTAMP_t  prevT3;    /* Previous RESP origin timestamp */
  DOT1AS_TIMESTAMP_t  prevT4;    /* Previous RESP receipt timestamp */
  DOT1AS_TIMESTAMP_t  pDelayT1;  /* REQ origin timestamp */
  DOT1AS_TIMESTAMP_t  pDelayT2;  /* REQ receipt timestamp */
  DOT1AS_TIMESTAMP_t  pDelayT3;  /* RESP origin timestamp */
  DOT1AS_TIMESTAMP_t  pDelayT4;  /* RESP receipt timestamp */

  /* BMC algorithm state */
  uint8_t       reselect;              /* TRUE if port role is to be updated */
  uint8_t       selected;              /* TRUE if port role is updated */
  uint8_t       updtInfo;              /* TRUE if port prio is to be updated */
  DOT1AS_BMC_INFO_t infoIs;            /* Port's priority vector info */
  DOT1AS_PRIO_VECTOR_t masterPrioVec;  /* Master Priority Vector */
  DOT1AS_PRIO_VECTOR_t portPrioVec;    /* Port Priority Vector */
  DOT1AS_PORT_ROLE_t selectedRole;  /* Master, slave, disabled etc */
  uint16_t       announceFlags;     /* Port's Announce flags */
  uint8_t        announceTimeSrc;   /* Port's Timesource value */
  uint16_t       announceUtcOfst;   /* Port's UTC offset value */
  uint16_t       portSteps;         /* Steps removed for the port */
  uint16_t       announceSeqId;     /* Sequence ID for TX announce PDUs */
  uint64_t       announceTransmitTimer; /* Running timer for transmit */
  uint8_t        announceTransmitTimerReset;
  uint64_t       meanAnnounceInterval;  /* Mean transmission interval */
  int32_t        currentLogAnnounceInterval;  /* Log Mean transmission interval */
  uint64_t       announceReceiptTimer;  /* Running timer for receipt  */
  uint8_t        announceReceiptTimerReset;
  uint64_t       announceReceiptTimeoutInterval;/* Announce receipt timeout */

  /* Time Sync Receive state */
  DOT1AS_SYNC_RX_STATE_t syncRxState;      /* SYNC receive state */
  DOT1AS_SYNC_INFO_t  syncRxInfo;          /* Last received SYNC info */
  uint64_t syncReceiptTimer;               /* SYNC receipt running timer */
  uint64_t syncReceiptTimeoutInterval;     /* SYNC receipt timeout */
  uint8_t  syncReceiptTimerReset;
  uint8_t  syncInfoAvailable;              /* Sync info copied from slave */
  uint64_t followupReceiptTimer;          /* FOLLOWUP receipt running timer */
  uint64_t followupReceiptTimeoutInterval;/* FOLLOWUP receipt timeout */
  uint8_t  followupReceiptTimerReset;
  uint32_t avnuSyncReadyCount;             /* Determine sync ready state based on the count of this variable */

  /* Time Sync Trasmit state */
  uint16_t   syncTxSeqId;        /* SeqId for SYNC sent from this port */
  uint64_t   syncTransmitTimer; /* SYNC transmit running timer */
  uint64_t   meanSyncInterval;  /* Mean SYNC transmit interval */
  int32_t    currentLogSyncInterval;  /* LOG mean SYNC transmit interval */
  uint8_t    syncTransmitTimerReset;
  DOT1AS_TIMESTAMP_t  lastSyncSentTime;

  /* Signaling state */
  uint16_t  signalingSeqId;     /* Sequence ID for TX Signaling PDUs */

  /* Interface statistics */
  DOT1AS_INTF_STATS_t stats;

} DOT1AS_INTF_STATE_t;

/*******************************************************************************
*                              Debug declarations
*******************************************************************************/
/* Trace Flags */
#define DOT1AS_TRACE_DBG_FLAG          0x1   /* Debug traces like func calls */
#define DOT1AS_TRACE_RX_FLAG           0x2   /* Received 802.1AS messages */
#define DOT1AS_TRACE_TX_FLAG           0x4   /* Transmitted 802.1AS messages */
#define DOT1AS_TRACE_INTF_FLAG         0x8   /* Interface events */
#define DOT1AS_TRACE_BMCA_FLAG         0x10  /* BMCA trace flag */
#define DOT1AS_TRACE_PDELAY_FLAG       0x20  /* Pdelay trace flag */
#define DOT1AS_TRACE_ERR_FLAG          0x40  /* Internal errors */
#define DOT1AS_TRACE_PDELAY_ERR_FLAG   0x100
#define DOT1AS_TRACE_DEV_FLAG          0x200 /* Clean flag for development use */
#define DOT1AS_TRACE_CFG_FLAG          0x400 /* Configuration messages */
#define DOT1AS_TRACE_SIG_FLAG          0x800 /* Signaling messages */

/* Debug flag and macro */
extern uint32_t dot1asTraceMask;

#define DOT1AS_TRACE(format, args...)                               \
 do                                                                 \
 {                                                                  \
   sal_printf(format, ##args);                                      \
 } while(0)


#define DOT1AS_TRACE_DBG(format, args...)                           \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_DBG_FLAG))                   \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_ERR(format, args...)                           \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_ERR_FLAG))                   \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)


#define DOT1AS_TRACE_BMCA(format, args...)                          \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_BMCA_FLAG))                  \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_PORT_ROLE(port,next)                           \
 do                                                                 \
 {                                                                  \
   DOT1AS_PORT_ROLE_t curr = pDot1asIntf_g[port].selectedRole;      \
   if ((dot1asTraceMask & DOT1AS_TRACE_BMCA_FLAG) &&                \
       ((curr) != (next)))                                          \
   {                                                                \
     sal_printf("\nDOT1AS: Port[%d] transitioned from [%s]->[%s]\n",\
                  port, dot1asDebugRoleStringGet(curr),            \
                  dot1asDebugRoleStringGet(next));                  \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_PDELAY(format, args...)                        \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_PDELAY_FLAG ))               \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_PDELAY_ERR(format, args...)                    \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_PDELAY_ERR_FLAG ))           \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_DEV(format, args...)                    \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_DEV_FLAG ))           \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)


#define DOT1AS_TRACE_RX(format, args...)                            \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_RX_FLAG ))                   \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_TX(format, args...)                            \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_TX_FLAG ))                   \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define DOT1AS_TRACE_SIG(format, args...)                           \
 do                                                                 \
 {                                                                  \
   if ((dot1asTraceMask & DOT1AS_TRACE_SIG_FLAG ))                  \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)


/*******************************************************************************
*                             AVnu Debug declarations
*******************************************************************************/
/* Trace Flags */
#define AVNU_TRACE_GM_FLAG             0x1   /* GM loss and recovery */
#define AVNU_TRACE_ASCAPABLE_FLAG      0x2   /* AVnu asCapable transitions */

/* Debug flag and macro */
extern uint32_t avnuTraceMask;

#define AVNU_TRACE_GM(format, args...)                              \
 do                                                                 \
 {                                                                  \
   if ((avnuTraceMask & AVNU_TRACE_GM_FLAG))                      \
   {                                                                \
     sal_printf("\n"format, ##args);                                \
   }                                                                \
 } while(0)

#define AVNU_TRACE_ASCAPABLE(_intf_, _flag_, _reason_)                 \
 do                                                                    \
 {                                                                     \
   if ((avnuTraceMask & AVNU_TRACE_ASCAPABLE_FLAG))                  \
   {                                                                   \
      DOT1AS_TRACE("\nAVnu: Port[%d] is %sasCapable. %s\n", (_intf_),  \
                  (_flag_) ? "" : "NOT ", (_reason_));                 \
   }                                                                   \
 } while(0)



/*******************************************************************************
*                              External declarations
*******************************************************************************/

/* Destination MAC for 802.1AS PDUs */
extern uint8_t ETHER_MAC_DOT1AS_g[];

/* Global state */
extern DOT1AS_BRIDGE_STATE_t *pDot1asBridge_g;

/* Interface state */
extern DOT1AS_INTF_STATE_t   *pDot1asIntf_g;

extern uint8_t dot1asTxBuffer[DOT1AS_MAX_PDU_SIZE];

/* Debug variables */
extern dot1asTime_t dot1asDbgSyncT1;
extern dot1asTime_t dot1asDbgSyncT2;
extern dot1asTime_t dot1asDbgFollowupT1;
extern dot1asTime_t dot1asDbgFollowupT2;

extern AVB_RC_t dot1asInit(void);

/* PDU handling routines */
extern uint64_t  ntohll(uint64_t netlong);
extern uint64_t  htonll(uint64_t hostlong);
extern uint32_t  dot1asPduAddEnetHeader(uint32_t intIfIndex,
                                        uint8_t *pdu, uint16_t *len);
extern uint32_t dot1asPduAddCommonHeader(uint32_t   intIfIndex, uint8_t  *pdu,
                                         uint32_t  type, uint16_t  seqId,
                                         int64_t   correction, int8_t    interval,
                                         uint16_t  flags, uint16_t *len);
extern AVB_RC_t dot1asIntfTimestampGet(uint32_t intIfIndex, uint32_t msgType,
                                   uint32_t seqId, uint8_t rxtx,
                                   DOT1AS_TIMESTAMP_t *pTimestamp);
extern void dot1asPackUint48(uint8_t *pdu, uint64_t seconds);
extern void dot1asUnpackUint48(uint8_t *pdu, uint64_t *seconds);
extern uint16_t dot1asPduLengthGet(uint32_t type);
extern void dot1asPduLengthSet(uint8_t *pdu, uint16_t len);
extern void dot1asIntervalUsAdjust(uint64_t *us);
extern int64_t dot1asSubExtTimestamps (DOT1AS_TIMESTAMP_t *pTs1, DOT1AS_TIMESTAMP_t *pTs2);

extern AVB_RC_t dot1asPduParse(uint8_t *pPktData, uint32_t dataLen, DOT1AS_PDU_t *pPdu);

extern AVB_RC_t dot1asPduReceiveProcess(uint32_t intfIndex, DOT1AS_PDU_t  *pPdu, uint32_t timeStamp);


extern AVB_RC_t dot1asIntervalUsec2Log(uint64_t uSec, int32_t *logInterval);
extern AVB_RC_t dot1asIntervalLog2Usec(int32_t logInterval, uint64_t *uSec);
extern void dot1asIntervalUsecAdjust(uint64_t *us);
extern AVB_RC_t dot1asIntervalUpdate(uint32_t intfIndex, int32_t type, int32_t value);


/* 802.1AS threads */
extern void dot1asTimerThread(void *arg);
extern void dot1asTransmitThread(void *arg);
extern void dot1asReceiveThread(void *arg);

/* Routines for PDELAY mechanism  */
extern void dot1asCheckPdelayTimers(void);
extern void dot1asIntfPdelayInit(uint32_t intfIndex);
extern void dot1asIntfPdelayDisable(uint32_t intfIndex);
extern uint8_t dot1asIntfIsPdelayCapable(uint32_t intfIndex);
extern uint8_t dot1asIsPdelayCapable(uint32_t intfIndex);
extern AVB_RC_t dot1asPdelayReqReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu, uint32_t timeStamp);
extern AVB_RC_t dot1asPdelayRespReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu, uint32_t timeStamp);
extern AVB_RC_t dot1asPdelayRespFollowupReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu);

/* Routines for BMCA mechanism */
extern void dot1asCheckBmcaTimers(void);
extern void dot1asInvokeBmca(uint32_t intfIndex, DOT1AS_BMC_EVENT_t event, void *pData);
extern AVB_RC_t dot1asAnnounceReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu);
extern uint8_t dot1asIsAnnounceCapable(uint32_t intfIndex);

/* Routines for Timesync mechanism */
extern uint8_t dot1asIsTimesyncCapable(uint32_t intfIndex);
extern void dot1asIntfTimesyncInit(uint32_t intfIndex);
extern void dot1asIntfTimesyncDisable(uint32_t intfIndex);
extern void dot1asCheckTimesyncTimers(void);
extern AVB_RC_t dot1asSyncReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu, uint32_t timeStamp);
extern AVB_RC_t dot1asFollowupReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu);
extern uint64_t dot1asBridgeLocalTimeGet(void);

/* Signaling handlers */
extern void dot1asCheckAvnuSignalingTimers(void);
extern uint8_t dot1asIsSignalingCapable(uint32_t intfIndex);
extern void dot1asIntfSignalingInit(uint32_t intfIndex);
extern AVB_RC_t dot1asSignalingReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu);
extern AVB_RC_t dot1asSignalingSend(uint32_t intfIndex, int8_t linkDelayInterval,
                                int8_t timeSyncInterval, int8_t announceInterval);
extern void dot1asSignalingLinkStateNotify(uint32_t port, uint8_t linkState);
extern void dot1asSignalingStateInit(void);

/* Routines for 802.1AS configuration */
extern AVB_RC_t dot1asIntfUpdate(uint32_t intfIndex);

/* Debug routines */
extern void dot1asDebugPdelayShow(uint32_t intfIndex, uint8_t showLog);
extern void dot1asDebugBmcaShow(void);
extern AVB_RC_t dot1asDebugIntlShow(void);
extern void dot1asDebugIntfStatsShow(uint32_t intfIndex);
extern uint8_t *dot1asDebugRoleStringGet(DOT1AS_PORT_ROLE_t role);
extern void dot1asDebugTimesyncShow(uint8_t verbose, uint32_t num);
extern void dot1asDebugFlagsSet(uint32_t flags);
extern void avnuDebugFlagsSet(uint32_t flags);
extern void dot1asDebugCorrectionShow(void);
extern void dot1asDebugCorrectionFld(DOT1AS_TIMESTAMP_t *pIngressTimestamp,
                                     int64_t  ingressCorrectionField,
                                     DOT1AS_TIMESTAMP_t *pEgressTimestamp,
                                     int64_t  egressCorrectionFld,
                                     DOT1AS_TIMESTAMP_t *pGmTimestamp);
#if (DOT1AS_DEBUG_TIMESYNC_LOG == 1)
extern void dot1asDebugTimesyncLogShow(uint32_t num);
extern void dot1asDebugTimesyncLog(DOT1AS_TIMESTAMP_t *pPrevRxTime,
                                   DOT1AS_TIMESTAMP_t *pCurrentRxTime,
                                   uint64_t gmSec, uint32_t gmNano,
                                   uint64_t localSec, uint32_t localNano,
                                   int64_t correction, int32_t rateRatioIn,
                                   int32_t rateRatioOut);
#endif

#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
extern void dot1asDebugNbrRateRatioShow(uint32_t intfIndex);
extern void dot1asDebugNbrRateRatioLog(uint32_t intfIndex,
                                       int64_t  deltaT1, int64_t  deltaT2,
                                       int64_t  deltaT3, int64_t  deltaT4,
                                       int64_t x1, int32_t y1, int64_t x2, int32_t y2,
                                       int32_t ratioIn, int32_t ratioOut);
#endif

extern void shell_avb_dot1as(int argc, char *argv[]);
extern void shell_avb_avnu(int argc, char *argv[]);
extern AVB_RC_t dot1asIntfStatus(uint32_t port, DOT1AS_INTF_STATE_t *as_intf_status);
extern AVB_RC_t dot1asIntfStatsGet(uint32_t intIfNum,
                               DOT1AS_COUNTER_TYPE_t type, uint32_t *value,
                               DOT1AS_INTF_STATS_t *as_stats);
extern void dot1asDebugIntfStatusShow(uint32_t port);
extern void dot1asTimestampUpdate(uint32_t timestamp, cbx_ts_time_t *tod);
extern AVB_RC_t dot1asPktSend(void *buffer, uint16_t bufLen, uint32_t intfIndex);
extern char *avb_lag_parent_string_get(uint32_t intIfNum, char *buf, uint8_t buflen);
extern uint8_t  portLinkStateGet(uint32_t port);
#endif /* DOT1AS_H */

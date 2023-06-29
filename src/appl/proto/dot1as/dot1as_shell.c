/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_shell.c
 */

#include "dot1as.h"
#include "dot1as_cfgx.h"

#define DOT1AS_TRACE_LINE(n)                                        \
  do {                                                              \
    uint32_t i;                                                     \
    for (i=0; i < (n); i++) { DOT1AS_TRACE("-"); }                  \
    DOT1AS_TRACE("\n");                                             \
  } while(0)                                                        \


/* Record to store max correction value */
typedef struct
{
  DOT1AS_TIMESTAMP_t ingressTimestamp;
  int64_t  ingressCorrectionField;
  DOT1AS_TIMESTAMP_t egressTimestamp;
  DOT1AS_TIMESTAMP_t gmTimestamp;
  int64_t  egressCorrectionFld;
  dot1asTime_t syncT1;
  dot1asTime_t syncT2;
  dot1asTime_t followupT1;
  dot1asTime_t followupT2;
} dot1asDebugCorrection_t;

static dot1asDebugCorrection_t dot1asDebugCorrectionValue;

#if (DOT1AS_DEBUG_TIMESYNC_LOG == 1)
#define DOT1AS_DEBUG_MAX_TIMESYNC_LOG  8
/* Structure to log the timesync clock update state */
typedef struct {
  DOT1AS_TIMESTAMP_t prevRxTime;
  DOT1AS_TIMESTAMP_t currentRxTime;
  uint64_t gmSec;
  uint32_t gmNano;
  uint64_t localSec;
  uint32_t localNano;
  int64_t  correction;
  int32_t  rateRatioIn;
  int32_t  rateRatioOut;
} dot1asDebugTimesyncLog_t;

/* Log of clock variables captured on every SYNC/FOLLOWUP PDU reception */
static dot1asDebugTimesyncLog_t dot1asTimesyncLogBuf[DOT1AS_DEBUG_MAX_TIMESYNC_LOG];
static uint32_t dot1asTimesyncLogIndex = 0;

void dot1asDebugTimesyncLogShow(uint32_t num);
#endif

#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
#define DOT1AS_DEBUG_MAX_PDELAY_LOG  8
typedef struct {
  uint32_t intfIndex;
  int64_t deltaT1;
  int64_t deltaT2;
  int64_t deltaT3;
  int64_t deltaT4;
  int64_t x1;
  int32_t y1;
  int64_t x2;
  int32_t y2;
  int32_t ratioIn;
  int32_t ratioOut;
} dot1asRateRatioLog_t;
static uint32_t dot1asNbrRateRatioLogIndex = 0;
static dot1asRateRatioLog_t dot1asNbrRateRatioLogBuf[DOT1AS_DEBUG_MAX_PDELAY_LOG];
#endif

/******************************************************************************
*
* @purpose Show the current peer delay measurement values for given interface(s)
*
* @param   port      @b{(input)} Internal Interface number
* @param   showLog   @b{(input)} Show the PDELAY filter log
*
* @returns None
*
* @notes   Use avbMaxIntfaces_g to dump peer delay information for
*          all "enabled" interfaces
*
* @end
*
*******************************************************************************/
void dot1asDebugPdelayShow(uint32_t port, uint8_t showLog)
{
  uint32_t index = 0;
  char     buf[12];
  uint8_t  buflen = sizeof(buf);
  uint32_t extIntf;
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t intfIfNum;

  if (avb_intf_external_to_internal(port, &intfIfNum) != AVB_SUCCESS)
  {
    DOT1AS_TRACE("\nInvalid port specified\n");
    return;
  }

#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
  if (showLog) /* Show filter log */
  {
    DOT1AS_TRACE("\n\n");
    dot1asDebugNbrRateRatioShow(intfIfNum);
    return;
  }
#endif

  DOT1AS_TRACE_LINE(98);
#ifndef CONFIG_EXTERNAL_HOST
  DOT1AS_TRACE("%-6s%-12s%-12s%-12s%-12s%-12s%-12s%-14s%-8s\n",
               "Intf", "T1", "T2", "T3", "T4", "Pdelay",
               "asCapable", "nbrRatio", "LAG");
#else
  printf("%-6s%-12s%-12s%-12s%-12s%-12s%-12s%-14s%-8s\n",
         "Intf", "T1", "T2", "T3", "T4", "Pdelay",
         "asCapable", "nbrRatio", "LAG");
#endif
  DOT1AS_TRACE_LINE(98);

  /* We don't show active LAGs but this could be considered */
  for (index=0; index < dot1asMaxInterfaceCount_g; index++)
  {
    pIntf = &pDot1asIntf_g[index];

    if ((intfIfNum != avbMaxIntfaces_g) && (intfIfNum != index))
    {
      continue;
    }

    if (dot1asIntfIsPdelayCapable(index) == TRUE)
    {
      extIntf = avbCfgxIntfNum[index];
#ifndef CONFIG_EXTERNAL_HOST
      DOT1AS_TRACE("%-6d%-12llu%-12llu%-12llu%-12llu%-12llu%-12s0x%-12x%-6s\n",
                   extIntf,
                   pIntf->pDelayT1.seconds,
                   pIntf->pDelayT2.seconds,
                   pIntf->pDelayT3.seconds,
                   pIntf->pDelayT4.seconds,
                   (uint64_t)pIntf->neighborPropDelay,
                   /* Still show dot1as disabled lag members, but display as False */
                   pIntf->dot1asCapable ? "True" : "False",
                   pIntf->neighborRateRatio,
                   avb_lag_parent_string_get(index, buf, buflen));

      DOT1AS_TRACE("%-6s%-12u%-12u%-12u%-12u\n\n",
                   "",
                   pIntf->pDelayT1.nanoseconds,
                   pIntf->pDelayT2.nanoseconds,
                   pIntf->pDelayT3.nanoseconds,
                   pIntf->pDelayT4.nanoseconds);
#else
      printf("%-6d%-12llu%-12llu%-12llu%-12llu%-12llu%-12s0x%-12x%-6s\n",
             extIntf,
             pIntf->pDelayT1.seconds,
             pIntf->pDelayT2.seconds,
             pIntf->pDelayT3.seconds,
             pIntf->pDelayT4.seconds,
             (uint64_t)pIntf->neighborPropDelay,
             /* Still show dot1as disabled lag members, but display as False */
             pIntf->dot1asCapable ? "True" : "False",
             pIntf->neighborRateRatio,
             avb_lag_parent_string_get(index, buf, buflen));

      printf("%-6s%-12u%-12u%-12u%-12u\n\n",
             "",
             pIntf->pDelayT1.nanoseconds,
             pIntf->pDelayT2.nanoseconds,
             pIntf->pDelayT3.nanoseconds,
             pIntf->pDelayT4.nanoseconds);

#endif
    }

    if (intfIfNum != avbMaxIntfaces_g)
    {
      break;
    }
  }

  DOT1AS_TRACE_LINE(98);
}

/* Function to convert BMCA INFOIS value to string */
uint8_t *dot1asDebugInfoisStringGet(DOT1AS_BMC_INFO_t infoIs)
{
  uint8_t *str;
  switch (infoIs)
  {
    case DOT1AS_INFO_RECEIVED:
      str = (uint8_t *)"Received";
      break;
    case DOT1AS_INFO_MINE:
      str = (uint8_t *)"Mine";
      break;
    case DOT1AS_INFO_AGED:
      str = (uint8_t *)"Aged";
      break;
    case DOT1AS_INFO_DISABLED:
      str = (uint8_t *)"Disabled";
      break;
    case DOT1AS_INFO_UNKNOWN:
      str = (uint8_t *)"Unknown";
      break;
    default:
      str = (uint8_t *)"";
      break;
  }

  return str;
}

/******************************************************************************
*
* @purpose Show Best Master Clock Algorithm state information like port roles,
*          priority vectors, GM and Local clock identities, priorities etc.
*
* @param   None
*
* @returns None
*
* @notes   None
*
* @end
*
*******************************************************************************/
void dot1asDebugBmcaShow(void)
{
  uint8_t *id;
  char     buf[12];
  uint8_t  buflen = sizeof(buf);
  char  buf1[32];
  char  buf2[32];
  uint32_t index = 0;
  uint32_t extIntf;
  uint32_t numPaths = 0;
  DOT1AS_INTF_STATE_t *pIntf;

  DOT1AS_TRACE("GM Present   : %s\n", pDot1asBridge_g->isGmPresent ? "Yes":"No");
  /* Note that the the gmClock below is really the root (best master clock),
   * and is a grandmaster only if priority1 field is < 128.
   */
  id = pDot1asBridge_g->gmClock.id;
  DOT1AS_TRACE("BMC Identity : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
               id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

  DOT1AS_TRACE("BMC Steps    : %d\n", pDot1asBridge_g->gmStepsRemoved);
  DOT1AS_TRACE("BMC Prio1    : %d\n", pDot1asBridge_g->gmPriority1);
  DOT1AS_TRACE("BMC Prio2    : %d\n", pDot1asBridge_g->gmPriority2);
  DOT1AS_TRACE("GM ChangeCnt : %d\n", pDot1asBridge_g->gmChangeCount);
  numPaths = pDot1asBridge_g->gmPath.length / DOT1AS_CLOCK_ID_LEN;
  DOT1AS_TRACE("GM PathTrace : %d hop(s)\n", numPaths);
  for (index = 0; index < numPaths; index++)
  {
    id = pDot1asBridge_g->gmPath.clk[index].id;
    DOT1AS_TRACE("%16s [%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x]\n", " ",
                 id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
  }

  id = pDot1asBridge_g->thisClock.id;
  DOT1AS_TRACE("\nLocal Clock Identity  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
               id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
  DOT1AS_TRACE("Local Clock Prio1     : %d\n", pDot1asBridge_g->priority1);
  DOT1AS_TRACE("Local Clock Prio2     : %d\n", pDot1asBridge_g->priority2);

  id = pDot1asBridge_g->parentId.clock.id;
  DOT1AS_TRACE("\nParent Clock Identity : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
               id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

  DOT1AS_TRACE("\n");
  DOT1AS_TRACE_LINE(70);

#ifndef CONFIG_EXTERNAL_HOST
  DOT1AS_TRACE("%-8s%-8s%-12s%-18s%-18s%-6s\n",
               "Index", "Role", "InfoIs", "PortPrioVec", "MasterPrioVec", "LAG");
#else
  printf("%-8s%-8s%-12s%-18s%-18s%-6s\n",
         "Index", "Role", "InfoIs", "PortPrioVec", "MasterPrioVec", "LAG");
#endif
  DOT1AS_TRACE_LINE(70);

  for (index=0; index < dot1asMaxInterfaceCount_g; index++)
  {
    if (dot1asIsAnnounceCapable(index) == TRUE)
    {
      extIntf = avbCfgxIntfNum[index];
      pIntf = &pDot1asIntf_g[index];
      id = pIntf->portPrioVec.sysId.clock.id;
      sal_sprintf (buf1, "%02u-%02u-%02x:%02x:%02x",
                    pIntf->portPrioVec.sysId.priority1,
                    pIntf->portPrioVec.sysId.priority2,
                    id[5], id[6], id[7]);

      id = pIntf->masterPrioVec.sysId.clock.id;
      sal_sprintf(buf2, "%02u-%02u-%02x:%02x:%02x",
                    pIntf->masterPrioVec.sysId.priority1,
                    pIntf->masterPrioVec.sysId.priority2,
                    id[5], id[6], id[7]);
#ifndef CONFIG_EXTERNAL_HOST
      DOT1AS_TRACE("%-8d%-8s%-12s%-18s%-18s%-6s\n", extIntf,
                   dot1asDebugRoleStringGet(pIntf->selectedRole),
                   dot1asDebugInfoisStringGet(pIntf->infoIs), buf1, buf2,
                   avb_lag_parent_string_get(index, buf, buflen));
#else
      printf("%-8d%-8s%-12s%-18s%-18s%-6s\n", extIntf,
             dot1asDebugRoleStringGet(pIntf->selectedRole),
             dot1asDebugInfoisStringGet(pIntf->infoIs), buf1, buf2,
             avb_lag_parent_string_get(index, buf, buflen));
#endif
    }
  }

  DOT1AS_TRACE_LINE(70);
}

/******************************************************************************
*
* @purpose Show the current local bridge time maintained by TimeSync mechanism
*
* @param   verbose @b{(input)} TRUE to dump log.
* @param   num @b{(input)} number of log entries.
*
* @returns None
*
* @notes   Pass argument 1 to dump the DPLL trace (if enabled)
*
* @end
*
*******************************************************************************/
void dot1asDebugTimesyncShow(uint8_t verbose, uint32_t num)
{
  DOT1AS_TRACE("\n");
  DOT1AS_TRACE("GM Time      : %llu secs, %u ns\n",
               pDot1asBridge_g->bridgeLocalTime.seconds,
               pDot1asBridge_g->bridgeLocalTime.nanoseconds);

  DOT1AS_TRACE("Clock state  : %d\n", pDot1asBridge_g->clkState);
  DOT1AS_TRACE("rateRatioIn  : 0x%08x\n", pDot1asBridge_g->rateRatioIn);
  DOT1AS_TRACE("rateRatioOut : 0x%08x\n", pDot1asBridge_g->rateRatioOut);

#if (DOT1AS_DEBUG_TIMESYNC_LOG == 1)
  if (verbose)
  {
    DOT1AS_TRACE("Timesync log:\n");
    dot1asDebugTimesyncLogShow(num);
  }
#endif
}

/******************************************************************************
*
* @purpose Show DOT1AS statistics for a given interface
*
* @param   intfIndex   @b{(input)} external port number
*
* @returns None
*
* @notes
*
* @end
*
*******************************************************************************/
void dot1asDebugIntfStatsShow(uint32_t port)
{
  DOT1AS_INTF_STATS_t *pStats;
  uint32_t             intIfNum;
  uint32_t             intf;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t member_port;
  AVB_RC_t rc;
#endif
  DOT1AS_INTF_STATS_t statsZero;
  AVB_BOOL_t bShowZero = AVB_FALSE;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    DOT1AS_TRACE("\nInvalid port specified\n");
    return;
  }

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    if (avb_lag_active_intf_get(port, &intf) != AVB_SUCCESS)
    {
      member_port = 0;
      rc = avb_lag_member_next_get(AVB_TRUE, port, &member_port);
      while (rc == AVB_SUCCESS)
      {
        if (pDot1asIntf_g[member_port].dot1asCapable == TRUE)
        {
          intf = member_port;
          break;
        }
        else
        {
          rc = avb_lag_member_next_get(AVB_FALSE, port, &member_port);
        }
      }
      if ( rc == AVB_FAILURE)
      {
        bShowZero = AVB_TRUE;
      }
    }
  }
  else
#endif
  {
    intf = intIfNum;
  }
  if ( bShowZero == AVB_FALSE)
  {
    pStats = &pDot1asIntf_g[intf].stats;
  }
  else
  {
    sal_memset(&statsZero, 0, sizeof(DOT1AS_INTF_STATS_t));
    pStats = &statsZero;
  }


  DOT1AS_TRACE("\n");
  DOT1AS_TRACE("Sync Rx                     : %d\n", pStats->syncRxCount);
  DOT1AS_TRACE("Sync Tx                     : %d\n", pStats->syncTxCount);
  DOT1AS_TRACE("Sync Tx Resends             : %d\n", pStats->syncTransmitTimeouts);
  DOT1AS_TRACE("FollowUp Rx                 : %d\n", pStats->followUpRxCount);
  DOT1AS_TRACE("FollowUp Tx                 : %d\n", pStats->followUpTxCount);
  DOT1AS_TRACE("Sync Rx Timeouts            : %d\n", pStats->syncReceiptTimeouts);
  DOT1AS_TRACE("FollowUp Rx Timeouts        : %d\n", pStats->followupReceiptTimeouts);
  DOT1AS_TRACE("Sync Rx Discards            : %d\n", pStats->syncRxDiscards);
  DOT1AS_TRACE("FollowUp Rx Discards        : %d\n", pStats->followupRxDiscards);

  DOT1AS_TRACE("\n");
  DOT1AS_TRACE("PDelay Req Rx               : %d\n", pStats->pDelayReqRxCount);
  DOT1AS_TRACE("PDelay Req Tx               : %d\n", pStats->pDelayReqTxCount);
  DOT1AS_TRACE("PDelay Resp Rx              : %d\n", pStats->pDelayRespRxCount);
  DOT1AS_TRACE("PDelay Resp Tx              : %d\n", pStats->pDelayRespTxCount);
  DOT1AS_TRACE("PDelay Resp FollowUp Rx     : %d\n", pStats->pDelayRespFollowUpRxCount);
  DOT1AS_TRACE("PDelay Resp FollowUp Tx     : %d\n", pStats->pDelayRespFollowUpTxCount);
  DOT1AS_TRACE("PDelay Req Rx Discards      : %d\n", pStats->pDelayReqRxDiscards);
  DOT1AS_TRACE("PDelay Resp Rx Discards     : %d\n", pStats->pDelayRespRxDiscards);
  DOT1AS_TRACE("PDelay Timeouts             : %d\n", pStats->pDelayReceiptTimeouts);
  DOT1AS_TRACE("Bad PDelay Values           : %d\n", pStats->badPdelayValues);
  DOT1AS_TRACE("Pdelay Lost Resp Exceeded   : %d\n", pStats->pDelayLostResponsesExceeded);

  DOT1AS_TRACE("\n");
  DOT1AS_TRACE("Announce Rx                 : %d\n", pStats->announceRxCount);
  DOT1AS_TRACE("Announce Tx                 : %d\n", pStats->announceTxCount);
  DOT1AS_TRACE("Announce Timeouts           : %d\n", pStats->announceReceiptTimeouts);
  DOT1AS_TRACE("Announce Rx Discards        : %d\n", pStats->announceRxDiscards);

  DOT1AS_TRACE("\n");
  DOT1AS_TRACE("Signaling Rx                : %d\n", pStats->signalingRxCount);
  DOT1AS_TRACE("Signaling Tx                : %d\n", pStats->signalingTxCount);
  DOT1AS_TRACE("Signaling Discards          : %d\n", pStats->signalingRxDiscards);

  DOT1AS_TRACE("\n");
  DOT1AS_TRACE("Sync Too Fast               : %d\n", pStats->syncTooFast);
  DOT1AS_TRACE("Tx Errors                   : %d\n", pStats->txErrors);
  DOT1AS_TRACE("Timestamp Errors            : %d\n", pStats->tsErrors);
  DOT1AS_TRACE("Bad Headers                 : %d\n", pStats->badHeaderCount);
  DOT1AS_TRACE("PTP Discard Count           : %d\n", pStats->ptpDiscardCount);
}


/******************************************************************************
*
* @purpose Get the DOT1AS intervals for all interfaces
*
* @param   None
*
* @returns AVB_SUCCESS or AVB_FAILURE
*
* @end
*
*******************************************************************************/
AVB_RC_t dot1asDebugIntlShow(void)
{
  uint32_t i;

  DOT1AS_INTF_STATE_t *pIntf;

  DOT1AS_TRACE_LINE(64);

#ifndef CONFIG_EXTERNAL_HOST
  DOT1AS_TRACE("%-8s%-12s%-12s%-12s%-12s%-12s\n",
              "Index", "PdelayTxRx", "BmcaTx", "BmcaRx", "syncTx", "syncRx");
#else
  printf("%-8s%-12s%-12s%-12s%-12s%-12s\n",
         "Index", "PdelayTxRx", "BmcaTx", "BmcaRx", "syncTx", "syncRx");
#endif

  DOT1AS_TRACE_LINE(64);

  for (i=0; i < dot1asMaxInterfaceCount_g; i++)
  {
    pIntf = &pDot1asIntf_g[i];

#ifndef CONFIG_EXTERNAL_HOST
    DOT1AS_TRACE("%-8u%-12llu%-12llu%-12llu%-12llu%-12llu\n", avbCfgxIntfNum[i],
                 DOT1AS_INTL_US_TO_MS(pIntf->meanPdelayReqInterval),
                 DOT1AS_INTL_US_TO_MS(pIntf->meanAnnounceInterval),
                 DOT1AS_INTL_US_TO_MS(pIntf->announceReceiptTimeoutInterval),
                 DOT1AS_INTL_US_TO_MS(pIntf->meanSyncInterval),
                 DOT1AS_INTL_US_TO_MS(pIntf->syncReceiptTimeoutInterval));
    DOT1AS_TRACE("%-8s%-12llu%-12llu%-12llu%-12llu%-12llu\n", "" ,
                 DOT1AS_INTL_US_TO_MS(pIntf->pDelayIntervalTimer),
                 DOT1AS_INTL_US_TO_MS(pIntf->announceTransmitTimer),
                 DOT1AS_INTL_US_TO_MS(pIntf->announceReceiptTimer),
                 DOT1AS_INTL_US_TO_MS(pIntf->syncTransmitTimer),
                 DOT1AS_INTL_US_TO_MS(pIntf->syncReceiptTimer));
#else
    printf("%-8u%-12llu%-12llu%-12llu%-12llu%-12llu\n", avbCfgxIntfNum[i],
           DOT1AS_INTL_US_TO_MS(pIntf->meanPdelayReqInterval),
           DOT1AS_INTL_US_TO_MS(pIntf->meanAnnounceInterval),
           DOT1AS_INTL_US_TO_MS(pIntf->announceReceiptTimeoutInterval),
           DOT1AS_INTL_US_TO_MS(pIntf->meanSyncInterval),
           DOT1AS_INTL_US_TO_MS(pIntf->syncReceiptTimeoutInterval));
    printf("%-8s%-12llu%-12llu%-12llu%-12llu%-12llu\n", "" ,
           DOT1AS_INTL_US_TO_MS(pIntf->pDelayIntervalTimer),
           DOT1AS_INTL_US_TO_MS(pIntf->announceTransmitTimer),
           DOT1AS_INTL_US_TO_MS(pIntf->announceReceiptTimer),
           DOT1AS_INTL_US_TO_MS(pIntf->syncTransmitTimer),
           DOT1AS_INTL_US_TO_MS(pIntf->syncReceiptTimer));
#endif
    DOT1AS_TRACE("\n");

  }
  DOT1AS_TRACE_LINE(64);
  return AVB_SUCCESS;
}


/******************************************************************************
*
* @purpose Log the worst correction field value seen
*
* @param   ingressTimestamp       @b{(input)} Ingress SYNC timestamp
* @param   ingressCorrectionField @b{(input)} Ingress correction fld
* @param   egressTimestamp        @b{(input)} Egress SYNC timestamp
* @param   egressCorrectionFld    @b{(input)} Egress correction fld
*
* @returns None
*
* @notes   This log is very useful to debug SYNC processing times
*
* @end
*
*******************************************************************************/
void dot1asDebugCorrectionFld(DOT1AS_TIMESTAMP_t *pIngressTimestamp,
                              int64_t  ingressCorrectionField,
                              DOT1AS_TIMESTAMP_t *pEgressTimestamp,
                              int64_t  egressCorrectionFld,
                              DOT1AS_TIMESTAMP_t *pGmTimestamp)
{
  dot1asDebugCorrection_t *pVal = &dot1asDebugCorrectionValue;

  if (pVal->egressCorrectionFld < egressCorrectionFld)
  {
    pVal->ingressTimestamp = *pIngressTimestamp;
    pVal->ingressCorrectionField = ingressCorrectionField;
    pVal->egressTimestamp = *pEgressTimestamp;
    pVal->egressCorrectionFld = egressCorrectionFld;
    pVal->gmTimestamp = *pGmTimestamp;
    pVal->syncT1 = dot1asDbgSyncT1;
    pVal->syncT2 = dot1asDbgSyncT2;
    pVal->followupT1 = dot1asDbgFollowupT1;
    pVal->followupT2 = dot1asDbgFollowupT2;
  }

  dot1asDbgSyncT1 = 0;
  dot1asDbgFollowupT1 = 0;
  dot1asDbgSyncT2 = 0;
  dot1asDbgFollowupT2 = 0;
}

/******************************************************************************
*
* @purpose Show the max Correction field values recorded
*
* @param   None
*
* @returns None
*
* @end
*
*******************************************************************************/
void dot1asDebugCorrectionShow(void)
{
  dot1asDebugCorrection_t  val;
  dot1asDebugCorrection_t *pVal;

  sal_memcpy(&val, &dot1asDebugCorrectionValue, sizeof(val));
  pVal = &dot1asDebugCorrectionValue;
  sal_memset(&pVal->ingressTimestamp, 0, sizeof(pVal->ingressTimestamp));
  sal_memset(&pVal->egressTimestamp, 0, sizeof(pVal->ingressTimestamp));
  pVal->ingressCorrectionField = 0;
  pVal->egressCorrectionFld = 0;
  pVal = &val;
  DOT1AS_TRACE("\nMax correction value recorded:\n");
  DOT1AS_TRACE("Ingress timestamp  : %llu:%u\n",
                                     pVal->ingressTimestamp.seconds,
                                     pVal->ingressTimestamp.nanoseconds);
  DOT1AS_TRACE("Egress timestamp   : %llu:%u\n",
                                     pVal->egressTimestamp.seconds,
                                     pVal->egressTimestamp.nanoseconds);
  DOT1AS_TRACE("GM timestamp       : %llu:%u\n",
                                     pVal->gmTimestamp.seconds,
                                     pVal->gmTimestamp.nanoseconds);
  DOT1AS_TRACE("Ingress Correction : %lld\n", pVal->ingressCorrectionField >> 16);
  DOT1AS_TRACE("Egress Correction  : %lld\n", pVal->egressCorrectionFld >> 16);
  DOT1AS_TRACE("Sync Rx T1         : %llu\n", pVal->syncT1);
  DOT1AS_TRACE("Sync Rx T2         : %llu\n", pVal->syncT2);
  DOT1AS_TRACE("Followup Rx T1     : %llu\n", pVal->followupT1);
  DOT1AS_TRACE("Followup Rx T2     : %llu\n", pVal->followupT2);
}

/* Function to convert DOT1AS status value to string */
void dot1asStatusString(uint32_t dot1asStatus, char str[])
{
  switch(dot1asStatus)
  {
    case DOT1AS_DISABLED:
      sal_sprintf(str, "%s", "802.1as    : Disabled\n");
    break;
    case DOT1AS_IEEE:
      sal_sprintf(str, "%s", "802.1as    : IEEE\n");
    break;
    case DOT1AS_AVNU_DISABLED:
      sal_sprintf(str, "%s", "802.1as    : AVnu (802.1as Disabled)\n");
    break;
    case DOT1AS_AVNU_AUTOMOTIVE:
      sal_sprintf(str, "%s", "802.1as    : AVnu Automotive Profile\n");
    break;
    default:
    break;
  }
}

/******************************************************************************
*
* @purpose Function to display AVnu status parameters
*
* @param   None
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void avnuStatusDisplay()
{
  uint32_t dot1asStatus;
  uint32_t gmStatus;
  uint32_t slavePortIndex;
  char str[40];

  avnuStatusGet(&dot1asStatus, &gmStatus, &slavePortIndex);
  sal_memset(str, 0x00, sizeof(str));
  dot1asStatusString(dot1asStatus, str);
  sal_printf("%s\n", str);
  if ((dot1asStatus == DOT1AS_IEEE) || (dot1asStatus ==  DOT1AS_DISABLED))
  {
    return;
  }

  if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_UPDATE_SLAVE)
  {
    switch (gmStatus)
    {
      case AVNU_GM_STARTUP_ABSENT:
        sal_printf("GM         : Absent At Startup\n");
        break;
      case AVNU_GM_OPERATIONAL_ABSENT:
        sal_printf("GM         : Absent\n");
        break;
      case AVNU_GM_UNDETERMINED:
        sal_printf("GM         : Undetermined\n");
        break;
      case AVNU_GM_DISABLED:
        sal_printf("GM         : Disabled\n");
        break;
      case AVNU_GM_NO_SLAVE_PORT:
        sal_printf("GM         : No slave port\n");
        break;
      case AVNU_GM_PRESENT:
        sal_printf("GM         : Present\n");
        break;
      default:
        break;
    }

    if (slavePortIndex != dot1asMaxInterfaceCount_g)
    {
      sal_printf("Slave Port : %d\n", avbCfgxIntfNum[slavePortIndex]);
    }
  }
}

void portRoleString(uint32_t portRole, char role[])
{
  switch (portRole)
  {
    case DOT1AS_ROLE_DISABLED:
      sal_sprintf (role, "disabled");
      break;
    case DOT1AS_ROLE_MASTER:
      sal_sprintf (role, "master");
      break;
    case DOT1AS_ROLE_SLAVE:
      sal_sprintf (role, "slave");
      break;
    case DOT1AS_ROLE_PASSIVE:
      sal_sprintf (role, "passive");
      break;
    default:
      sal_sprintf (role, "unknown");
      break;
  }
}

void portAVnuASStatusString(uint32_t asStatus, char str[])
{
  switch (asStatus)
  {
    case AVNU_ASCAPABLE_STATUS_TRUE:
      sal_sprintf(str, "%s", "True");
      break;
    case AVNU_ASCAPABLE_STATUS_PORT_DOWN:
      sal_sprintf(str, "%s(%s)", "False", "Link Down");
      break;
    case AVNU_ASCAPABLE_STATUS_CONFIG_AVNU:
      sal_sprintf(str, "%s(%s)", "False", "AVnu Config");
      break;
    case AVNU_ASCAPABLE_STATUS_CONFIG_DOT1AS:
      sal_sprintf(str, "%s(%s)", "False", "DOT1AS Config");
      break;
    default:
      sal_sprintf(str, "%s", "");
      break;
  }
}
/******************************************************************************
*
* @purpose Function to display AVnu status parameters per port
*
* @param   Port number
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void avnuPortStatusDisplay(uint32_t intIfNum)
{
  char str[40];
  uint8_t intfSyncReceiptTimeout;
  uint32_t asStatus;
  uint32_t index;
  DOT1AS_INTF_STATE_t *pIntfStatus;
  uint32_t dot1asStatus;
  uint32_t gmStatus;
  uint32_t slavePort;
  uint32_t extIntf;

  avnuStatusGet(&dot1asStatus, &gmStatus, &slavePort);
  sal_memset(str, 0x00, sizeof(str));
  dot1asStatusString(dot1asStatus, str);
  sal_printf("\n%s\n", str);

  if (intIfNum != AVB_INTF_INVALID)
  {
    if (avb_intf_external_to_internal(intIfNum, &index) == AVB_FAILURE)
    {
      sal_printf("Invalid port specified\n");
      return;
    }

    pIntfStatus = &pDot1asIntf_g[index];
    sal_memset(str, 0x00, sizeof(str));
    portRoleString(pIntfStatus->selectedRole, str);
    sal_printf("%-20s : %s\n", "Port Role", str);
    if ((pIntfStatus->selectedRole == DOT1AS_ROLE_PASSIVE) ||
       (pIntfStatus->selectedRole == DOT1AS_ROLE_DISABLED))
    {
      return;
    }

    sal_memset(str, 0x00, sizeof(str));
    avnuASCapableIntfStatusGet(intIfNum, &asStatus);
    portAVnuASStatusString(asStatus, str);
    sal_printf("%-20s : %s\n", "AS Capable Status", str);
    if (pIntfStatus->dot1asCapable == FALSE)
    {
      return;
    }

    if (pIntfStatus->currentLogPdelayReqInterval == PDELAY_TX_DISABLE_VAL)
    {
      sal_printf("%-20s : %s\n", "PDelay Value","Disabled");
    }
    else
    {
      sal_printf("%-20s : %d\n", "PDelay Value", pIntfStatus->neighborPropDelay);
    }

    sal_printf("%-20s : 0x%08x\n", "Rate Ratio", pIntfStatus->neighborRateRatio);
    if (pIntfStatus->selectedRole == DOT1AS_ROLE_MASTER)
    {
      sal_printf("%-20s : %dms\n", "SyncTx Interval", DOT1AS_INTL_US_TO_MS(pIntfStatus->meanSyncInterval));
      sal_printf("%-20s : %d\n", "SyncTx Resends", pIntfStatus->stats.syncTransmitTimeouts);
    }
    else if (pIntfStatus->selectedRole == DOT1AS_ROLE_SLAVE)
    {
      if (dot1asCfgxIntfSyncReceiptTimeoutGet(intIfNum, &intfSyncReceiptTimeout) != AVB_SUCCESS)
      {
        intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
      }
      sal_printf("%-20s : %dms\n", "SyncRx Interval", DOT1AS_INTL_US_TO_MS(pIntfStatus->syncReceiptTimeoutInterval)/intfSyncReceiptTimeout);
      sal_printf("%-20s : %dms\n", "Sync Timeout", DOT1AS_INTL_US_TO_MS(pIntfStatus->syncReceiptTimeoutInterval));
      sal_printf("%-20s : %d\n", "Sync Timeouts", pIntfStatus->stats.syncReceiptTimeouts);
      sal_printf("%-20s : %d\n", "PDelay Timeouts", pIntfStatus->stats.pDelayReceiptTimeouts);
    }
    return;
  }

  AVB_TRACE_LINE(114);
  sal_printf("%-5s %8s %10s %9s %12s %12s %9s %12s %9s %9s %9s\n", "", "", "", "", "", "syncTx", "syncTx", "syncRx", "syncRx", "syncRx", "PDelay");
  sal_printf("%-5s %8s %10s %9s %12s %12s %9s %12s %9s %9s %9s\n", "Intf", "PortRole", "asCapable", "PDelay", "RateRatio", "Interval",
         "Resends", "Interval", "Timeout", "Timeouts", "Timeouts");
  AVB_TRACE_LINE(114);
  for (intIfNum = 0; intIfNum < dot1asMaxInterfaceCount_g; intIfNum++)
  {
    pIntfStatus = &pDot1asIntf_g[intIfNum];
    extIntf = avbCfgxIntfNum[intIfNum];
    sal_printf("%-6d", extIntf);
    sal_memset(str, 0x00, sizeof(str));
    portRoleString(pIntfStatus->selectedRole, str);
    sal_printf("%8s",str);
    if ((pIntfStatus->selectedRole == DOT1AS_ROLE_PASSIVE) ||
        (pIntfStatus->selectedRole == DOT1AS_ROLE_DISABLED))
    {
      sal_printf("\n");
      continue;
    }

    sal_memset(str, 0x00, sizeof(str));
    sal_printf("%10s", (pIntfStatus->dot1asCapable == TRUE) ? "True" : "False");
    if (pIntfStatus->dot1asCapable == FALSE)
    {
      sal_printf("\n");
      continue;
    }

    sal_memset(str, 0x00, sizeof(str));
    if (pIntfStatus->currentLogPdelayReqInterval == PDELAY_TX_DISABLE_VAL)
    {
      sal_sprintf(str, "%s", "Disabled");
    }
    else
    {
      sal_sprintf(str, "%d", pIntfStatus->neighborPropDelay);
    }

    sal_printf("%11s", str);
    sal_memset(str, 0x00, sizeof(str));
    sal_sprintf(str, "0x%x", pIntfStatus->neighborRateRatio);
    sal_printf("%13s", (pIntfStatus->dot1asCapable == TRUE) ? str : "");
    if (pIntfStatus->selectedRole == DOT1AS_ROLE_MASTER)
    {
      sal_printf("%11dms", DOT1AS_INTL_US_TO_MS(pIntfStatus->meanSyncInterval));
      sal_printf("%10d", pIntfStatus->stats.syncTransmitTimeouts);
    }

    if (pIntfStatus->selectedRole == DOT1AS_ROLE_SLAVE)
    {
      if (dot1asCfgxIntfSyncReceiptTimeoutGet(avbCfgxIntfNum[intIfNum], &intfSyncReceiptTimeout) != AVB_SUCCESS)
      {
        intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
      }
      sal_printf("%13s", "");
      sal_printf("%10s", "");
      sal_printf("%11dms", DOT1AS_INTL_US_TO_MS(pIntfStatus->syncReceiptTimeoutInterval)/intfSyncReceiptTimeout);
      sal_printf("%8dms", DOT1AS_INTL_US_TO_MS(pIntfStatus->syncReceiptTimeoutInterval));
      sal_printf("%10d", pIntfStatus->stats.syncReceiptTimeouts);
      sal_printf("%10d", pIntfStatus->stats.pDelayReceiptTimeouts);
    }
    sal_printf("\n");
  }

  AVB_TRACE_LINE(114);
}

/******************************************************************************
*
* @purpose Function to enable/disable AVnu trace flags
*
* @param   flags - see AVNU_TRACE_XXX flags for more details.
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void avnuDebugFlagsSet(uint32_t flags)
{
  avnuTraceMask = flags;
}

/******************************************************************************
*
* @purpose Function to enable/disable trace flags
*
* @param   flags - see DOT1AS_TRACE_XXX flags for more details.
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void dot1asDebugFlagsSet(uint32_t flags)
{
  dot1asTraceMask = flags;
}

#if (DOT1AS_DEBUG_TIMESYNC_LOG == 1)
void dot1asDebugTimesyncLog(DOT1AS_TIMESTAMP_t *pPrevRxTime,
                            DOT1AS_TIMESTAMP_t *pCurrentRxTime,
                            uint64_t gmSec, uint32_t gmNano,
                            uint64_t localSec, uint32_t localNano,
                            int64_t correction, int32_t rateRatioIn,
                            int32_t rateRatioOut)
{
  uint32_t index;
  dot1asDebugTimesyncLog_t *logBuf;

  index = dot1asTimesyncLogIndex;
  if (index >= DOT1AS_DEBUG_MAX_TIMESYNC_LOG)
  {
    index = 0;
    dot1asTimesyncLogIndex = 0;
  }

  logBuf = &dot1asTimesyncLogBuf[index];
  logBuf->prevRxTime = *pPrevRxTime;
  logBuf->currentRxTime = *pCurrentRxTime;
  logBuf->gmSec = gmSec;
  logBuf->gmNano = gmNano;
  logBuf->localSec = localSec;
  logBuf->localNano = localNano;
  logBuf->correction = (correction >> 16);
  logBuf->rateRatioIn = rateRatioIn;
  logBuf->rateRatioOut = rateRatioOut;
  dot1asTimesyncLogIndex++;
}

/* Dump the captured Timesync log starting from the first iteration */
void dot1asDebugTimesyncLogShow(uint32_t num)
{
  uint32_t index = 0, count = 0;
  dot1asDebugTimesyncLog_t *logBuf;

  DOT1AS_TRACE_LINE(110);
#ifndef CONFIG_EXTERNAL_HOST
  DOT1AS_TRACE("%-12s%-12s%-12s%-12s%-12s%-12s%-17s%-12s%-12s\n",
               "prevRx", "curRx", "gmSec", "gmNano", "sec",
               "nano", "correction", "ratioIn", "ratioOut");
#else
  printf("%-12s%-12s%-12s%-12s%-12s%-12s%-17s%-12s%-12s\n",
         "prevRx", "curRx", "gmSec", "gmNano", "sec",
         "nano", "correction", "ratioIn", "ratioOut");
#endif
  DOT1AS_TRACE_LINE(110);

  count = (num < dot1asTimesyncLogIndex) ? num : dot1asTimesyncLogIndex;
  index = (num < dot1asTimesyncLogIndex) ? (dot1asTimesyncLogIndex - num) : 0;
  logBuf = &dot1asTimesyncLogBuf[index];
  for (index = 0; index < count; index++, logBuf++)
  {
#ifndef CONFIG_EXTERNAL_HOST
    DOT1AS_TRACE("%-12u%-12u%-12llu%-12u%-12llu%-12u%-17lld0x%-12x0x%-12x\n",
                 logBuf->prevRxTime.nanoseconds, logBuf->currentRxTime.nanoseconds,
                 logBuf->gmSec, logBuf->gmNano, logBuf->localSec, logBuf->localNano,
                 logBuf->correction, logBuf->rateRatioIn, logBuf->rateRatioOut);
#else
    printf("%-12u%-12u%-12llu%-12u%-12llu%-12u%-17lld0x%-12x0x%-12x\n",
           logBuf->prevRxTime.nanoseconds, logBuf->currentRxTime.nanoseconds,
           logBuf->gmSec, logBuf->gmNano, logBuf->localSec, logBuf->localNano,
           logBuf->correction, logBuf->rateRatioIn, logBuf->rateRatioOut);
#endif
  }
}
#endif


#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
void dot1asDebugNbrRateRatioLog(uint32_t intfIndex,
                                int64_t  deltaT1, int64_t  deltaT2,
                                int64_t  deltaT3, int64_t  deltaT4,
                                int64_t x1, int32_t y1, int64_t x2, int32_t y2,
                                int32_t ratioIn, int32_t ratioOut)
{
  dot1asRateRatioLog_t *p;

  if (dot1asNbrRateRatioLogIndex >= DOT1AS_DEBUG_MAX_PDELAY_LOG)
  {
    dot1asNbrRateRatioLogIndex = 0;
  }

  p = &dot1asNbrRateRatioLogBuf[dot1asNbrRateRatioLogIndex];

  p->intfIndex = intfIndex;
  p->deltaT1 = deltaT1;
  p->deltaT2 = deltaT2;
  p->deltaT3 = deltaT3;
  p->deltaT4 = deltaT4;
  p->x1 = x1;
  p->y1 = y1;
  p->x2 = x2;
  p->y2 = y2;
  p->ratioIn = ratioIn;
  p->ratioOut = ratioOut;
  dot1asNbrRateRatioLogIndex++;
}

void dot1asDebugNbrRateRatioShow(uint32_t intfIndex)
{
  uint32_t i;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t intf;
#endif
  uint32_t extIntf;
  dot1asRateRatioLog_t *p;

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intfIndex))
  {
    uint32_t lag;

    lag = avbCfgxIntfNum[intfIndex];
    if (avb_lag_active_intf_get(lag, &intf) == AVB_SUCCESS)
    {
      intfIndex = intf;
    }
  }
#endif
  extIntf = avbCfgxIntfNum[intfIndex];

  DOT1AS_TRACE("Nbr Rate Ratio log for intf %d\n", extIntf);

  DOT1AS_TRACE_LINE(120);
#ifndef CONFIG_EXTERNAL_HOST
  DOT1AS_TRACE("%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s\n",
               "DeltaT1", "DeltaT2", "DeltaT3", "DeltaT4", "x1", "y1", "x2", "y2",
               "ratioIn", "ratioOut");
#else
  printf("%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s\n",
         "DeltaT1", "DeltaT2", "DeltaT3", "DeltaT4", "x1", "y1", "x2", "y2",
         "ratioIn", "ratioOut");
#endif
  DOT1AS_TRACE_LINE(120);

  for (i=0; i < dot1asNbrRateRatioLogIndex; i++)
  {
    p = &dot1asNbrRateRatioLogBuf[i];

    if (p->intfIndex != intfIndex)
    {
      continue;
    }
#ifndef CONFIG_EXTERNAL_HOST
    DOT1AS_TRACE("%-12llu%-12llu%-12llu%-12llu%-12lld%-12d%-12lld%-12d0x%-12x0x%-12x\n",
                 p->deltaT1, p->deltaT2, p->deltaT3, p->deltaT4,
                 p->x1, p->y1, p->x2, p->y2, p->ratioIn, p->ratioOut);
#else
    printf("%-12llu%-12llu%-12llu%-12llu%-12lld%-12d%-12lld%-12d0x%-12x0x%-12x\n",
           p->deltaT1, p->deltaT2, p->deltaT3, p->deltaT4,
           p->x1, p->y1, p->x2, p->y2, p->ratioIn, p->ratioOut);
#endif
  }
}
#endif

/******************************************************************************
*
* @purpose Show the current status for given interface(s)
*
* @param   intfIfNum @b{(input)} Internal Interface number
*
* @returns None
*
* @notes   Use avbMaxIntfaces_g to dump peer delay information for
*          all "enabled" interfaces
*
* @end
*
*******************************************************************************/
void dot1asDebugIntfStatusShow(uint32_t port)
{
  DOT1AS_INTF_STATE_t pIntfStatus;
  uint32_t            intfInfdex;
  char                link[12];
  char                role[12];
  char                syncRx[12];
  char                strPDelayReqState[12];
  uint8_t             intfSyncReceiptTimeout;
  uint32_t            link_status = AVB_PORT_LINK_DOWN;
  uint32_t            member_port;
#ifdef DOT1AS_LAG_SUPPORT
  AVB_RC_t            rc;
#endif

  if (avb_intf_external_to_internal(port, &intfInfdex) != AVB_SUCCESS)
  {
    sal_printf("Invalid port %d specified\n", port);
    return;
  }

  if (dot1asIntfStatus(port, &pIntfStatus) == AVB_SUCCESS)
  {
    switch (pIntfStatus.pDelayReqState)
    {
      case DOT1AS_PDELAY_DISABLED:
        sal_sprintf (strPDelayReqState, "Disabled");
        break;
      case DOT1AS_PDELAY_REQ_SEND:
        sal_sprintf (strPDelayReqState, "ReqSend");
        break;
      case DOT1AS_PDELAY_WAIT_FOR_RESP:
        sal_sprintf (strPDelayReqState, "Wait4Resp");
        break;
      case DOT1AS_PDELAY_WAIT_FOR_RESP_FOLLOWUP:
        sal_sprintf (strPDelayReqState, "Wait4FollUp");
        break;
      default:
        sal_sprintf (strPDelayReqState, "Unknown");
        break;
    }
    portRoleString(pIntfStatus.selectedRole, role);
    switch (pIntfStatus.syncRxState)
    {
      case DOT1AS_SYNC_RX_DISABLED:
        sal_sprintf (syncRx, "Disabled");
        break;
      case DOT1AS_SYNC_RX_WAIT:
        sal_sprintf (syncRx, "Wait");
        break;
      case DOT1AS_SYNC_RX_WAIT_FOLLOWUP:
        sal_sprintf (syncRx, "FollowUp");
        break;
      default:
        sal_sprintf (syncRx, "Unknown");
        break;
    }

#ifdef DOT1AS_LAG_SUPPORT
    if (AVB_INTERNAL_INTF_IS_LAG(intfInfdex))
    {
      rc = avb_lag_member_next_get(AVB_TRUE, port, &member_port);
      while (rc == AVB_SUCCESS)
      {
        cbx_port_attribute_get(member_port, cbxPortAttrLinkStatus, &link_status);
        if (link_status == AVB_PORT_LINK_UP)
        {
          break;
        }
        else if (avb_lag_member_next_get(AVB_FALSE, port, &member_port) == AVB_FAILURE)
        {
          member_port = AVB_INTF_INVALID;
          break;
        }
      }
    }
    else
    {
      member_port = intfInfdex;
    }
#else
    member_port = intfInfdex;
#endif

    if (dot1asCfgxIntfSyncReceiptTimeoutGet(port, &intfSyncReceiptTimeout) != AVB_SUCCESS)
    {
      DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to get sync receipt timeout for intf = %u\n", __FUNCTION__, port);
      intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
    }

    cbx_port_attribute_get(member_port, cbxPortAttrLinkStatus, &link_status);
    sal_sprintf (link, link_status ? "Up" : "Down");

    sal_printf("Admin mode                : %s\n",    pIntfStatus.pttPortEnabled ? "Enabled":"Disabled");
    sal_printf("Link state                : %s\n",    link);
    sal_printf("Dot1as capable            : %s\n",    pIntfStatus.dot1asCapable ? "True":"False");
    sal_printf("Peer delay                : %d\n",    pIntfStatus.neighborPropDelay);
    sal_printf("Rate ratio                : 0x%08x\n",pIntfStatus.neighborRateRatio);
    sal_printf("Is measuring delay        : %s\n",    pIntfStatus.isMeasuringPdelay ? "True":"False");
    sal_printf("Allowed lost responses    : %d\n",    pIntfStatus.allowedLostReponses);
    sal_printf("Number of lost responses  : %d\n",    pIntfStatus.numPdelayRespLost);
    sal_printf("Peer delay threshold      : %d\n",    pIntfStatus.neighborPropDelayThres);
    sal_printf("Port role                 : %s\n",    role);
    sal_printf("Peer delay request state  : %s\n",    strPDelayReqState);
    sal_printf("Sync receive state        : %s\n",    syncRx);
    sal_printf("Announce interval         : %dms\n",  DOT1AS_INTL_US_TO_MS(pIntfStatus.meanAnnounceInterval));
    sal_printf("Sync interval             : %dms\n",  DOT1AS_INTL_US_TO_MS(pIntfStatus.syncReceiptTimeoutInterval)/intfSyncReceiptTimeout);
    if (pIntfStatus.currentLogPdelayReqInterval == DOT1AS_INTF_AVNU_INIT_PDELAY_MAX)
    {
      sal_printf("PDelay interval           : Disabled\n");
    }
    else
    {
      sal_printf("PDelay interval           : %dms\n",  DOT1AS_INTL_US_TO_MS(pIntfStatus.meanPdelayReqInterval));
    }
    sal_printf("Sync reciept timeout      : %dms\n",  DOT1AS_INTL_US_TO_MS(pIntfStatus.syncReceiptTimeoutInterval));
    sal_printf("Announce reciept timeout  : %dms\n",  DOT1AS_INTL_US_TO_MS(pIntfStatus.announceReceiptTimeoutInterval));
  }
  else
  {
    sal_printf("Error getting 802.1as port status for port %d\n", port);
  }

}


#if DEV_BUILD_DOT1AS
extern void dot1asAvnuSignalingSyncRxTimeoutSet(uint32_t intfIndex, int8_t logSyncRx);


/******************************************************************************
* @purpose  Send an 802.1AS SIGNALING PDU.
*
* @param    port                  @b{(input)} External port number
* @param    linkDelayLogInterval  @b{(input)} log pdelay interval value
* @param    timeSyncLogInterval   @b{(input)} log sync interval value
* @param    announceLogInterval   @b{(input)} log announce interval value
*
* @returns  none
*
* @comments Used for testing the signaling functionality.
*
* @end
*******************************************************************************/
void dot1asDevSigSend(uint32_t port, int8_t linkDelayLogInterval, int8_t timeSyncLogInterval, int8_t announceLogInterval)
{
  uint32_t  intfIndex;
  DOT1AS_INTF_STATE_t *pIntf;

  if (avb_intf_external_to_internal(port, &intfIndex) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_SIG("\n%s: Invalid port specified\n", __FUNCTION__);
    return;
  }

  if (dot1asSignalingSend(intfIndex, linkDelayLogInterval, timeSyncLogInterval, announceLogInterval) != AVB_SUCCESS)
  {
    DOT1AS_TRACE("\n%s: Failed to send signaling message on interface %d\n", __FUNCTION__, port);
  }
  else
  {
    /* Only do this for AVnu mode. Still want to allow sending signals in IEEE mode to test that it has no effect */
    if (pDot1asBridge_g->avnuMode == AVB_ENABLE)
    {
      pIntf = &pDot1asIntf_g[intfIndex];

      pDot1asBridge_g->avnuSigSyncLogInterval = timeSyncLogInterval;
      pDot1asBridge_g->avnuSigSyncRxCount = 0;
      if (timeSyncLogInterval > pIntf->currentLogSyncInterval) /* If the new sync interval will be larger then increase the timeout ahead of time */
      {
        dot1asAvnuSignalingSyncRxTimeoutSet(intfIndex, timeSyncLogInterval);
        pDot1asBridge_g->avnuSigSyncAdjustTimeout = FALSE; /* Only need to adjust if the signal is ignored */
      }
      else
      {
        pDot1asBridge_g->avnuSigSyncAdjustTimeout = TRUE;
      }
    }
  }

}
#endif

/******************************************************************************
* @purpose Converts an external port number to internal port
*
* @param=extIntf - external port number
* @param=intIntf - internal avb port number
*
* @returnval=0 - OK
* @returnval!=0 - failed
*
* @comments Can be called by other components
*
* @end
*******************************************************************************/
AVB_RC_t avb_intf_external_to_internal(uint32_t extIntf, uint32_t *intIntf)
{
  if (extIntf < avbMaxIntfaces_g)
  {
    *intIntf = extIntf;
    return AVB_SUCCESS;
  }
  else
  { 
    return AVB_FAILURE;
  } 
}

/******************************************************************************
* @purpose  Get the link state for the port
*
* @param    port  @b{(input)}  The external port number
*
* @returns  Link state
*
* @comments Can be called by other components
*
* @end
*******************************************************************************/
uint8_t portLinkStateGet(uint32_t port)
{
  uint32_t link_status;

  cbx_port_attribute_get(port, cbxPortAttrLinkStatus, &link_status);
  return link_status;
}

/******************************************************************************
* @purpose Returns the string for the parent LAG index,or null if not LAG member
*
* @param=intIfNum - AVB internal interface number
* @param=buf      - buffer to hold the string
* @param=buflen   - buffer length
*
* @return char-buffer
* @comments
*
* @end
*******************************************************************************/
char *avb_lag_parent_string_get(uint32_t intIfNum, char *buf, uint8_t buflen)
{
  *buf = '\0';
  return buf;
}

/* Dummy function required in AVNU mode. Needs to be updated  When AVNU mode
 * is supported */
uint32_t portLinkChangeCountGet(uint32_t port)
{
  return 0;
}

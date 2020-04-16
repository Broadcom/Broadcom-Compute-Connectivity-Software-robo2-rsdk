/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:    dot1as_timesync.c
 */

#include "osal/sal_time.h"
#include "dot1as.h"
#include "dot1as_cfgx.h"
#include "fsal/ts.h"
#include "avb.h"

extern uint8_t avb_port_master_slave_cfg_get(uint32_t port);

AVB_RC_t dot1asSyncSend(uint32_t intfIndex);

AVB_RC_t dot1asFollowupSend(uint32_t intfIndex,
                           DOT1AS_TIMESTAMP_t *pSyncEgrHwTimestamp);

void dot1asConvertHwTimestamp(DOT1AS_TIMESTAMP_t *pTimestamp,
                              DOT1AS_TIMESTAMP_t *pConvertedTS,
                              int32_t rateRatio);


AVB_RC_t dot1asPropagateSyncInfo(uint32_t rxIntfIndex,
                                DOT1AS_SYNC_INFO_t *pRcvdSyncInfo);

void dot1asComputeCorrectionFld(DOT1AS_TIMESTAMP_t *pEgrTimestamp,
                                DOT1AS_TIMESTAMP_t *pSyncRxTimestamp,
                                uint64_t            pDelay,
                                int64_t            *pCorrection);

void dot1asSlaveClockInit(DOT1AS_TIMESTAMP_t *pGmTimestamp,
                          DOT1AS_TIMESTAMP_t *pSyncRxTimestamp,
                          int64_t            correction,
                          uint64_t           pDelay);

void dot1asSlaveClockUpdate(DOT1AS_TIMESTAMP_t *pGmTimestamp,
                            DOT1AS_TIMESTAMP_t *pSyncRxTimestamp,
                            int64_t            correction,
                            uint64_t           pDelay,
                            int32_t            rateRatioIn,
                            int32_t            neighborRateRatio);

void dot1asMasterClockUpdate(DOT1AS_TIMESTAMP_t *pSyncRxTimestamp);
void dot1asMasterClockInit(DOT1AS_TIMESTAMP_t *pSyncRxTimestamp);

/******************************************************************************
* @purpose  Initialize TimeSync state on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntfTimesyncInit(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
  pIntf = &pDot1asIntf_g[intfIndex];

  pIntf->syncTxSeqId = 0; /* TODO */

  pIntf->syncTransmitTimerReset = TRUE;
  pIntf->syncReceiptTimerReset = TRUE;
  pIntf->followupReceiptTimerReset = TRUE;
  pIntf->syncInfoAvailable = FALSE;
  sal_memset(&pIntf->lastSyncSentTime, 0x00, sizeof(pIntf->lastSyncSentTime));
  sal_memset(&pIntf->syncRxInfo, 0x00, sizeof(pIntf->syncRxInfo));
  pIntf->syncRxState = DOT1AS_SYNC_RX_WAIT;
}

/******************************************************************************
* @purpose  Disable TimeSync state on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments Internal state machine variables are cleared on init.
*
* @end
*******************************************************************************/
void dot1asIntfTimesyncDisable(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
  pIntf = &pDot1asIntf_g[intfIndex];

  pIntf->syncRxState = DOT1AS_SYNC_RX_DISABLED;
  pIntf->avnuSyncReadyCount = 0;
}


/******************************************************************************
* @purpose  Check whether Time Synchronization mechanism can be enabled on the
*           given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  uint8_t - TRUE if interface is capable, else FALSE
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIntfIsTimesyncCapable(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
  pIntf = &pDot1asIntf_g[intfIndex];
  uint8_t capable = TRUE;

  if (pDot1asBridge_g->avnuMode == TRUE)
  {
    (void) dot1asCfgxIntfAVNUDot1asCapableGet(avbCfgxIntfNum[intfIndex], &capable);
  }

  /* 802.1AS Global admin mode must be enabled,
   * 802.1AS Interface admin mode must be enabled,
   * Interface must be asCapable in IEEE mode or link should be up in AVnu mode
   */

  if (DOT1AS_IS_INTF_CPU(intfIndex)) {
    return FALSE;
  }
  if ((pDot1asBridge_g->adminMode) &&
      (pIntf->pttPortEnabled == TRUE) &&
      (capable == TRUE))
  {
    return TRUE;
  }

  return FALSE;
}

/******************************************************************************
* @purpose  Check whether Time Synchronization mechanism can be enabled on the
*           given interface taking LAG membership into account
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  uint8_t - TRUE if interface is capable, else FALSE
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIsTimesyncCapable(uint32_t intfIndex)
{
  uint8_t  capable = FALSE;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t lagIntf;
  uint32_t lagExtIntfNum;
  uint32_t activeLagIntf;
#endif
  capable = dot1asIntfIsTimesyncCapable(intfIndex);

  /* Is this interface a member of the Master LAG?
     If there is an active lag member and this is not it, return FALSE - NOT Capable
   */
#ifdef DOT1AS_LAG_SUPPORT
  if (avb_lag_intf_for_member_port(intfIndex, &lagIntf) == AVB_SUCCESS)
  {
    lagExtIntfNum = AVB_LAG_INTF_NUM_FROM_INTERNAL(lagIntf);
    if (avb_port_master_slave_cfg_get(lagExtIntfNum) == AVB_PORT_MS_MASTER)
    {
      if (avb_lag_active_intf_get(lagExtIntfNum, &activeLagIntf) == AVB_SUCCESS)
      {
        if (intfIndex != activeLagIntf)
        {
          capable = FALSE;
        }
      }
    }
  }
#endif
  return capable;
}

/******************************************************************************
* @purpose  Process the received 802.1AS SYNC PDU.
*
* @param    intfIndex  @b{(input)} interface index
* @param    pPdu  @b{(input)} pointer to the received PDU
*
* @returns  none
*
* @comments Basic validations for DA, ETHTYPE, Interface number have already
*           been done. Caller frees the buffer.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asSyncReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu, uint32_t timeStamp)
{
  uint64_t interval;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_SYNC_INFO_t  *pRxSyncInfo;
  DOT1AS_PDU_HEADER_t *pHdr;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t  lagCfgIntf;
#endif
  uint16_t  port;
  uint8_t   intfSyncReceiptTimeout;
  uint8_t   adjustTimeout = TRUE;
  cbx_ts_time_t tod;

  pHdr = &pPdu->hdr;

  pIntf = &pDot1asIntf_g[intfIndex];
#ifdef DOT1AS_LAG_SUPPORT
  if (avb_lag_intf_for_member_port(intfIndex, &lagCfgIntf) == AVB_SUCCESS)
  {
    port = avbCfgxIntfNum[lagCfgIntf];
  }
  else
  {
    port = avbCfgxIntfNum[intfIndex];
  }
#else
  port = avbCfgxIntfNum[intfIndex];
#endif
  if (dot1asCfgxIntfSyncReceiptTimeoutGet(port, &intfSyncReceiptTimeout) != AVB_SUCCESS)
  {
    intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
  }

  pRxSyncInfo = &pIntf->syncRxInfo;
  /* Fetch the ingress timestamp of the SYNC message  */
  if (timeStamp == 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to get ingress timestamp on %d\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.tsErrors++;
  }
  dot1asTimestampUpdate(timeStamp, &tod);
  pRxSyncInfo->syncRxTimestamp.seconds = tod.sec;
  pRxSyncInfo->syncRxTimestamp.nanoseconds = tod.nsec;

  switch (pIntf->syncRxState)
  {
    case DOT1AS_SYNC_RX_DISABLED:
      /* Discard SYNC if interface is not enabled */
      pIntf->stats.syncRxDiscards++;
      break;

    case DOT1AS_SYNC_RX_WAIT:
      /* Check if a GM is present and the port role is Slave */
      /* If a port is not enabled, the port role will be DISABLED. So, by
       * checking for port role, we implicitly check for port enabled.
       * Also, the DOT1AS_SYNC_RX_WAIT state is set only if TimeSync criteria
       * is met. See dot1asIsTimesyncCapable().
       * In IEEE mode, also check, if (rcvdMDSyncPtr->sourcePortIdentity ==
       * gmPriority.sourcePortIdentity), that way we accept SYNC message
       * only from the parent clock known to this system.
       * In AVnu mode do not check source port identity because
       * BMCA is not enabled in this mode.
       */
      if ((pDot1asBridge_g->isGmPresent == TRUE) &&
          (pIntf->selectedRole == DOT1AS_ROLE_SLAVE) &&
          ((pDot1asBridge_g->avnuMode == TRUE) ? TRUE
                                               : (DOT1AS_CLOCK_ID_IS_EQ(&pHdr->portId.clock,&pDot1asBridge_g->parentId.clock))) &&
          ((pDot1asBridge_g->avnuMode == TRUE) ? TRUE
                                               : (pHdr->portId.num == pDot1asBridge_g->parentId.num)))
      {
        /* Store the received SYNC info */
        pRxSyncInfo->lastSeqId = pHdr->sequenceId;
        pRxSyncInfo->rxIntfIndex  = intfIndex;
        /* Message interval from SYNC PDU */
        pRxSyncInfo->pduInterval = pHdr->pduInterval;
        if (pDot1asBridge_g->avnuMode == AVB_ENABLE)
        {
          if (pDot1asBridge_g->avnuSigSyncLogInterval != DOT1AS_SIGNALING_MSG_INT_INVALID)
          {
            adjustTimeout = FALSE;
            /* Are we still waiting for the link partner to adjust the sync interval? */
            if (pRxSyncInfo->pduInterval != pDot1asBridge_g->avnuSigSyncLogInterval)
            {
              pDot1asBridge_g->avnuSigSyncRxCount++;
              if (pDot1asBridge_g->avnuSigSyncRxCount >= DOT1AS_AVNU_SIGNALING_SYNC_COUNT_MAX)
              {
                DOT1AS_TRACE_SIG("\n%s: Signal was ignored on port %d\n", __FUNCTION__, port);
                pDot1asBridge_g->avnuSigSyncLogInterval = DOT1AS_SIGNALING_MSG_INT_INVALID;
                pDot1asBridge_g->avnuSigSyncRxCount = 0;
                pDot1asBridge_g->avnuSigSyncAdjustTimeout = FALSE;
                adjustTimeout = TRUE;
              }
            }
            else /* Link partner has adjusted to the signaling message */
            {
              pDot1asBridge_g->avnuSigSyncLogInterval = DOT1AS_SIGNALING_MSG_INT_INVALID;
              pDot1asBridge_g->avnuSigSyncRxCount = 0;
              /* In cases where the sync frequency was decreased, we need to adjust the timeout now */
              if (pDot1asBridge_g->avnuSigSyncAdjustTimeout == TRUE)
              {
                pDot1asBridge_g->avnuSigSyncAdjustTimeout = FALSE;
                adjustTimeout = TRUE;
              }
            }
          }
        }

        /* Set SYNC and followup timeout based on incoming interval */
        if (dot1asIntervalLog2Usec(pHdr->pduInterval, &interval) != AVB_SUCCESS)
        {
          /* Use this port's interval value */
          interval = pIntf->meanSyncInterval;
        }

        /* Adjust nanosec value to align with timer tick interval */
        dot1asIntervalUsAdjust(&interval);

        /* Sync timeout is 'n' times interval
           Check to see if interval complies with signal and adjust timeout accordingly */
        if (adjustTimeout == TRUE)
        {
          pIntf->syncReceiptTimeoutInterval = interval * intfSyncReceiptTimeout;
        }

        /* Followup timeout is one-sync interval */
        pIntf->followupReceiptTimeoutInterval = interval;
        pIntf->followupReceiptTimerReset = TRUE;

        /* Calculate upstreamTxTime. This is not required as the correction
         * field will adjusted based on the incoming PDELAY
         */
        pIntf->syncRxState = DOT1AS_SYNC_RX_WAIT_FOLLOWUP;
        DOT1AS_TRACE_RX("DOT1AS PDU event: Received SYNC on port[%d]\n", avbCfgxIntfNum[intfIndex]);
      }
      else
      {
        pIntf->stats.syncRxDiscards++; /* SYNC received on non-SLAVE port */
      }
      break;

    case DOT1AS_SYNC_RX_WAIT_FOLLOWUP:
      /* Received SYNC, expecting FOLLOWUP. Discard */
      pIntf->stats.syncRxDiscards++;
      break;

    default:
      break;
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Process the received 802.1AS FOLLOWUP PDU.
*
* @param    intfIndex  @b{(input)} interface index
* @param    pPdu  @b{(input)} pointer to the received PDU
*
* @returns  none
*
* @comments Basic validations for DA, ETHTYPE, Interface number have already
*           been done. Caller frees the buffer.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asFollowupReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu)
{
  int32_t  rateRatioIn;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_SYNC_INFO_t  *pRxSyncInfo;
  DOT1AS_PDU_HEADER_t *pHdr;
  DOT1AS_FOLLOWUP_PDU_t   *pFollowupPdu;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
  avbLogBuffer_t logEntry;
#endif

  pIntf = &pDot1asIntf_g[intfIndex];
  pHdr = &pPdu->hdr;
  pFollowupPdu =&pPdu->body.followUpPdu;

  switch (pIntf->syncRxState)
  {
    case DOT1AS_SYNC_RX_DISABLED:
      /* Discard FOLLOWUP if interface is not enabled */
      pIntf->stats.followupRxDiscards++;
      break;

    case DOT1AS_SYNC_RX_WAIT:
      /* Received FOLLOWUP, expecting SYNC. Discard */
      pIntf->stats.followupRxDiscards++;
      break;

    case DOT1AS_SYNC_RX_WAIT_FOLLOWUP:
      if ((pDot1asBridge_g->isGmPresent == TRUE) &&
          (pIntf->selectedRole == DOT1AS_ROLE_SLAVE) &&
          ((pDot1asBridge_g->avnuMode == TRUE) ? TRUE
                                               : (DOT1AS_CLOCK_ID_IS_EQ(&pHdr->portId.clock,&pDot1asBridge_g->parentId.clock))) &&
          ((pDot1asBridge_g->avnuMode == TRUE) ? TRUE
                                               : (pHdr->portId.num == pDot1asBridge_g->parentId.num)))
      {
        pRxSyncInfo = &pIntf->syncRxInfo;

        if ((pRxSyncInfo->lastSeqId != pHdr->sequenceId) ||
            (pRxSyncInfo->rxIntfIndex != intfIndex))
        {
          pIntf->stats.followupRxDiscards++;
        }
        else
        {
          /* Sync and its Followup Received, set the first sync received to TRUE */
          pDot1asBridge_g->isSyncReceived = TRUE;

          if (pDot1asBridge_g->avnuMode == TRUE)
          {
            pIntf->avnuSyncReadyCount++;

            if (pDot1asBridge_g->avnuGMLost == TRUE)
            {
              pDot1asBridge_g->avnuGMLost = FALSE;
              AVNU_TRACE_GM("%s: GM has recovered", __FUNCTION__);
            }

            if ((pIntf->avnuSyncReadyCount > 1) &&
                (pDot1asBridge_g->avnuDeviceState != AVB_AVNU_DEVICE_STATE_AVB_SYNC))
            {
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
              /* Add log entry */
              sal_memset(&logEntry, 0, sizeof(avbLogBuffer_t));
              logEntry.eventType = AVB_LOG_EVENT_SYNC_STATUS_CHANGE;
              logEntry.u.avbSyncStatus.syncStatusOld = pDot1asBridge_g->avnuDeviceState;
              logEntry.u.avbSyncStatus.syncStatusNew = AVB_AVNU_DEVICE_STATE_AVB_SYNC;
              avbLogBufferEntryAdd(&logEntry);
              /* set asynchronous event */
              avbAsyncEventTypeSet(AVB_GLOBAL_ASYNC_EVENTS_INDEX, AVB_ASYNC_EVENT_GLOBAL_AVB_SYNC);
              pDot1asBridge_g->avnuDeviceState = AVB_AVNU_DEVICE_STATE_AVB_SYNC;
#endif
            }
          }
          /* Note, some of the fields are copied from SYNC and the rest from
           * FOLLOWUP
           */

          pRxSyncInfo->followupCorrectionField = pHdr->correctionFld;

          sal_memcpy(&pRxSyncInfo->preciseOriginTimestamp,
                 &pFollowupPdu->preciseOriginTimestamp,
                 sizeof(pRxSyncInfo->preciseOriginTimestamp));

          /* Compute rateRatio from cumulative rate offset field. TBD */
          /* rateRatio is set equal to the quantity.
           * (cumulativeScaledRateOffset * 2^-41) + 1.0
           */
          rateRatioIn = (pFollowupPdu->gmTlv.cumulativeRateOfst >> 11) +
                         DOT1AS_RATE_RATIO_DEFAULT ; /* TBD */

          /* Check for TLV only when present, although mandatory */
          if (pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN+DOT1AS_FOLLOWUP_PDU_LEN))
          {
            pRxSyncInfo->gmTimeBaseIndicator =
                  pFollowupPdu->gmTlv.gmTimeBaseIndicator;

            /* Compute lastGmFreqChange from scaledLastGmFreqChange. TBD */
            pRxSyncInfo->lastGmFreqChange = pFollowupPdu->gmTlv.lastGmFreqChange;

            /* Any conversions required? TBD */
            sal_memcpy(pRxSyncInfo->lastGmPhaseChange,
                   pFollowupPdu->gmTlv.lastGmPhaseChange,
                   sizeof(pRxSyncInfo->lastGmPhaseChange));
          }

          /* Update local time TBD */
          if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_TRANSITION_TO_SLAVE)
          {
            dot1asSlaveClockInit(&pFollowupPdu->preciseOriginTimestamp,
                                 &pRxSyncInfo->syncRxTimestamp,
                                 pRxSyncInfo->followupCorrectionField,
                                 pIntf->neighborPropDelay);
            pDot1asBridge_g->clkState = DOT1AS_CLOCK_UPDATE_SLAVE;
          }
          else if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_UPDATE_SLAVE)
          {
            dot1asSlaveClockUpdate(&pFollowupPdu->preciseOriginTimestamp,
                                   &pRxSyncInfo->syncRxTimestamp,
                                   pRxSyncInfo->followupCorrectionField,
                                   pIntf->neighborPropDelay,
                                   rateRatioIn,
                                   pIntf->neighborRateRatio);
          }
          DOT1AS_TRACE_RX("DOT1AS PDU event: Received FOLLOWUP on port[%d]\n", avbCfgxIntfNum[intfIndex]);

          /* Send SYNC on all the Master ports of this system */
          dot1asPropagateSyncInfo(intfIndex, pRxSyncInfo);
          pIntf->followupReceiptTimerReset = TRUE;
          pIntf->syncRxState = DOT1AS_SYNC_RX_WAIT;
          /* Reset the sync receipt timer, as the sync and its followup received */
          pIntf->syncReceiptTimerReset = TRUE;
        }
      }
      else
      {
        pIntf->stats.followupRxDiscards++;
      }
      break;

    default:
      break;
  }

  return AVB_SUCCESS;
}

/*******************************************************************************
* @purpose  Propagate timesync info on all the Master ports of this system
*
* @param    rxIntfIndex   @b{(input)} DOT1AS interface on which SYNC was received
* @param    pRcvdSyncInfo @b{(input)} Received timesync information
*
* @returns  AVB_RC_t
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPropagateSyncInfo(uint32_t rxIntfIndex,
                                DOT1AS_SYNC_INFO_t *pRcvdSyncInfo)
{
  uint32_t             index = 0;
  DOT1AS_TIMESTAMP_t   syncEgrTimestamp;
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             linkStatus = AVB_PORT_LINK_DOWN;

  for (index=0; index < dot1asMaxInterfaceCount_g; index++)
  {
    pIntf = &pDot1asIntf_g[index];

    if ((index != rxIntfIndex) && (dot1asIsTimesyncCapable(index) == TRUE) &&
        (pIntf->selectedRole == DOT1AS_ROLE_MASTER))
    {
      if (pDot1asBridge_g->avnuMode == TRUE)
      {
        cbx_port_attribute_get(rxIntfIndex, cbxPortAttrLinkStatus, &linkStatus);
        if ((linkStatus != AVB_PORT_LINK_UP) || (pIntf->dot1asCapable != TRUE))
        {
          continue;
        }
      }
      /* Copy the received SYNC information from Slave port to Master port */
      sal_memcpy(&pIntf->syncRxInfo, pRcvdSyncInfo, sizeof(pIntf->syncRxInfo));
      pIntf->syncInfoAvailable = TRUE;
      /* Reset the sync RX timer on this MASTER port */
      pIntf->syncReceiptTimerReset = TRUE;
      pIntf->followupReceiptTimerReset = TRUE;

      /*The sync tx timer must be >= half the sync transmit interval */
      if (pIntf->syncTransmitTimer < pIntf->meanSyncInterval/2)
      {
        continue;
      }
      /* Send SYNC and FOLLOWUP */
      pIntf->syncTxSeqId++;

      /* Reset the sync Transmit timer, as we are about to send SYNC  */
      pIntf->syncTransmitTimerReset = TRUE;
      pIntf->syncInfoAvailable = FALSE; /* Clear when we send the sync */

      /* The delay between SYNC and FOLLOWUP has to be minimal, as the endpoints
       * are sensitive to that delay. Depending on the number of MASTER ports,
       * last MASTER port will have the worst residence time (as it incurs the
       * penalty of sending SYNC/FOLLOWUP on all previous master ports). Other
       * way is to send SYNC first on all MASTER ports and then send FOLLOWUP,
       * which reduces the residence times, but increases the delay between SYNC
       * and FOLLOWUP on a given port.
       */

      if (dot1asSyncSend(index) == AVB_SUCCESS)
      {
        if (dot1asIntfTimestampGet(index,
                                   DOT1AS_PDU_SYNC,
                                   pIntf->syncTxSeqId, 1,
                                   &syncEgrTimestamp) == AVB_SUCCESS)
        {
          /* This function uses the SYNC information from pIntf->syncRxInfo */
          dot1asFollowupSend(index, &syncEgrTimestamp);

          /* Update the last sync sent time on this interface */
          pIntf->lastSyncSentTime = syncEgrTimestamp;
        }
        else
        {
          pIntf->stats.tsErrors++;
          DOT1AS_TRACE_ERR("%s: failed reading timestamp on port[%d]\n",
                           __FUNCTION__, avbCfgxIntfNum[index]);
        }
      }
      else
      {
        DOT1AS_TRACE_ERR("%s: failed sending SYNC on port[%d]\n",
                         __FUNCTION__, avbCfgxIntfNum[index]);
      }
    }
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Send a SYNC PDU out of the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  AVB_RC_t
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asSyncSend(uint32_t intfIndex)
{
  uint8_t   *pdu = NULL;
  int32_t    interval;
  uint16_t   hdrsize = 0;
  uint16_t   flags = 0;
  DOT1AS_INTF_STATE_t *pIntf;
  uint16_t  pduLen = 0;

#if DEV_BUILD_DOT1AS
  if (avbDot1asTest_g.isStopSync)
  {
    return AVB_SUCCESS;
  }
#endif
  pIntf = &pDot1asIntf_g[intfIndex];

  /* Get a frag from ethernet driver */
  pdu = &dot1asTxBuffer[0];
  sal_memset(dot1asTxBuffer, 0, DOT1AS_MAX_PDU_SIZE);
  /* Add DA, SA, and EthType */
  if (dot1asPduAddEnetHeader(intfIndex, pdu, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* Add 802.1AS header */
  hdrsize = 0;

  /* Set the mean message interval */
  interval = pIntf->currentLogSyncInterval;

  flags = (DOT1AS_CLK_TWO_STEP_FLAG | DOT1AS_CLK_PTP_TIMESCALE_FLAG);

  /* For SYNC, correction field is set to 0. See 11.2.10.2.1 */
  if (dot1asPduAddCommonHeader(intfIndex, pdu, DOT1AS_PDU_SYNC,
                               pIntf->syncTxSeqId, 0, (int8_t)interval,
                               flags, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* SYNC PDU body has a reserved field of 10bytes each */
  sal_memset(pdu, 0x00, 10);
  pduLen += 10;

  DOT1AS_TRACE_TX("DOT1AS PDU event: Sending SYNC on port[%d]\n", avbCfgxIntfNum[intfIndex]);

  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to trasmit on port[%d]\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  /* All is good */
  pIntf->stats.syncTxCount++;

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Send a FOLLOWUP PDU out of the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
* @param    pSyncEgrHwTimestamp  @b{(input)} Egress timestamp
*
* @returns  AVB_RC_t
*
* @comments This function uses the SYNC information from pIntf->syncRxInfo,
*           which is updated either from received SYNC/FOLLOWUP PDUs on SLAVE
*           port (if this system is not GrandMaster), OR from SYNC info
*           generated by the clock master entity of this system (if this system
*           is the Grandmaster);
*
* @end
*******************************************************************************/
AVB_RC_t dot1asFollowupSend(uint32_t intfIndex,
                           DOT1AS_TIMESTAMP_t *pSyncEgrHwTimestamp)
{
  uint8_t   *pdu = NULL;
  int32_t    interval;
  uint16_t   hdrsize = 0;
  uint16_t   flags = 0;
  uint16_t   val16 = 0;
  uint32_t   val32 = 0;
  int32_t    ratio = 0;
  int64_t    correctionFld = 0;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_INTF_STATE_t *pRxIntf;
  DOT1AS_SYNC_INFO_t  *pRxSyncInfo;
  uint16_t  pduLen = 0;

#if DEV_BUILD_DOT1AS
  if (avbDot1asTest_g.isStopFollowup)
  {
    return AVB_SUCCESS;
  }
#endif

  pIntf = &pDot1asIntf_g[intfIndex];
  pRxSyncInfo = &pIntf->syncRxInfo;

  /* Get a frag from ethernet driver */
  pdu = &dot1asTxBuffer[0];
  sal_memset(dot1asTxBuffer, 0, DOT1AS_MAX_PDU_SIZE);
  /* Add DA, SA, and EthType */
  if (dot1asPduAddEnetHeader(intfIndex, pdu, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* Add 802.1AS header. Use the same SYNC seqId */
  hdrsize = 0;

  /* Calculate the correction field: sum of
   *    correctionField received on slave port +
   *    rateRatio * (syncEgressTimestamp - syncRxTime) +
   *    neighborPropDelay of the upstream port
   */
  correctionFld = pRxSyncInfo->followupCorrectionField;
  if (pRxSyncInfo->rxIntfIndex >= avbMaxIntfPhysP8_g) /* TRGTRG TODO - unsure about this */
  {
    val32 = 0; /* If this clock is GM, there is no incoming PDELAY */
  }
  else
  {
    pRxIntf = &pDot1asIntf_g[pRxSyncInfo->rxIntfIndex];
    val32 = pRxIntf->neighborPropDelay;
  }
  dot1asComputeCorrectionFld(pSyncEgrHwTimestamp, &pRxSyncInfo->syncRxTimestamp, val32, &correctionFld);

  /* Set the mean message interval */
  interval = pIntf->currentLogSyncInterval;

  flags = DOT1AS_CLK_PTP_TIMESCALE_FLAG;
  if (dot1asPduAddCommonHeader(intfIndex, pdu, DOT1AS_PDU_FOLLOWUP,
                               pIntf->syncTxSeqId, correctionFld,
                               (int8_t)interval, flags, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* FOLLOWUP PDU body */

  /* 10bytes of origin timestamp */
  dot1asPackUint48(pdu, pRxSyncInfo->preciseOriginTimestamp.seconds);
  val32 = sal_htonl(pRxSyncInfo->preciseOriginTimestamp.nanoseconds);
  sal_memcpy(pdu+6, &val32, sizeof(val32));
  pdu += DOT1AS_TIMESTAMP_LEN;
  pduLen  += DOT1AS_TIMESTAMP_LEN;

  /* TLV type, len, OID, Org Subtype */
  val16 = DOT1AS_TLV_TYPE_ORG_EXT;
  val16 = sal_htons(val16);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16);
  pduLen  += sizeof(val16);

  val16 = 28; /* TLV len */
  val16 = sal_htons(val16);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16);
  pduLen  += sizeof(val16);

  /* OrgId is 00-80-C2 */
  *pdu++  = 0x00; *pdu++  = 0x80; *pdu++  = 0xC2;
  pduLen += 3;

  /* OrgSubType is 0x000001 */
  *pdu++ = 0x00; *pdu++ = 0x00; *pdu++ = 0x01;
  pduLen += 3;

  /* Cumulative Rate offset */
  ratio = (pDot1asBridge_g->rateRatioOut - DOT1AS_RATE_RATIO_DEFAULT) << 11;
  ratio = sal_htonl(ratio);
  sal_memcpy(pdu, &ratio, sizeof(ratio));
  pdu += sizeof(ratio);
  pduLen += sizeof(ratio);

  /* GM timebase, phase, freq */
  val16 = sal_htons(pRxSyncInfo->gmTimeBaseIndicator);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16);
  pduLen += sizeof(val16);

  sal_memcpy(pdu, pRxSyncInfo->lastGmPhaseChange, sizeof(pRxSyncInfo->lastGmPhaseChange));
  pdu += 12;
  pduLen += 12;

  val32 = sal_htonl(pRxSyncInfo->lastGmFreqChange);
  sal_memcpy(pdu, &val32, sizeof(val32));
  pduLen += sizeof(val32);

  DOT1AS_TRACE_TX("DOT1AS PDU event: Sending FOLLOWUP on port[%d]\n", avbCfgxIntfNum[intfIndex]);

  /* Send the PDU */
  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to trasmit on port[%d]\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }
  /* All is good */
  pIntf->stats.followUpTxCount++;

  dot1asDbgFollowupT2 = 1000 * sal_time_usecs();
#ifdef SHELL
  dot1asDebugCorrectionFld(&pRxSyncInfo->syncRxTimestamp,
                           pRxSyncInfo->followupCorrectionField,
                           pSyncEgrHwTimestamp, correctionFld,
                           &pRxSyncInfo->preciseOriginTimestamp);
#endif

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Check Timesync MASTER timers on the given interface
*
* @param    none
*
* @returns  none
*
* @comments Called every timer tick interval, for every master port
*
* @end
*******************************************************************************/
void dot1asTimesyncMasterTimers(uint32_t intfIndex)
{
  uint8_t   intervalExpired = FALSE;
  uint8_t   isSlave = FALSE;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_TIMESTAMP_t  syncEgrTimestamp;
  uint64_t txTimeoutIntl;
  uint32_t linkStatus = AVB_PORT_LINK_DOWN;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
  avbLogBuffer_t logEntry;
#endif

  pIntf = &pDot1asIntf_g[intfIndex];

  /* Check if the SYNC transmit interval expired */
  if (pIntf->syncTransmitTimerReset == TRUE)
  {
    /* Sync info was already propagated on this Master port, either up on
     * reception of SYNC on SLAVE port (if this system is not Grandmaster),
     * or expiry of the master SYNC timer
     */
    pIntf->syncTransmitTimerReset = FALSE;
    pIntf->syncTransmitTimer = 0;
  }
  else
  {
    pIntf->syncTransmitTimer += DOT1AS_TIMER_TICK_US;
    txTimeoutIntl = pIntf->meanSyncInterval;

    /* BMCA is disabled in AVnu mode so use static configuration for clock state */
    if ((pDot1asBridge_g->adminMode) &&
        (pDot1asBridge_g->isGmPresent == TRUE) &&
        ((pDot1asBridge_g->avnuMode == TRUE) ? (pDot1asBridge_g->clkState == DOT1AS_AVNU_CLOCK_STATE_SLAVE)
                                             : (DOT1AS_CLOCK_ID_IS_NOT_EQ(&pDot1asBridge_g->gmClock, &pDot1asBridge_g->thisClock))))
    {
       isSlave = TRUE;
    }

    intervalExpired = (pIntf->syncTransmitTimer >= txTimeoutIntl) ? TRUE:FALSE;

    /* If transmit interval expired we'll send a SYNC whether we received a SYNC or not */
    if (intervalExpired == TRUE)
    {
      /* Reset timer */
      pIntf->syncTransmitTimer = 0;

      if (pDot1asBridge_g->avnuMode == TRUE)
      {
        cbx_port_attribute_get(intfIndex, cbxPortAttrLinkStatus, &linkStatus);
        if ((linkStatus != AVB_PORT_LINK_UP) || (pIntf->dot1asCapable != TRUE))
        {
          return;
        }
      }

      /* If we are the GM we need to send syncs. If we are a slave device we do not want to send syncs unless we have
         received at least one sync and the absence timer has expired. If the absence timer expires and we never
         received a sync the avnuGMLost flag will be TRUE */
      if ((isSlave != TRUE) || !((pDot1asBridge_g->avnuGMLost == FALSE) && (pDot1asBridge_g->isSyncReceived == FALSE)))
      {
        if (isSlave == TRUE)
        {
          /* If no SYNC since last transmit, bump the counter */
          if (pIntf->syncInfoAvailable == FALSE)
          {
            pIntf->stats.syncTransmitTimeouts++;
          }
        }

        /* Clear the syncInfoAvailable flag as we are about to send SYNC */
        pIntf->syncInfoAvailable = FALSE;

        pIntf->syncTxSeqId++;

        /* Send SYNC and FOLLOWUP with old timesync information */
        if (dot1asSyncSend(intfIndex) == AVB_SUCCESS)
        {
          /* Get the egress timestamp */
          if (dot1asIntfTimestampGet(intfIndex,
                                     DOT1AS_PDU_SYNC,
                                     pIntf->syncTxSeqId, 1,
                                     &syncEgrTimestamp) == AVB_SUCCESS)
          {
            /* BMCA is disabled in AVnu mode so use static configuration for clock state */
            if (((pDot1asBridge_g->adminMode) &&
                (pDot1asBridge_g->isGmPresent == TRUE) &&
                ((pDot1asBridge_g->avnuMode == TRUE) ? (pDot1asBridge_g->clkState == DOT1AS_AVNU_CLOCK_STATE_GM)
                                                     : (DOT1AS_CLOCK_ID_IS_EQ(&pDot1asBridge_g->gmClock, &pDot1asBridge_g->thisClock)))))

            {
              /* If we are the GM update the clock each time SYNC is sent */
              dot1asMasterClockUpdate(&syncEgrTimestamp);

              /* Set up SYNC information to send followup */
              pIntf->syncRxInfo.followupCorrectionField = 0x00;
              pIntf->syncRxInfo.rxIntfIndex = avbMaxIntfPhysP8_g; /* TRGTRG */
              pIntf->syncRxInfo.syncRxTimestamp = syncEgrTimestamp;
              dot1asConvertHwTimestamp(&syncEgrTimestamp,
                                       &pIntf->syncRxInfo.preciseOriginTimestamp,
                                       DOT1AS_RATE_RATIO_DEFAULT);
              pIntf->syncReceiptTimerReset = TRUE;
              pIntf->followupReceiptTimerReset = TRUE;
              if (pDot1asBridge_g->avnuMode == TRUE)
              {
                pIntf->avnuSyncReadyCount++;
                if ((pIntf->avnuSyncReadyCount > 1) &&
                    (pDot1asBridge_g->avnuDeviceState != AVB_AVNU_DEVICE_STATE_AVB_SYNC))
                {
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
                  pDot1asBridge_g->avnuDeviceState = AVB_AVNU_DEVICE_STATE_AVB_SYNC;
                  /* Add log entry */
                  sal_memset(&logEntry, 0, sizeof(avbLogBuffer_t));
                  logEntry.eventType = AVB_LOG_EVENT_SYNC_STATUS_CHANGE;
                  logEntry.u.avbSyncStatus.syncStatusOld = AVB_AVNU_DEVICE_STATE_ETH_READY;
                  logEntry.u.avbSyncStatus.syncStatusNew = AVB_AVNU_DEVICE_STATE_AVB_SYNC;
                  avbLogBufferEntryAdd(&logEntry);
                  /* set asynchronous event */
                  avbAsyncEventTypeSet(AVB_GLOBAL_ASYNC_EVENTS_INDEX, AVB_ASYNC_EVENT_GLOBAL_AVB_SYNC);
#endif
                }
              }
            }
            else if ((pDot1asBridge_g->avnuGMLost == TRUE) && (pDot1asBridge_g->clkState == DOT1AS_AVNU_CLOCK_STATE_SLAVE))
            {
              /* Act as proxy GM incase of operational/startup GM loss  to the remaining network */
              int64_t avnuFollowUpCorrectionFieldOnGMLoss = 0;
              uint64_t correctionSeconds = 0;
              int32_t correctionNanoSeconds = 0;
              DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;

              /* Use default rate ratio to compute correction field in case of operational GM loss */
              if (pDot1asBridge_g->isSyncReceived == FALSE)
              {
                pDot1asBridge_g->rateRatioOut = DOT1AS_RATE_RATIO_DEFAULT;
              }
              /*Calculate correction factor on GM loss in AVnu mode*/
              dot1asComputeCorrectionFld(&syncEgrTimestamp, &pIntf->syncRxInfo.syncRxTimestamp, 0, &avnuFollowUpCorrectionFieldOnGMLoss);
              correctionSeconds = (avnuFollowUpCorrectionFieldOnGMLoss >> 16)/ONE_SEC;
              correctionNanoSeconds = (int32_t)((avnuFollowUpCorrectionFieldOnGMLoss >> 16)%ONE_SEC);
              /* Local Time update */
              pLocalTime->seconds = pIntf->syncRxInfo.preciseOriginTimestamp.seconds;
              pLocalTime->seconds += correctionSeconds;
              pLocalTime->seconds += pIntf->syncRxInfo.preciseOriginTimestamp.nanoseconds/ONE_SEC;
              pLocalTime->nanoseconds = pIntf->syncRxInfo.preciseOriginTimestamp.nanoseconds%ONE_SEC;
              pLocalTime->nanoseconds += correctionNanoSeconds;
              if ( pLocalTime->nanoseconds > ONE_SEC)
              {
                pLocalTime->seconds++;
                pLocalTime->nanoseconds -= ONE_SEC;
              }
              pIntf->syncReceiptTimerReset = FALSE;
              pIntf->followupReceiptTimerReset = FALSE;
            }
            dot1asFollowupSend(intfIndex, &syncEgrTimestamp);
            pIntf->lastSyncSentTime = syncEgrTimestamp; /* Update last sync sent */
          }
          else
          {
            pDot1asIntf_g[intfIndex].stats.tsErrors++;
            DOT1AS_TRACE_ERR("%s: failed reading timestamp on port[%d]\n",
                              __FUNCTION__, avbCfgxIntfNum[intfIndex]);
          }
        }
      }
    }
  }
}


/******************************************************************************
* @purpose  Check Timesync SLAVE timers on the given interface
*
* @param    none
*
* @returns  none
*
* @comments Called every timer tick interval, only if port role is SLAVE
*
* @end
*******************************************************************************/
void dot1asTimesyncSlaveTimers(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
  avbLogBuffer_t logEntry;
#endif

  pIntf = &pDot1asIntf_g[intfIndex];

  /* Check if SYNC receipt interval expired */
  if (pIntf->syncReceiptTimerReset == TRUE)
  {
    pIntf->syncReceiptTimerReset = FALSE;
    pIntf->syncReceiptTimer = 0ULL;
  }
  else if ((pIntf->syncRxState == DOT1AS_SYNC_RX_WAIT) || (pIntf->syncRxState == DOT1AS_SYNC_RX_WAIT_FOLLOWUP))
  {
    if ((pDot1asBridge_g->avnuMode == TRUE) &&
        (pDot1asBridge_g->clkState == DOT1AS_AVNU_CLOCK_STATE_SLAVE) &&
        (pDot1asBridge_g->isSyncReceived == FALSE) &&
        (pDot1asBridge_g->avnuSyncAbsenceTimer < pDot1asBridge_g->avnuSyncAbsenceTimeout))
    {
      /* increment sync absence timer until we receive sync packets on this bridge */
      pDot1asBridge_g->avnuSyncAbsenceTimer += DOT1AS_TIMER_TICK_US;
    }

    pIntf->syncReceiptTimer += DOT1AS_TIMER_TICK_US;
    if (pIntf->syncReceiptTimer >= pIntf->syncReceiptTimeoutInterval)
    {
      pIntf->stats.syncReceiptTimeouts++;
      pIntf->syncReceiptTimer = 0ULL;

      /* Determine GM loss in avnu mode if sync rx count is not
         zero on slave port of this device, syncRxTimedOut is true
         and AVnu mode is enabled */
      if ((pDot1asBridge_g->avnuMode == TRUE) && (pDot1asBridge_g->clkState == DOT1AS_AVNU_CLOCK_STATE_SLAVE))
      {
        if (pDot1asBridge_g->avnuDeviceState != AVB_AVNU_DEVICE_STATE_SYNC_RECEIPT_TIMEOUT)
        {
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
           /* Add log entry */
          sal_memset(&logEntry, 0, sizeof(avbLogBuffer_t));
          logEntry.eventType = AVB_LOG_EVENT_SYNC_STATUS_CHANGE;
          logEntry.u.avbSyncStatus.syncStatusOld = pDot1asBridge_g->avnuDeviceState;
          logEntry.u.avbSyncStatus.syncStatusNew = AVB_AVNU_DEVICE_STATE_SYNC_RECEIPT_TIMEOUT;
          avbLogBufferEntryAdd(&logEntry);
          pDot1asBridge_g->avnuDeviceState = AVB_AVNU_DEVICE_STATE_SYNC_RECEIPT_TIMEOUT;
          /* set asynchronous event */
          avbAsyncEventTypeSet(AVB_GLOBAL_ASYNC_EVENTS_INDEX, AVB_ASYNC_EVENT_GLOBAL_SYNC_RECEIPT_TIMEOUT);
#endif
        }
        if (pDot1asBridge_g->isSyncReceived == TRUE)
        {
          if (pDot1asBridge_g->avnuGMLost == FALSE)
          {
            pDot1asBridge_g->avnuGMLost = TRUE;
            pIntf->avnuSyncReadyCount = 0;
            AVNU_TRACE_GM("%s: Port[%d] GM Lost", __FUNCTION__, avbCfgxIntfNum[intfIndex]);
          }
        }
        else
        {
          if (pDot1asBridge_g->avnuSyncAbsenceTimer >= pDot1asBridge_g->avnuSyncAbsenceTimeout)
          {
            if (pDot1asBridge_g->avnuGMLost == FALSE)
            {
              pDot1asBridge_g->avnuGMLost = TRUE;
              AVNU_TRACE_GM("%s: GM Absent on Startup", __FUNCTION__);
              pDot1asBridge_g->avnuSyncAbsenceOnStartUp = TRUE;
            }
          }
        }
      }
      /* Loss of SYNC information implies GM is lost. Invoke BMCA. */
      dot1asInvokeBmca(intfIndex, DOT1AS_BMC_SYNC_TIMEOUT, NULL);
    }
  }

  if (pIntf->followupReceiptTimerReset == TRUE)
  {
    pIntf->followupReceiptTimerReset = FALSE;
    pIntf->followupReceiptTimer = 0;
  }
  else if (pIntf->syncRxState == DOT1AS_SYNC_RX_WAIT_FOLLOWUP)
  {
    /* Check if FOLLOWUP receipt interval expired */

    /* FOLLOWUP receipt timeout is one SYNC interval. */
    pIntf->followupReceiptTimer += DOT1AS_TIMER_TICK_US;
    if (pIntf->followupReceiptTimer >= pIntf->followupReceiptTimeoutInterval)
    {
      pIntf->followupReceiptTimer = 0;
      pIntf->stats.followupReceiptTimeouts++;
      pIntf->stats.ptpDiscardCount++;

      /* BCMA is not invoked here, as SYNC timeout has not yet occured */
      pIntf->syncRxState = DOT1AS_SYNC_RX_WAIT;
    }
  }
}


/******************************************************************************
* @purpose  Check Timesync timers on each of the interfaces
*
* @param    none
*
* @returns  none
*
* @comments Called every timer tick interval
*
* @end
*******************************************************************************/
void dot1asCheckTimesyncTimers(void)
{
  uint32_t intfIndex = 0;
  DOT1AS_INTF_STATE_t *pIntf;

  /* For each SLAVE/MASTER port check the SYNC receive and transmit timers */
  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];

    /* Check if TimeSync is enabled on the interface */
    if (dot1asIsTimesyncCapable(intfIndex) != TRUE)
    {
      continue;
    }
    else
    {
      /* Initialize the Timesync state as required, to accomodate
       * configuration or link status changes
       */
      if (pIntf->syncRxState == DOT1AS_SYNC_RX_DISABLED)
      {
        dot1asIntfTimesyncInit(intfIndex);
      }
    }

    /* Check Timers for SLAVE ports */
    if ((pDot1asBridge_g->isGmPresent == TRUE) &&
        (pIntf->selectedRole == DOT1AS_ROLE_SLAVE))
    {
      dot1asTimesyncSlaveTimers(intfIndex);
    }

    /* Check Timers for MASTER ports */
    /* if a GM is present and the port role is Master */
    if ((pDot1asBridge_g->isGmPresent == TRUE) &&
        (pIntf->selectedRole == DOT1AS_ROLE_MASTER))
    {
      dot1asTimesyncMasterTimers(intfIndex);
    }
  }
}


/******************************************************************************
* @purpose  Adjust/Convert local timestamp based on the local time
*
* @param    pTimestamp  @b{(input)} timestamp as provided by the hardware
* @param    pConvertedTS @b{(output)} adjusted/converted DOT1AS timestamp
* @param    rateRatio @b{(input)} rate ratio
*
* @returns  none
*
* @comments none
*
* @end
*******************************************************************************/
void dot1asConvertHwTimestamp(DOT1AS_TIMESTAMP_t *pTimestamp,
                              DOT1AS_TIMESTAMP_t *pConvertedTS,
                              int32_t rateRatio)
{
  int64_t t2;
  int64_t t3;
  int32_t ns;
  DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;

  /* Adjust HW timestamps to compensate for Freq Offset */
  /* t2 can be positive or negative! */
  t2 = dot1asSubExtTimestamps(pTimestamp, &pDot1asBridge_g->prevRxTimestamp);

  t3 = t2 * rateRatio;

  ns = pLocalTime->nanoseconds + (int32_t)(SHIFTROUND(t3,30));

  pConvertedTS->seconds = pLocalTime->seconds;

  if (ns > ONE_SEC)
  {
    ns -= ONE_SEC;
    pConvertedTS->seconds++;
  }
  else if (ns < 0)
  {
    ns += ONE_SEC;
    pConvertedTS->seconds--;
  }

  pConvertedTS->nanoseconds = ns; /* ns is +ve here */
}


/******************************************************************************
* @purpose  Calculate correction field in the FOLLOWUP PDU
*
* @param  pEgrTimestamp    @b{(input)} Raw egress timestamp of SYNC PDU
* @param  pSyncRxTimeStamp @b{(input)} Last received SYNC PDU timestamp
* @param  pdelay           @b{(input)} Propagation delay of outgoing port
* @param  pCorrection      @b{(output)}Correction field of FOLLOWUP PDU
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asComputeCorrectionFld(DOT1AS_TIMESTAMP_t *pEgrTimestamp,
                                DOT1AS_TIMESTAMP_t *pSyncRxTimeStamp,
                                uint64_t            pDelay,
                                int64_t            *pCorrection)
{
  int64_t t1;
  int64_t lower32, upper32;

  t1 = dot1asSubExtTimestamps(pEgrTimestamp, pSyncRxTimeStamp);
  /* ------------------------------------------------------------------------
     In AVnu mode we need to continue sending syncs even in the case where we
     have lost the GM. In this case the correction field can become very
     large so we need to break it into an upper and lower half in order to
     apply the rate ratio without having overflow.
     ------------------------------------------------------------------------ */
  lower32 = t1 & ((1LL << 32) - 1);
  upper32 = (t1 >> 32);
  lower32 = lower32 * pDot1asBridge_g->rateRatioOut;
  upper32 = upper32 * pDot1asBridge_g->rateRatioOut;
  lower32 = SHIFTROUND(lower32, 14);
  upper32 = (upper32 << 18);
  *pCorrection += (upper32 + lower32 + ((uint32_t)pDelay << 16));

#if 0
  /* Prior to AVnu mode support, the calculation was much simpler */
  int64_t t2;

  t1 = dot1asSubExtTimestamps(pEgrTimestamp, &pDot1asBridge_g->prevRxTimestamp);
  t2 = t1 * pDot1asBridge_g->rateRatioOut;
  *pCorrection += (SHIFTROUND(t2,14) +
                   ((uint32_t)pDelay << 16));
#endif
}


/******************************************************************************
* @purpose  Init the local time of this system based on the SYNC/FOLLOWUP PDU
*           received from Grandmaster.
*
* @param    pGmTimestamp      @b{(input)} Precise origin timestamp in FOLLOWUP
* @param    pSyncRxTimestamp  @b{(input)} Local ingress timestamp of SYNC PDU
* @param    correction        @b{(input)} Correction field in the FOLLOWUP
* @param    pDelay            @b{(input)} Propagation delay on incoming link
*
* @returns  AVB_RC_t
*
* @comments Invoked once on every transition to SLAVE mode.
*
* @end
*******************************************************************************/
void dot1asSlaveClockInit(DOT1AS_TIMESTAMP_t *pGmTimestamp,
                          DOT1AS_TIMESTAMP_t *pSyncRxTimestamp,
                          int64_t            correction,
                          uint64_t           pDelay)
{
  DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;

  pDot1asBridge_g->prevRxTimestamp = *pSyncRxTimestamp;
  pLocalTime->nanoseconds = pGmTimestamp->nanoseconds;
  pLocalTime->seconds = pGmTimestamp->seconds;

  pLocalTime->nanoseconds += (uint32_t)pDelay;
  pLocalTime->nanoseconds += (int32_t)SHIFTROUND(correction, 16);

  if (pLocalTime->nanoseconds > ONE_SEC)
  {
    pLocalTime->nanoseconds -= ONE_SEC;
    pLocalTime->seconds++;
  }
}



/******************************************************************************
* @purpose  Update the local time of this system based on the SYNC/FOLLOWUP PDU
*           received from Grandmaster.
*
* @param    pGmTimestamp      @b{(input)} Precise origin timestamp in FOLLOWUP
* @param    pSyncRxTimestamp  @b{(input)} Local ingress timestamp of SYNC PDU
* @param    correction        @b{(input)} Correction field in the FOLLOWUP
* @param    pDelay            @b{(input)} Propagation delay on incoming link
# @param    rateRatioIn       @b{(input)} Cumulative rate ratio
# @param    neighborRateRatio @b{(input)} Rate ratio on the incoming link
*
* @returns  AVB_RC_t
*
* @comments Invoked on every SYNC/FOLLOWUP message received from the GM
*
* @end
*******************************************************************************/
void dot1asSlaveClockUpdate(DOT1AS_TIMESTAMP_t *pGmTimestamp,
                            DOT1AS_TIMESTAMP_t *pSyncRxTimestamp,
                            int64_t            correction,
                            uint64_t           pDelay,
                            int32_t            rateRatioIn,
                            int32_t            neighborRateRatio)
{
  DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;
  uint64_t correctionSeconds = 0;
  int32_t correctionNanoSeconds = 0;

  pDot1asBridge_g->rateRatioIn  = rateRatioIn;
  pDot1asBridge_g->rateRatioOut = UUMULT(rateRatioIn, neighborRateRatio) >> 30;
  correctionSeconds = (correction >> 16)/ONE_SEC;
  correctionNanoSeconds = (int32_t)(correction >> 16)%ONE_SEC;

  /* Local Time update */
  pLocalTime->nanoseconds = pGmTimestamp->nanoseconds;
  pLocalTime->seconds = pGmTimestamp->seconds;
  pLocalTime->seconds += correctionSeconds;
  pLocalTime->nanoseconds += (uint32_t)pDelay;
  pLocalTime->nanoseconds += correctionNanoSeconds;

  if (pLocalTime->nanoseconds > ONE_SEC)
  {
    pLocalTime->nanoseconds -= ONE_SEC;
    pLocalTime->seconds++;
  }

#if (DOT1AS_DEBUG_TIMESYNC_LOG == 1)
  dot1asDebugTimesyncLog(&pDot1asBridge_g->prevRxTimestamp, pSyncRxTimestamp,
                         pGmTimestamp->seconds, pGmTimestamp->nanoseconds,
                         pLocalTime->seconds, pLocalTime->nanoseconds,
                         correction, rateRatioIn, pDot1asBridge_g->rateRatioOut);
#endif /* DOT1AS_DEBUG_TIMESYNC_LOG */

  pDot1asBridge_g->prevRxTimestamp = *pSyncRxTimestamp;
}

/******************************************************************************
* @purpose  Init local time information based on the current timestamp
*           available.
*
* @param    DOT1AS_TIMESTAMP_t pSyncRxTimestamp - SYNC receive timestamp
*
* @returns  none
*
* @comments Invoked only when this system is the Grand Master.
*           See "Switch and Endpoint 802.1AS Software algorithms" document.
*
* @end
*******************************************************************************/
void dot1asMasterClockInit(DOT1AS_TIMESTAMP_t *pSyncRxTimestamp)
{
  DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;
  cbx_ts_time_t tod;
  uint64 timestamp;

  if(cbx_ts_tod_get(&tod, &timestamp) == 0)
  {
    pLocalTime->seconds = tod.sec;
    pLocalTime->nanoseconds = tod.nsec;
  }
  pDot1asBridge_g->prevRxTimestamp = *pSyncRxTimestamp;
}

/******************************************************************************
* @purpose  Update local time information based on the current egress timestamp
*           available.
*
* @param    DOT1AS_TIMESTAMP_t pSyncRxTimestamp - SYNC receive timestamp
*
* @returns  none
*
* @comments Invoked only when this system is the Grand Master.
*           See "Switch and Endpoint 802.1AS Software algorithms" document.
*
* @end
*******************************************************************************/
void dot1asMasterClockUpdate(DOT1AS_TIMESTAMP_t *pSyncRxTimestamp)
{
  DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;

  if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_INIT_GM)
  {
    dot1asMasterClockInit(pSyncRxTimestamp);
    pDot1asBridge_g->clkState = DOT1AS_CLOCK_UPDATE_GM;
    return;
  }
  else if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_TRANSITION_TO_GM)
  {
    pDot1asBridge_g->clkState = DOT1AS_CLOCK_UPDATE_GM;
    /* Fall through */
  }
  else if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_UPDATE_GM)
  {
    /* Fall through */
  }
  else
  {
    if ((pDot1asBridge_g->clkState == DOT1AS_CLOCK_UPDATE_SLAVE) && (pDot1asBridge_g->avnuMode == TRUE) &&
        (pDot1asBridge_g->avnuSyncAbsenceTimer >= pDot1asBridge_g->avnuSyncAbsenceTimeout))
    {
      /* In case of Sync Absence Timed Out update the local clock on slave device*/
    }
    else
    {
      return;/* other states are not applicable while in GM mode*/
    }
  }

  pLocalTime->seconds = pSyncRxTimestamp->seconds;
  pLocalTime->nanoseconds = pSyncRxTimestamp->nanoseconds;

  /* Use updated SwitchLocalTime to generate FOLLOWUP message */
  pDot1asBridge_g->prevRxTimestamp = *pSyncRxTimestamp;
}

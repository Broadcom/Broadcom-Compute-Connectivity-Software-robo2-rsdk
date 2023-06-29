/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_sig.c
 */

#include "dot1as.h"
#include "dot1as_cfgx.h"
#include "avb.h"

/*******************************************************************************
* @purpose  Check whether interface is enabled for transmitting/receiving
*           SIGNALING PDU
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  uint8_t - TRUE if interface is enabled, else FALSE
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIsSignalingCapable(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  /* 802.1AS Global admin mode must be enabled,
   * 802.1AS Interface admin mode must be enabled,
   * Interface must be active,
   * See LinkDelaySyncIntervalSetting and AnnounceIntervalSetting state machine.
   */
  if ((pDot1asBridge_g->adminMode == DOT1AS_ENABLE) &&
      (pIntf->pttPortEnabled == DOT1AS_ENABLE))
  {
    return TRUE;
  }

  return FALSE;
}


/******************************************************************************
* @purpose  Initialize Signaling state settings
*
* @param    none
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asSignalingStateInit(void)
{
  pDot1asBridge_g->avnuSignalingTimer = DOT1AS_AVNU_SIGNALING_TIMER_STOP;
  pDot1asBridge_g->avnuSigSyncLogInterval = DOT1AS_SIGNALING_MSG_INT_INVALID;
  pDot1asBridge_g->avnuSigSyncRxCount = 0;
  pDot1asBridge_g->avnuSigSyncAdjustTimeout = FALSE;
}


/******************************************************************************
* @purpose  Initialize Signaling state on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntfSignalingInit(uint32_t intfIndex)
{
  AVB_RC_t             rc;
  DOT1AS_INTF_STATE_t *pIntf;
  uint16_t             port = avbCfgxIntfNum[intfIndex];
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t             lagCfgIntf;
#endif
  int8_t               val;

  pIntf = &pDot1asIntf_g[intfIndex];

  pIntf->signalingSeqId = 0x7327; /* TBD */

  /* Initialize the interval values of PDELAY/SYNC/ANNOUNCE for the interface */

#ifdef DOT1AS_LAG_SUPPORT
  if (avb_lag_intf_for_member_port(intfIndex, &lagCfgIntf) == AVB_SUCCESS)
  {
    port = avbCfgxIntfNum[lagCfgIntf];
  }

  if (pDot1asBridge_g->avnuMode == TRUE)
  {
    rc = dot1asCfgxIntfAVNUInitialLogSyncIntervalGet(port, &val);
  }
  else
  {
    rc = dot1asCfgxIntfInitialLogSyncIntervalGet(port, &val);
  }
#else
    rc = dot1asCfgxIntfInitialLogSyncIntervalGet(port, &val);
#endif
  if (rc == AVB_SUCCESS)
  {
    rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_SYNC_TX_INTERVAL, val);
  }
  if (rc != AVB_SUCCESS)
  {
    DOT1AS_TRACE_SIG("%s: Failed to set sync interval\n", __FUNCTION__);
  }
  if (pDot1asBridge_g->avnuMode == TRUE)
  {
    if (pIntf->avnuOperationalPdelayTimer >= AVNU_PDELAY_OPERATIONAL_CUTOVER_TIME)
    {
      rc = dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalGet(port, &val);
    }
    else
    {
      rc = dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet(port, &val);
    }
  }
  else
  {
    rc = dot1asCfgxIntfInitialLogNbrPDelayIntervalGet(port, &val);
  }

  if (rc == AVB_SUCCESS)
  {
    rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_PDELAY_TX_INTERVAL, val);
  }
  if (rc != AVB_SUCCESS)
  {
    DOT1AS_TRACE_SIG("%s: Failed to set link delay interval\n", __FUNCTION__);
  }

  rc = dot1asCfgxIntfInitialLogAnnounceIntervalGet(port, &val);
  if (rc == AVB_SUCCESS)
  {
    rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL, val);
  }
  if (rc != AVB_SUCCESS)
  {
    DOT1AS_TRACE_SIG("%s: Failed to set announce interval\n", __FUNCTION__);
  }
}


/******************************************************************************
* @purpose  Round off the given 64-bit microsec value to the nearest multiple of
*           timer interval
*
* @param    us @b{(output)} value of the interval to be adjusted
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntervalUsecAdjust(uint64_t *us)
{
  uint64_t val;
  uint64_t tick;
  uint64_t diff;

  val = *us;
  tick = DOT1AS_TIMER_TICK_US;
  diff = val % tick;
  if ((diff > (tick/2)) && ((val + tick) > val))
  {
    val = (val) + (tick - diff);
  }
  else
  {
    val = (val - diff);
  }

  *us = val;
}

/******************************************************************************
* @purpose  Function to update the PDELAY, SYNC and ANNOUNCE transmit and receipt
*           timeout intervals.
*
* @param    intfIndex @b{(input)} Internal Interface number
* @param    type @b{(input)} DOT1AS interval type
* @param    value @b{(input)} interval/timeout value
*
* @returns  AVB_RC_t
*
* @notes    Interval is specified in logarithm base 2 format. Timeout is
*           specified in number of transmission intervals.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntervalUpdate(uint32_t intfIndex, int32_t type, int32_t value)
{
  uint64_t  interval = 0, timeout = 0;
  DOT1AS_INTF_STATE_t *pIntf;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t  lagCfgIntf;
#endif
  uint16_t  port;
  uint8_t   intfAnnounceReceiptTimeout;
  uint8_t   intfSyncReceiptTimeout;

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
  if (dot1asCfgxIntfAnnounceReceiptTimeoutGet(port, &intfAnnounceReceiptTimeout) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to get announce receipt timeout for intf = %u\n", __FUNCTION__, port);
    intfAnnounceReceiptTimeout = DOT1AS_INTF_ANN_TIMEOUT_DEFAULT;
  }
  if (dot1asCfgxIntfSyncReceiptTimeoutGet(port, &intfSyncReceiptTimeout) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to get sync receipt timeout for intf = %u\n", __FUNCTION__, port);
    intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
  }

  if ((type == DOT1AS_CURRENT_SYNC_TX_INTERVAL) ||
      (type == DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL) ||
      (type == DOT1AS_CURRENT_PDELAY_TX_INTERVAL))
  {
    if (dot1asIntervalLog2Usec(value, &interval) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
    /* Adjust interval to timer tick */
    dot1asIntervalUsecAdjust(&interval);
  }

  /* This function only updates the current interval */
  switch (type)
  {
    case DOT1AS_CURRENT_SYNC_TX_INTERVAL:
      pIntf->meanSyncInterval = interval;
      pIntf->currentLogSyncInterval = value;

      /* Set SYNC receipt timeout to 'n' times the SYNC interval. This will
       * be the initial value of receipt timeout. Once SYNC is received, the
       * timeout will be updated based on the logMessageInterval in the PDU
       */
      interval = interval * intfSyncReceiptTimeout;
      pIntf->syncReceiptTimeoutInterval = interval;
      pIntf->followupReceiptTimeoutInterval = pIntf->meanSyncInterval;
      pIntf->syncReceiptTimerReset = TRUE;
      pIntf->syncTransmitTimerReset = TRUE;
      pIntf->syncInfoAvailable = FALSE;
#if 0
      DOT1AS_TRACE_SIG("%s: Adjusted sync tx to %llu mSec for port %d\n", __FUNCTION__, pIntf->meanSyncInterval/1000, avbCfgxIntfNum[intfIndex]);
      DOT1AS_TRACE_SIG("%s: Adjusted sync rx timeout to %llu mSec for port %d\n", __FUNCTION__, pIntf->syncReceiptTimeoutInterval/1000, avbCfgxIntfNum[intfIndex]);
#endif
      break;

    case DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL:
      pIntf->meanAnnounceInterval = interval;
      pIntf->currentLogAnnounceInterval = value;

      /* Set ANNOUNCE receipt timeout to 'n' times the ANNOUNCE interval
       * This will be the initial value of receipt timeout. Once ANNOUNCE
       * is received, the timeout will be updated based on the logMessageInterval
       * in the PDU
       */
      interval = interval * intfAnnounceReceiptTimeout;
      pIntf->announceReceiptTimeoutInterval = interval;
      pIntf->announceTransmitTimerReset = TRUE;
      pIntf->announceReceiptTimerReset = TRUE;
#if 0
      DOT1AS_TRACE_SIG("%s: Adjusted announce to %llu mSec for port %d\n", __FUNCTION__, pIntf->meanAnnounceInterval/1000, avbCfgxIntfNum[intfIndex]);
      DOT1AS_TRACE_SIG("%s: Adjusted sync rx timeout to %llu mSec for port %d\n", __FUNCTION__, pIntf->announceReceiptTimeoutInterval/1000, avbCfgxIntfNum[intfIndex]);
#endif
      break;

    case DOT1AS_CURRENT_PDELAY_TX_INTERVAL:
      /* PDELAY receipt timeout is same as PDELAY transmit interval.
       * There are no separate timers for transmit and receipt.
       */
      pIntf->currentLogPdelayReqInterval = value;
      pIntf->meanPdelayReqInterval = interval;
      pIntf->pDelayIntervalTimerReset = TRUE;
#if 0
      DOT1AS_TRACE_SIG("%s: Adjusted pDelay to %llu mSec for port %d\n", __FUNCTION__, pIntf->meanPdelayReqInterval/1000, avbCfgxIntfNum[intfIndex]);
#endif
      break;

    case DOT1AS_SYNC_RX_TIMEOUT:
      /* Note that the cmd param is the number of intervals in this case */
      interval = pIntf->meanSyncInterval;
      timeout = interval * value;
      /* This timeout can change based on the incoming logMessageInterval */
      pIntf->syncReceiptTimeoutInterval = timeout;
      pIntf->syncReceiptTimerReset = TRUE;
      pIntf->syncTransmitTimerReset = TRUE;
#if 0
      DOT1AS_TRACE_SIG("%s: Adjusted sync rx timeout to %llu mSec for port %d\n", __FUNCTION__, timeout/1000, avbCfgxIntfNum[intfIndex]);
#endif
      break;

    case DOT1AS_ANNOUNCE_RX_TIMEOUT:
      /* Note that the cmd param is the number of intervals in this case */
      interval = pIntf->meanAnnounceInterval;
      timeout = interval * value;
      /* This timeout can change based on the incoming logMessageInterval */
      pIntf->announceReceiptTimeoutInterval = timeout;
      pIntf->announceReceiptTimerReset = TRUE;
      break;

    default:
      return AVB_FAILURE;
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Process the received 802.1AS SIGNALING PDU.
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
AVB_RC_t dot1asSignalingReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu)
{
  DOT1AS_INTF_STATE_t                *pIntf;
  DOT1AS_SIGNALING_PDU_t             *pSigPdu;
  DOT1AS_SIGNALING_MSG_INT_REQ_TLV_t *pTlv;
  AVB_RC_t                            rc = AVB_SUCCESS;
  uint16_t                            port;
  int8_t                              initialLogAnnounceInterval;
  int8_t                              initialLogSyncInterval;
  int8_t                              initialLogPDelayInterval;
  uint64_t                            prevMeanSyncInterval;
  uint8_t                             clockState = DOT1AS_CLOCK_UPDATE_SLAVE;

  pIntf = &pDot1asIntf_g[intfIndex];
  port  = avbCfgxIntfNum[intfIndex];
  prevMeanSyncInterval = pIntf->meanSyncInterval;

  /* The interface must be enabled for 802.1as and if AVnu mode is enabled
     we only process signals on Master Ports.
     For now we are only supporting signaling in AVnu mode.
     Support could be added for IEEE mode by simply removing the third chech below.
  */
  if ((dot1asIsSignalingCapable(intfIndex) != TRUE) ||
      ((pIntf->selectedRole != DOT1AS_ROLE_MASTER) && (pDot1asBridge_g->avnuMode == AVB_ENABLE)) ||
      (pDot1asBridge_g->avnuMode != AVB_ENABLE))
  {
    pIntf->stats.signalingRxDiscards++;
    return AVB_FAILURE;
  }

  if (dot1asCfgxIntfInitialLogNbrPDelayIntervalGet(port, &initialLogPDelayInterval) != AVB_SUCCESS)
  {
    initialLogPDelayInterval = DOT1AS_INTF_INIT_NBR_PDELAY_DEFAULT;
  }
  /* initialLogSyncInterval */
  if (dot1asCfgxIntfInitialLogSyncIntervalGet(port, &initialLogSyncInterval) != AVB_SUCCESS)
  {
    initialLogSyncInterval = DOT1AS_INTF_INIT_SYNC_DEFAULT;
  }
  /* initialLogAnnounceInterval */
  if (dot1asCfgxIntfInitialLogAnnounceIntervalGet(port, &initialLogAnnounceInterval) != AVB_SUCCESS)
  {
    initialLogAnnounceInterval = DOT1AS_INTF_INIT_ANN_DEFAULT;
  }

  pSigPdu = &pPdu->body.signalingPdu;
  pTlv = &pSigPdu->msgIntReqTlv;

  /* Set intervals - LinkDelaySyncIntervalSetting, AnnounceIntervalSetting */
  DOT1AS_TRACE_SIG("%s: Intf %d, sync %d, pdelay %d, announce %d\n", __FUNCTION__,
                   avbCfgxIntfNum[intfIndex], pTlv->timeSyncInterval, pTlv->linkDelayInterval,
                   pTlv->announceInterval);
#if 0
  DOT1AS_TRACE_SIG("%s: targetPortIdentity.port %d, targetPortIdentity.clock %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", __FUNCTION__,
                   pSigPdu->targetPortIdentity.num,
                   pSigPdu->targetPortIdentity.clock.id[0], pSigPdu->targetPortIdentity.clock.id[1],
                   pSigPdu->targetPortIdentity.clock.id[2], pSigPdu->targetPortIdentity.clock.id[3],
                   pSigPdu->targetPortIdentity.clock.id[4], pSigPdu->targetPortIdentity.clock.id[5],
                   pSigPdu->targetPortIdentity.clock.id[6], pSigPdu->targetPortIdentity.clock.id[7]);
#endif

  /* If we're in AVnu mode only process sync interval and
     ignore the pDelay and Announce intervals */
  if (pDot1asBridge_g->avnuMode != AVB_ENABLE)
  {
    /* PDELAY interval */
    switch (pTlv->linkDelayInterval)
    {
      case DOT1AS_SIGNALING_MSG_INT_REQ_NONE: /* Ignore. Do not change */
        rc = AVB_SUCCESS;
        break;

      case DOT1AS_SIGNALING_MSG_INT_REQ_INIT: /* Set to initial value */
        rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_PDELAY_TX_INTERVAL, (int32_t) initialLogPDelayInterval);
        break;

      case DOT1AS_SIGNALING_MSG_INT_REQ_STOP:
      default:
        /* Value of 127 is so large (10^30 years) that it implies stop */
        rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_PDELAY_TX_INTERVAL, pTlv->linkDelayInterval);
        break;
    }

    if (rc != AVB_SUCCESS)
    {
      /* Note that only certain interval values are supported. Requests
       * for unsupported intervals are ignored.
       */
       DOT1AS_TRACE_SIG("%s: Failed to set link delay interval\n", __FUNCTION__);
    }

    /* Announce interval */
    switch (pTlv->announceInterval)
    {
      case DOT1AS_SIGNALING_MSG_INT_REQ_NONE: /* Ignore. Do not change */
        rc = AVB_SUCCESS;
        break;

      case DOT1AS_SIGNALING_MSG_INT_REQ_INIT: /* Set to initial value */
        rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL, initialLogAnnounceInterval);
        break;

      case DOT1AS_SIGNALING_MSG_INT_REQ_STOP:
      default:
        /* Value of 127 is so large (10^30 years) that it implies stop */
        /* Note that only certain interval values are supported. Requests
         * for unsupported intervals are ignored
         */
        rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL, pTlv->announceInterval);
        break;
    }

    if (rc != AVB_SUCCESS)
    {
      /* Note that only certain interval values are supported. Requests
       * for unsupported intervals are ignored.
       */
       DOT1AS_TRACE_SIG("%s: Failed to set announce interval\n", __FUNCTION__);
    }
  }
  else
  {
      (void) dot1asCfgxAVNUClockStateGet(&clockState);
      if (clockState == DOT1AS_CLOCK_UPDATE_SLAVE)
      {
        /* Start the signaling timer if AVnu mode is enabled and we are a slave device */
        pDot1asBridge_g->avnuSignalingTimer = DOT1AS_AVNU_SIGNALING_TIMER_START;
      }
  }

  /* Timesync interval */
  switch (pTlv->timeSyncInterval)
  {
    int8_t logSyncInterval;

    case DOT1AS_SIGNALING_MSG_INT_REQ_NONE: /* Ignore. Do not change */
      rc = AVB_SUCCESS;
      break;

    case DOT1AS_SIGNALING_MSG_INT_REQ_INIT: /* Set to initial value */
      rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_SYNC_TX_INTERVAL, initialLogSyncInterval);
      break;

    case DOT1AS_SIGNALING_MSG_INT_REQ_STOP:
    default:
      /* Value of 127 is so large (10^30 years) that it implies stop, not supported in AVnu mode */
      /* Note that only certain interval values are supported. Requests
       * for unsupported intervals are ignored
       */
      if (pTlv->timeSyncInterval > DOT1AS_INTF_AVNU_INIT_SYNC_MAX)
      {
        logSyncInterval = DOT1AS_INTF_AVNU_INIT_SYNC_MAX;
      }
      else if (pTlv->timeSyncInterval < DOT1AS_INTF_AVNU_INIT_SYNC_MIN)
      {
        logSyncInterval = DOT1AS_INTF_AVNU_INIT_SYNC_MIN;
      }
      else
      {
        logSyncInterval = pTlv->timeSyncInterval;
      }
      rc = dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_SYNC_TX_INTERVAL, logSyncInterval);
      break;
  }

  if (rc != AVB_SUCCESS)
  {
    /* Note that only certain interval values are supported. Requests
     * for unsupported intervals are ignored.
     */
     DOT1AS_TRACE_SIG("%s: Failed to set sync interval\n", __FUNCTION__);
  }
  else
  {
    if ((pDot1asBridge_g->avnuMode == AVB_ENABLE) && (clockState == DOT1AS_CLOCK_UPDATE_SLAVE))
    {
      /* If the SYNC interval is less than the current SYNC interval then we need to immediately signal the slave port */
 #if 0
     DOT1AS_TRACE_SIG("%s: Sync interval changed from %llums to %llums\n", __FUNCTION__, prevMeanSyncInterval/1000, pIntf->meanSyncInterval/1000);
#endif
      if (prevMeanSyncInterval > pIntf->meanSyncInterval)
      {
        pDot1asBridge_g->avnuSignalingTimer = DOT1AS_AVNU_SIGNALING_TIMER_TIMEOUT+1;
      }
    }
  }

  return rc;
}

/******************************************************************************
* @purpose  Send an Signaling PDU out of the given interface
*
* @param    intfIndex   @b{(input)} DOT1AS internal interface index
* @param    logInterval @b{(input)} Log message interval value
*
* @returns  AVB_RC_t
*
* @comments The standard does not specify the conditions under which a signaling
*           PDU is transmitted; it specifies only the actions a time aware system
*           takes when a signaling PDU is received.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asSignalingSend(uint32_t intfIndex,
                            int8_t linkDelayInterval,
                            int8_t timeSyncInterval,
                            int8_t announceInterval)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint8_t      *pdu = NULL;
  uint16_t      pduLen = 0;
  uint16_t      hdrsize = 0;
  uint16_t      val16 = 0;
  uint16_t      flags = 0;
  /* uint64_t      interval = 0; */
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t      lagCfgIntf;
#endif
  uint16_t      port;
  uint8_t       intfSyncReceiptTimeout;
  uint8_t       intfAnnounceReceiptTimeout;

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
  if (dot1asCfgxIntfAnnounceReceiptTimeoutGet(port, &intfAnnounceReceiptTimeout) != AVB_SUCCESS)
  {
    intfAnnounceReceiptTimeout = DOT1AS_INTF_ANN_TIMEOUT_DEFAULT;
  }
  if (dot1asCfgxIntfSyncReceiptTimeoutGet(port, &intfSyncReceiptTimeout) != AVB_SUCCESS)
  {
    intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
  }

  pdu = &dot1asTxBuffer[0];
  sal_memset(pdu, 0x00, DOT1AS_MAX_PDU_SIZE);

  /* Add DA, SA, and EthType */
  if (dot1asPduAddEnetHeader(intfIndex, pdu, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* Add 802.1AS header */
  hdrsize = 0;
  flags = DOT1AS_CLK_PTP_TIMESCALE_FLAG;
  if (dot1asPduAddCommonHeader(intfIndex, pdu, DOT1AS_PDU_SIGNALING,
                               pIntf->signalingSeqId, 0, 0x7F,
                               flags, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* 10bytes target port identity. This is 0xFF per section 10.5.4.2.1 of
   * 802.1AS spec
   */
  sal_memset(pdu, 0, DOT1AS_CLOCK_ID_LEN);
  pdu += DOT1AS_CLOCK_ID_LEN; pduLen += DOT1AS_CLOCK_ID_LEN;

  val16 = sal_htons(0xFF);
  sal_memcpy(pdu, &val16, sizeof(uint16_t));
  pdu += sizeof(uint16_t); pduLen += sizeof(uint16_t);

  /* Message interval request TLV */

  /* 2-bytes of TLV type */
  val16 = sal_htons(DOT1AS_TLV_TYPE_ORG_EXT);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16); pduLen += sizeof(val16);

  /* 2-bytes of TLV length */
  val16 = sal_htons(DOT1AS_SIGNALING_MSG_INT_TLV_LEN);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16); pduLen += sizeof(val16);

  /* OrgId is 00-80-C2 */
  *pdu++  = 0x00; *pdu++  = 0x80; *pdu++  = 0xC2;
  pduLen += 3;

  /* OrgSubType is 0x000002 */
  *pdu++ = 0x00; *pdu++ = 0x00; *pdu++ = 0x02;
  pduLen += 3;

  /* linkDelayInterval - 1byte */
  *pdu++ = linkDelayInterval; pduLen++;

  /* timeSyncInterval - 1byte */
  *pdu++ = timeSyncInterval; pduLen++;

  /* announceInterval - 1byte */
  *pdu++ = announceInterval; pduLen++;

  /* Flags - 1 bytes */
  *pdu++ = DOT1AS_SIGNALING_NBR_RATE_RATIO_FLAG | DOT1AS_SIGNALING_NBR_PDELAY_FLAG;
  pduLen++;

  /* Reserved - 2bytes */
  val16 = 0x0000;
  sal_memcpy(pdu, &val16, sizeof(val16));
  pduLen += sizeof(val16);

  DOT1AS_TRACE_TX("DOT1AS PDU event: Sending SIGNALING PDU on intf %d\n", avbCfgxIntfNum[intfIndex]);
  DOT1AS_TRACE_SIG("DOT1AS PDU event: Sending SIGNALING PDU on intf %d with sync=%d, ann=%d, pdelay=%d\n",
                    avbCfgxIntfNum[intfIndex], timeSyncInterval, announceInterval, linkDelayInterval);
  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to transmit\n", __FUNCTION__);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  pIntf->stats.signalingTxCount++;
  pIntf->signalingSeqId++;

  /* All is good. Now adjust the receipt timeouts on this interface */

  /* Sync receipt timeout. See section 10.5.4.3.7 and 10.5.4.3.8 of
   * 802.1AS Draft 7.0.
   */

  /* For ANnu mode we handle the SYNC RX timeout adjustment with dot1asAvnuSignalingSyncRxTimeoutSet() */

#if 0 /* Not supporting IEEE Mode Signaling */
  if (pDot1asBridge_g->avnuMode != AVB_ENABLE)
  {
    uint64_t  interval = 0;

    /* In IEEE mode we need to adjust the timeout now if the interval has increased, otherwise
       we can wait for receipt of the next pdu and adjust it at that time */

    /* Sync receipt timeout */
    (void) dot1asIntervalLog2Usec(timeSyncInterval, &interval);
    dot1asIntervalUsecAdjust(&interval);
    if (pIntf->syncReceiptTimeoutInterval < (interval*intfSyncReceiptTimeout))
    {
      dot1asIntervalUpdate(intfIndex, DOT1AS_SYNC_RX_TIMEOUT, timeSyncInterval);
    }

    /* Announce receipt timeout */
    (void) dot1asIntervalLog2Usec(announceInterval, &interval);
    dot1asIntervalUsecAdjust(&interval);
    if (pIntf->announceReceiptTimeoutInterval < (interval*intfAnnounceReceiptTimeout))
    {
      dot1asIntervalUpdate(intfIndex, DOT1AS_ANNOUNCE_RX_TIMEOUT, announceInterval);
    }
  }
#endif /* Not supporting IEEE Mode Signaling */

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Set the signaling timeout value
*
* @param    intfIndex   @b{(input)} DOT1AS internal interface index
* @param    logSyncRx @b{(input)} Log message interval value
*
* @returns  none
*
* @comments Used to set the sync timeout based on a value other than the current
*           sync rx value.
*
* @end
*******************************************************************************/
void dot1asAvnuSignalingSyncRxTimeoutSet(uint32_t intfIndex, int8_t logSyncRx)
{
  DOT1AS_INTF_STATE_t  *pIntf;
  uint8_t               syncTimeout;
  uint64_t              interval = 0;

  pIntf = &pDot1asIntf_g[intfIndex];
  if (dot1asCfgxIntfSyncReceiptTimeoutGet(avbCfgxIntfNum[intfIndex], &syncTimeout) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to get sync receipt timeout for intf = %u\n", __FUNCTION__, avbCfgxIntfNum[intfIndex]);
    syncTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
  }
  if (dot1asIntervalLog2Usec(logSyncRx, &interval) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to convert log to uSec for intf = %u\n", __FUNCTION__, avbCfgxIntfNum[intfIndex]);
    return;
  }
  /* Adjust interval to timer tick */
  dot1asIntervalUsecAdjust(&interval);

  pIntf->syncReceiptTimeoutInterval = (interval * syncTimeout);
  pIntf->syncReceiptTimerReset = TRUE;
  pIntf->syncTransmitTimerReset = TRUE;

#if 0
  DOT1AS_TRACE_SIG("%s: Adjusted sync rx timeout to %llu mSec for port %d\n", __FUNCTION__, (interval * syncTimeout / 1000), avbCfgxIntfNum[intfIndex]);
#endif
}

/******************************************************************************
* @purpose  Check the signaling timer to see if there is anything to do
*
* @param    none
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asCheckAvnuSignalingTimers(void)
{
  DOT1AS_INTF_STATE_t *pIntf;
  int8_t               logSync = DOT1AS_SIGNALING_MSG_INT_INVALID;
  int32_t              slaveLogSync;  /* Current log sync interval for the slave port */
  uint32_t             intfIndex;
  uint32_t             slaveIntfIndex = avbMaxIntfaces_g;
  uint8_t              intfSyncReceiptTimeout;

  if ((pDot1asBridge_g->avnuSignalingTimer == DOT1AS_AVNU_SIGNALING_TIMER_STOP) || (pDot1asBridge_g->avnuMode != TRUE))
  {
    return; /* Nothing to do */
  }

  /* Increment running counter and check for time elapsed */
  pDot1asBridge_g->avnuSignalingTimer += DOT1AS_TIMER_TICK_US;

  if (pDot1asBridge_g->avnuSignalingTimer >= DOT1AS_AVNU_SIGNALING_TIMER_TIMEOUT)
  {
#if 0
    DOT1AS_TRACE_SIG("%s: Signaling timer expired\n", __FUNCTION__);
#endif
    /* Restarting the timer to run the signaling logic at the periodic interval to make it to robust */
    pDot1asBridge_g->avnuSignalingTimer = DOT1AS_AVNU_SIGNALING_TIMER_START;

    /* Send signal out the slave port with the smallest sync interval of the asCapable master ports
    */
    for (intfIndex = 0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
    {
      pIntf = &pDot1asIntf_g[intfIndex];
      if ((pIntf->dot1asCapable != TRUE) ||
          (pIntf->selectedRole != DOT1AS_ROLE_MASTER) ||
          (portLinkStateGet(avbCfgxIntfNum[intfIndex]) != AVB_PORT_LINK_UP))
      {
        continue;
      }
      if (pIntf->currentLogSyncInterval < logSync)
      {
         logSync = pIntf->currentLogSyncInterval;
      }
    }

    if (logSync == DOT1AS_SIGNALING_MSG_INT_INVALID)
    {
      DOT1AS_TRACE_SIG("%s: Nothing to do. No active Master ports.\n", __FUNCTION__);
      return; /* Nothing to to, no active master ports */
    }

    for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
    {
      pIntf = &pDot1asIntf_g[intfIndex];
      if ((pIntf->dot1asCapable == TRUE) && (pIntf->selectedRole == DOT1AS_ROLE_SLAVE))
      {
        slaveIntfIndex = intfIndex;
        break;
      }
    }
    if (slaveIntfIndex == avbMaxIntfaces_g)
    {
      DOT1AS_TRACE_SIG("%s: Nothing to do. No Slave interface found.\n", __FUNCTION__);
      return; /* No slave interface found */
    }

    if (dot1asCfgxIntfSyncReceiptTimeoutGet(avbCfgxIntfNum[slaveIntfIndex], &intfSyncReceiptTimeout) != AVB_SUCCESS)
    {
      DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to get sync receipt timeout for intf = %u\n", __FUNCTION__, avbCfgxIntfNum[slaveIntfIndex]);
      intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
    }

    pIntf = &pDot1asIntf_g[slaveIntfIndex];
    (void) dot1asIntervalUsec2Log(pIntf->syncReceiptTimeoutInterval/intfSyncReceiptTimeout, &slaveLogSync);
    if (logSync == slaveLogSync)
    {
      DOT1AS_TRACE_SIG("%s: No signal needed\n", __FUNCTION__);
      return; /* Nothing to do */
    }

    if (dot1asSignalingSend(slaveIntfIndex, DOT1AS_SIGNALING_MSG_INT_REQ_STOP, logSync, DOT1AS_SIGNALING_MSG_INT_REQ_STOP) == AVB_SUCCESS)
    {
      pDot1asBridge_g->avnuSigSyncLogInterval = logSync;
      pDot1asBridge_g->avnuSigSyncRxCount = 0;
      if (logSync > slaveLogSync) /* If the new sync interval will be larger then increase the timeout ahead of time */
      {
        dot1asAvnuSignalingSyncRxTimeoutSet(slaveIntfIndex, logSync);
        pDot1asBridge_g->avnuSigSyncAdjustTimeout = FALSE; /* Only need to adjust if the signal is ignored */
      }
      else
      {
        pDot1asBridge_g->avnuSigSyncAdjustTimeout = TRUE;
      }
    }
    else
    {
      DOT1AS_TRACE_ERR("ERROR: (%s) : dot1asSignalingSend() failed on port %u\n", __FUNCTION__, avbCfgxIntfNum[slaveIntfIndex]);
    }
  }
}


/******************************************************************************
* @purpose  Signaling callback for any link state changes
*
* @param    port      @b{(input)} External port number
*
* @returns  linkState @b{(input)} The new link state
*
* @comments
* If an AED port experiences a link down/up event then, on link up, the Sync Interval state machine(s) should
* revert to its/their initialization state. Specifically:
* • On an AED-B master port, SyncInterval for the port should revert to initialLogSyncInterval. The AED-B
*    should signal for an updated interval on its slave port as required
* • On an AED-B slave port, the AED-B should expect the link partner port to revert to it's initialLogSyncInterval,
*    and then signal the required rate on the port according to the current rate on each of its slave master ports.
* If AVnu mode:
* If slave port goes down, re-init the signaling state.
* If slave port comes up, start the signaling timer
* If a master port goes down, configure to init sync interval
* If a master port comes up, signal slave port to got to init sync interval (should aready be at init sync)
*
* @end
*******************************************************************************/
void dot1asSignalingLinkStateNotify(uint32_t port, uint8_t linkState)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint8_t              intfIndex;
  uint8_t              idx;
  uint8_t              slaveIntfIndex = avbMaxIntfaces_g;
  uint8_t              asCapableCfg;
  int8_t               initialLogSyncInterval;
  int32_t              slaveLogSync;
  uint8_t              clockState = DOT1AS_CLOCK_UPDATE_SLAVE;
  uint8_t              intfSyncReceiptTimeout;

  if ((!DOT1AS_IS_READY) || (pDot1asBridge_g->avnuMode != TRUE)) { return; }

  if (cfgxIntfIndexGet(port, &intfIndex) != AVB_SUCCESS) { return; }

  pIntf = &pDot1asIntf_g[intfIndex];

  if (dot1asCfgxIntfAVNUDot1asCapableGet((uint16_t) port, &asCapableCfg) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("(%s) Failed to get asCapable config for port %d", __FUNCTION__, port);
    asCapableCfg = AVB_FALSE;
  }

  if ((asCapableCfg != AVB_TRUE) || (dot1asIsSignalingCapable((uint32_t) intfIndex) != TRUE))
  {
    return;
  }

  /* We don't care about the first link up, only link up after link goes down and then comes back up */
  if ((linkState == AVB_PORT_LINK_UP) && (portLinkChangeCountGet(port) < 2)) { return; }

  (void) dot1asCfgxAVNUClockStateGet(&clockState);

  if (dot1asCfgxIntfInitialLogSyncIntervalGet(port, &initialLogSyncInterval) != AVB_SUCCESS)
  {
    initialLogSyncInterval = DOT1AS_INTF_INIT_SYNC_DEFAULT;
  }

  if (pIntf->selectedRole == DOT1AS_ROLE_SLAVE)
  {
    if (linkState == AVB_PORT_LINK_DOWN)
    {
      dot1asSignalingStateInit();
    }
    else /* Start the signaling timer so we can slow down the slave */
    {
      if (clockState == DOT1AS_CLOCK_UPDATE_SLAVE)
      {
        /* Start the signaling timer if AVnu mode is enabled and we are a slave device */
        pDot1asBridge_g->avnuSignalingTimer = DOT1AS_AVNU_SIGNALING_TIMER_START;
      }
    }
  }
  else if (pIntf->selectedRole == DOT1AS_ROLE_MASTER)
  {
    if (linkState == AVB_PORT_LINK_DOWN)
    {
      if (dot1asIntervalUpdate(intfIndex, DOT1AS_CURRENT_SYNC_TX_INTERVAL, initialLogSyncInterval) != AVB_SUCCESS)
      {
        DOT1AS_TRACE_ERR("(%s) Failed to set initial sync interval for port %d", __FUNCTION__, port);
      }
    }
    else
    {
      /* Link UP on a MASTER port:
         Send signal out slave port to change to init sync interval.
      */
      if (clockState == DOT1AS_CLOCK_UPDATE_GM)
      {
        return;  /* Nothing to do if we are the GM */
      }

      for (idx=0; idx < dot1asMaxInterfaceCount_g; idx++)
      {
        pIntf = &pDot1asIntf_g[idx];
        if ((pIntf->dot1asCapable == TRUE) && (pIntf->selectedRole == DOT1AS_ROLE_SLAVE))
        {
          slaveIntfIndex = idx;
          break;
        }
      }
      if (slaveIntfIndex == avbMaxIntfaces_g)
      {
        return; /* No slave interface found */
      }

      if (dot1asCfgxIntfSyncReceiptTimeoutGet(avbCfgxIntfNum[slaveIntfIndex], &intfSyncReceiptTimeout) != AVB_SUCCESS)
      {
        DOT1AS_TRACE_ERR("ERROR: (%s) : Failed to get sync receipt timeout for intf = %u\n", __FUNCTION__, avbCfgxIntfNum[slaveIntfIndex]);
        intfSyncReceiptTimeout = DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT;
      }

      pIntf = &pDot1asIntf_g[slaveIntfIndex];
      (void) dot1asIntervalUsec2Log(pIntf->syncReceiptTimeoutInterval/intfSyncReceiptTimeout, &slaveLogSync);

      if (initialLogSyncInterval > slaveLogSync)
      {
        /* If we're running slower than the slave, start the timer and see if we need to slow down the slave */
        pDot1asBridge_g->avnuSignalingTimer = DOT1AS_AVNU_SIGNALING_TIMER_START;
      }
      else if (initialLogSyncInterval < slaveLogSync)
      {
        /* If we're running faster than the slave, we need to signal now */
        if ((asCapableCfg == AVB_TRUE) && (dot1asIsSignalingCapable((uint32_t) slaveIntfIndex) == TRUE))
        {
          if (dot1asSignalingSend(slaveIntfIndex, DOT1AS_SIGNALING_MSG_INT_REQ_STOP, initialLogSyncInterval, DOT1AS_SIGNALING_MSG_INT_REQ_STOP) == AVB_SUCCESS)
          {
            pDot1asBridge_g->avnuSigSyncLogInterval = initialLogSyncInterval;
            pDot1asBridge_g->avnuSigSyncRxCount = 0;
            pDot1asBridge_g->avnuSigSyncAdjustTimeout = TRUE;
          }
          else
          {
            DOT1AS_TRACE_ERR("ERROR: (%s) : dot1asSignalingSend() failed on port %u\n", __FUNCTION__, avbCfgxIntfNum[slaveIntfIndex]);
          }
        }
      }
    }
  }
}

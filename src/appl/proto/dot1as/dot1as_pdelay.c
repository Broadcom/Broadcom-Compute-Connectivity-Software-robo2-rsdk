/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_pdelay.c
 */

#include "osal/sal_console.h"
#include "osal/sal_alloc.h"
#include "osal/sal_util.h"
#include "osal/sal_time.h"
#include "shared/types.h"
#include "fsal/port.h"
#include "fsal/ts.h"
#include "dot1as.h"
#include "dot1as_cfgx.h"
#include "avb.h"

extern uint8_t avb_port_master_slave_get(uint32_t port, uint32_t *master_slave);
extern uint8_t avb_port_master_slave_cfg_get(uint32_t port);

#define DOT1AS_FORCE_PDELAY FALSE
/* In AVnu mode we don't want to see this trace */
#define DOT1AS_PDELAY_TRACE_AS_CAPABLE(_intf_, _flg_, _rsn_)                   \
  do                                                                           \
  {                                                                            \
    if (pDot1asBridge_g->avnuMode != TRUE)                                     \
    {                                                                          \
      DOT1AS_TRACE("\nDOT1AS: Port[%d] is %sasCapable. %s\n", (_intf_),        \
                   (_flg_) ? "" : "NOT ", (_rsn_));                            \
    }                                                                          \
  } while(0)                                                                   \

/* config save thread id */
#ifdef DOT1AS_CONFIG_SAVE_SUPPORT
mos_thread_t * pConfigSaveThreadId = (mos_thread_t *)NULL;

void configSaveThread(void *arg)
{
  if (avb_config_json_save() != CFG_SUCCESS) {
    DOT1AS_TRACE_ERR("%s: Config save failed\n", __FUNCTION__);
  }
  pConfigSaveThreadId = (mos_thread_t *)NULL;
#if 0
  DOT1AS_TRACE_DEV("%s: Config saved\n", __FUNCTION__);
#endif
}
#endif

/******************************************************************************
* @purpose  Initialize PDELAY filter state
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntfPdelayFilterInit(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  pIntf->pDelayFilterInit = TRUE;
  pIntf->delayEstimate = 0;
  pIntf->neighborRateRatio = DOT1AS_RATE_RATIO_DEFAULT;
  sal_memset(&pIntf->prevT1, 0x00, sizeof(pIntf->prevT1));
  sal_memset(&pIntf->prevT2, 0x00, sizeof(pIntf->prevT2));
  sal_memset(&pIntf->prevT3, 0x00, sizeof(pIntf->prevT3));
  sal_memset(&pIntf->prevT4, 0x00, sizeof(pIntf->prevT4));
}

/******************************************************************************
* @purpose  Initialize PDELAY state on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntfPdelayInit(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  pIntf->isMeasuringPdelay = FALSE;
  if(pDot1asBridge_g->avnuMode != TRUE)
  {
    pIntf->dot1asCapable = FALSE;
  }
  pIntf->pDelaySeqId = DOT1AS_PDELAY_REQ_PDU_START_SEQUANCE_ID;

  pIntf->pDelayIntervalTimerReset = TRUE;

  pIntf->numPdelayRespLost = 0;
  DOT1AS_TRACE_PDELAY("DOT1AS[%d] Pdelay Init. Moving to send_req\n", avbCfgxIntfNum[intfIndex]);
  pIntf->pDelayReqState = DOT1AS_PDELAY_REQ_SEND;

  dot1asIntfPdelayFilterInit(intfIndex);
}

/******************************************************************************
* @purpose  Re-Initialize PDELAY on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntfPdelayReInit(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
  avbLogBuffer_t logEntry;
#endif

  pIntf = &pDot1asIntf_g[intfIndex];

  if (pIntf->numPdelayRespLost <= pIntf->allowedLostReponses)
  {
    pIntf->numPdelayRespLost++;
    if (pIntf->numPdelayRespLost > pIntf->allowedLostReponses)
    {
      pIntf->isMeasuringPdelay = FALSE;
      pIntf->stats.pDelayLostResponsesExceeded++;

      /* Re init the filter */
      dot1asIntfPdelayFilterInit(intfIndex);

      if (pIntf->dot1asCapable == TRUE)
      {
        if(pDot1asBridge_g->avnuMode != TRUE)
        {
          DOT1AS_PDELAY_TRACE_AS_CAPABLE(avbCfgxIntfNum[intfIndex], FALSE,
                                         "Lost resp exceeded");
          pIntf->dot1asCapable = FALSE;
        }
        else
        {
          int8_t initialPdelayIntl;
          uint32_t intfCfgIndex;

          AVNU_TRACE_ASCAPABLE(avbCfgxIntfNum[intfIndex], TRUE, "Lost resp exceeded");
#ifdef DOT1AS_LAG_SUPPORT
          if (avb_lag_index_for_member_port(intfIndex, &intfCfgIndex) != AVB_SUCCESS)
          {
            intfCfgIndex = intfIndex;
          }
#else
          intfCfgIndex = intfIndex;
#endif
          dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet(avbCfgxIntfNum[intfCfgIndex],
                                                           &initialPdelayIntl);
          pIntf->currentLogPdelayReqInterval = initialPdelayIntl;
          dot1asIntervalLog2Usec(pIntf->currentLogPdelayReqInterval,
                                 &pIntf->meanPdelayReqInterval);
          dot1asIntervalUsecAdjust(&pIntf->meanPdelayReqInterval);
          pIntf->avnuOperationalPdelayTimer = 0;
        }

#ifdef DOT1AS_LAG_SUPPORT
        avb_lag_state_update_port(intfIndex, AVB_LAG_PORT_UPDATE_OP_AS_PDELAY_DISABLE);
#endif
        /* Disable BMCA and Timesync state machines */
        dot1asInvokeBmca(intfIndex, DOT1AS_BMC_INTF_EVENT, NULL);

        /* Notify applications that are registered for DOT1AS callback - TODO */
      }

#ifdef DOT1AS_LOG_BUFFER_SUPPORT
      /* Add log entry */
      sal_memset(&logEntry, 0, sizeof(avbLogBuffer_t));
      logEntry.eventType = AVB_LOG_EVENT_PDELAY_LOST_RESPONSE_EXCEEDED;
      logEntry.u.pDelayLostResponseExceed.intfNum = avbCfgxIntfNum[intfIndex];
      logEntry.u.pDelayLostResponseExceed.val = AVB_TRUE;
      avbLogBufferEntryAdd(&logEntry);
      /* set asynchronous event */
      avbAsyncEventTypeSet(intfIndex, AVB_ASYNC_EVENT_PORT_PDELAY_LOST_RESPONSE_EXCEEDED);
#endif
    }
  }

  /* Keep sending req, as long as the PDELAY is enabled */
  DOT1AS_TRACE_PDELAY("DOT1AS[%d] Pdelay Reinit. Moving to send_req\n", avbCfgxIntfNum[intfIndex]);
  pIntf->pDelayReqState = DOT1AS_PDELAY_REQ_SEND;
}


/******************************************************************************
* @purpose  Disable PDELAY state on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments Internal state machine variables are cleared on init.
*
* @end
*******************************************************************************/
void dot1asIntfPdelayDisable(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t pdelay;
  uint32_t intfCfgIndex;
  uint32_t linkStatus = FALSE;

  pIntf = &pDot1asIntf_g[intfIndex];

  /* Disable the PDELAY_REQ state machine */
  DOT1AS_TRACE_PDELAY("DOT1AS[%d] Pdelay Disable.\n", avbCfgxIntfNum[intfIndex]);
  pIntf->pDelayReqState = DOT1AS_PDELAY_DISABLED;

  /* Mark the interface as NOT capable */
  pIntf->isMeasuringPdelay = FALSE;
  if (pDot1asBridge_g->avnuMode == TRUE)
  {
#ifdef DOT1AS_LAG_SUPPORT
    if (avb_lag_index_for_member_port(intfIndex, &intfCfgIndex) != AVB_SUCCESS)
    {
      intfCfgIndex = intfIndex;
    }
#else
    intfCfgIndex = intfIndex;
#endif
    dot1asCfgxIntfAVNUNeighborPropDelayGet(avbCfgxIntfNum[intfCfgIndex], &pdelay);
  }

  if (pIntf->dot1asCapable == TRUE)
  {
    cbx_port_attribute_get(intfIndex, cbxPortAttrLinkStatus, &linkStatus);
    if (linkStatus != TRUE)
    {
      DOT1AS_PDELAY_TRACE_AS_CAPABLE(avbCfgxIntfNum[intfIndex], FALSE, "Link down event");
      if (pDot1asBridge_g->avnuMode == TRUE)
      {
        pIntf->isAVnuPdelayConfigSaved = FALSE;
      }
    }
    else
    {
      DOT1AS_PDELAY_TRACE_AS_CAPABLE(avbCfgxIntfNum[intfIndex], FALSE, "");
    }
    if (pDot1asBridge_g->avnuMode == TRUE)
    {
      int8_t initialPdelayIntl;

      dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet(avbCfgxIntfNum[intfCfgIndex], &initialPdelayIntl);
      pIntf->currentLogPdelayReqInterval = initialPdelayIntl;
      dot1asIntervalLog2Usec(pIntf->currentLogPdelayReqInterval, &pIntf->meanPdelayReqInterval);
      dot1asIntervalUsecAdjust(&pIntf->meanPdelayReqInterval);
      pIntf->avnuOperationalPdelayTimer = 0;
    }
    pIntf->dot1asCapable = FALSE;
#ifdef DOT1AS_LAG_SUPPORT
    avb_lag_state_update_port(intfIndex, AVB_LAG_PORT_UPDATE_OP_AS_PDELAY_DISABLE);
#endif
  }

  if (pDot1asBridge_g->avnuMode == TRUE)
  {
    /* Use configured pdelay value */
    pIntf->neighborPropDelay = pdelay;
    pIntf->neighborRateRatio = 1;
  }
  else
  {
    /* Invalidate the PDELAY measurement */
    pIntf->neighborPropDelay = 0;
  }
}


/******************************************************************************
* @purpose  Check whether PDELAY mechanism can be enabled on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  uint8_t - TRUE if interface is capable, else FALSE
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIntfIsPdelayCapable(uint32_t intfIndex)
{
  uint8_t              capable = FALSE;
  uint32_t             linkStatus = FALSE;
  uint8_t              intfAvnuAsCapable = FALSE;
  uint32_t             intfCfgIndex;
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  if (DOT1AS_IS_INTF_CPU(intfIndex)) 
  {
    return FALSE;
  }
  if (pIntf->pttPortEnabled == FALSE) 
  {
    return FALSE;
  }
  cbx_port_attribute_get(intfIndex, cbxPortAttrLinkStatus, &linkStatus);

  /* 802.1AS Global admin mode must be enabled,
   * 802.1AS Interface admin mode must be enabled,
   * Interface must be active.
   * In AVnu mode interface avnuAsCapable must be true
   */
  if ((pDot1asBridge_g->adminMode == TRUE) &&
      (linkStatus == TRUE))
  {
    capable = TRUE;
    if (pDot1asBridge_g->avnuMode == TRUE)
    {
#ifdef DOT1AS_LAG_SUPPORT
      if (avb_lag_intf_for_member_port(intfIndex, &intfCfgIndex) == AVB_FAILURE)
      {
        intfCfgIndex = intfIndex;
      }
#else
      intfCfgIndex = intfIndex;
#endif
      dot1asCfgxIntfAVNUDot1asCapableGet(avbCfgxIntfNum[intfCfgIndex], &intfAvnuAsCapable);
      if (intfAvnuAsCapable == FALSE)
      {
        capable = FALSE;
      }
    }
  }

  return capable;
}

/******************************************************************************
* @purpose  Check whether PDELAY mechanism can be enabled on the given
*           interface taking LAG membership into account
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  uint8_t - TRUE if interface is capable, else FALSE
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIsPdelayCapable(uint32_t intfIndex)
{
  uint8_t  capable = FALSE;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t lagIntf;
  uint32_t lagExtIntfNum;
  uint32_t activeLagIntf;
  DOT1AS_INTF_STATE_t *pIntf;
#endif

  capable = dot1asIntfIsPdelayCapable(intfIndex);

#ifdef DOT1AS_LAG_SUPPORT
  /* Is this interface a member of the Master LAG?
     If there is an active lag member and this is not it, return FALSE - NOT Capable
   */
  if (avb_lag_intf_for_member_port(intfIndex, &lagIntf) == AVB_SUCCESS)
  {
    lagExtIntfNum = AVB_LAG_INTF_NUM_FROM_INTERNAL(lagIntf);
    if (avb_port_master_slave_cfg_get(lagExtIntfNum) == AVB_PORT_MS_MASTER)
    {
      if (avb_lag_active_intf_get(lagExtIntfNum, &activeLagIntf) == AVB_SUCCESS)
      {
        if (intfIndex != activeLagIntf)
        {
          /* Make it clear that this interface is not running pDelay */
          pIntf = &pDot1asIntf_g[intfIndex];
          pIntf->neighborRateRatio = DOT1AS_RATE_RATIO_DEFAULT;
          sal_memset(&pIntf->pDelayT1, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
          sal_memset(&pIntf->pDelayT2, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
          sal_memset(&pIntf->pDelayT3, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
          sal_memset(&pIntf->pDelayT4, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
          capable = FALSE;
        }
      }
    }
  }
#endif
  return capable;
}

/******************************************************************************
* @purpose  Send a PDELAY_RESP for the given PDELAY_REQ messages
*
* @param    intfIndex     @b{(input)} Interface index
* @param    pPdu          @b{(input)} DOT1AS message for incoming PDELAY_REQ
* @param    pIngTimestamp @b{(output)} Ingress timestamp for request
* @param    pEgrTimestamp @b{(output)} Egress timestamp for response
*
* @returns  AVB_RC_t
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPdelayRespSend(uint32_t intfIndex, DOT1AS_PDU_t *pPdu,
                          DOT1AS_TIMESTAMP_t *pIngTimestamp,
                          DOT1AS_TIMESTAMP_t *pEgrTimestamp)
{
  uint16_t    hdrsize = 0;
  uint16_t    pduLen = 0;
  uint16_t    flags = 0;
  uint16_t    val16 = 0;
  uint32_t    val32 = 0;
  uint8_t     *pRespPdu = NULL;
  DOT1AS_PDU_HEADER_t     *pDelayReqHdr;

#if DEV_BUILD_DOT1AS
  if (avbDot1asTest_g.isStopPdelayResp)
  {
    return AVB_SUCCESS;
  }
#endif

  /* Get a frag from ethernet driver */
  pRespPdu = &dot1asTxBuffer[0];
  sal_memset(dot1asTxBuffer, 0, DOT1AS_MAX_PDU_SIZE);
  pDelayReqHdr = &pPdu->hdr;

  /* Add DA, SA, and EthType */
  if (dot1asPduAddEnetHeader(intfIndex, pRespPdu, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pRespPdu += hdrsize;
  pduLen += hdrsize;

  /* Add 802.1AS header, use same sequence ID as the request */

  /* For PDELAY, correction field is sub-nanoseconds (fractional ns) value of
   * timestamp, which is not available from Hardware (only nanoseconds).
   */

  /* Correction field is set to zero by default. */
  hdrsize = 0;
  flags = (DOT1AS_CLK_TWO_STEP_FLAG | DOT1AS_CLK_PTP_TIMESCALE_FLAG);
  if (dot1asPduAddCommonHeader(intfIndex, pRespPdu,
                               DOT1AS_PDU_PDELAY_RESP, pDelayReqHdr->sequenceId,
                               0, 0x7F, flags, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pRespPdu += hdrsize;
  pduLen += hdrsize;

  /* PDELAY Response body consists of request receipt timestamp and requesting
   * port identity
   */
  dot1asPackUint48(pRespPdu, pIngTimestamp->seconds);
  val32 = sal_htonl(pIngTimestamp->nanoseconds);
  sal_memcpy(pRespPdu+6, &val32, sizeof(val32));
  pRespPdu += DOT1AS_TIMESTAMP_LEN;
  pduLen += DOT1AS_TIMESTAMP_LEN;

  sal_memcpy(pRespPdu, pDelayReqHdr->portId.clock.id, DOT1AS_CLOCK_ID_LEN);
  pRespPdu += DOT1AS_CLOCK_ID_LEN;
  pduLen += DOT1AS_CLOCK_ID_LEN;

  /* Increment the port number by 1 in the messages to make it compliant with
   * 802.1AS port numbering which starts with 1
   */
  val16 = sal_htons(pDelayReqHdr->portId.num + 1);
  sal_memcpy(pRespPdu, &val16, sizeof(uint16_t));
  pduLen += sizeof(uint16_t);

  DOT1AS_TRACE_TX("DOT1AS PDU event: Sending PDELAY_RESP on intf %d\n", avbCfgxIntfNum[intfIndex]);

  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to trasmit on %d\n", __FUNCTION__, avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  /* All is good */
  pDot1asIntf_g[intfIndex].stats.pDelayRespTxCount++;

  /* Get the egress timestamp */
  if (dot1asIntfTimestampGet(intfIndex, DOT1AS_PDU_PDELAY_RESP,
                             pDelayReqHdr->sequenceId, DOT1AS_TX_TIMESTAMP,
                             pEgrTimestamp) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("%s: failed to get egress timestamp on %d\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.tsErrors++;
    return AVB_FAILURE;
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Send a PDELAY_RESP_FOLLOWUP for the given PDELAY_REQ messages
*
* @param    intfIndex     @b{(input)} Interface index
* @param    pPdu          @b{(input)} DOT1AS message for incoming PDELAY_REQ
* @param    pEgrTimestamp @b{(output)} Egress timestamp for response
*
* @returns  AVB_RC_t
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPdelayRespFollowupSend(uint32_t intfIndex, DOT1AS_PDU_t *pPdu,
                                  DOT1AS_TIMESTAMP_t *pEgrTimestamp)
{
  uint16_t    hdrsize = 0;
  uint16_t    pduLen = 0;
  uint16_t    val16 = 0;
  uint16_t    flags = 0;
  uint32_t    val32 = 0;
  uint8_t     *pRespPdu = NULL;
  DOT1AS_PDU_HEADER_t     *pDelayReqHdr;

#if DEV_BUILD_DOT1AS
  if (avbDot1asTest_g.isStopPdelayRespFollowup)
  {
    return AVB_SUCCESS;
  }
#endif
  /* Get a frag from ethernet driver */
  pRespPdu = &dot1asTxBuffer[0];
  pDelayReqHdr = &pPdu->hdr;

  sal_memset(pRespPdu, 0x00, DOT1AS_MAX_PDU_SIZE);

  /* Add DA, SA, and EthType */
  if (dot1asPduAddEnetHeader(intfIndex, pRespPdu, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pRespPdu += hdrsize;
  pduLen += hdrsize;

  /* Add 802.1AS header, use same sequence ID as the request */
  /* For PDELAY, correction field is sub-nanoseconds (fractional ns) value of
   * timestamp, which is not available from Hardware (only nanoseconds).
   */

  /* Correction field is set to zero by default. */
  hdrsize = 0;
  flags = DOT1AS_CLK_PTP_TIMESCALE_FLAG;
  if (dot1asPduAddCommonHeader(intfIndex, pRespPdu,
                               DOT1AS_PDU_PDELAY_RESP_FOLLOWUP,
                               pDelayReqHdr->sequenceId,
                               0, 0x7F, flags, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pRespPdu += hdrsize;
  pduLen += hdrsize;

  /* PDELAY RESP_FOLLOWUP body consists of response egress timestamp and
   * requesting port identity
   */
  dot1asPackUint48(pRespPdu, pEgrTimestamp->seconds);
  val32 = sal_htonl(pEgrTimestamp->nanoseconds);
  sal_memcpy(pRespPdu+6, &val32, sizeof(val32));
  pRespPdu += DOT1AS_TIMESTAMP_LEN;
  pduLen += DOT1AS_TIMESTAMP_LEN;

  sal_memcpy(pRespPdu, &pDelayReqHdr->portId.clock.id, DOT1AS_CLOCK_ID_LEN);
  pRespPdu += DOT1AS_CLOCK_ID_LEN;
  pduLen += DOT1AS_CLOCK_ID_LEN;

  /* Increment the port number by 1 in the messages to make it compliant with
   * 802.1AS port numbering which starts with 1
   */
  val16 = sal_htons(pDelayReqHdr->portId.num + 1);
  sal_memcpy(pRespPdu, &val16, sizeof(uint16_t));
  pduLen += sizeof(uint16_t);

  DOT1AS_TRACE_TX("DOT1AS PDU event: Sending PDELAY_RESP_FOLLOWUP on intf %d\n",
                  avbCfgxIntfNum[intfIndex]);


  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to trasmit on %d\n", __FUNCTION__, avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  /* All is good */
  pDot1asIntf_g[intfIndex].stats.pDelayRespFollowUpTxCount++;

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Process the received 802.1AS PDELAY_REQ PDU.
*
* @param    intfIndex  @b{(input)} interface index
* @param    pPdu  @b{(input)} pointer to the received PDU
*
* @returns  AVB_RC_t
*
* @comments Basic validations for DA, ETHTYPE, Interface number have already
*           been done. Caller frees the buffer.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPdelayReqReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu, uint32_t timeStamp)
{
  DOT1AS_TIMESTAMP_t    ingTimestamp;
  DOT1AS_TIMESTAMP_t    egrTimestamp;
  cbx_ts_time_t tod;

  if (timeStamp == 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to get ingress timestamp on %d\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.tsErrors++;
    return AVB_FAILURE;

  }
  dot1asTimestampUpdate(timeStamp, &tod);
  ingTimestamp.seconds = tod.sec;
  ingTimestamp.nanoseconds = tod.nsec;

  /* Honor the request if interface is PDELAY capable */
  if (dot1asIsPdelayCapable(intfIndex) != TRUE)
  {
    pDot1asIntf_g[intfIndex].stats.pDelayReqRxDiscards++;
    return AVB_FAILURE;
  }

  DOT1AS_TRACE_RX("DOT1AS PDU event: Received PDELAY Request on port[%d]\n", avbCfgxIntfNum[intfIndex]);
  /* Send the PDELAY_RESP  and get the egress timestamp */
  sal_memset(&egrTimestamp, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
  if (dot1asPdelayRespSend(intfIndex, pPdu, &ingTimestamp, &egrTimestamp) != AVB_SUCCESS)
  {
    pDot1asIntf_g[intfIndex].stats.pDelayReqRxDiscards++;
    return AVB_FAILURE;
  }

  /* Send the PDELAY_RESP_FOLLOWUP */
  if (dot1asPdelayRespFollowupSend(intfIndex, pPdu, &egrTimestamp) != AVB_SUCCESS)
  {
    pDot1asIntf_g[intfIndex].stats.pDelayReqRxDiscards++;
    return AVB_FAILURE;
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Process the received 802.1AS PDELAY_RESP PDU.
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
AVB_RC_t dot1asPdelayRespReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu, uint32_t timeStamp)
{
  uint8_t                  discard = TRUE;
  DOT1AS_PDU_HEADER_t      *pHdr;
  DOT1AS_PDELAY_RESP_PDU_t *pPdelayResp;
  DOT1AS_INTF_STATE_t      *pIntf;
  cbx_ts_time_t            tod;

  pIntf = &pDot1asIntf_g[intfIndex];
  pHdr = &pPdu->hdr;
  pPdelayResp =&pPdu->body.pDelayRespPdu;

  DOT1AS_TRACE_PDELAY("DOT1AS[%d]: pdelay_resp recv at time %llu, seq_id %d, state %d\n",
                      avbCfgxIntfNum[intfIndex], 1000 * sal_time_usecs(), pHdr->sequenceId, pIntf->pDelayReqState);

  /* Fetch the response receipt timestamp */
  if (timeStamp == 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to get ingress timestamp on %d\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.tsErrors++;
  }
  dot1asTimestampUpdate(timeStamp, &tod);
  pIntf->pDelayT4.seconds = tod.sec;
  pIntf->pDelayT4.nanoseconds = tod.nsec;

  switch (pIntf->pDelayReqState)
  {
    case DOT1AS_PDELAY_DISABLED:
      /* Discard PDELAY response if PDELAY is disabled */
      DOT1AS_TRACE_PDELAY_ERR("PDELAY[%d]: Discarding pdelay_resp disabled state\n",
                              avbCfgxIntfNum[intfIndex]);
      break;

    case DOT1AS_PDELAY_REQ_SEND:
      /* Discard PDELAY response if we are not waiting for it */
      DOT1AS_TRACE_PDELAY_ERR("PDELAY[%d]: Discarding pdelay_resp req_send state. "
                              "Incoming seqId %d, Expected %d\n", avbCfgxIntfNum[intfIndex],
                              pHdr->sequenceId, pIntf->pDelaySeqId);
      break;

    case DOT1AS_PDELAY_WAIT_FOR_RESP:
      /* Check the Sequence ID and the requesting port id */
      if (pHdr->sequenceId == pIntf->pDelaySeqId)
      {
        /* Check whether requesting port identity in the response matches ours */
        if ((DOT1AS_CLOCK_ID_IS_EQ(&pDot1asBridge_g->thisClock,
                                   &pPdelayResp->reqPortId.clock)) &&
            (avbCfgxIntfNum[intfIndex] == pPdelayResp->reqPortId.num))
        {
          /* Note who has sent the response */
          sal_memcpy(&pIntf->partnerId, &pHdr->portId, sizeof(DOT1AS_PORT_IDENTITY_t));

          /* Copy the Request receipt time stamp from the PDU */
          pIntf->pDelayT2 = pPdelayResp->reqReceiptTimestamp;
          discard = FALSE;
          DOT1AS_TRACE_RX("DOT1AS PDU event: Received PDELAY Response on port[%d]\n", avbCfgxIntfNum[intfIndex]);
          DOT1AS_TRACE_PDELAY("DOT1AS[%d] Received resp. Moving to wait_for_followup\n",
                              avbCfgxIntfNum[intfIndex]);
          pIntf->pDelayReqState = DOT1AS_PDELAY_WAIT_FOR_RESP_FOLLOWUP;
        }
        else /* discard if we didn't request for a response */
        {
          DOT1AS_TRACE_PDELAY("DOT1AS[%d] recd response does not match local clkid\n",
                              avbCfgxIntfNum[intfIndex]);
          pIntf->stats.ptpDiscardCount++;
          dot1asIntfPdelayReInit(intfIndex);
        }
      }
      else /* dicard PDELAY response if sequence Id does not match */
      {
        DOT1AS_TRACE_PDELAY_ERR("PDELAY[%d]: Discarding pdelay_resp, seq id "
                  "mismatch Exp: %d, Rcvd: %d\n", avbCfgxIntfNum[intfIndex], pIntf->pDelaySeqId,
                   pHdr->sequenceId);
        pIntf->stats.ptpDiscardCount++;
        dot1asIntfPdelayReInit(intfIndex);
      }
      break;

    case DOT1AS_PDELAY_WAIT_FOR_RESP_FOLLOWUP:
      DOT1AS_TRACE_PDELAY_ERR("PDELAY[%d]: Discarding multiple pdelay_resp\n",
                              avbCfgxIntfNum[intfIndex]);
      /* Handle the case where more than one response is received */
      if (pHdr->sequenceId == pIntf->pDelaySeqId)
      {
        pIntf->stats.ptpDiscardCount++;
        /* Reset rate ratio */
        dot1asIntfPdelayReInit(intfIndex);
      }
      break;

    default:
      DOT1AS_TRACE_PDELAY_ERR("PDELAY: Discarding pdelay_resp invalid state\n");
      discard = TRUE;
      break;
  }

  if (discard == TRUE)
  {
    pIntf->stats.pDelayRespRxDiscards++;
  }

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Subtract two timestamp values considering the rollover time period
*
* @param    ts1  @b{(input)} raw hardware timestamp, nanoseconds
* @param    ts2  @b{(input)} raw hardware timestamp, nanoseconds
*
* @returns  AVB_RC_t
*
* @comments Rollover time period is platform-specific.
*
* @end
*******************************************************************************/
int32_t dot1asSubTimestamps (uint32_t ts1, uint32_t ts2)
{
  /* Calculate the difference (TS1 - TS2) using Hawkeye Timestamp Hardware Format */
  int32_t a = (int32_t)(ts1 - ts2);

  if (a > (HALF_SEC))
  {
    a -= ONE_SEC;
  }
  else if (a < -(HALF_SEC))
  {
    a += ONE_SEC;
  }

  return(a);
}

/******************************************************************************
* @purpose  Subtract two extended timestamp values
*
* @param    pTs1  @b{(input)} DOT1AS timestamp, nanoseconds
* @param    pTs2  @b{(input)} DOT1AS timestamp, nanoseconds
*
* @returns  int64 - difference in nanoseconds
*
* @comments Rollover time period is platform-specific.
*
* @end
*******************************************************************************/
int64_t dot1asSubExtTimestamps (DOT1AS_TIMESTAMP_t *pTs1,
                                DOT1AS_TIMESTAMP_t *pTs2)
{
  int64_t a;

  a =  (pTs1->seconds - pTs2->seconds) * ONE_SEC;
  a += (int32_t)(pTs1->nanoseconds - pTs2->nanoseconds);

  return (a);
}


/******************************************************************************
* @purpose  Compute the neighbor rate ratio on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS interface index
*
* @returns  none
*
* @comments This function assumes that all 4 timestamp values, T1, T2, T3 and
*           T4 are available.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asComputeNbrRateRatio(uint32_t intfIndex)
{
#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
  int32_t ratioIn;
#endif
  int32_t y1, y2;
  int64_t deltaT1, deltaT2, deltaT3, deltaT4, x1,x2;
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  /* deltaT1 - The difference between the local TX timestamp for Pdelay_Req and
   * its previous value.
   * deltaT2 - The difference between the far end RX timestamp for Pdelay_Req
   * and its previous value.
   * deltaT3 - The difference between the far end TX timestamp for Pdelay_Resp
   * and its previous value.
   * deltaT4 - The difference between the local RX timestamp for Pdelay_Resp and
   * its previous value
   */
  if ((pIntf->pDelayT1.seconds < pIntf->prevT1.seconds) ||
      (pIntf->pDelayT2.seconds < pIntf->prevT2.seconds) ||
      (pIntf->pDelayT3.seconds < pIntf->prevT3.seconds) ||
      (pIntf->pDelayT4.seconds < pIntf->prevT4.seconds))
  {
#if 0
    DOT1AS_TRACE_DEV("DOT1AS: dot1asComputeNbrRateRatio - zero or rollover");
    /* Store the current T1,T2,T3,T4 timestamps as previous values
       if the previous values are all zero or if one of the timestamps
       rolled over */
#endif
    DOT1AS_TRACE_PDELAY("DOT1AS: Timestamp rollover intf[%d] (current) T1 %llu:%u, T2 %llu:%u, T3 %llu:%u, T4 %llu:%u\n", avbCfgxIntfNum[intfIndex],
                        pIntf->pDelayT1.seconds, pIntf->pDelayT1.nanoseconds, pIntf->pDelayT2.seconds, pIntf->pDelayT2.nanoseconds,
                        pIntf->pDelayT3.seconds, pIntf->pDelayT3.nanoseconds, pIntf->pDelayT4.seconds, pIntf->pDelayT4.nanoseconds);
    DOT1AS_TRACE_PDELAY("DOT1AS: Timestamp rollover intf[%d] (previous) T1 %llu:%u, T2 %llu:%u, T3 %llu:%u, T4 %llu:%u\n", avbCfgxIntfNum[intfIndex],
                        pIntf->prevT1.seconds, pIntf->prevT1.nanoseconds, pIntf->prevT2.seconds, pIntf->prevT2.nanoseconds,
                        pIntf->prevT3.seconds, pIntf->prevT3.nanoseconds, pIntf->prevT4.seconds, pIntf->prevT4.nanoseconds);
    pIntf->prevT1 = pIntf->pDelayT1;
    pIntf->prevT2 = pIntf->pDelayT2;
    pIntf->prevT3 = pIntf->pDelayT3;
    pIntf->prevT4 = pIntf->pDelayT4;
    return AVB_FAILURE;
  }

#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
  ratioIn = pIntf->neighborRateRatio;
#endif

  /* Calculate deltas */
  deltaT1 = dot1asSubExtTimestamps(&pIntf->pDelayT1, &pIntf->prevT1);
  deltaT2 = dot1asSubExtTimestamps(&pIntf->pDelayT2, &pIntf->prevT2);
  deltaT3 = dot1asSubExtTimestamps(&pIntf->pDelayT3, &pIntf->prevT3);
  deltaT4 = dot1asSubExtTimestamps(&pIntf->pDelayT4, &pIntf->prevT4);
#if 0
  DOT1AS_TRACE_DEV("DOT1AS[%d] Current: T1 %llu:%u, T2 %llu:%u, T3 %llu:%u, T4 %llu:%u", avbCfgxIntfNum[intfIndex],
                      pIntf->pDelayT1.seconds, pIntf->pDelayT1.nanoseconds, pIntf->pDelayT2.seconds, pIntf->pDelayT2.nanoseconds,
                      pIntf->pDelayT3.seconds, pIntf->pDelayT3.nanoseconds, pIntf->pDelayT4.seconds, pIntf->pDelayT4.nanoseconds);
  DOT1AS_TRACE_DEV("DOT1AS[%d] Previous: T1 %llu:%u, T2 %llu:%u, T3 %llu:%u, T4 %llu:%u", avbCfgxIntfNum[intfIndex],
                      pIntf->prevT1.seconds, pIntf->prevT1.nanoseconds, pIntf->prevT2.seconds, pIntf->prevT2.nanoseconds,
                      pIntf->prevT3.seconds, pIntf->prevT3.nanoseconds, pIntf->prevT4.seconds, pIntf->prevT4.nanoseconds);
  DOT1AS_TRACE_DEV("DOT1AS[%d] neighborRateRatioIn %d", avbCfgxIntfNum[intfIndex], ratioIn);
#endif

  x1 = (UUMULT(pIntf->neighborRateRatio, deltaT1)) >> 30;
  y1 = deltaT2 - x1;


  x2 = (UUMULT(pIntf->neighborRateRatio, deltaT4)) >> 30;
  y2 = deltaT3 - x2;

  /* Update neighborRateRatio */
  pIntf->neighborRateRatio += (y1 >> PRATIO_K1);
  pIntf->neighborRateRatio += (y2 >> PRATIO_K1);

#if (DOT1AS_DEBUG_PDELAY_LOG == 1)
  dot1asDebugNbrRateRatioLog(intfIndex, deltaT1, deltaT2, deltaT3, deltaT4,
                             x1, y1, x2, y2, ratioIn, pIntf->neighborRateRatio);
#endif /* DOT1AS_DEBUG_PDELAY_LOG */

#if 0
  DOT1AS_TRACE_DEV("x1 %lld, y1 %d, x2 %lld, y2 %d, neighborRateRatio %lld \n",
                    x1,      y1,      x2,      y2,      pIntf->neighborRateRatio);
#endif

  /* Store the current T1,T2,T3,T4 timestamps as previous values */
  pIntf->prevT1 = pIntf->pDelayT1;
  pIntf->prevT2 = pIntf->pDelayT2;
  pIntf->prevT3 = pIntf->pDelayT3;
  pIntf->prevT4 = pIntf->pDelayT4;

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Compute the mean propagation delay on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS interface index
*
* @returns  none
*
* @comments This function assumes that all 4 timestamp values, T1, T2, T3 and
*           T4 are available.
*
* @end
*******************************************************************************/
void dot1asPdelayComputePropTime(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_TIMESTAMP_t *t1, *t2, *t3, *t4;
  int64_t e, e1, e2;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
  avbLogBuffer_t logEntry;
#endif

  pIntf = &pDot1asIntf_g[intfIndex];

  /* Pdelay filter algorithm */
  /* Local timestamps are 32-bit nanoseconds value. Remote timestamps have both
   * seconds and nanoseconds. The clock offsets do not matter in this calculation
   */

  t1 = &pIntf->pDelayT1;
  t2 = &pIntf->pDelayT2;
  t3 = &pIntf->pDelayT3;
  t4 = &pIntf->pDelayT4;

  e1 = (dot1asSubExtTimestamps(t4, t1) << 31);
  e2 = dot1asSubExtTimestamps(t3, t2);
  e2 = (e2 * pIntf->neighborRateRatio) << 1;

  e = (e1 - e2) - pIntf->delayEstimate;

#if 0
  DOT1AS_TRACE_DEV("DOT1AS[%d],T1,%llu,%u,T2,%llu,%u,T3,%llu,%u,T4,%llu,%u,NRR,%d,pDly,%u",
                    avbCfgxIntfNum[intfIndex],
                    t1->seconds, t1->nanoseconds, t2->seconds, t2->nanoseconds,
                    t3->seconds, t3->nanoseconds, t4->seconds, t4->nanoseconds,
                    (int32_t) pIntf->neighborRateRatio, (uint32_t) pIntf->neighborPropDelay);
#endif
#if 0
  DOT1AS_TRACE_DEV("DOT1AS[%d] delay_est %lld, e1 %lld, e2 %lld, e %lld \n",
                    avbCfgxIntfNum[intfIndex],
                    pIntf->delayEstimate, e1, e2, e);
#endif
#if 0
  DOT1AS_TRACE_DEV("DOT1AS dot1asPdelayComputePropTime: intf[%d]", avbCfgxIntfNum[intfIndex]);
  DOT1AS_TRACE_DEV("T1 %llu:%u, T2 %llu:%u, T3 %llu:%u, T4 %llu:%u\n"
                    " e1 %lld, e2 %lld, delay_est %lld (%d), ratio %d(0x%x)",
                    t1->seconds, t1->nanoseconds, t2->seconds, t2->nanoseconds, t3->seconds,
                    t3->nanoseconds, t4->seconds, t4->nanoseconds, e1, e2, pIntf->delayEstimate,
                    (uint32_t)(pIntf->delayEstimate >> 32),
                    pIntf->neighborRateRatio, pIntf->neighborRateRatio);
#endif

  /* Toss out invalid PDELAY values. Note, that there is a PDELAY threshold
   * per interface, which is verified if the PDELAY is with in reasonable limits.
   * The goal here is to ignore erroneous timestamps.
   */
  if ((e1 - e2) < 0)
  {
    DOT1AS_TRACE_PDELAY_ERR("\nDOT1AS: invalid PDELAY on port[%d], diff %lld\n",
                            avbCfgxIntfNum[intfIndex], e);
    DOT1AS_TRACE_PDELAY_ERR("DOT1AS: T1 %llu:%u, T2 %llu:%u, T3 %llu:%u, T4 %llu:%u,"
                            " e1 %lld, e2 %lld, delay_est %lld, ratio %d(0x%x)\n",
                            t1->seconds, t1->nanoseconds, t2->seconds, t2->nanoseconds, t3->seconds,
                            t3->nanoseconds, t4->seconds, t4->nanoseconds, e1, e2, pIntf->delayEstimate,
                            pIntf->neighborRateRatio, pIntf->neighborRateRatio);

    pIntf->stats.badPdelayValues++;
    /* in case of bad pdelay value force the device to use configured pdelay value in AVnu mode*/
    if (pDot1asBridge_g->avnuMode == TRUE)
    {
      uint32_t val32;
      dot1asCfgxIntfAVNUNeighborPropDelayGet(avbCfgxIntfNum[intfIndex], &val32);
      pIntf->neighborPropDelay = val32;
    }
    else
    {
      #if (DOT1AS_FORCE_PDELAY == TRUE)
      uint32_t val32;
      dot1asCfgxIntfAVNUNeighborPropDelayGet(avbCfgxIntfNum[intfIndex], &val32);
      pIntf->neighborPropDelay = val32;
      #endif
    }
    return;
  }

  if (pIntf->pDelayFilterInit == TRUE)
  {
    pIntf->pDelayFilterInit = FALSE;
    pIntf->delayEstimate = e;
  }
  else
  {
    pIntf->delayEstimate += (e >> PDELAY_K1);
  }

  pIntf->neighborPropDelay = (uint32_t)(pIntf->delayEstimate >> 32);


#if 0
  DOT1AS_TRACE_DEV("DOT1AS[%d]: delayEstimate: %lld   neighborPropDelay: %d\n",
                          avbCfgxIntfNum[intfIndex], pIntf->delayEstimate, pIntf->neighborPropDelay);
#endif

  /* Due to one erroneous timestamp, the pdelay value could be thrown off bounds.
   * And the filter would take a long time to converge as the error is large.
   * Re-init the filter to converge quickly. This has to be checked irrespective
   * of whether port is asCapable or not, as the erroneous timestamp could occur
   * any time.
   * In AVnu mode we do not want to re-init the filter.
   */
  if ((pIntf->neighborPropDelayThres < pIntf->neighborPropDelay) && (pDot1asBridge_g->avnuMode != TRUE))
  {
    dot1asIntfPdelayFilterInit(intfIndex);
  }
  if ((pIntf->isAVnuPdelayConfigSaved == FALSE) &&
      (pIntf->neighborPropDelay < pIntf->neighborPropDelayThres) &&
      (pDot1asBridge_g->avnuMode == TRUE) &&
      (pIntf->avnuOperationalPdelayTimer >= AVNU_PDELAY_OPERATIONAL_CUTOVER_TIME))
  {
    uint32_t pDelayConfig = 0;
    uint32_t intfCfgIndex;
#ifdef DOT1AS_LAG_SUPPORT
    if (avb_lag_intf_for_member_port(intfIndex, &intfCfgIndex) == AVB_FAILURE)
    {
      intfCfgIndex = intfIndex;
    }
#else
    intfCfgIndex = intfIndex;
#endif
    dot1asCfgxIntfAVNUNeighborPropDelayGet(avbCfgxIntfNum[intfCfgIndex], &pDelayConfig);
    if (abs(pIntf->neighborPropDelay - pDelayConfig) > DOT1AS_AVNU_PDELAY_DIFF_MAX)
    {
      dot1asCfgxIntfAVNUNeighborPropDelaySet(avbCfgxIntfNum[intfCfgIndex], pIntf->neighborPropDelay);
      pIntf->isAVnuPdelayConfigSaved = TRUE;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
      /* Add log entry */
      sal_memset(&logEntry, 0, sizeof(avbLogBuffer_t));
      logEntry.eventType = AVB_LOG_EVENT_PDELAY_CONFIG_CHANGE;
      logEntry.u.pDelayConfigChange.intfNum = avbCfgxIntfNum[intfIndex];
      logEntry.u.pDelayConfigChange.pDelayOld = pDelayConfig;
      logEntry.u.pDelayConfigChange.pDelayNew =  pIntf->neighborPropDelay;
      avbLogBufferEntryAdd(&logEntry);
      /* set asynchronous event */
      avbAsyncEventTypeSet(intfIndex, AVB_ASYNC_EVENT_PORT_PDELAY_CONFIG_CHANGE);
#endif
#ifdef DOT1AS_CONFIG_SAVE_SUPPORT
      if (pConfigSaveThreadId == (mos_thread_t *)NULL)
      {
        pConfigSaveThreadId = mos_thread_create("AVB/config_save", 1024, LOW_PRIO_APP, 0,
                                                 &configSaveThread, NULL);
        if (pConfigSaveThreadId == (mos_thread_t *)NULL)
        {
          DOT1AS_TRACE_ERR("%s: Failed to store pdelay value for %d, thread create failed \n",
                            __FUNCTION__, avbCfgxIntfNum[intfCfgIndex]);
          pIntf->isAVnuPdelayConfigSaved =  FALSE;
          dot1asCfgxIntfAVNUNeighborPropDelaySet(avbCfgxIntfNum[intfCfgIndex], DOT1AS_INTF_AVNU_NBR_PDELAY_DEFAULT);
        }
      }
#endif
    }
  }
}


/******************************************************************************
* @purpose  Process the received 802.1AS PDELAY_RESP_FOLLOWUP PDU.
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
AVB_RC_t dot1asPdelayRespFollowupReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu)
{
  uint8_t discard = TRUE;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_PDU_HEADER_t *pHdr;
  DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_t *pPdelayRespFollowup;
  uint32_t intfCfgIndex;
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
  avbLogBuffer_t logEntry;
#endif
  pIntf = &pDot1asIntf_g[intfIndex];
  pHdr = &pPdu->hdr;
  pPdelayRespFollowup = &pPdu->body.pDelayRespFollowupPdu;

#ifdef DOT1AS_LAG_SUPPORT
  if (avb_lag_intf_for_member_port(intfIndex, &intfCfgIndex) == AVB_FAILURE)
  {
    intfCfgIndex = intfIndex;
  }
#else
  intfCfgIndex = intfIndex;
#endif
  switch (pIntf->pDelayReqState)
  {
    case DOT1AS_PDELAY_DISABLED:
    case DOT1AS_PDELAY_REQ_SEND:
    case DOT1AS_PDELAY_WAIT_FOR_RESP:
      DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: Discarding pdelay_followup, invalid"
                              " state %d. SeqId %d, Expected %d\n", avbCfgxIntfNum[intfIndex],
                              pIntf->pDelayReqState, pHdr->sequenceId, pIntf->pDelaySeqId);
      /* Discard response if PDELAY is disabled or if we are not waiting */
      break;

    case DOT1AS_PDELAY_WAIT_FOR_RESP_FOLLOWUP:
      /* Check the Sequence ID and the requesting port id */
      if (pHdr->sequenceId == pIntf->pDelaySeqId)
      {
        /* Check whether requesting port identity in the response matches ours. */
        if ((DOT1AS_CLOCK_ID_IS_EQ(&pDot1asBridge_g->thisClock,
                                   &pPdelayRespFollowup->reqPortId.clock)) &&
            (avbCfgxIntfNum[intfIndex] == pPdelayRespFollowup->reqPortId.num))
        {
          /* Check if previous response and followup are sent by same port */
          if ((DOT1AS_CLOCK_ID_IS_EQ(&pIntf->partnerId.clock,
                                     &pHdr->portId.clock)) &&
              (pIntf->partnerId.num == pHdr->portId.num))
          {
            /* Copy the Response origin time stamp from the PDU. */
            pIntf->pDelayT3 = pPdelayRespFollowup->respOriginTimestamp;

            if ((pIntf->prevT1.nanoseconds == 0) &&
                (pIntf->prevT2.nanoseconds == 0) &&
                (pIntf->prevT3.nanoseconds == 0) &&
                (pIntf->prevT4.nanoseconds == 0))
            {
              discard = FALSE;
              pIntf->prevT1 = pIntf->pDelayT1;
              pIntf->prevT2 = pIntf->pDelayT2;
              pIntf->prevT3 = pIntf->pDelayT3;
              pIntf->prevT4 = pIntf->pDelayT4;
            }
            /* Compute Rate Ratio. */
            else if (dot1asComputeNbrRateRatio(intfIndex) == AVB_SUCCESS)
            {
              discard = FALSE;
              if (pIntf->numPdelayRespLost > pIntf->allowedLostReponses)
              {
#ifdef DOT1AS_LOG_BUFFER_SUPPORT
                /* Add log entry */
                sal_memset(&logEntry, 0, sizeof(avbLogBuffer_t));
                logEntry.eventType = AVB_LOG_EVENT_PDELAY_LOST_RESPONSE_EXCEEDED;
                logEntry.u.pDelayLostResponseExceed.intfNum = avbCfgxIntfNum[intfIndex];
                logEntry.u.pDelayLostResponseExceed.val = AVB_FALSE;
                avbLogBufferEntryAdd(&logEntry);
                /* set asynchronous event */
                avbAsyncEventTypeSet(intfIndex, AVB_ASYNC_EVENT_PORT_PDELAY_LOST_RESPONSE_RECOVERED);
#endif
              }

              pIntf->numPdelayRespLost = 0;
              pIntf->isMeasuringPdelay = TRUE;
              /* Compute mean propagation time from T1,T2,T3 and T4 timestamps */
              dot1asPdelayComputePropTime(intfIndex);

              if (pIntf->neighborPropDelay > pIntf->neighborPropDelayThres)
              {
                if (pDot1asBridge_g->avnuMode == TRUE)
                {
                  uint32_t val32;
                  dot1asCfgxIntfAVNUNeighborPropDelayGet(avbCfgxIntfNum[intfCfgIndex], &val32);
                  pIntf->neighborPropDelay = val32;
                }
                else
                {
                  #if (DOT1AS_FORCE_PDELAY == TRUE)
                  uint32_t val32;
                  dot1asCfgxIntfAVNUNeighborPropDelayGet(avbCfgxIntfNum[intfCfgIndex], &val32);
                  pIntf->neighborPropDelay = val32;
                  #endif
                }
              }

              /* Set asCapable based on delay, rate ratio and source. */
              if (DOT1AS_CLOCK_ID_IS_EQ(&pDot1asBridge_g->thisClock,
                                        &pHdr->portId.clock))
              {
                DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: PDELAY responder is this bridge."
                                        " Discard\n", avbCfgxIntfNum[intfIndex]);
              }
              else if (pIntf->neighborPropDelay < pIntf->neighborPropDelayThres)
              {
                if (pIntf->dot1asCapable == FALSE)
                {
#ifdef DOT1AS_LAG_SUPPORT
                  if (avb_lag_state_update_port(intfIndex, AVB_LAG_PORT_UPDATE_OP_AS_PDELAY_ENABLE) == AVB_SUCCESS)
#endif
                  {
                    if (pDot1asBridge_g->avnuMode == FALSE)
                    {
                      DOT1AS_PDELAY_TRACE_AS_CAPABLE(avbCfgxIntfNum[intfIndex], TRUE, "");
                      pIntf->dot1asCapable = TRUE;
                    }
                    /* asCapable property is set, enable BMCA/Timesync state machine */
                    dot1asInvokeBmca(intfIndex, DOT1AS_BMC_INTF_EVENT, NULL);
                  }
                }
              }
              else
              {
                if (pIntf->dot1asCapable == TRUE)
                {
                  DOT1AS_PDELAY_TRACE_AS_CAPABLE(avbCfgxIntfNum[intfIndex], FALSE, "Delay out of bound");

                  pIntf->dot1asCapable = FALSE;
#ifdef DOT1AS_LAG_SUPPORT
                  avb_lag_state_update_port(intfIndex, AVB_LAG_PORT_UPDATE_OP_AS_PDELAY_DISABLE);
#endif

                  /* asCapable property is reset, disable BMCA/Timesync */
                  dot1asInvokeBmca(intfIndex, DOT1AS_BMC_INTF_EVENT, NULL);
                }
              }
            }
            else
            {
              DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: timestamp rolled over. Discard\n", avbCfgxIntfNum[intfIndex]);
            }
            /* Received followup, update the state to pdelay request send */
            DOT1AS_TRACE_PDELAY("DOT1AS[%d] Received followup. Moving to send_req\n",
                                avbCfgxIntfNum[intfIndex]);
            pIntf->pDelayReqState = DOT1AS_PDELAY_REQ_SEND;

          } /* else, Discard response if 2 different partners responded */
          else
          {
            DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: responder does not match partner."
                                    " Discard\n", avbCfgxIntfNum[intfIndex]);
          }
        }
        else /* else, Discard response if we didn't request for it */
        {
          DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: requesting port id does not match."
                                  " Discard\n", avbCfgxIntfNum[intfIndex]);
        }
      } /* else, Discard response if sequence ID doesn't match */
      else
      {
        DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: Discarding pdelay_followup, seq_id "
                   "mismatch Exp: %d, Rcvd: %d\n", avbCfgxIntfNum[intfIndex], pIntf->pDelaySeqId,
                   pHdr->sequenceId);
      }
      break;

    default:
      DOT1AS_TRACE_PDELAY_ERR("DOT1AS[%d]: Discarding pdelay_followup, default state\n",
                              avbCfgxIntfNum[intfIndex]);
      break;
  }
  if (discard == TRUE)
  {
    pIntf->stats.pDelayRespRxDiscards++;
  }

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Send a PDELAY_REQ pdu out of the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  AVB_RC_t
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPdelayReqSend(uint32_t intfIndex)
{
  uint8_t  *pdu = NULL;
  int32_t   interval;
  uint16_t  hdrsize = 0;
  uint16_t  flags = 0;
  DOT1AS_INTF_STATE_t *pIntf;
  uint16_t  pduLen = 0;

  pIntf = &pDot1asIntf_g[intfIndex];

  /* Get a frag from ethernet driver */
  pdu = &dot1asTxBuffer[0];
  sal_memset(dot1asTxBuffer, 0, DOT1AS_MAX_PDU_SIZE);

  /* Add DA, SA, and EthType */
  if (dot1asPduAddEnetHeader(intfIndex, pdu, &hdrsize) != AVB_SUCCESS)
  {
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* Add 802.1AS header */
  hdrsize = 0;
  flags = DOT1AS_CLK_PTP_TIMESCALE_FLAG;

  interval = pIntf->currentLogPdelayReqInterval;

  if (dot1asPduAddCommonHeader(intfIndex, pdu, DOT1AS_PDU_PDELAY_REQ,
                               pIntf->pDelaySeqId, 0, (int8_t)interval,
                               flags, &hdrsize) != AVB_SUCCESS)
  {
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;
  /* PDELAY Request body has 2 reserved fields of 10bytes each */
  sal_memset(pdu, 0x00, 20);
  pduLen += 20;

  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to trasmit on %d\n", __FUNCTION__, avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  /* Wait for response */
  pIntf->pDelayReqState = DOT1AS_PDELAY_WAIT_FOR_RESP;

  /* All is good */
  pIntf->stats.pDelayReqTxCount++;

  /* Get the egress timestamp */
  if (dot1asIntfTimestampGet(intfIndex, DOT1AS_PDU_PDELAY_REQ,
                             pIntf->pDelaySeqId, DOT1AS_TX_TIMESTAMP,
                             &pIntf->pDelayT1) != AVB_SUCCESS)
  {
    DOT1AS_TRACE_ERR("%s: failed to get egress timestamp on %d\n", __FUNCTION__,
                     avbCfgxIntfNum[intfIndex]);
    pDot1asIntf_g[intfIndex].stats.tsErrors++;
    return AVB_FAILURE;
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose PDELAY Request Initiator state machine
*
* @param    intfIndex   @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments Called every timer tick interval, if PDELAY is enabled
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPdelayInitiatorMachine(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint8_t  intervalExpired = FALSE;

  pIntf = &pDot1asIntf_g[intfIndex];

  if (pIntf->pDelayReqState == DOT1AS_PDELAY_DISABLED)
  {
    return AVB_SUCCESS;
  }

  if (pIntf->pDelayIntervalTimerReset == TRUE)
  {
    pIntf->pDelayIntervalTimerReset = FALSE;
    if(pIntf->pDelaySeqId == DOT1AS_PDELAY_REQ_PDU_START_SEQUANCE_ID)
    {
      /* Send the first pDelay Request message immediately */
      pIntf->pDelayIntervalTimer = pIntf->meanPdelayReqInterval;
    }
    else
    {
      pIntf->pDelayIntervalTimer = 0;
      return AVB_SUCCESS;
    }
  }

  /* Increment running counter and check for time elapsed */
  pIntf->pDelayIntervalTimer += DOT1AS_TIMER_TICK_US;
  intervalExpired = (pIntf->pDelayIntervalTimer >=
                     pIntf->meanPdelayReqInterval) ? TRUE : FALSE;

  if ((pDot1asBridge_g->avnuMode == TRUE) &&
      (pIntf->avnuOperationalPdelayTimer < AVNU_PDELAY_OPERATIONAL_CUTOVER_TIME) &&
      (pIntf->avnuOperationalPdelayTimer != 0))
  {
    pIntf->avnuOperationalPdelayTimer += DOT1AS_TIMER_TICK_US;
    if ((pIntf->avnuOperationalPdelayTimer >= AVNU_PDELAY_OPERATIONAL_CUTOVER_TIME))
    {
      int8_t val;
      uint32_t intfCfgIndex;
#ifdef DOT1AS_LAG_SUPPORT
      if (avb_lag_intf_for_member_port(intfIndex, &intfCfgIndex) == AVB_FAILURE)
      {
        intfCfgIndex = intfIndex;
      }
#else
      intfCfgIndex = intfIndex;
#endif
      dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalGet(avbCfgxIntfNum[intfCfgIndex], &val);
      pIntf->currentLogPdelayReqInterval = val;
      dot1asIntervalLog2Usec(val, &pIntf->meanPdelayReqInterval);
      dot1asIntervalUsecAdjust(&pIntf->meanPdelayReqInterval);
    }
  }

  /* Check whether interval expired */
  if (intervalExpired == TRUE)
  {
    DOT1AS_TRACE_PDELAY("DOT1AS[%d]: pdelay timer expired at time %llu, state %d\n",
                        avbCfgxIntfNum[intfIndex], 1000 * sal_time_usecs(), pIntf->pDelayReqState);
    /* Reset timer */
    pIntf->pDelayIntervalTimer = 0;

    /* Check for Timeout */
    if ((pIntf->pDelayReqState == DOT1AS_PDELAY_WAIT_FOR_RESP) ||
        (pIntf->pDelayReqState == DOT1AS_PDELAY_WAIT_FOR_RESP_FOLLOWUP))
    {
      pIntf->stats.pDelayReceiptTimeouts++;
      pIntf->stats.ptpDiscardCount++;

      dot1asIntfPdelayReInit(intfIndex);
    }

    /* Send the request, both when interval expires or timeout occurs */
    if (pIntf->pDelayReqState == DOT1AS_PDELAY_REQ_SEND)
    {
      pIntf->pDelaySeqId++;

      sal_memset(&pIntf->pDelayT1, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
      sal_memset(&pIntf->pDelayT2, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
      sal_memset(&pIntf->pDelayT3, 0x00, sizeof(DOT1AS_TIMESTAMP_t));
      sal_memset(&pIntf->pDelayT4, 0x00, sizeof(DOT1AS_TIMESTAMP_t));

      DOT1AS_TRACE_TX("DOT1AS PDU event: Sending PDELAY_REQ on intf %d\n",
                      avbCfgxIntfNum[intfIndex]);

      if (dot1asPdelayReqSend(intfIndex) == AVB_SUCCESS)
      {
        if ((pDot1asBridge_g->avnuMode == TRUE) && (pIntf->avnuOperationalPdelayTimer == 0))
        {
          pIntf->avnuOperationalPdelayTimer += DOT1AS_TIMER_TICK_US;
        }
        DOT1AS_TRACE_PDELAY("DOT1AS[%d] Req sent with seq_id %d at %llu\n",
                            avbCfgxIntfNum[intfIndex], pIntf->pDelaySeqId, 1000 * sal_time_usecs());
      }
      else
      {
        DOT1AS_TRACE_PDELAY_ERR("DOT1AS: failed to sending PDELAY_REQ on intf %d\n",
                                avbCfgxIntfNum[intfIndex]);
        /* There was some error sending the PDELAY request */
        dot1asIntfPdelayReInit(intfIndex);
      }
    }
  }

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Check PDELAY timers on each of the interfaces
*
* @param    none
*
* @returns  none
*
* @comments Called every timer tick interval
*
* @end
*******************************************************************************/
void dot1asCheckPdelayTimers(void)
{
  uint32_t intfIndex = 0;
  DOT1AS_INTF_STATE_t *pIntf;

  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];

    if (dot1asIsPdelayCapable(intfIndex) == TRUE)
    {
      if (pDot1asBridge_g->avnuMode == TRUE)
      {
        if (pIntf->dot1asCapable == FALSE)
        {
          AVNU_TRACE_ASCAPABLE(avbCfgxIntfNum[intfIndex], TRUE, "");
          pIntf->dot1asCapable = TRUE;
        }
      }

      if (pIntf->pDelayReqState == DOT1AS_PDELAY_DISABLED)
      {
        dot1asIntfPdelayInit(intfIndex);
      }
      /* Run PDelay Request state machine */
      dot1asPdelayInitiatorMachine(intfIndex);
    }
    else
    {
      if (pIntf->pDelayReqState != DOT1AS_PDELAY_DISABLED)
      {
        dot1asIntfPdelayDisable(intfIndex);
      }
    }
  }
}

/******************************************************************************
* @purpose  Get the updated TOD for the time stamp captured PTP packets
*
* @param    timestamp  @b{(input)} Ingress/Egress timestamp captured on PTP pkt
* @param    tod        @b{(in/out)} Time of Day value
*
* @returns  none
*
* @comments
*          Compare the lower 32bit TS with the tod timestamp,and add the
*          difference in nsec to the ToD.
*
* @end
*******************************************************************************/
void dot1asTimestampUpdate(uint32_t timestamp, cbx_ts_time_t *tod)
{
  uint64_t diff;
  uint64_t diffSec;
  uint64_t diffNsec;
  uint8_t diffAdd;
  uint64_t nsec;
  uint64_t todTs;
  uint32_t todTsLow;

  if (timestamp == 0)
  {
    return;
  }
  if(cbx_ts_tod_get(tod, &todTs) == 0)
  {
    todTsLow = todTs & 0xffffffff;

    if (timestamp >= todTsLow)
    {
      diff = timestamp - todTsLow;
      if (diff > 4000000000)
      {
        diff =  0xffffffff - timestamp + todTsLow;
        diffAdd = 0;
      } else
      {
        diffAdd = 1;
      }
    }
    else
    {
      diff = todTsLow - timestamp;
      if (diff > 4000000000)
      {
        diff =  0xffffffff - todTsLow + timestamp;
        diffAdd = 1;
      }
      else
      {
        diffAdd = 0;
      }
    }
    if (diffAdd)
    {
      nsec = (uint64_t)tod->nsec + diff;
      tod->nsec = nsec % (uint64_t)1e9;
      tod->sec += nsec/(uint64_t)1e9;
    }
    else
    {
      diffNsec = diff % (uint64_t)1e9;
      diffSec = diff / (uint64_t)1e9;
      if (tod->nsec < diffNsec)
      {
        tod->sec--;
        nsec = (uint64_t)tod->nsec + (uint64_t)1e9 - diffNsec;
        tod->nsec = (uint32_t) nsec;
      }
      else
      {
        tod->nsec -= (uint32_t) diffNsec;
      }
      tod->sec -= diffSec;
    }
  }
}


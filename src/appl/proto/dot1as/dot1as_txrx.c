/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_txrx.c
 */

#include "osal/sal_util.h"
#include "osal/sal_console.h"
#include "osal/sal_alloc.h"
#include "osal/sal_time.h"
#include "shared/types.h"
#include "fsal/ptp.h"
#include "fsal/ts.h"
#include "dot1as.h"

/* Timestamps to profile processing of SYNC and FOLLOWUP packets */
dot1asTime_t dot1asDbgSyncT1 = 0;
dot1asTime_t dot1asDbgSyncT2 = 0;
dot1asTime_t dot1asDbgFollowupT1 = 0;
dot1asTime_t dot1asDbgFollowupT2 = 0;

AVB_RC_t dot1asFollowUpPduParse(uint8_t *data, uint16_t dataLen,
                            DOT1AS_FOLLOWUP_PDU_t *pFollowUp);

AVB_RC_t dot1asAnnouncePduParse(uint8_t *data, uint16_t dataLen,
                            DOT1AS_ANNOUNCE_PDU_t *pAnnounce);

AVB_RC_t dot1asSignalingPduParse(uint8_t *data, uint16_t dataLen,
                             DOT1AS_SIGNALING_PDU_t *pSignalingPdu);

/**************************************************************************
*
* @purpose  Convert network byte order to host byte order in long integer
*
* @param    netlong   @b{(input)} network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/

uint64_t  ntohll(uint64_t netlong)
{
 uint64_t hostlong;

#ifdef BIG_ENDIAN
  hostlong = netlong;
#else
  hostlong = (((unsigned long long)(sal_ntohl((int)((netlong << 32) >> 32))) << 32) |
                     (unsigned int)sal_ntohl(((int)(netlong >> 32))));
#endif

  return hostlong;
}

/**************************************************************************
*
* @purpose  Convert host byte order to network byte order in long integer
*
* @param    hostlong   @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/

uint64_t  htonll(uint64_t hostlong)
{
  uint64_t netlong;

#ifdef BIG_ENDIAN
  netlong = hostlong;
#else
  netlong = (((unsigned long long)(sal_ntohl((int)((hostlong << 32) >> 32))) << 32) |
                     (unsigned int)sal_ntohl(((int)(hostlong >> 32))));
#endif

  return netlong;
}


void dumpBuffer(char *descr, uint8_t *data, uint32_t len)
{
    uint32_t i;
    sal_printf("%s: %d bytes", descr, len);
    for (i = 0; i < len; ++i) {
        sal_printf("%c%02x", (i % 16) ? ' ' : '\n', data[i] & 0xff);
    }
    sal_printf("\n");
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
void dot1asIntervalUsAdjust(uint64_t *us)
{
  uint64_t val;
  uint64_t tick;
  uint64_t diff;

  val = *us;
  tick = DOT1AS_TIMER_TICK_US;
  if (val < DOT1AS_TIMER_TICK_US)
  {
    return;
  }
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
* @purpose  Routine to pack the 48bit seconds value in the network order
*
* @param    pdu @b{(output)} pointer to the PDU at timestamp offset
* @param    seconds @b{(input)} seconds value to be stored
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asPackUint48(uint8_t *pdu, uint64_t seconds)
{
  pdu[0] = (uint8_t)((seconds >> 40) & 0xFF);
  pdu[1] = (uint8_t)((seconds >> 32) & 0xFF);
  pdu[2] = (uint8_t)((seconds >> 24) & 0xFF);
  pdu[3] = (uint8_t)((seconds >> 16) & 0xFF);
  pdu[4] = (uint8_t)((seconds >> 8) & 0xFF);
  pdu[5] = (uint8_t)(seconds & 0xFF);
}

/******************************************************************************
* @purpose Routine to unpack the 48bit seconds timestamp value from the message
*
* @param    pdu @b{(input)} pointer to the PDU at timestamp offset
* @param    seconds @b{(output)} seconds value
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asUnpackUint48(uint8_t *pdu, uint64_t *seconds)
{
  uint64_t val;

  val = (uint64_t)pdu[0] << 40;
  val |= ((uint64_t)pdu[1] << 32);
  val |= ((uint64_t)pdu[2] << 24);
  val |= ((uint64_t)pdu[3] << 16);
  val |= ((uint64_t)pdu[4] << 8);
  val |= (uint64_t)pdu[5];

  *seconds = val;
}


/******************************************************************************
* @purpose  Convert the given logarithm base 2 interval to microsecond value
*
* @param    logInterval @b{(input)} interval specified in log base 2 format
* @param    uSec @b{(output)} interval specified in microseconds
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntervalLog2Usec(int32_t logInterval, uint64_t *uSec)
{
  /* Check for reserved values of interval */
  if (DOT1AS_IS_INT_LOG2_RESERVED(logInterval))
  {
    return AVB_FAILURE;
  }

  /* Check if the interval value is meant to disable transmission */
  if (logInterval == DOT1AS_SIGNALING_MSG_INT_REQ_STOP)
  {
    *uSec = ~0; /* Do not transmit */
    return AVB_SUCCESS;
  }

  /* Check for minimum and maximum log base 2 interval values.
   * Note:The current implementation supports only a subset of interval values.
   * The min/max values are as defined by specification are different.
   */
  if ((logInterval < DOT1AS_PDU_TX_INTL_LOG2_MIN) ||
      (logInterval > DOT1AS_PDU_TX_INTL_LOG2_MAX))
  {
    return AVB_FAILURE;
  }

  if (logInterval < 0)
  {
    logInterval = -(logInterval);
    *uSec = (uint64_t)(1000 * 1000) / (1 << logInterval);
  }
  else
  {
    *uSec = (uint64_t)(1000 * 1000) * (1 << logInterval);
  }

  return AVB_SUCCESS;
}



/******************************************************************************
* @purpose  Convert the given microsecond interval to logarithm base 2 value
*
* @param    uSec  @b{(input)} interval specified in microsecond
* @param    logInterval @b{(output)} interval specified in log base 2 format
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntervalUsec2Log(uint64_t uSec, int32_t *logInterval)
{
  int32_t i;
  uint32_t val32;

  if (uSec < DOT1AS_INTL_SEC_TO_US(1)) /* If less than one-second */
  {
    val32 = (uint32_t)(DOT1AS_INTL_SEC_TO_US(1) / uSec);
  }
  else
  {
    val32 = (uint32_t)(uSec / DOT1AS_INTL_SEC_TO_US(1));
  }

  /* Convert the per-second value to log base 2 value */
  for (i = (sizeof(val32)*8 - 1); i >= 0; i--)
  {
    if (val32 & (1 << i))  /* interval is specified in power of 2 values */
    {
      break;
    }
  }

  *logInterval = (uSec < DOT1AS_INTL_SEC_TO_US(1)) ? -(i) : i;

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose   Add common ENET header to the PDU being transmitted
*
* @param     intIfIndex    @b{(input)} Internal interface index
* @param     pdu           @b{(input)} handle to the PDU buffer
* @param     len           @b{(output)} length of header added
*
* @returns   AVB_RC_t
*
* @comments
*
* @end
*******************************************************************************/
uint32_t dot1asPduAddEnetHeader(uint32_t intIfIndex,
                                uint8_t *pdu, uint16_t *len)
{
  uint32_t offset = 0;
  uint16_t val16;

  /* Copy Destination MAC - 6 bytes */
  sal_memcpy(pdu, ETHER_MAC_DOT1AS_g, ETHER_ADDR_LEN);
  offset += ETHER_ADDR_LEN;

  /* Copy Source MAC - 6 bytes */
  sal_memcpy(pdu+offset, dot1asEthSrcAddr, ETHER_ADDR_LEN);
  offset += ETHER_ADDR_LEN;

  /* Copy EtherType - 2 bytes */
  val16 = ETHER_PROTO_DOT1AS;
  val16 = sal_htons(val16);

  sal_memcpy(pdu+offset, (uint8_t *)&val16, sizeof(val16));
  offset += sizeof(val16);

  *len = offset;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose   Get the length of the PDU based on the type
*
* @param     type   @b{(input)} PDU type
*
* @returns   Length of the PDU
*
* @comments
*
* @end
*******************************************************************************/
uint16_t dot1asPduLengthGet(uint32_t type)
{
  uint16_t len = 0;
  switch(type)
  {
    case DOT1AS_PDU_SYNC:
      len = DOT1AS_SYNC_PDU_LEN;
      break;
    case DOT1AS_PDU_FOLLOWUP:
      len = DOT1AS_FOLLOWUP_PDU_LEN;
      break;
    case DOT1AS_PDU_PDELAY_REQ:
      len = DOT1AS_PDELAY_REQ_PDU_LEN;
      break;
    case DOT1AS_PDU_PDELAY_RESP:
      len = DOT1AS_PDELAY_RESP_PDU_LEN;
      break;
    case DOT1AS_PDU_PDELAY_RESP_FOLLOWUP:
      len = DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_LEN;
      break;
    case DOT1AS_PDU_ANNOUNCE:
      len = DOT1AS_ANNOUNCE_PDU_LEN; /* With out path trace TLV */
      break;
    case DOT1AS_PDU_SIGNALING:
      len = DOT1AS_SIGNALING_PDU_LEN;
    default:
      break;
  }

  len += DOT1AS_PDU_HEADER_LEN;
  return len;
}


/******************************************************************************
* @purpose   Set the length of the PDU
*
* @param     pdu   @b{(input)} pointer to PDU
* @param     len   @b{(input)} Length of the PDU
*
* @returns   none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asPduLengthSet(uint8_t *pdu, uint16_t len)
{
  uint16_t val16;
  uint16_t ofst = (ETHER_HEADER_LEN + 2);
  val16 = sal_htons(len);
  sal_memcpy(pdu+ofst, (uint8_t *)&val16, sizeof(val16));
}


/******************************************************************************
* @purpose   Add common 802.1AS header to the PDU being transmitted
*
* @param     intIfIndex    @b{(input)} DOT1AS internal interface index
* @param     pdu           @b{(input)} handle to the PDU buffer
* @param     type          @b{(input)} PDU type
* @param     correction    @b{(input)} Value of correction field
* @param     len           @b{(output)} length of header added
* @param     interval      @b{(output)} msg transmission interval
*
* @returns   AVB_RC_t
*
* @comments  The PDU buffer length is assumed to be long enough to fit header
*
* @end
*******************************************************************************/
uint32_t dot1asPduAddCommonHeader(uint32_t   intIfIndex,
                                   uint8_t  *pdu,
                                   uint32_t  type,
                                   uint16_t  seqId,
                                   int64_t   correction,
                                   int8_t    interval,
                                   uint16_t  flags,
                                   uint16_t *len)
{
  uint16_t offset = 0;
  uint8_t val8;
  uint16_t val16;
  int64_t val64;

  /* PDU Type + transportSpecific bit, 1byte*/
  val8 = (uint8_t)(type & DOT1AS_PDU_TYPE_MASK);
#if DEV_BUILD_DOT1AS
  if (avbDot1asTest_g.isTSValWrong == TRUE)
  {
    val8 |= (2 << DOT1AS_PDU_TRANSPORT_SHIFT); /* wrong transportSpecific val */
  }
  else
  {
#endif
    val8 |= (1 << DOT1AS_PDU_TRANSPORT_SHIFT); /* transportSpecific bit */
#if DEV_BUILD_DOT1AS
  }
#endif
  *pdu = val8;
  offset++;

  /* PDU version, 1byte*/
  *(pdu+offset) = (uint8_t)(DOT1AS_PDU_VERSION & DOT1AS_PDU_VER_MASK);
  offset++;

  /* PDU length, 2bytes */
  val16 = dot1asPduLengthGet(type);
  val16 = sal_htons(val16);
  sal_memcpy(pdu+offset, (uint8_t *)&val16, sizeof(val16));
  offset += sizeof(val16);

  /* Domain number, 1byte */
  *(pdu+offset) = (uint8_t)(DOT1AS_GPTP_DOMAIN_NUM);
  offset++;

  /* Reserved, 1byte */
  *(pdu+offset) = (uint8_t)(0x00);
  offset++;

  /* Flags, 2bytes */
  val16 = sal_htons(flags);
  sal_memcpy(pdu+offset, (uint8_t *)&val16, sizeof(val16));
  offset += sizeof(val16);

  /* Correction field 8bytes */
  val64 = htonll(correction);
  sal_memcpy(pdu+offset, (uint8_t *)&val64, sizeof(val64));
  offset += sizeof(val64);

  /* Reserved, 4bytes */
  sal_memset(pdu+offset, 0x00, sizeof(uint32_t));
  offset += sizeof(uint32_t);

  /* Source port identity, 10 bytes */
  sal_memcpy(pdu+offset, pDot1asBridge_g->thisClock.id, DOT1AS_CLOCK_ID_LEN);
  offset += DOT1AS_CLOCK_ID_LEN;

  /* Increment the port number by 1 in the messages to make it compliant with
   * 802.1AS port numbering which starts with 1
   */
  val16 = sal_htons(intIfIndex + 1);
  sal_memcpy(pdu+offset, (uint8_t *)&val16, sizeof(val16));
  offset += sizeof(val16);

#if DEV_BUILD_DOT1AS
  if (((avbDot1asTest_g.isFollowupSeqIdWrong == TRUE) &&
       (type == DOT1AS_PDU_FOLLOWUP)) ||
      ((avbDot1asTest_g.isPdelayRespSeqIdWrong == TRUE) &&
       (type == DOT1AS_PDU_PDELAY_RESP)) ||
      ((avbDot1asTest_g.isPdelayRespFollowupSeqIdWrong == TRUE) &&
      (type == DOT1AS_PDU_PDELAY_RESP_FOLLOWUP)))
  {
    /* Sequence Id, 2bytes */
    val16 = sal_htons(seqId - 2);
  }
  else
  {
#endif
    /* Sequence Id, 2bytes */
    val16 = sal_htons(seqId);
#if DEV_BUILD_DOT1AS
  }
#endif
  sal_memcpy(pdu+offset, (uint8_t *)&val16, sizeof(val16));
  offset += sizeof(val16);

  /* Control Field, 1bytes */
  if (type == DOT1AS_PDU_SYNC)
  {
    val8 = DOT1AS_SYNC_CONTROL_TYPE;
  }
  else if (type == DOT1AS_PDU_FOLLOWUP)
  {
    val8 = DOT1AS_FOLLOWUP_CONTROL_TYPE;
  }
  else
  {
    val8 = DOT1AS_OTHER_CONTROL_TYPE;
  }

  *(pdu+offset) = val8;
  offset++;

  /* Mean PDU interval, 1byte */
  *(pdu+offset) = interval;
  offset++;

  *len = offset;
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the time stamp value of the 802.1AS PDU
*
* @param    intfIndex  @b{(input)} internal interface index
* @param    msgType    @b{(input)} Message type
* @param    seqId      @b{(input)} Sequence Id of the message
* @param    rxtx       @b{(input)} Message is sent or received (tx=1, rx=0)
* @param    pTimestamp @b{(output)}Timestamp value
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfTimestampGet(uint32_t intfIndex, uint32_t msgType,
                            uint32_t seqId, uint8_t rxtx,
                            DOT1AS_TIMESTAMP_t *pTimestamp)
{
  AVB_RC_t rc = AVB_FAILURE;
  cbx_ts_time_t tod;

  rc = cbx_ptp_seqid_timestamp_get(intfIndex, seqId, &pTimestamp->nanoseconds);
  if (rc == AVB_SUCCESS)
  {
    dot1asTimestampUpdate(pTimestamp->nanoseconds, &tod);
    pTimestamp->seconds = tod.sec;
    pTimestamp->nanoseconds = tod.nsec;
  }
  else
  {
    DOT1AS_TRACE_ERR("Time stamp Error: Intf %d Message type %d Seq ID %d\n",
                      intfIndex, msgType, seqId);
  }
  return rc;
}

/******************************************************************************
* @purpose   Convert the packet from network byte order (Big-endian) to host
*            host byte order. Also, handle alignment of individual PDU fields
*
* @param     pPktData   @b{(input)} pointer to the rx buffer
* @param     dataLen    @b{(input)} length of the rx buffer
* @param     pIntfIndex @b{(output)} incoming interface index
* @param     pPdu       @b{(output)} the handle to the 802.1AS PDU
*
* @returns   AVB_SUCCESS
* @returns   AVB_FAILURE
*
* @comments  Do not type-cast any of the fields directly from the rxBuf (to
*            avoid alignment issues).
*
* @end
*******************************************************************************/
AVB_RC_t dot1asPduParse(uint8_t *pPktData, uint32_t dataLen, DOT1AS_PDU_t *pPdu)
{
  AVB_RC_t      rc = AVB_SUCCESS;
  uint8_t   tmp8;
  uint16_t  tmp16;
  uint32_t  tmp32;
  uint64_t  tmp64;
  uint16_t  ethType = ETHER_PROTO_DOT1AS;
  DOT1AS_PDU_HEADER_t *pHdr;

  /* Check for length */
  if (dataLen <= (ETHER_HEADER_LEN))
  {
    DOT1AS_TRACE_DBG("%s: invalid datalen %u\n", __FUNCTION__, dataLen);
    return AVB_FAILURE;
  }

  /* Skip the DA and SA mac */
  pPktData += ETHER_ADDR_LEN;
  pPktData += ETHER_ADDR_LEN;

  /* Check the ethertype of the frame */
  sal_memcpy((uint8_t *)&tmp16, pPktData, sizeof(tmp16));
  tmp16 = sal_ntohs(tmp16);
  pPktData += sizeof(tmp16);

  if (sal_memcmp(&tmp16, &ethType, sizeof(tmp16)) != 0)
  {
    DOT1AS_TRACE_DBG("%s: invalid ethertype 0x%x\n", __FUNCTION__, tmp16);
    return AVB_FAILURE;
  }

  /* Parse the 802.1AS header fields */
  pHdr = &pPdu->hdr;

  if ((dataLen - ETHER_HEADER_LEN) < DOT1AS_PDU_HEADER_LEN)
  {
    DOT1AS_TRACE_DBG("%s: invalid datalen %u\n", __FUNCTION__, dataLen);
    return AVB_FAILURE;
  }

  dataLen = dataLen - ETHER_HEADER_LEN;

  /* First byte represents 4bit transport and 4bit pduType */
  tmp8 = *pPktData;
  pHdr->pduType = (tmp8 & DOT1AS_PDU_TYPE_MASK);
  tmp8 = (tmp8 & DOT1AS_PDU_TRANSPORT_MASK);
  pHdr->transportSpecific = (tmp8 >> DOT1AS_PDU_TRANSPORT_SHIFT);
  if (pHdr->transportSpecific != 1)
  {
    DOT1AS_TRACE_DBG("%s: invalid transportSpecific value %d\n",
                      __FUNCTION__, pHdr->transportSpecific);
    return AVB_FAILURE;
  }
  pPktData++; /* Move to next raw byte */

  /* Second byte represents 4bit 802.1AS version and 4bit reserved field */
  pHdr->version =  (*pPktData & DOT1AS_PDU_VER_MASK);
  pPktData++; /* Move to next raw byte */

  /* Next 2bytes represent PDU length */
  sal_memcpy(&tmp16, pPktData, sizeof(tmp16));
  pHdr->pduLen = sal_ntohs(tmp16);
  pPktData += sizeof(tmp16);

  if ((pHdr->pduLen < DOT1AS_PDU_HEADER_LEN) ||
      (pHdr->pduLen > DOT1AS_MAX_PDU_SIZE))
  {
    DOT1AS_TRACE_DBG("%s: invalid pdu len %d\n", __FUNCTION__, pHdr->pduLen);
    return AVB_FAILURE;
  }

  /* Next 1byte represents domain number */
  pHdr->domain = *(uint8_t *)pPktData;
  pPktData++;

  /* Next 1byte is reserved */
  pPktData++;

  /* Next 2bytes represent flags */
  sal_memcpy(&tmp16, pPktData, sizeof(tmp16));
  pHdr->flags = sal_ntohs(tmp16);
  pPktData += sizeof(tmp16);

  /* Next 8bytes represent correction field */
  sal_memcpy(&tmp64, pPktData, sizeof(tmp64));
  pHdr->correctionFld = ntohll(tmp64);
  pPktData += sizeof(tmp64);

  /* Next 4bytes are reserved */
  pPktData += sizeof(tmp32);

  /* Next 10bytes represent source port id */
  sal_memcpy(pHdr->portId.clock.id, pPktData, DOT1AS_CLOCK_ID_LEN);
  pPktData += DOT1AS_CLOCK_ID_LEN;

  sal_memcpy(&tmp16, pPktData, sizeof(tmp16));
  pHdr->portId.num = sal_ntohs(tmp16);
  /* Decrement the port number received in the messages by 1 to map it
   * with internal port numbering which starts with 0
   */
  pHdr->portId.num--;
  pPktData += sizeof(tmp16);

  /* Next 2bytes represent sequence id */
  sal_memcpy(&tmp16, pPktData, sizeof(tmp16));
  pHdr->sequenceId = sal_ntohs(tmp16);
  pPktData += sizeof(tmp16);

  /* Next 1byte represents control field */
  pHdr->control = *pPktData;
  pPktData++;

  /* Next 1byte represents mean message interval */
  pHdr->pduInterval = *(int8_t *)pPktData;
  pPktData++;

  /* Now, Parse the individual PDU types */
  switch (pHdr->pduType)
  {
    case DOT1AS_PDU_SYNC:
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_SYNC_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_SYNC_PDU_LEN)))
      {
        /* Parse SYNC PDU */
        DOT1AS_SYNC_PDU_t *pSync;

        pSync = &pPdu->body.syncPdu;
        sal_memcpy(pSync->reserved, pPktData, DOT1AS_SYNC_PDU_LEN);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid SYNC len %d\n", __FUNCTION__, pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;

    case DOT1AS_PDU_FOLLOWUP:
      /* Followup TLV is mandatory. Must check for length */
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_FOLLOWUP_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_FOLLOWUP_PDU_LEN)))
      {
        /* Parse FOLLOWUP PDU */
        DOT1AS_FOLLOWUP_PDU_t *pFollowUp;
        pFollowUp = &pPdu->body.followUpPdu;
        dot1asFollowUpPduParse(pPktData, dataLen, pFollowUp);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid FOLLOWUP len %d\n", __FUNCTION__, pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;
    case DOT1AS_PDU_PDELAY_REQ:
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_PDELAY_REQ_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_PDELAY_REQ_PDU_LEN)))
      {
        /* Parse PDELAY_REQ PDU */
        DOT1AS_PDELAY_REQ_PDU_t *pPdelayReqPdu;

        pPdelayReqPdu =  &pPdu->body.pDelayReqPdu;
        sal_memcpy(pPdelayReqPdu->reserved1, pPktData, 10);
        sal_memcpy(pPdelayReqPdu->reserved2, pPktData+10, 10);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid PDELAY_REQ len %d\n", __FUNCTION__,
                         pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;

    case DOT1AS_PDU_PDELAY_RESP:
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_PDELAY_RESP_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_PDELAY_RESP_PDU_LEN)))
      {
        /* Parse PDELAY_RESP PDU */
        DOT1AS_PDELAY_RESP_PDU_t *pPdelayRespPdu;

        pPdelayRespPdu =  &pPdu->body.pDelayRespPdu;

        dot1asUnpackUint48(pPktData, &pPdelayRespPdu->reqReceiptTimestamp.seconds);
        pPktData += 6;

        sal_memcpy(&tmp32, pPktData, sizeof(tmp32));
        pPdelayRespPdu->reqReceiptTimestamp.nanoseconds = sal_ntohl(tmp32);
        pPktData += sizeof(tmp32);

        sal_memcpy(pPdelayRespPdu->reqPortId.clock.id, pPktData, DOT1AS_CLOCK_ID_LEN);
        pPktData += DOT1AS_CLOCK_ID_LEN;

        sal_memcpy(&tmp16, pPktData, sizeof(tmp16));
        pPdelayRespPdu->reqPortId.num = sal_ntohs(tmp16);
        /* Decrement the port number received in the messages by 1 to map it
         * with internal port numbering which starts with 0
         */
        pPdelayRespPdu->reqPortId.num--;
        pPktData += sizeof(tmp16);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid PDELAY_RESP len %d\n", __FUNCTION__,
                         pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;

    case DOT1AS_PDU_PDELAY_RESP_FOLLOWUP:
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN +
                           DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN +
                       DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_LEN)))
      {
        /* Parse PDELAY_RESP_FOLLOWUP PDU */
        DOT1AS_PDELAY_RESP_FOLLOWUP_PDU_t *pPdelayRespFollowupPdu;

        pPdelayRespFollowupPdu =  &pPdu->body.pDelayRespFollowupPdu;
        dot1asUnpackUint48(pPktData,
                         &pPdelayRespFollowupPdu->respOriginTimestamp.seconds);
        pPktData += 6;

        sal_memcpy(&tmp32, pPktData, sizeof(tmp32));
        tmp32 = sal_ntohl(tmp32);
        pPdelayRespFollowupPdu->respOriginTimestamp.nanoseconds = tmp32;
        pPktData += sizeof(tmp32);

        sal_memcpy(pPdelayRespFollowupPdu->reqPortId.clock.id, pPktData,
               DOT1AS_CLOCK_ID_LEN);

        pPktData += DOT1AS_CLOCK_ID_LEN;

        sal_memcpy(&tmp16, pPktData, sizeof(tmp16));
        pPdelayRespFollowupPdu->reqPortId.num = sal_ntohs(tmp16);
        /* Decrement the port number received in the messages by 1 to map it
         * with internal port numbering which starts with 0
         */
        pPdelayRespFollowupPdu->reqPortId.num--;
        pPktData += sizeof(tmp16);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid PDELAY_RESP_FOLLOWUP len %d\n", __FUNCTION__,
                         pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;

    case DOT1AS_PDU_ANNOUNCE:
      /* Path trace TLV is mandatory, but may be skipped depending on the
       * link MTU configured on the egress port of a time-aware system.
       * So, check for minimum length of ANNOUNCE PDU.
       */
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_ANNOUNCE_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_ANNOUNCE_PDU_LEN)))
      {
        /* Parse ANNOUNCE PDU */
        DOT1AS_ANNOUNCE_PDU_t *pAnnouncePdu;

        pAnnouncePdu = &pPdu->body.announcePdu;
        dot1asAnnouncePduParse(pPktData, dataLen, pAnnouncePdu);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid ANNOUNCE len %d\n", __FUNCTION__,
                        pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;
    case DOT1AS_PDU_SIGNALING:
      /* Check for minimum length of SIGNALING PDU */
      if ((pHdr->pduLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_SIGNALING_PDU_LEN)) &&
          (dataLen >= (DOT1AS_PDU_HEADER_LEN + DOT1AS_SIGNALING_PDU_LEN)))
      {
        /* Parse SIGNALING PDU */
        DOT1AS_SIGNALING_PDU_t *pSignalingPdu;

        pSignalingPdu = &pPdu->body.signalingPdu;
        /* dumpBuffer("Signaling PDU", pPktData, DOT1AS_SIGNALING_PDU_LEN); */
        dot1asSignalingPduParse(pPktData, DOT1AS_SIGNALING_PDU_LEN, pSignalingPdu);
      }
      else
      {
        DOT1AS_TRACE_DBG("%s: invalid SINGALING len %d\n", __FUNCTION__,
                         pHdr->pduLen);
        rc = AVB_FAILURE;
      }
      break;

    default:
      rc = AVB_FAILURE;
      break;
  }

  return rc;
}

/******************************************************************************
* @purpose   Parse the receive buffer and extract FOLLOWUP PDU fields
*
* @param     data      @b{(input)} the handle to the 802.1AS raw buffer
* @param     dataLen   @b{(input)} the length of the 802.1AS FOLLOWUP PDU
* @param     pFollowUp @b{(input)} the handle to the 802.1AS FOLLOWUP PDU
*
* @returns   AVB_SUCCESS
* @returns   AVB_FAILURE
*
* @comments  Do not type-cast any of the fields directly from the rxBuf. The
*            receive buffer length is verified by caller.
* @end
*******************************************************************************/
AVB_RC_t dot1asFollowUpPduParse(uint8_t *data, uint16_t dataLen,
                            DOT1AS_FOLLOWUP_PDU_t *pFollowUp)
{
  AVB_RC_t      rc = AVB_SUCCESS;
  uint16_t  tmp16;
  uint32_t  tmp32;
  DOT1AS_FOLLOWUP_TLV_t *pFollowupTlv;

  (void)dataLen;

  /* First 10 bytes are precise origin timestamp. 6bytes secs, 4bytes nano */
  dot1asUnpackUint48(data, &pFollowUp->preciseOriginTimestamp.seconds);
  data += 6;

  sal_memcpy(&tmp32, data, sizeof(tmp32));
  pFollowUp->preciseOriginTimestamp.nanoseconds = sal_ntohl(tmp32);
  data += sizeof(tmp32);

  /* Parse standard TLV */
  pFollowupTlv = &pFollowUp->gmTlv;

  /* Next 2 bytes are TLV type */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pFollowupTlv->type = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* Next 2 bytes are TLV length */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pFollowupTlv->length = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* Next 3 bytes are OrgId */
  sal_memcpy(pFollowupTlv->orgId, data, 3);
  data += 3;

  /* Next 3 bytes are Org Sub Type */
  sal_memcpy(pFollowupTlv->orgSubType, data, 3);
  data += 3;

  /* Check TLV */
  if ((pFollowupTlv->type != DOT1AS_TLV_TYPE_ORG_EXT) ||
      (pFollowupTlv->length != DOT1AS_FOLLOWUP_TLV_LEN) ||
      (pFollowupTlv->orgId[0] != 0x00) || (pFollowupTlv->orgId[1] != 0x80) ||
      (pFollowupTlv->orgId[2] != 0xC2) || (pFollowupTlv->orgSubType[2] != 0x01))
  {
    DOT1AS_TRACE_DBG("DOT1AS: Invalid Followup TLV type %d, len %d, orgId %x:%x:%x, sub %x\n",
                     pFollowupTlv->type, pFollowupTlv->length, pFollowupTlv->orgId[0],
                     pFollowupTlv->orgId[1], pFollowupTlv->orgId[2], pFollowupTlv->orgSubType[2]);
    /* return AVB_FAILURE; */
  }

  /* Next 4 bytes are cumulative scaled rate offset */
  sal_memcpy(&tmp32, data, sizeof(tmp32));
  pFollowupTlv->cumulativeRateOfst = sal_ntohl(tmp32);
  data += sizeof(tmp32);

  /* Next 2 bytes are GM time base indicator */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pFollowupTlv->gmTimeBaseIndicator = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* Next 12 bytes are GM phase change value */
  /* Any conversions required? TBD */
  sal_memcpy(pFollowupTlv->lastGmPhaseChange, data, 12);
  data += 12;

  /* Next 4 bytes are GM freq change value */
  sal_memcpy(&tmp32, data, sizeof(tmp32));
  pFollowupTlv->lastGmFreqChange = sal_ntohl(tmp32);
  data += sizeof(tmp32);

  return rc;
}

/******************************************************************************
* @purpose   Parse the receive buffer and extract ANNOUNCE PDU fields
*
* @param     data      @b{(input)} the pointer to the 802.1AS raw buffer
* @param     dataLen   @b{(input)} the length of the 802.1AS ANNOUNCE PDU
* @param     pAnnounce @b{(input)} the handle to the 802.1AS ANNOUNCE PDU
*
* @returns   AVB_SUCCESS
* @returns   AVB_FAILURE
*
* @comments  Do not type-cast any of the fields directly from the rxBuf. The
*            receive buffer length is verified by caller.
* @end
*******************************************************************************/
AVB_RC_t dot1asAnnouncePduParse(uint8_t *data, uint16_t dataLen,
                            DOT1AS_ANNOUNCE_PDU_t *pAnnounce)
{
  AVB_RC_t      rc = AVB_SUCCESS;
  uint16_t  index;
  uint16_t  tmp16;
  uint16_t  tlvLen;
  DOT1AS_PATH_TRACE_TLV_t *pTlv;

  /* First 10 bytes are reserved */
  sal_memcpy(pAnnounce->reserved1, data, 10);
  data += 10;

  /* Next 2 bytes are UTC offset value */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pAnnounce->currentUTCoffset = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  pAnnounce->reserved2 = *data;
  data++;

  /* Next 1 byte is GM priority1 value */
  pAnnounce->gmPriority1 = *data;
  data++;

  /* Next 4 bytes are class, accuracy and variance values*/
  pAnnounce->gmClockQuality.class = *data;
  data++;
  pAnnounce->gmClockQuality.accuracy = *data;
  data++;
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pAnnounce->gmClockQuality.variance = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* Next 1 byte is GM priority2 value */
  pAnnounce->gmPriority2 = *data;
  data++;
  /* Next 8 bytes is GM clock identity */
  sal_memcpy(pAnnounce->gmClock.id, data, DOT1AS_CLOCK_ID_LEN);
  data += DOT1AS_CLOCK_ID_LEN;

  /* Next 2 bytes are steps removed value */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pAnnounce->stepsRemoved = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* Next 1 byte is time source value */
  pAnnounce->timeSource = *data;
  data++;

  /* Check for path Trace TLV */
  pTlv = &pAnnounce->tlv;
  sal_memset(pTlv, 0x00, sizeof(DOT1AS_PATH_TRACE_TLV_t));
  if (dataLen > (DOT1AS_PDU_HEADER_LEN + DOT1AS_ANNOUNCE_PDU_LEN))
  {
    tlvLen = dataLen - (DOT1AS_PDU_HEADER_LEN + DOT1AS_ANNOUNCE_PDU_LEN);

    /* Check for minimum length of path trace TLV */
    if (tlvLen >= DOT1AS_PATH_TRACE_TLV_MIN_LEN)
    {
      /* First 2 bytes of TLV are type */
      sal_memcpy(&tmp16, data, sizeof(tmp16));
      tmp16 = sal_ntohs(tmp16);
      data += sizeof(tmp16);

      if (tmp16 == DOT1AS_TLV_TYPE_PATH_TRACE)
      {
        /* Next 2 bytes of TLV are length. Must be multiple of clockId */
        sal_memcpy(&tmp16, data, sizeof(tmp16));
        tmp16 = sal_ntohs(tmp16);
        data += sizeof(tmp16);

        if ((tlvLen >= (tmp16 + 4)) && ((tmp16 % DOT1AS_CLOCK_ID_LEN) == 0))
        {
          pTlv->type = DOT1AS_TLV_TYPE_PATH_TRACE;
          pTlv->length = tmp16;
          /* Parse each of the clock identities in the PDU */
          for (index=0; index < (tmp16/DOT1AS_CLOCK_ID_LEN); index++)
          {
            sal_memcpy(pTlv->clk[index].id, data, DOT1AS_CLOCK_ID_LEN);
            data += DOT1AS_CLOCK_ID_LEN;
          }
        }
        else
        {
          DOT1AS_TRACE_DBG("DOT1AS: Invalid path trace tlv data len %d\n",
                            tlvLen);
        }
      }
    }
  }
  /* else no path trace TLV. */

  return rc;
}

/******************************************************************************
* @purpose   Parse the receive buffer and extract SIGNALING PDU fields
*
* @param     data      @b{(input)} the pointer to the 802.1AS raw buffer
* @param     dataLen   @b{(input)} the length of the 802.1AS SIGNALING PDU
* @param     pSignalingPdu @b{(input)} the handle to the 802.1AS SIGNALING PDU
*
* @returns   AVB_SUCCESS
* @returns   AVB_FAILURE
*
* @comments  Do not type-cast any of the fields directly from the rxBuf. The
*            receive buffer length is verified by caller.
* @end
*******************************************************************************/
AVB_RC_t dot1asSignalingPduParse(uint8_t *data, uint16_t dataLen,
                             DOT1AS_SIGNALING_PDU_t *pSignalingPdu)
{
  AVB_RC_t      rc = AVB_SUCCESS;
  uint16_t  tmp16;
  DOT1AS_SIGNALING_MSG_INT_REQ_TLV_t *pTlv;

  (void)dataLen;

  /* First 10 bytes are targetPortIdentity */
  sal_memcpy(pSignalingPdu->targetPortIdentity.clock.id, data,
         DOT1AS_CLOCK_ID_LEN);
  data += DOT1AS_CLOCK_ID_LEN;

  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pSignalingPdu->targetPortIdentity.num = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* Next 16 bytes are message interval TLV */
  pTlv = &pSignalingPdu->msgIntReqTlv;

  /* 2-bytes of TLV type */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pTlv->type = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* 2-bytes of TLV length */
  sal_memcpy(&tmp16, data, sizeof(tmp16));
  pTlv->length = sal_ntohs(tmp16);
  data += sizeof(tmp16);

  /* 3-bytes of org Id */
  sal_memcpy(pTlv->orgId, data, 3); data += 3;

  /* 3-bytes are Org Sub Type */
  sal_memcpy(pTlv->orgSubType, data, 3); data += 3;

  /* Check TLV */
  if (pTlv->length != DOT1AS_SIGNALING_MSG_INT_TLV_LEN)
  {
    DOT1AS_TRACE_SIG("DOT1AS: Invalid Signaling TLV length %d\n",
                     pTlv->length);
    return AVB_FAILURE;
  }

  if ((pTlv->type != DOT1AS_TLV_TYPE_ORG_EXT) ||
      (pTlv->orgId[0] != 0x00) || (pTlv->orgId[1] != 0x80) ||
      (pTlv->orgId[2] != 0xC2) || (pTlv->orgSubType[2] != 0x02))
  {
    DOT1AS_TRACE_SIG("DOT1AS: Invalid Signaling Message Interval TLV\n");
  }

  /* 1-byte for linkDelayInterval */
  pTlv->linkDelayInterval = *data; data++;

  /* 1-byte for timeSyncInterval */
  pTlv->timeSyncInterval = *data; data++;

  /* 1-byte for announceInterval */
  pTlv->announceInterval = *data; data++;

  /* 1-byte for flags */
  pTlv->flags = *data; data++;

  return rc;
}

/*@api
 * dot1asPduReceiveProcess
 *
 * @brief
 *    Process incoming 802.1AS PDU
 *
 * @param=intfIndex - interface index on which PDU was received
 * @param=pdu - pointer to the 802.1AS PDU
 */
AVB_RC_t dot1asPduReceiveProcess(uint32_t intfIndex, DOT1AS_PDU_t  *pPdu, uint32_t timeStamp)
{
  AVB_RC_t         rc = AVB_SUCCESS;
  uint8_t      pduType;
  DOT1AS_INTF_STATS_t *pStats;

  pduType = pPdu->hdr.pduType;
  pStats = &pDot1asIntf_g[intfIndex].stats;

  switch (pduType)
  {
    case DOT1AS_PDU_SYNC:
      pStats->syncRxCount++;
      dot1asDbgSyncT1 = 1000 * sal_time_usecs();
      rc = dot1asSyncReceive(intfIndex, pPdu, timeStamp);
      dot1asDbgSyncT2 = 1000 * sal_time_usecs();
      break;

    case DOT1AS_PDU_FOLLOWUP:
      pStats->followUpRxCount++;
      dot1asDbgFollowupT1 = 1000 * sal_time_usecs();
      rc = dot1asFollowupReceive(intfIndex, pPdu);
      break;

    case DOT1AS_PDU_PDELAY_REQ:
      pStats->pDelayReqRxCount++;
      rc = dot1asPdelayReqReceive(intfIndex, pPdu, timeStamp);
      break;

    case DOT1AS_PDU_PDELAY_RESP:
      pStats->pDelayRespRxCount++;
      rc = dot1asPdelayRespReceive(intfIndex, pPdu, timeStamp);
      break;

    case DOT1AS_PDU_PDELAY_RESP_FOLLOWUP:
      pStats->pDelayRespFollowUpRxCount++;
      rc = dot1asPdelayRespFollowupReceive(intfIndex, pPdu);
      break;

    case DOT1AS_PDU_ANNOUNCE:
      pStats->announceRxCount++;
      rc = dot1asAnnounceReceive(intfIndex, pPdu);
      break;

    case DOT1AS_PDU_SIGNALING:
      pStats->signalingRxCount++;
      rc = dot1asSignalingReceive(intfIndex, pPdu);
      break;

    default:
      /* Shouldn't come here, as packet is parsed/checked on receive callback */
      DOT1AS_TRACE_DBG("dot1asPduReceiveProcess; Invalid PDU type");
      break;

  }

  return rc;
}


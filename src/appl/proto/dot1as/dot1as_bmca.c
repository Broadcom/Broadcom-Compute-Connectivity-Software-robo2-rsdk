/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_bmca.c
 */

#include "osal/sal_alloc.h"
#include "dot1as.h"
#include "dot1as_cfgx.h"
#include "avb.h"

typedef enum {
  DOT1AS_VECT_EQUAL=0,
  DOT1AS_VECT_SUPERIOR,
  DOT1AS_VECT_INFERIOR
} DOT1AS_VECT_COMP_RESULT_t;

static DOT1AS_PRIO_VECTOR_t *gmPathPrioVec = NULL;

DOT1AS_VECT_COMP_RESULT_t dot1asBmcaCompareVectors(DOT1AS_PRIO_VECTOR_t *pV1,
                                                   DOT1AS_PRIO_VECTOR_t *pV2);
uint8_t dot1asIsSysIdBetter(DOT1AS_SYSTEM_IDENTITY_t *pSysId1,
                            DOT1AS_SYSTEM_IDENTITY_t *pSysId2);
uint8_t dot1asIsSysIdEqual(DOT1AS_SYSTEM_IDENTITY_t *pSysId1,
                           DOT1AS_SYSTEM_IDENTITY_t *pSysId2);

uint8_t *dot1asDebugRoleStringGet(DOT1AS_PORT_ROLE_t role);

/*******************************************************************************
* @purpose  Check whether Announce mechanism can be enabled on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  TRUE if interface is capable, else FALSE
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIsAnnounceCapable(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  /* 802.1AS Global admin mode must be enabled,
   * 802.1AS Interface admin mode must be enabled,
   * Interface must be dot1asCapable (as determined by peer-delay).
   * See PortAnnounceInformation state machine.
   */
  if (DOT1AS_IS_INTF_CPU(intfIndex)) {
    return FALSE;
  }
  if ((pDot1asBridge_g->adminMode == TRUE) &&
      (pIntf->pttPortEnabled == TRUE) &&
      (pIntf->dot1asCapable == TRUE))
  {
    return TRUE;
  }

  return FALSE;
}



/******************************************************************************
* @purpose  Initialize ANNOUNCE state on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  none
*
* @comments
*
* @end
*******************************************************************************/
void dot1asIntfAnnounceInit(uint32_t intfIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  pIntf->announceSeqId = 0;

  pIntf->announceTransmitTimerReset = TRUE;
  pIntf->announceReceiptTimerReset = TRUE;
}



/******************************************************************************
* @purpose  Process the received 802.1AS ANNOUNCE PDU.
*
* @param    intfIndex  @b{(input)} interface index
* @param    pPdu  @b{(input)} DOT1AS PDU
*
* @returns  none
*
* @comments Basic validations for DA, ETHTYPE, Interface number have already
*           been done. Caller frees the buffer.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asAnnounceReceive(uint32_t intfIndex, DOT1AS_PDU_t *pPdu)
{
  uint16_t index;
  uint16_t numPaths;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_PDU_HEADER_t *pHdr;
  DOT1AS_ANNOUNCE_PDU_t   *pAnnouncePdu;
  DOT1AS_PATH_TRACE_TLV_t *pTlv;

  pIntf = &pDot1asIntf_g[intfIndex];

  if (dot1asIsAnnounceCapable(intfIndex) != TRUE)
  {
    pIntf->stats.announceRxDiscards++;
    DOT1AS_TRACE_BMCA("DOT1AS[%d]: Discard Announce as intf not asCapable\n",
                      avbCfgxIntfNum[intfIndex]);
    return AVB_FAILURE;
  }

  pHdr = &pPdu->hdr;
  pAnnouncePdu =&pPdu->body.announcePdu;

  /* Qualify Announce PDU. qualifyAnnounce() */

  if (DOT1AS_CLOCK_ID_IS_EQ(&pHdr->portId.clock, &pDot1asBridge_g->thisClock))
  {
    /* If ANNOUNCE was sent by this bridge, discard */
    DOT1AS_TRACE_BMCA("DOT1AS[%d]: Disqualify Announce sent by self\n",
                      avbCfgxIntfNum[intfIndex]);
    pIntf->stats.announceRxDiscards++;
    pIntf->stats.ptpDiscardCount++;
    return AVB_FAILURE;
  }

  if (pAnnouncePdu->stepsRemoved >= DOT1AS_MAX_STEPS_REMOVED)
  {
    /* If stepsRemoved is greater than max limit, discard */
    pIntf->stats.announceRxDiscards++;
    pIntf->stats.ptpDiscardCount++;
    DOT1AS_TRACE_BMCA("DOT1AS[%d]: Disqualify Announce with steps > %d\n",
                      avbCfgxIntfNum[intfIndex], pAnnouncePdu->stepsRemoved);
    return AVB_FAILURE;
  }

  /* If the ANNOUNCE msg claims this bridge as GM, then discard it */
  if (DOT1AS_CLOCK_ID_IS_EQ(&pAnnouncePdu->gmClock, &pDot1asBridge_g->thisClock))
  {
    /* We don't want to react to a Announce msg containing stale GM info */
    DOT1AS_TRACE_BMCA("DOT1AS[%d]: Disqualify Announce claiming thisClock as GM\n",
                      avbCfgxIntfNum[intfIndex]);
    pIntf->stats.announceRxDiscards++;
    pIntf->stats.ptpDiscardCount++;
    return AVB_FAILURE;
  }


  /* If a PATH TLV is present and one of the elements in the TLV array
   * is this clock, discard.
   */
  pTlv = &pAnnouncePdu->tlv;
  for (index = 0; index < DOT1AS_MAX_HOPS_PER_DOMAIN; index++)
  {
    if (DOT1AS_CLOCK_ID_IS_EQ(&pTlv->clk[index], &pDot1asBridge_g->thisClock))
    {
      DOT1AS_TRACE_BMCA("DOT1AS[%d]: Disqualify Announce due to path TLV\n",
                        avbCfgxIntfNum[intfIndex]);
      pIntf->stats.announceRxDiscards++;
      pIntf->stats.ptpDiscardCount++;
      return AVB_FAILURE;
    }
  }

  /* Announce message is qualified now. */
  /* Copy the path trace TLV only when the Announce was received from the
   * current parent clock, i,e received on the slave port. This avoids
   * over-writing the path trace with any inferior Announce message
   * received on master ports.
   */
  if ((pIntf->selectedRole == DOT1AS_ROLE_SLAVE) &&
      (DOT1AS_CLOCK_ID_IS_EQ(&pDot1asBridge_g->gmClock,
                             &pAnnouncePdu->gmClock)))
  {
    sal_memset(&pDot1asBridge_g->gmPath, 0x00, sizeof(DOT1AS_PATH_TRACE_TLV_t));
    pTlv = &pAnnouncePdu->tlv;

    pDot1asBridge_g->gmPath.type = DOT1AS_TLV_TYPE_PATH_TRACE;
    pDot1asBridge_g->gmPath.length = pTlv->length;

    /* Store the path trace in the global array */
    numPaths = (pTlv->length / DOT1AS_CLOCK_ID_LEN);
    for (index = 0; index < numPaths; index++)
    {
      DOT1AS_CLOCK_ID_ASSIGN(&pDot1asBridge_g->gmPath.clk[index],
                             &pTlv->clk[index]);
    }

    if (numPaths < DOT1AS_MAX_HOPS_PER_DOMAIN)
    {
      /* Append thisClock to the path trace */
      index = numPaths;
      DOT1AS_CLOCK_ID_ASSIGN(&pDot1asBridge_g->gmPath.clk[index],
                             &pDot1asBridge_g->thisClock);
      pDot1asBridge_g->gmPath.length += DOT1AS_CLOCK_ID_LEN;
    }
  }

  dot1asInvokeBmca(intfIndex, DOT1AS_BMC_ANNOUNCE_RX, (void *)pPdu);

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Send an ANNOUNCE PDU out of the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
*
* @returns  AVB_RC_t
*
* @comments Uses the clock information from bridge state information
*
* @end
*******************************************************************************/
AVB_RC_t dot1asAnnounceSend(uint32_t intfIndex)
{
  uint8_t   *pdu = NULL;
  int32_t   interval;
  uint16_t  pduLen = 0;
  uint16_t  hdrsize = 0;
  uint16_t  index = 0;
  uint16_t  val16 = 0;
  uint16_t  flags = 0;
  uint16_t  numPaths = 0;
  DOT1AS_INTF_STATE_t *pIntf;

  pIntf = &pDot1asIntf_g[intfIndex];

  /* Get a eth fragment to transmit ANNOUNCE message. */
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
  flags = (pDot1asBridge_g->gmFlags | DOT1AS_CLK_PTP_TIMESCALE_FLAG);

  interval = pIntf->currentLogAnnounceInterval;
  /* For Announce, correctionField is set to 0. See 10.5.2.2.7 */
  if (dot1asPduAddCommonHeader(intfIndex, pdu, DOT1AS_PDU_ANNOUNCE,
                               pIntf->announceSeqId, 0, (int8_t)interval,
                               flags, &hdrsize) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pdu += hdrsize;
  pduLen += hdrsize;

  /* ANNOUNCE PDU body. */
  /* 10bytes reserved */
  sal_memset(pdu, 0x00, 10);
  pdu += 10;
  pduLen += 10;

  /* Current UTC offset, 2 bytes. TBD */
  val16 = 0x0000;
  val16 = sal_htons(val16);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16);
  pduLen += sizeof(val16);

  /* Reserved, 1 byte */
  *pdu = 0x00;
  pdu++; pduLen++;

  /* GM Priority1, 1 byte */
  *pdu = pIntf->masterPrioVec.sysId.priority1;
  pdu++; pduLen++;

  /* GM clock quality, 4 bytes */
  *pdu = pIntf->masterPrioVec.sysId.quality.class; /* Clock Class */
  pdu++; pduLen++;

  *pdu = pIntf->masterPrioVec.sysId.quality.accuracy; /* Clock Accuracy */
  pdu++; pduLen++;

  val16 = sal_htons(pIntf->masterPrioVec.sysId.quality.variance);
  sal_memcpy(pdu, &val16, sizeof(val16)); /* Clock variance */
  pdu += sizeof(val16); pduLen += sizeof(val16);

  /* GM Priority2, 1 byte */
  *pdu = pIntf->masterPrioVec.sysId.priority2;
  pdu++; pduLen++;

  /* GM identity, 8 bytes */
  sal_memcpy(pdu, pIntf->masterPrioVec.sysId.clock.id, DOT1AS_CLOCK_ID_LEN);
  pdu += DOT1AS_CLOCK_ID_LEN; pduLen += DOT1AS_CLOCK_ID_LEN;

  /* Steps removed, 2 bytes */
  val16 = sal_htons(pDot1asBridge_g->gmStepsRemoved);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16); pduLen += sizeof(val16);

  /* Time Source, 1 byte */
  *pdu = pDot1asBridge_g->gmTimeSrc;
  pdu++; pduLen++;

  /* Check whether the PDU length exceeds link MTU. If PDU lenght is greater
   * than link MTU, do not append the TLV. TBD.
   */
  /* Add PATH TRACE TLVs here. First 2 bytes are TLV type */
  val16 = sal_htons(pDot1asBridge_g->gmPath.type);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16); pduLen += sizeof(val16);

  /* TLV length, 2 bytes */
  val16 = sal_htons(pDot1asBridge_g->gmPath.length);
  sal_memcpy(pdu, &val16, sizeof(val16));
  pdu += sizeof(val16); pduLen += sizeof(val16);


  /* Array of clock identities */
  numPaths = pDot1asBridge_g->gmPath.length / DOT1AS_CLOCK_ID_LEN;
  for (index = 0; index < numPaths; index++)
  {
    sal_memcpy(pdu, pDot1asBridge_g->gmPath.clk[index].id, DOT1AS_CLOCK_ID_LEN);
    pdu += DOT1AS_CLOCK_ID_LEN; pduLen += DOT1AS_CLOCK_ID_LEN;
  }

  /* Adjust the length in the header based on the TLV length */
  pdu = &dot1asTxBuffer[0];
  val16 = dot1asPduLengthGet(DOT1AS_PDU_ANNOUNCE) +
          (numPaths * DOT1AS_CLOCK_ID_LEN) + 4;
  dot1asPduLengthSet(pdu, val16);

  DOT1AS_TRACE_TX("DOT1AS PDU event: Sending ANNOUNCE on intf %d\n", avbCfgxIntfNum[intfIndex]);

  if (dot1asPktSend(dot1asTxBuffer, pduLen, intfIndex) != 0)
  {
    DOT1AS_TRACE_ERR("%s: failed to trasmit\n", __FUNCTION__);
    pDot1asIntf_g[intfIndex].stats.txErrors++;
    return AVB_FAILURE;
  }

  /* All is good */
  pIntf->stats.announceTxCount++;

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Update DOT1AS role for each of the interfaces
*
* @param    none
*
* @returns  none
*
* @comments This is the "global" part of the BMCA
*
* @end
*******************************************************************************/
void dot1asBmcaUpdateRoles(void)
{
  uint32_t intfIndex;
  uint32_t sysTime;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_PRIO_VECTOR_t gmPrioVec;
  DOT1AS_VECT_COMP_RESULT_t result;

  DOT1AS_TRACE_DBG("%s: invoked\n", __FUNCTION__);

  if (gmPathPrioVec == NULL )
  {
    gmPathPrioVec = (DOT1AS_PRIO_VECTOR_t*)
                    sal_alloc(sizeof(DOT1AS_PRIO_VECTOR_t) * dot1asMaxInterfaceCount_g, "DOT1AS");
    if (gmPathPrioVec == NULL)
    {
      sal_printf("Could not able allocate memory for gmPathPrioVec\n");
      return;
    }
  }
  /* Numerically lower value is better, so set default to 0xFF */
  sal_memset(gmPathPrioVec, 0xFF, (sizeof(DOT1AS_PRIO_VECTOR_t) * dot1asMaxInterfaceCount_g));
  sal_memset(&gmPrioVec, 0, sizeof(gmPrioVec));

  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];
    /* Clear reselect for each of the interfaces */
    pIntf->reselect = FALSE;
  }


  /* Compute gmPathPriority vector for each port */
  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];
    if ((dot1asIsAnnounceCapable(intfIndex) == TRUE) &&
        (pIntf->infoIs == DOT1AS_INFO_RECEIVED))
    {
      sal_memcpy(&gmPathPrioVec[intfIndex], &pIntf->portPrioVec,
             sizeof(DOT1AS_PRIO_VECTOR_t));
      gmPathPrioVec[intfIndex].steps++;
    }
  }

  /* Compute gmPrioVec. Start with 'this' system as grand master.
   * Initialize gmPrioVec with systemPriorityVector.
   */
  gmPrioVec.sysId.priority1 = pDot1asBridge_g->priority1;
  gmPrioVec.sysId.priority2 = pDot1asBridge_g->priority2;
  gmPrioVec.sysId.quality =  pDot1asBridge_g->thisClockQuality;

  DOT1AS_CLOCK_ID_ASSIGN(&gmPrioVec.sysId.clock, &pDot1asBridge_g->thisClock);
  gmPrioVec.steps = 0x00;

  DOT1AS_CLOCK_ID_ASSIGN(&gmPrioVec.sourcePort.clock, &pDot1asBridge_g->thisClock);
  gmPrioVec.sourcePort.num = 0x00;
  gmPrioVec.rcvdPortNum = 0x00;

  /* Compare the gmPrioVec with each of the gmPathPrioVec */
  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];

    /* Check whether interface is enabled, received an ANNOUNCE message
     * and the message was not from 'this' time aware system
     */
    if ((dot1asIsAnnounceCapable(intfIndex) == TRUE) &&
        (pIntf->infoIs == DOT1AS_INFO_RECEIVED) &&
        (DOT1AS_CLOCK_ID_IS_NOT_EQ(&pIntf->portPrioVec.sourcePort.clock,
                                   &pDot1asBridge_g->thisClock)))
    {
      result = dot1asBmcaCompareVectors(&gmPrioVec, &gmPathPrioVec[intfIndex]);

      if (result == DOT1AS_VECT_INFERIOR) /* gmPrioVec is inferior */
      {
        /* Replace the gmPrioVec with gmPathPrioVec */
        sal_memcpy(&gmPrioVec, &gmPathPrioVec[intfIndex], sizeof(gmPrioVec));
      }
      /* else, if superior or equal, the gmPrioVec is already the best */
    }
  }

  /* Store the best clock identity, priority1 and priority2 values */
  if (DOT1AS_CLOCK_ID_IS_NOT_EQ(&pDot1asBridge_g->gmClock,
                                &gmPrioVec.sysId.clock))
  {
    uint8_t *id;

    id = gmPrioVec.sysId.clock.id;

    /* There has been a change in the root. Trace it */
    DOT1AS_TRACE("DOT1AS: New root id is %02x:%02x:%02x:%02x:%02x:%02x:%02x"
                 ":%02x\n",id[0],id[1],id[2],id[3],id[4],id[5],id[6],id[7]);

    DOT1AS_CLOCK_ID_ASSIGN(&pDot1asBridge_g->gmClock, &gmPrioVec.sysId.clock);
    pDot1asBridge_g->gmPriority1 = gmPrioVec.sysId.priority1;
    pDot1asBridge_g->gmPriority2 = gmPrioVec.sysId.priority2;
    pDot1asBridge_g->gmQuality = gmPrioVec.sysId.quality;
    pDot1asBridge_g->gmChangeCount++;
    sysTime = 0; /* TODO osapiUTCTimeNow(); */
    pDot1asBridge_g->lastGmChangeEvent = sysTime;
    pDot1asBridge_g->lastGmPhaseChangeEvent = sysTime;
    pDot1asBridge_g->lastGmFreqChangeEvent = sysTime;

    if (DOT1AS_CLOCK_ID_IS_EQ(&pDot1asBridge_g->gmClock,
                              &pDot1asBridge_g->thisClock))
    {
      /* Set clock to transition state, only if there was a GM present before
       * and a GM change has occured with this bridge elected as GM
       */
      if (pDot1asBridge_g->isGmPresent == TRUE)
      {
        pDot1asBridge_g->clkState = DOT1AS_CLOCK_TRANSITION_TO_GM;
      }
    }
    else
    {
      pDot1asBridge_g->clkState = DOT1AS_CLOCK_TRANSITION_TO_SLAVE;
    }
  }
  else if ((pDot1asBridge_g->gmPriority1 != gmPrioVec.sysId.priority1) ||
           (pDot1asBridge_g->gmPriority2 != gmPrioVec.sysId.priority2))
  {
    /* Here if there is no change in the Grandmaster, but its priority values
     * have changed and it continued to be a better clock
     */
     pDot1asBridge_g->gmPriority1 = gmPrioVec.sysId.priority1;
     pDot1asBridge_g->gmPriority2 = gmPrioVec.sysId.priority2;
  }

  /* If 'this' timeaware system is not the best, note the steps from root */
  if (DOT1AS_CLOCK_ID_IS_NOT_EQ(&gmPrioVec.sysId.clock,
                                &pDot1asBridge_g->thisClock))
  {
    pDot1asBridge_g->gmStepsRemoved = gmPrioVec.steps;
    intfIndex = gmPrioVec.rcvdPortNum;
    pIntf = &pDot1asIntf_g[intfIndex];

    /* Set the GM time properties based on the received Announce information */
    pDot1asBridge_g->gmFlags = pIntf->announceFlags;
    pDot1asBridge_g->gmTimeSrc = pIntf->announceTimeSrc;
    pDot1asBridge_g->gmUtcOfst = pIntf->announceUtcOfst;
  }
  else
  {
    pDot1asBridge_g->gmStepsRemoved = 0x00; /* 'this' system is the root */

    /* Set the GM clock properties to thisClock. TBD. Should this go in
     * platform specs?
     */
    pDot1asBridge_g->gmFlags = pDot1asBridge_g->thisClockFlags;
    pDot1asBridge_g->gmTimeSrc = DOT1AS_CLOCK_TIMESOURCE;
    pDot1asBridge_g->gmUtcOfst = 0;

    /* Set the parent clock of this bridge to self */
    DOT1AS_CLOCK_ID_ASSIGN(&pDot1asBridge_g->parentId.clock,
                           &pDot1asBridge_g->thisClock);
    pDot1asBridge_g->parentId.num = 0;

    /* Set the pathTrace global array to contain single element thisClock */
    sal_memset(&pDot1asBridge_g->gmPath, 0x00, sizeof(DOT1AS_PATH_TRACE_TLV_t));
    pDot1asBridge_g->gmPath.type = DOT1AS_TLV_TYPE_PATH_TRACE;
    pDot1asBridge_g->gmPath.length = DOT1AS_CLOCK_ID_LEN;
    DOT1AS_CLOCK_ID_ASSIGN(&pDot1asBridge_g->gmPath.clk[0],
                           &pDot1asBridge_g->thisClock);
  }

  /* Update port roles */
  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];
    /* Compute Master prio vector */
    sal_memcpy(&pIntf->masterPrioVec, &gmPrioVec, sizeof(gmPrioVec));

    DOT1AS_CLOCK_ID_ASSIGN(&pIntf->masterPrioVec.sourcePort.clock,
                           &pDot1asBridge_g->thisClock);
    pIntf->masterPrioVec.sourcePort.num = intfIndex;
    pIntf->masterPrioVec.rcvdPortNum = intfIndex;

    /* If the port info is disabled, set port role to disabled */
    if (pIntf->infoIs == DOT1AS_INFO_DISABLED)
    {
      DOT1AS_TRACE_PORT_ROLE(avbCfgxIntfNum[intfIndex], DOT1AS_ROLE_DISABLED);
      pIntf->selectedRole = DOT1AS_ROLE_DISABLED;
      pIntf->updtInfo = FALSE;
      continue;
    }

    /* If Announce/Sync timeout has occured, set role to master */
    if (pIntf->infoIs == DOT1AS_INFO_AGED)
    {
      /* If the port is transitioning to MASTER, reset the ANNOUNCE transmit
       * timer as per PortAnnounceTransmit state machine
       */
      if (pIntf->selectedRole != DOT1AS_ROLE_MASTER)
      {
        pIntf->announceTransmitTimerReset = TRUE;
      }

      DOT1AS_TRACE_PORT_ROLE(avbCfgxIntfNum[intfIndex], DOT1AS_ROLE_MASTER);
      pIntf->selectedRole = DOT1AS_ROLE_MASTER;
      pIntf->updtInfo = TRUE;
      continue;
    }

    /* If the portPrioVec was derived from 'this' system, set role to master */
    /* TBD. Is this check enough for 10.3.12.1.4 e5. */
    if ((pIntf->infoIs == DOT1AS_INFO_MINE) &&
        (DOT1AS_CLOCK_ID_IS_EQ(&pIntf->portPrioVec.sourcePort.clock,
                               &pDot1asBridge_g->thisClock)))
    {
      if (pIntf->selectedRole != DOT1AS_ROLE_MASTER)
      {
        pIntf->announceTransmitTimerReset = TRUE;
      }

      DOT1AS_TRACE_PORT_ROLE(avbCfgxIntfNum[intfIndex], DOT1AS_ROLE_MASTER);
      pIntf->selectedRole = DOT1AS_ROLE_MASTER;
      pIntf->updtInfo = TRUE;
    }
    else
    {
      /* portPrioVec was derived from a received ANNOUNCE message */

      if ((pIntf->infoIs == DOT1AS_INFO_RECEIVED) &&
          ((dot1asIsSysIdEqual(&pIntf->portPrioVec.sysId,
                               &gmPrioVec.sysId) == TRUE) &&
           (DOT1AS_CLOCK_ID_IS_EQ(&pIntf->portPrioVec.sourcePort.clock,
                                  &gmPrioVec.sourcePort.clock)) &&
           (pIntf->portPrioVec.sourcePort.num == gmPrioVec.sourcePort.num) &&
           (pIntf->portPrioVec.rcvdPortNum == gmPrioVec.rcvdPortNum)))
      {
        /* The portPrioVec was derived from a received ANNOUNCE message and
         * the gmPrioVec is now derived from portPrioVec, set role to slave
         */
        DOT1AS_TRACE_PORT_ROLE(avbCfgxIntfNum[intfIndex], DOT1AS_ROLE_SLAVE);
        pIntf->selectedRole = DOT1AS_ROLE_SLAVE;
        pIntf->updtInfo = FALSE;

        /* Also update the parent clock of this bridge */
        sal_memcpy(&pDot1asBridge_g->parentId, &pIntf->portPrioVec.sourcePort,
               sizeof(DOT1AS_PORT_IDENTITY_t));

        continue;
      }

      if ((pIntf->infoIs == DOT1AS_INFO_RECEIVED) &&
          ((dot1asIsSysIdEqual(&pIntf->portPrioVec.sysId,
                               &gmPrioVec.sysId) == FALSE) ||
           (DOT1AS_CLOCK_ID_IS_NOT_EQ(&pIntf->portPrioVec.sourcePort.clock,
                                      &gmPrioVec.sourcePort.clock)) ||
           (pIntf->portPrioVec.sourcePort.num != gmPrioVec.sourcePort.num) ||
           (pIntf->portPrioVec.rcvdPortNum != gmPrioVec.rcvdPortNum)))
      {
        result = dot1asBmcaCompareVectors(&pIntf->masterPrioVec,
                                          &pIntf->portPrioVec);

        if (result == DOT1AS_VECT_SUPERIOR) /* masterPrioVec is better */
        {
          /* The portPrioVec was derived from a received ANNOUNCE message, but
           * gmPrioVec is NOT derived from portPrioVec, and masterPrioVec is
           * better, set role to master.
           */
          DOT1AS_TRACE_PORT_ROLE(avbCfgxIntfNum[intfIndex], DOT1AS_ROLE_MASTER);
          pIntf->selectedRole = DOT1AS_ROLE_MASTER;
          pIntf->updtInfo = TRUE;
        }
        else /* masterPrio is NOT better (equal or inferior) */
        {
          /* The portPrioVec was derived from a received ANNOUNCE message, but
           * gmPrioVec is NOT derived from portPrioVec, and masterPrioVec is NOT
           * better, set role to passive.
           */
          DOT1AS_TRACE_PORT_ROLE(avbCfgxIntfNum[intfIndex], DOT1AS_ROLE_PASSIVE);
          pIntf->selectedRole = DOT1AS_ROLE_PASSIVE;
          pIntf->updtInfo = FALSE;
        }
      }
    }
  }

  /* Update gmPresent */
  if (gmPrioVec.sysId.priority1 == DOT1AS_PRIO1_NOT_GM_CAPABLE)
  {
    pDot1asBridge_g->isGmPresent = FALSE;
  }
  else /* < 255 */
  {
    pDot1asBridge_g->isGmPresent = TRUE;
  }

  /* Set selected and update if required */
  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];

    if (pIntf->updtInfo == TRUE)
    {
      /* Copy masterPrioVec into portPrioVec */
      sal_memcpy(&pIntf->portPrioVec, &pIntf->masterPrioVec,
             sizeof(DOT1AS_PRIO_VECTOR_t));
      pIntf->portSteps = pDot1asBridge_g->gmStepsRemoved;
      pIntf->updtInfo = FALSE;
      pIntf->infoIs = DOT1AS_INFO_MINE;
    }

    /* Set selected to TRUE */
    pIntf->selected = TRUE;
  }
}



/*******************************************************************************
* @purpose  Compare the two system identities and check if pSysId1 is better
*           than pSysId2
*
* @param    pSysId1    @b{(input)} system identity 1
* @param    pSysId2    @b{(input)} system identity 2
*
* @returns  TRUE    pSysId1 is numerically less than pSysId2 (BETTER)
* @returns  FALSE   pSysId1 is either equal or greater(WORSE) than pSysId2
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIsSysIdBetter(DOT1AS_SYSTEM_IDENTITY_t *pSysId1,
                            DOT1AS_SYSTEM_IDENTITY_t *pSysId2)
{
  if (pSysId1->priority1 < pSysId2->priority1)
  {
    return TRUE;
  }

  if ((pSysId1->priority1 == pSysId2->priority1) &&
      (pSysId1->quality.class < pSysId2->quality.class))
  {
    return TRUE;
  }

  if ((pSysId1->priority1 == pSysId2->priority1) &&
      (pSysId1->quality.class == pSysId2->quality.class) &&
      (pSysId1->quality.accuracy < pSysId2->quality.accuracy))
  {
    return TRUE;
  }

  if ((pSysId1->priority1 == pSysId2->priority1) &&
      (pSysId1->quality.class == pSysId2->quality.class) &&
      (pSysId1->quality.accuracy == pSysId2->quality.accuracy) &&
      (pSysId1->quality.variance < pSysId2->quality.variance))
  {
    return TRUE;
  }

  if ((pSysId1->priority1 == pSysId2->priority1) &&
      (pSysId1->quality.class == pSysId2->quality.class) &&
      (pSysId1->quality.accuracy == pSysId2->quality.accuracy) &&
      (pSysId1->quality.variance == pSysId2->quality.variance) &&
      (pSysId1->priority2 < pSysId2->priority2))
  {
    return TRUE;
  }

  if ((pSysId1->priority1 == pSysId2->priority1) &&
      (pSysId1->quality.class == pSysId2->quality.class) &&
      (pSysId1->quality.accuracy == pSysId2->quality.accuracy) &&
      (pSysId1->quality.variance == pSysId2->quality.variance) &&
      (pSysId1->priority2 == pSysId2->priority2) &&
      (sal_memcmp(pSysId1->clock.id, pSysId2->clock.id, DOT1AS_CLOCK_ID_LEN) < 0))
  {
    return TRUE;
  }

  return FALSE;
}


/*******************************************************************************
* @purpose  Compare the two given system identities and check if they are equal
*
* @param    pSysId1    @b{(input)} system identity 1
* @param    pSysId2    @b{(input)} system identity 2
*
* @returns  TRUE    pSysId1 is same as pSysId2
* @returns  FALSE   other wise
*
* @comments
*
* @end
*******************************************************************************/
uint8_t dot1asIsSysIdEqual(DOT1AS_SYSTEM_IDENTITY_t *pSysId1,
                           DOT1AS_SYSTEM_IDENTITY_t *pSysId2)
{
  if ((pSysId1->priority1 == pSysId2->priority1) &&
      (pSysId1->quality.class == pSysId2->quality.class) &&
      (pSysId1->quality.accuracy == pSysId2->quality.accuracy) &&
      (pSysId1->quality.variance == pSysId2->quality.variance) &&
      (pSysId1->priority2 == pSysId2->priority2) &&
      (DOT1AS_CLOCK_ID_IS_EQ(&pSysId1->clock, &pSysId2->clock)))
  {
    return TRUE;
  }

  return FALSE;
}


/******************************************************************************
* @purpose  Compare priority vectors
*
* @param    pV1        @b{(input)} priorit vector 1
* @param    pV2        @b{(input)} priorit vector 2
*
* @returns  DOT1AS_VECT_COMP_RESULT_t pV1 is equal/superior/inferior to pV2.
*
* @comments A numerically lesser value is considered better or superior
*
* @end
*******************************************************************************/
DOT1AS_VECT_COMP_RESULT_t dot1asBmcaCompareVectors(DOT1AS_PRIO_VECTOR_t *pV1,
                                                   DOT1AS_PRIO_VECTOR_t *pV2)
{
  if ((dot1asIsSysIdEqual(&pV1->sysId, &pV2->sysId) == TRUE) &&
      (pV1->steps == pV2->steps) &&
      (DOT1AS_CLOCK_ID_IS_EQ(&pV1->sourcePort.clock, &pV2->sourcePort.clock)) &&
      (pV1->sourcePort.num == pV2->sourcePort.num) &&
      (pV1->rcvdPortNum == pV2->rcvdPortNum))
  {
    return DOT1AS_VECT_EQUAL;
  }
  else if ((dot1asIsSysIdBetter(&pV1->sysId, &pV2->sysId) == TRUE) ||

           ((dot1asIsSysIdEqual(&pV1->sysId, &pV2->sysId) == TRUE) &&
            (pV1->steps < pV2->steps)) ||

           ((dot1asIsSysIdEqual(&pV1->sysId, &pV2->sysId) == TRUE) &&
            (pV1->steps == pV2->steps) &&
            (sal_memcmp(pV1->sourcePort.clock.id, pV2->sourcePort.clock.id,
                    DOT1AS_CLOCK_ID_LEN) < 0)) ||


           ((dot1asIsSysIdEqual(&pV1->sysId, &pV2->sysId) == TRUE) &&
            (pV1->steps == pV2->steps) &&
            (DOT1AS_CLOCK_ID_IS_EQ(&pV1->sourcePort.clock,
                                   &pV2->sourcePort.clock)) &&
            (pV1->sourcePort.num < pV2->sourcePort.num)) ||

           ((dot1asIsSysIdEqual(&pV1->sysId, &pV2->sysId) == TRUE) &&
            (pV1->steps == pV2->steps) &&
            (DOT1AS_CLOCK_ID_IS_EQ(&pV1->sourcePort.clock,
                                   &pV2->sourcePort.clock)) &&
            (pV1->sourcePort.num == pV2->sourcePort.num) &&
            (pV1->rcvdPortNum < pV2->rcvdPortNum)))

  {
    return DOT1AS_VECT_SUPERIOR;
  }

  return DOT1AS_VECT_INFERIOR;
}


/******************************************************************************
* @purpose  Invoke BMCA state machine on the given interface
*
* @param    intfIndex  @b{(input)} DOT1AS internal interface index
* @param    event      @b{(input)} DOT1AS internal BMC event
* @param    pData      @b{(input)} Event information
*
* @returns  none
*
* @comments Part of this function is interface specific. If the event requires
*           updating port roles, then the priority vectors of all the ports is
*           compared.
*
* @end
*******************************************************************************/
void dot1asInvokeBmca(uint32_t intfIndex,
                      DOT1AS_BMC_EVENT_t event,
                      void *pData)
{
  uint32_t index;
  uint64_t interval;
  DOT1AS_PDU_t *pPdu;
  DOT1AS_PDU_HEADER_t *pHdr;
  DOT1AS_ANNOUNCE_PDU_t *pAnnounce;
  DOT1AS_PATH_TRACE_TLV_t *pTlv;
  DOT1AS_INTF_STATE_t *pIntf;
  DOT1AS_PRIO_VECTOR_t msgPrioVec;
  DOT1AS_VECT_COMP_RESULT_t result;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t  lagCfgIntf;
#endif
  uint16_t  port;
  uint8_t   intfAnnounceReceiptTimeout;
  uint8_t   val8;

  if (dot1asCfgxAVNUModeGet(&val8) != AVB_SUCCESS) { val8 = AVB_DISABLE; }
  if (val8 == AVB_ENABLE)
  {
    return;
  }

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

  switch(event)
  {
    case DOT1AS_BMC_INTF_EVENT:
      /* For Interface events, check whether the interface is enabled
       * from BMC standpoint
       */
      if (dot1asIsAnnounceCapable(intfIndex) != TRUE)
      {
        if (pIntf->infoIs != DOT1AS_INFO_DISABLED)
        {
          /* If not already in DISABLED state, update the port role */
          pIntf->infoIs = DOT1AS_INFO_DISABLED;
        }
      }
      else
      {
        dot1asIntfAnnounceInit(intfIndex);
        /* AGED state */
        pIntf->infoIs = DOT1AS_INFO_AGED;
      }

      /* Interface events are triggered even when prio1/prio2 changes. In which
       * case, we need to recompute the BMC (even if interface role is DISABLED).
       */
      pIntf->reselect = TRUE;
      pIntf->selected = FALSE;
      break;

    case DOT1AS_BMC_ANNOUNCE_RX:
      /* ANNOUNCE pdu received. Compare port priority and msg priority vectors */
      pPdu = (DOT1AS_PDU_t *)pData;
      pHdr = &pPdu->hdr;
      pAnnounce = &pPdu->body.announcePdu;

      sal_memset(&msgPrioVec, 0, sizeof(msgPrioVec));
      msgPrioVec.sysId.priority1 = pAnnounce->gmPriority1;
      msgPrioVec.sysId.priority2 = pAnnounce->gmPriority2;
      msgPrioVec.sysId.quality =  pAnnounce->gmClockQuality;
      DOT1AS_CLOCK_ID_ASSIGN(&msgPrioVec.sysId.clock, &pAnnounce->gmClock);
      msgPrioVec.steps = pAnnounce->stepsRemoved;
      sal_memcpy(&msgPrioVec.sourcePort, &pHdr->portId,
             sizeof(msgPrioVec.sourcePort));

      msgPrioVec.rcvdPortNum = intfIndex;

      /* Adjust the incoming logMessageInterval */
      if (dot1asIntervalLog2Usec(pHdr->pduInterval, &interval) != AVB_SUCCESS)
      {
        /* Use this port's interval value */
        interval = pIntf->meanAnnounceInterval;
      }
      dot1asIntervalUsAdjust(&interval);

      result = dot1asBmcaCompareVectors(&msgPrioVec, &pIntf->portPrioVec);

      if (result == DOT1AS_VECT_SUPERIOR)
      {
        /* Message priority vector is better than Port priority vector.
         * Sending port is the new master port
         */

        DOT1AS_TRACE_BMCA("DOT1AS: Received better msg on %d. PortPrio=%d,%d,"
                     "%02x:%02x:%02x, MsgPrio=%d,%d,%02x:%02x:%02x\n", avbCfgxIntfNum[intfIndex],
                     pIntf->portPrioVec.sysId.priority1,
                     pIntf->portPrioVec.sysId.priority2,
                     pIntf->portPrioVec.sysId.clock.id[5],
                     pIntf->portPrioVec.sysId.clock.id[6],
                     pIntf->portPrioVec.sysId.clock.id[7],
                     msgPrioVec.sysId.priority1,
                     msgPrioVec.sysId.priority2,
                     msgPrioVec.sysId.clock.id[5],
                     msgPrioVec.sysId.clock.id[6],
                     msgPrioVec.sysId.clock.id[7]);

        pTlv = &pAnnounce->tlv;
        DOT1AS_TRACE_BMCA("DOT1AS: Rx TLV length %d\n", pTlv->length);
        for (index = 0; index < pTlv->length/DOT1AS_CLOCK_ID_LEN ; index++)
        {
          uint8_t *id;

          id = pTlv->clk[index].id;
          DOT1AS_TRACE_BMCA(" [%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x]\n",
                 id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

        }


        sal_memcpy(&pIntf->portPrioVec, &msgPrioVec, sizeof(msgPrioVec));
        pIntf->portSteps = pAnnounce->stepsRemoved;

        /* Record other ANNOUNCE fields */
        pIntf->announceFlags = pHdr->flags;
        pIntf->announceTimeSrc = pAnnounce->timeSource;
        pIntf->announceUtcOfst =  pAnnounce->currentUTCoffset;

        /* Update announceReceiptTimeoutInterval based on incoming interval */
        pIntf->announceReceiptTimeoutInterval = (interval * intfAnnounceReceiptTimeout);
        pIntf->announceReceiptTimerReset = TRUE; /* Reset timer */

        pIntf->infoIs = DOT1AS_INFO_RECEIVED;
        pIntf->reselect = TRUE;
        pIntf->selected = FALSE;
      }
      else if (result == DOT1AS_VECT_INFERIOR)
      {
        /* Message priority vector is inferior to Port priority vector */
        /* Ignore the received message */
      }
      else /* Repeated or Equal */
      {
        /* Message priority vector is equal to Port priority vector */

        /* Update announceReceiptTimeoutInterval based on incoming interval */
        pIntf->announceReceiptTimeoutInterval = (interval * intfAnnounceReceiptTimeout);
        pIntf->announceReceiptTimerReset = TRUE; /* Reset timer */
      }
      break;

    case DOT1AS_BMC_SYNC_TIMEOUT:
    case DOT1AS_BMC_ANNOUNCE_TIMEOUT:
      /* A SYNC timeout or ANNOUNCE timeout has occured */
      if (pIntf->infoIs == DOT1AS_INFO_RECEIVED)
      {
        pIntf->infoIs = DOT1AS_INFO_AGED;
        pIntf->reselect = TRUE;
        pIntf->selected = FALSE;
        DOT1AS_TRACE_BMCA("DOT1AS: BMCA %s rx timeout on port[%d]\n",
                          (event == DOT1AS_BMC_SYNC_TIMEOUT)? "Sync" : "Announce",
                          avbCfgxIntfNum[intfIndex]);
      }
      break;

    default:
      break;
  }

  /* Update the port roles if reselect if TRUE */
  if (pIntf->reselect == TRUE)
  {
    dot1asBmcaUpdateRoles();
  }
}


/******************************************************************************
* @purpose  Check BMCA timers on each of the interfaces
*
* @param    none
*
* @returns  none
*
* @comments Called every timer tick interval
*
* @end
*******************************************************************************/
void dot1asCheckBmcaTimers(void)
{
  uint32_t intfIndex = 0;
  uint8_t   intervalExpired = FALSE;
  DOT1AS_INTF_STATE_t *pIntf;

  for (intfIndex=0; intfIndex < dot1asMaxInterfaceCount_g; intfIndex++)
  {
    pIntf = &pDot1asIntf_g[intfIndex];
    intervalExpired = FALSE;

    if (dot1asIsAnnounceCapable(intfIndex) == FALSE)
    {
      /* There is no interface event callback mechanism. So, if the port becomes
       * inactive, re-run the BMCA to detect new GM and update the roles
       */
      if (pIntf->infoIs != DOT1AS_INFO_DISABLED)
      {
        dot1asInvokeBmca(intfIndex, DOT1AS_BMC_INTF_EVENT, NULL);
      }
      continue;
    }


    /* ANNOUNCE Trasmit timer */
    if (pIntf->announceTransmitTimerReset == TRUE)
    {
      pIntf->announceTransmitTimerReset = FALSE;
      pIntf->announceTransmitTimer = 0;

    }
    else
    {
      /* Note that the transmit timers are incremented irrespective of port role */
      pIntf->announceTransmitTimer += DOT1AS_TIMER_TICK_US;
      if (pIntf->announceTransmitTimer >= pIntf->meanAnnounceInterval)
      {
        intervalExpired = TRUE;
      }
      else
      {
        intervalExpired = FALSE;
      }

      if (intervalExpired == TRUE)
      {
        /* Reset timer */
        pIntf->announceTransmitTimer = 0;

        if ((pIntf->selected == TRUE) && (pIntf->updtInfo == FALSE) &&
            (pIntf->selectedRole == DOT1AS_ROLE_MASTER))
        {
          pIntf->announceSeqId++;
          dot1asAnnounceSend(intfIndex);
        }
      }
    }

    /* ANNOUNCE Receive timer */
    if (pIntf->announceReceiptTimerReset == TRUE)
    {
      pIntf->announceReceiptTimerReset = FALSE;
      pIntf->announceReceiptTimer = 0;
    }
    else
    {
      /* Note that the receive timers are incremented irrespective of port role */
      pIntf->announceReceiptTimer += DOT1AS_TIMER_TICK_US;
      intervalExpired = (pIntf->announceReceiptTimer >=
                         pIntf->announceReceiptTimeoutInterval) ? TRUE:FALSE;

      if (intervalExpired == TRUE)
      {
        /* Reset timer */
        pIntf->announceReceiptTimer = 0;
        if (pIntf->infoIs == DOT1AS_INFO_RECEIVED)
        {
          pIntf->stats.announceReceiptTimeouts++;
          dot1asInvokeBmca(intfIndex, DOT1AS_BMC_ANNOUNCE_TIMEOUT, NULL);
        }
      }
    }
  }
}


/* Function to convert BMCA port role to string */
uint8_t *dot1asDebugRoleStringGet(DOT1AS_PORT_ROLE_t role)
{
  uint8_t *str;
  switch (role)
  {
    case DOT1AS_ROLE_DISABLED:
      str = (uint8_t *)"Disabled";
      break;
    case DOT1AS_ROLE_MASTER:
      str = (uint8_t *)"Master";
      break;
    case DOT1AS_ROLE_SLAVE:
      str = (uint8_t *)"Slave";
      break;
    case DOT1AS_ROLE_PASSIVE:
      str = (uint8_t *)"Passive";
      break;
    default:
      str = (uint8_t *)"";
      break;
  }

  return str;
}


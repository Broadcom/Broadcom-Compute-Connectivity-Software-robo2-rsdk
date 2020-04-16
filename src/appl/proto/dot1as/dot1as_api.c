/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:    dot1as_api.c
 */

#include "dot1as.h"
#include "dot1as_cfgx.h"

/******************************************************************************
* @purpose  Get the gPTP Timestamp base
*
* @param    None
*
* @returns  gptp timestamp base in nano seconds
*
* @comments
*
* @end
*******************************************************************************/

uint64_t dot1asBridgeLocalTimeGet(void)
{
  uint64_t gPTPTime = 0;
  DOT1AS_TIMESTAMP_t *pLocalTime = &pDot1asBridge_g->bridgeLocalTime;;

  if (DOT1AS_IS_READY)
  {
    gPTPTime = ((pLocalTime->seconds * ONE_SEC) + (uint64_t)pLocalTime->nanoseconds);
  }

  return gPTPTime;
}

/******************************************************************************
* @purpose  Set the DOT1AS Global Admin Mode
*
* @param    mode @b{(input)} the new value of the DOT1AS mode
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asModeSet(DOT1AS_ADMIN_MODE_t mode)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t index;
  uint8_t val8;

  if (DOT1AS_IS_READY)
  {
    if (dot1asCfgxGlobalAdminGet(&val8) !=  AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
    if (val8 == mode)
    {
      return AVB_SUCCESS; /* Nothing to be done. */
    }

    /* Update the persistent storage */
    if (dot1asCfgxGlobalAdminSet(mode) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }

    /* Update the state machines. Change in global mode affects all ports */
    pDot1asBridge_g->adminMode = mode;
    for (index = 0; index < dot1asMaxInterfaceCount_g; index++)
    {
      rc = dot1asIntfUpdate(index);
    }
  }
  return rc;
}


/******************************************************************************
* @purpose  Get the DOT1AS Admin Mode
*
* @param    mode @b{(output)} the current value of the DOT1AS mode
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asModeGet(DOT1AS_ADMIN_MODE_t *mode)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t val8;

  if (DOT1AS_IS_READY)
  {
    rc = dot1asCfgxGlobalAdminGet(&val8);
    if (rc == AVB_SUCCESS)
    {
      *mode = (DOT1AS_ADMIN_MODE_t) val8;
    }
  }
  return rc;
}


/*******************************************************************************
* @purpose  Check whether the interface is eligible for 802.1AS operation
*
* @param    intIfNum  @b({input}) The internal interface number being checked
*
* @returns  TRUE is eligible, else FALSE
*
* @notes    none
*
* @end
*******************************************************************************/
uint8_t dot1asIsValidIntf(uint32_t intIfNum)
{
  /* Interface number is zero based */
  if (intIfNum >= dot1asMaxInterfaceCount_g)
  {
    return FALSE;
  }

  return TRUE;
}


/******************************************************************************
* @purpose  Set the DOT1AS Interface Mode
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    mode     @b{(input)} the new value of the DOT1AS mode
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfModeSet(uint32_t port, DOT1AS_ADMIN_MODE_t mode)
{
  AVB_RC_t             rc = AVB_FAILURE;
  DOT1AS_INTF_STATE_t *pIntf;
#ifdef DOT1AS_LAG_SUPPORT
  AVB_RC_t             port_found;
  uint32_t             intf;
  uint32_t             port_bitmap;
#endif
  uint8_t              val8;

  if (DOT1AS_IS_READY)
  {
    if (dot1asCfgxIntfPttPortEnabledGet((uint16_t) port, &val8) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }

    if (mode == (DOT1AS_ADMIN_MODE_t) val8)
    {
      return AVB_SUCCESS; /* Nothing to be done */
    }
    /* Update configuration */
    if (dot1asCfgxIntfPttPortEnabledSet((uint16_t) port, (uint8_t) mode) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }

#ifdef DOT1AS_LAG_SUPPORT
    port_bitmap = avb_bitmask_for_cfg_apply_get(port);
    port_found = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      pIntf = &pDot1asIntf_g[intf];
      pIntf->pttPortEnabled = mode;  /* Update state information */
      rc = dot1asIntfUpdate(intf);
      if (rc != AVB_SUCCESS)
      {
        break; /* Give up on first failure */
      }
      port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
    }
#else
      pIntf = &pDot1asIntf_g[port];
      pIntf->pttPortEnabled = mode;  /* Update state information */
      rc = dot1asIntfUpdate(port);
#endif

  }

  return rc;
}

/******************************************************************************
* @purpose  Get the configured DOT1AS Interface Mode
*
 @param    intIfNum @b{(input)} NIM Interface number
* @param    mode @b{(output)} the configured value of the DOT1AS mode
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfModeGet(uint32_t port, DOT1AS_ADMIN_MODE_t *mode)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t              val8;

  if (DOT1AS_IS_READY)
  {
    if (dot1asCfgxIntfPttPortEnabledGet((uint16_t) port, &val8) == AVB_SUCCESS)
    {
      *mode = (DOT1AS_ADMIN_MODE_t) val8;
      rc = AVB_SUCCESS;
    }
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the DOT1AS Interface Mode Active Status
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    mode     @b{(output)} the active value of the DOT1AS mode
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments For LAG interfaces the configured value is returned.
*           For physical interfaces the active state is returned since this can
*           be different than the configured value if the port is a member of a LAG.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfModeStatus(uint32_t port, DOT1AS_ADMIN_MODE_t *mode)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intIfNum;

  if (DOT1AS_IS_READY)
  {
    if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
#ifdef DOT1AS_LAG_SUPPORT
    if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
    {
      /* Get the configured value for the LAG */
      return dot1asIntfModeGet(port, mode);
    }
    else
    {
      pIntf = &pDot1asIntf_g[intIfNum];
      *mode = pIntf->pttPortEnabled;
      return AVB_SUCCESS;
    }
#else
    pIntf = &pDot1asIntf_g[intIfNum];
    *mode = pIntf->pttPortEnabled;
    return AVB_SUCCESS;
#endif
  }
  return AVB_FAILURE;
}


/******************************************************************************
* @purpose  Set the priority1/2 value of local clock in this 802.1AS system
*
* @param    type     @b{(input)} Priority1 or Priority2
* @param    priority @b{(input)} 0 - 255
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asBridgePrioritySet(DOT1AS_CLOCK_PRIORITY_t type, uint32_t priority)
{
  uint8_t  index;
  uint8_t  val8;

  if (priority > DOT1AS_MAX_PRIORITY_VALUE)
  {
    return AVB_FAILURE;
  }
  if (DOT1AS_IS_READY)
  {
    switch (type)
    {
      case DOT1AS_CLOCK_PRIORITY1:
        dot1asCfgxPriority1Get(&val8);
        if (val8 == (uint8_t) priority)
        {
          return AVB_SUCCESS; /* Nothing to be done */
        }
        dot1asCfgxAVNUClockStateGet(&val8);
        if ((priority == DOT1AS_MAX_PRIORITY_VALUE) && (val8 == DOT1AS_CLOCK_UPDATE_GM))
        {
          sal_printf("Cannot be configured priority1 as 255, since AVnu device clock state is GM\n");
          return AVB_FAILURE;
        }
        if (dot1asCfgxPriority1Set((uint8_t) priority) != AVB_SUCCESS)
        {
          return AVB_FAILURE;
        }
        pDot1asBridge_g->priority1 = priority;
        break;

      case DOT1AS_CLOCK_PRIORITY2:
        if (dot1asCfgxPriority2Get(&val8) != AVB_SUCCESS)
        {
          return AVB_FAILURE;
        }
        if (val8 == (uint8_t) priority)
        {
          return AVB_SUCCESS; /* Nothing to be done */
        }
        if (dot1asCfgxPriority2Set((uint8_t) priority) != AVB_SUCCESS)
        {
          return AVB_FAILURE;
        }
        pDot1asBridge_g->priority2 = priority;
        break;

      default:
        return AVB_FAILURE;
    }
  }

  /* As priority values have the highest precedence, any change in these
   * values will require re-computing GmPathPrio, GmPrio, and MasterPrio
   * vectors, which inturn causes port roles to be changed. Invoke BCMA,
   * enforcing interface event on each valid DOT1AS interface.
   */
  for (index = 0; index < dot1asMaxInterfaceCount_g; index++)
  {
    dot1asInvokeBmca(index, DOT1AS_BMC_INTF_EVENT, NULL);
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the priority1/2 value of local clock in this 802.1AS system
*
* @param    type     @b{(input)} Priority1 or Priority2
* @param    priority @b{(output)} 0 - 255
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asBridgePriorityGet(DOT1AS_CLOCK_PRIORITY_t type,
                                 uint32_t *priority)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  val8;

  if (DOT1AS_IS_READY)
  {
    if (type == DOT1AS_CLOCK_PRIORITY1)
    {
      if (dot1asCfgxPriority1Get(&val8) != AVB_SUCCESS)
      {
        return AVB_FAILURE;
      }
      *priority = (uint32_t) val8;
      rc = AVB_SUCCESS;
    }
    else if (type == DOT1AS_CLOCK_PRIORITY2)
    {
      if (dot1asCfgxPriority2Get(&val8) != AVB_SUCCESS)
      {
        return AVB_FAILURE;
      }
      *priority = (uint32_t) val8;
      rc = AVB_SUCCESS;
    }
  }

  return rc;
}


/******************************************************************************
* @purpose  Check if there is a Grand Master elected in the 802.1AS domain
*
* @param    isGmPresent @b{(output)} TRUE is GM is elected, else FALSE
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIsGrandmasterPresent(uint32_t *isGmPresent)
{
  *isGmPresent = pDot1asBridge_g->isGmPresent;
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the Best clock identity as detected by this time-aware system
*
* @param    id_upper @b{(output)} Clock identity (upper half)
* @param    id_lower @b{(output)} Clock identity (lower half)
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asBestClkIdGet(uint32_t *id_upper, uint32_t *id_lower)
{
  sal_memcpy(id_upper, pDot1asBridge_g->gmClock.id+4, DOT1AS_CLOCK_ID_LEN/2);
  sal_memcpy(id_lower, pDot1asBridge_g->gmClock.id, DOT1AS_CLOCK_ID_LEN/2);

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the priority1/2 value of Best clock in the 802.1AS domain
*
* @param    type     @b{(input)} Priority1 or Priority2
* @param    priority @b{(output)} 0 - 255
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asBestClkPriorityGet(DOT1AS_CLOCK_PRIORITY_t type,
                                  uint32_t *priority)
{
  if (type == DOT1AS_CLOCK_PRIORITY1)
  {
    *priority = pDot1asBridge_g->gmPriority1;
  }
  else if (type == DOT1AS_CLOCK_PRIORITY2)
  {
    *priority = pDot1asBridge_g->gmPriority2;
  }
  else
  {
     return AVB_FAILURE;
  }

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the steps to Best clock as detected by this time-aware system
*
* @param    steps @b{(output)} Steps removed value for this time-aware system
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asBestClkStepsGet(uint32_t *steps)
{
  *steps = pDot1asBridge_g->gmStepsRemoved;
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the clock identity of this bridge
*
* @param    id_upper @b{(output)} Clock identity (upper half)
* @param    id_lower @b{(output)} Clock identity (lower half)
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asBridgeClkIdGet(uint32_t *id_upper, uint32_t *id_lower)
{

  sal_memcpy(id_upper, pDot1asBridge_g->thisClock.id+4, DOT1AS_CLOCK_ID_LEN/2);
  sal_memcpy(id_lower, pDot1asBridge_g->thisClock.id, DOT1AS_CLOCK_ID_LEN/2);

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Check if the given interface 802.1AS capable (as determined by the
*           peer delay mechanism)
*
* @param    port @b{(input)} external port number
* @param    asCapable @b{(output)} TRUE if capable, else FALSE
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfIsCapable(uint32_t port, uint32_t *asCapable)
{
  uint32_t intIfNum;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }
#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    uint32_t memberIntf;
    if (avb_lag_active_intf_get(port, &memberIntf) != AVB_SUCCESS)
    {
      *asCapable = FALSE;
    }
    else
    {
      *asCapable = pDot1asIntf_g[memberIntf].dot1asCapable;
    }
  }
  else
  {
    *asCapable = pDot1asIntf_g[intIfNum].dot1asCapable;
  }
#else
  *asCapable = pDot1asIntf_g[intIfNum].dot1asCapable;
#endif

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the propagation delay on the given interface (as computed by the
*           pdelay mechanism)
*
* @param    intIfNum @b{(input)} External AVB Interface number
* @param    delay @b{(output)} the current value of the Pdelay
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfPdelayGet(uint32_t port, uint32_t *delay)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intIfNum;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    uint32_t memberIntf;
    if (avb_lag_active_intf_get(port, &memberIntf) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
    intIfNum = memberIntf;
  }
#endif

  pIntf = &pDot1asIntf_g[intIfNum];

  *delay = (uint32_t)(pIntf->neighborPropDelay);
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the neighbor rate ratio on the given interface (as computed by
*           the pdelay mechanism)
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    rateRatio @b{(output)} the current value of the rate ratio
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfNeighborRateRatioGet(uint32_t port, int32_t *rateRatio)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intIfNum;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    uint32_t memberIntf;
    if (avb_lag_active_intf_get(port, &memberIntf) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
    intIfNum = memberIntf;
  }
#endif

  pIntf = &pDot1asIntf_g[intIfNum];

  /* Neighbor rate ratio is expressed as the fractional frequency offset
   * multiplied by 2**41, i.e., the quantity (neighborRateRatio - 1.0)(2^41).
   */
  *rateRatio = (pIntf->neighborRateRatio - DOT1AS_RATE_RATIO_DEFAULT) << 11;

  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the 'isMeasuringPdelay' property, which is set to TRUE if the
*           given interface is receiving PDELAY response messages from other
*           end of the link
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    isMeasuringPdelay @b{(output)} TRUE if other end if responding
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfIsMeasuringPdelay(uint32_t port,
                                     uint32_t *isMeasuringPdelay)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intIfNum;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    uint32_t memberIntf;
    if (avb_lag_active_intf_get(port, &memberIntf) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
    intIfNum = memberIntf;
  }
#endif

  pIntf = &pDot1asIntf_g[intIfNum];

  *isMeasuringPdelay = pIntf->isMeasuringPdelay;
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Get the number of Pdelay_Req messages for which a valid response is
*           not received, above which a port is considered to not be exchanging
*            peer delay messages with its neighbor
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    numAllowedLostResp @b{(output)} Number of allowed lost response
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments For LAG interfaces the configured value is returned.
*           For physical interfaces the active state is returned since this can
*           be different than the configured value if the port is a member of a LAG.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfAllowedLostRespStatus(uint32_t port, uint32_t *numAllowedLostResp)
{
  if (DOT1AS_IS_READY)
  {
#ifdef DOT1AS_LAG_SUPPORT
    if (AVB_LAG_INTF_IS_LAG(port))
    {
      /* Get the configured value for the LAG */
      return dot1asCfgxIntfAllowedLostResponsesGet(port, numAllowedLostResp);
    }
    else
    {
#endif
      DOT1AS_INTF_STATE_t *pIntf;
      uint32_t             intIfNum;

      if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
      {
        return AVB_FAILURE;
      }
      pIntf = &pDot1asIntf_g[intIfNum];
      *numAllowedLostResp = pIntf->allowedLostReponses;
      return AVB_SUCCESS;
#ifdef DOT1AS_LAG_SUPPORT
    }
#endif
  }
  return AVB_FAILURE;
}



/******************************************************************************
* @purpose  Set the number of Pdelay_Req messages for which a valid response is
*           not received, above which a port is considered to not be exchanging
*            peer delay messages with its neighbor
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    numAllowedLostResp @b{(input)} Number of allowed lost response
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfAllowedLostRespSet(uint32_t port, uint32_t numAllowedLostResp)
{
  AVB_RC_t             rc = AVB_FAILURE;
  DOT1AS_INTF_STATE_t *pIntf;
#ifdef DOT1AS_LAG_SUPPORT
  AVB_RC_t             port_found;
  uint32_t             intf;
  uint32_t             port_bitmap;
#endif
  uint32_t             val32;

  if (DOT1AS_IS_READY)
  {
    if (dot1asCfgxIntfAllowedLostResponsesGet((uint16_t) port, &val32) == AVB_FAILURE)
    {
      return AVB_FAILURE;
    }
    if (val32 == numAllowedLostResp)
    {
      return AVB_SUCCESS; /* Nothing to be done */
    }
    /* Update configuration */
    if (dot1asCfgxIntfAllowedLostResponsesSet(port, numAllowedLostResp) == AVB_FAILURE)
    {
      return AVB_FAILURE;
    }
#ifdef DOT1AS_LAG_SUPPORT
    port_bitmap = avb_bitmask_for_cfg_apply_get(port);
    port_found = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      pIntf = &pDot1asIntf_g[intf];
      pIntf->allowedLostReponses = numAllowedLostResp;  /* Update state information */
      port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
    }
#else
    pIntf = &pDot1asIntf_g[port];
    pIntf->allowedLostReponses = numAllowedLostResp;  /* Update state information */
#endif
    rc = AVB_SUCCESS;
  }
  return rc;
}


/******************************************************************************
* @purpose  Get the configured propagation delay threshold on the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    threshold @b{(output)} the configured Pdelay threshold (nanosecs)
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments The threshold determines whether the port is capable of participating
*           in the 802.1AS protocol. The peer delay must be less than the
*           threshold value configured on the given interface.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfPdelayThresholdGet(uint32_t port, uint32_t *threshold)
{
  AVB_RC_t rc = AVB_FAILURE;

  if (DOT1AS_IS_READY)
  {
    rc = dot1asCfgxIntfAllowedLostResponsesGet((uint16_t) port, threshold);
  }
  return rc;
}


/******************************************************************************
* @purpose  Get the propagation delay threshold on the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    threshold @b{(output)} the current Pdelay threshold (nanosecs)
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments The threshold determines whether the port is capable of participating
*           in the 802.1AS protocol. The peer delay must be less than the
*           threshold value configured on the given interface.
*           For LAG interfaces the configured value is returned.
*           For physical interfaces the active state is returned since this can
*           be different than the configured value if the port is a member of a LAG.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfPdelayThresholdStatus(uint32_t port, uint32_t *threshold)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intIfNum;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  if (DOT1AS_IS_READY)
  {
#ifdef DOT1AS_LAG_SUPPORT
    if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
    {
      /* Get the configured value for the LAG */
      return dot1asIntfPdelayThresholdGet(port, threshold);
    }
    else
    {
      pIntf = &pDot1asIntf_g[intIfNum];
      *threshold = pIntf->neighborPropDelayThres;
      return AVB_SUCCESS;
    }
#else
    pIntf = &pDot1asIntf_g[intIfNum];
    *threshold = pIntf->neighborPropDelayThres;
    return AVB_SUCCESS;
#endif
  }

  return AVB_FAILURE;
}

/******************************************************************************
* @purpose  Set the propagation delay threshold on the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    threshold @b{(input)} the Pdelay threshold (nanosecs)
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments The threshold determines whether the port is capable of participating
*           in the 802.1AS protocol. The peer delay must be less than the
*           threshold value configured on the given interface.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfPdelayThresholdSet(uint32_t port, uint32_t threshold)
{
  AVB_RC_t             rc = AVB_FAILURE;
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             val32;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t             intf;
  uint32_t             port_bitmap;
  AVB_RC_t             port_found;
#endif

  if (DOT1AS_IS_READY)
  {
    if (dot1asCfgxIntfNeighborPropDelayThreshGet((uint16_t) port, &val32) == AVB_FAILURE)
    {
      return AVB_FAILURE;
    }
    if (val32 == threshold)
    {
      return AVB_SUCCESS; /* Nothing to be done */
    }
    /* Update configuration */
    if (dot1asCfgxIntfNeighborPropDelayThreshSet(port, threshold) == AVB_FAILURE)
    {
      return AVB_FAILURE;
    }
#ifdef DOT1AS_LAG_SUPPORT
    port_bitmap = avb_bitmask_for_cfg_apply_get(port);
    port_found = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      pIntf = &pDot1asIntf_g[intf];
      pIntf->neighborPropDelayThres = threshold;  /* Update state information */
      port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
    }
#else
    pIntf = &pDot1asIntf_g[port];
    pIntf->neighborPropDelayThres = threshold;  /* Update state information */
#endif
    rc = AVB_SUCCESS;
  }

  return rc;
}


/******************************************************************************
* @purpose  Get the 802.1AS role of the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    role @b{(output)} the current value of the interface's role
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfRoleGet(uint32_t port, DOT1AS_PORT_ROLE_t *role)
{
  uint32_t             intIfNum;

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    uint32_t memberIntf;
    if (avb_lag_active_intf_get(port, &memberIntf) != AVB_SUCCESS)
    {
      return AVB_FAILURE;
    }
    intIfNum = memberIntf;
  }
#endif

  *role = pDot1asIntf_g[intIfNum].selectedRole;

  return AVB_SUCCESS;
}



/******************************************************************************
* @purpose  Get the given interval on the given interface
*
* @param    type @b{(input)} DOT1AS interval type
* @param    intIfNum @b{(input)}  NIM Interface number
* @param    val @b{(output)} interval/timeout value
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    Interval is specified in logarithm base 2 format. Timeout is
*           specified in number of transmission intervals. Timeout interval is
*           specified in 32-bit nanosecond value.
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfIntervalGet(DOT1AS_INTF_INTERVAL_t type,
                           uint32_t port,
                           int32_t *val)
{
  AVB_RC_t             rc = AVB_SUCCESS;
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intIfNum;
  int8_t               intvlVal; 

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }
  if (DOT1AS_IS_READY == FALSE)
  {
    return AVB_FAILURE;
  }

#ifdef DOT1AS_LAG_SUPPORT
  if (AVB_INTERNAL_INTF_IS_LAG(intIfNum))
  {
    uint32_t memberIntf;
    if (avb_lag_active_intf_get(port, &memberIntf) != AVB_SUCCESS)
    {
      *val = 0;
      return AVB_FAILURE;
    }
    pIntf = &pDot1asIntf_g[memberIntf]; /* State information needs to point to a lag member */
  }
  else
  {
    pIntf = &pDot1asIntf_g[intIfNum];
  }
#else
    pIntf = &pDot1asIntf_g[intIfNum];
#endif
  switch (type)
  {
    case DOT1AS_INITIAL_SYNC_TX_INTERVAL:
      /* Get the configured SYNC transmit interval on the given interface */
      rc = dot1asCfgxIntfInitialLogSyncIntervalGet((uint16_t) port, &intvlVal);
      *val = intvlVal;
      break;

    case DOT1AS_CURRENT_SYNC_TX_INTERVAL:
      /* Get the current SYNC transmit interval on the given interface */
      *val = pIntf->currentLogSyncInterval;
      break;

    case DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL:
      /* Get the configured ANNOUNCE transmit interval on the given interface */
      rc = dot1asCfgxIntfInitialLogAnnounceIntervalGet((uint16_t) port, &intvlVal);
      *val = intvlVal;
      break;

    case DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL:
      /* Get the current ANNOUNCE transmit interval on the given interface */
      *val = pIntf->currentLogAnnounceInterval;
      break;

    case DOT1AS_INITIAL_PDELAY_TX_INTERVAL:
      /* Get the configured PDELAY transmit interval on the given interface */
      rc = dot1asCfgxIntfInitialLogNbrPDelayIntervalGet((uint16_t) port, &intvlVal);
      *val = intvlVal;
      break;

    case DOT1AS_CURRENT_PDELAY_TX_INTERVAL:
      /* Get the current PDELAY transmit interval on the given interface */
      *val = pIntf->currentLogPdelayReqInterval;
      break;

    case DOT1AS_SYNC_RX_TIMEOUT:
      /* Specifies the number of SYNC intervals that have to pass without
       * receipt of SYNC information, before considering the master is no
       * longer transmitting.
       */
      rc = dot1asCfgxIntfSyncReceiptTimeoutGet((uint16_t) port, (uint8_t *) val);
      break;

    case DOT1AS_ANNOUNCE_RX_TIMEOUT:
      /* Specifies the number of ANNOUNCE intervals that have to pass without
       * receipt of ANNOUNCE PDU, before considering the master is no
       * longer transmitting.
       */
      rc = dot1asCfgxIntfAnnounceReceiptTimeoutGet((uint16_t) port, (uint8_t *) val);
      break;

    case DOT1AS_PDELAY_RX_TIMEOUT:
      /* Specifies the number of PDELAY intervals that have to pass without
       * receipt of PDELAY_RESP PDU, before considering the other end is no
       * longer responding. The transmit interval is also the timeout interval.
       */
      *val = 1;
      break;

    case DOT1AS_SYNC_RX_TIMEOUT_INTERVAL:
       /* Specifies the interval after which SYNC timeout occurs if timesync
        * information is not received on this port
        */
      *val = (int32_t)pIntf->syncReceiptTimeoutInterval;
      break;

    default:
      rc = AVB_FAILURE;
      break;
  }

  return rc;
}


/******************************************************************************
* @purpose  Set the given interval on the given interface
*
* @param    type @b{(input)} DOT1AS interval type
* @param    port @b{(input)}  NIM Interface number
* @param    val @b{(input)} interval/timeout value
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    Interval is specified in logarithm base 2 format. Timeout is
*           specified in number of transmission intervals.
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfIntervalSet(DOT1AS_INTF_INTERVAL_t type,
                               uint32_t port,
                               int32_t val)
{
  AVB_RC_t rc = AVB_SUCCESS;
  int8_t   ival8;
  uint8_t  val8;

  if (DOT1AS_IS_READY == FALSE)
  {
    return AVB_FAILURE;
  }

  switch (type)
  {
    case DOT1AS_INITIAL_SYNC_TX_INTERVAL:
      if (dot1asCfgxIntfInitialLogSyncIntervalGet((uint16_t) port, &ival8) != AVB_SUCCESS) { return AVB_FAILURE; }
      if (ival8 == (int8_t) val) { return AVB_SUCCESS; } /* Nothing to do */
      if (dot1asCfgxIntfInitialLogSyncIntervalSet((uint16_t) port, (int8_t) val) != AVB_SUCCESS) { return AVB_FAILURE; }
      break;

    case DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL:
      if (dot1asCfgxIntfInitialLogAnnounceIntervalGet((uint16_t) port, &ival8) != AVB_SUCCESS) { return AVB_FAILURE; }
      if (ival8 == (int8_t) val) { return AVB_SUCCESS; } /* Nothing to do */
      if (dot1asCfgxIntfInitialLogAnnounceIntervalSet((uint16_t) port, (int8_t) val) != AVB_SUCCESS) { return AVB_FAILURE; }
      break;

    case DOT1AS_INITIAL_PDELAY_TX_INTERVAL:
      if (dot1asCfgxIntfInitialLogNbrPDelayIntervalGet((uint16_t) port, &ival8) != AVB_SUCCESS) { return AVB_FAILURE; }
      if (ival8 == (int8_t) val) { return AVB_SUCCESS; } /* Nothing to do */
      if (dot1asCfgxIntfInitialLogNbrPDelayIntervalSet((uint16_t) port, (int8_t) val) != AVB_SUCCESS) { return AVB_FAILURE; }
      break;

    case DOT1AS_SYNC_RX_TIMEOUT:
      if (val > 255) { return AVB_FAILURE; } 
      if (dot1asCfgxIntfSyncReceiptTimeoutGet((uint16_t) port, &val8) != AVB_SUCCESS) { return AVB_FAILURE; }
      if (val8 == (int8_t) val) { return AVB_SUCCESS; } /* Nothing to do */
      if (dot1asCfgxIntfSyncReceiptTimeoutSet((uint16_t) port, (uint8_t) val) != AVB_SUCCESS) { return AVB_FAILURE; }
      break;

    case DOT1AS_ANNOUNCE_RX_TIMEOUT:
      if (val > 255) { return AVB_FAILURE; } 
      if (dot1asCfgxIntfAnnounceReceiptTimeoutGet((uint16_t) port, &val8) != AVB_SUCCESS) { return AVB_FAILURE; }
      if (val8 == (int8_t) val) { return AVB_SUCCESS; } /* Nothing to do */
      if (dot1asCfgxIntfAnnounceReceiptTimeoutSet((uint16_t) port, (uint8_t) val) != AVB_SUCCESS) { return AVB_FAILURE; }
      break;

    default:
      return AVB_FAILURE;
  }

  /* As per the LinkDelaySyncIntervalSetting and AnnounceIntervalSetting state
   * machine, the intervals are not applied until interface is re-initialized.
   * So, only rx timeouts are applied here.
   */
  if ((type == DOT1AS_SYNC_RX_TIMEOUT) || (type == DOT1AS_ANNOUNCE_RX_TIMEOUT))
  {
#ifdef DOT1AS_LAG_SUPPORT
    uint32_t intf;
    uint32_t port_bitmap = avb_bitmask_for_cfg_apply_get(port);
    uint32_t port_found  = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      dot1asIntervalUpdate(intf, type, val);
      port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
    }
#else
    dot1asIntervalUpdate(port, type, val);
#endif
  }

  /* Update initial sync, pdelay and announce intervals only when the interface
   * is in disabled state
   */
  if (dot1asCfgxIntfPttPortEnabledGet((uint16_t) port, &val8) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }
  if (val8 != DOT1AS_ENABLE)
  {
    /* Allow interval settings only when the intf in disabled state */
    dot1asIntervalUpdate(port, type, val);
  }
  return rc;
}
/******************************************************************************
* @purpose  Get the DOT1AS Interface status for the given interface
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *value         @b{(output)} values of interface status
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfStatus(uint32_t port, DOT1AS_INTF_STATE_t *as_intf_status)
{
  uint32_t             intIfNum;
  uint32_t             intf;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t             member_port;
  AVB_RC_t             rc;
#endif

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
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
        memset(as_intf_status, 0x00, sizeof(DOT1AS_INTF_STATE_t));
        return AVB_SUCCESS;
      }
    }
  }
  else
  {
    intf = intIfNum;
  }
#else
  intf = intIfNum;
#endif
  sal_memcpy(as_intf_status, &pDot1asIntf_g[intf], sizeof(DOT1AS_INTF_STATE_t));
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Get the value of given DOT1AS counter for the given interface
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    counter        @b{(input)} one of the counter types
* @param    *value         @b{(output)} value of the counter
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfStatsGet(uint32_t port,
                        DOT1AS_COUNTER_TYPE_t type,
                        uint32_t *value, DOT1AS_INTF_STATS_t *as_stats)
{
  DOT1AS_INTF_STATS_t *pStats;
  uint32_t             intIfNum;
  uint32_t             intf;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t             member_port;
  AVB_RC_t             rc;
#endif

  if (avb_intf_external_to_internal(port, &intIfNum) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
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
        *value = 0;
        return AVB_SUCCESS;
      }
    }
  }
  else
  {
    intf = intIfNum;
  }

#else
  intf = intIfNum;
#endif
  pStats = &pDot1asIntf_g[intf].stats;

  switch (type)
  {
    case DOT1AS_SYNC_TX_COUNT:
      *value = pStats->syncTxCount;
      break;
    case DOT1AS_SYNC_RX_COUNT:
      *value = pStats->syncRxCount;
      break;
    case DOT1AS_FOLLOWUP_TX_COUNT:
      *value = pStats->followUpTxCount;
      break;
    case DOT1AS_FOLLOWUP_RX_COUNT:
      *value = pStats->followUpRxCount;
      break;
    case DOT1AS_ANNOUNCE_TX_COUNT:
      *value = pStats->announceTxCount;
      break;
    case DOT1AS_ANNOUNCE_RX_COUNT:
      *value = pStats->announceRxCount;
      break;
    case DOT1AS_PDELAY_REQ_TX_COUNT:
      *value = pStats->pDelayReqTxCount;
      break;
    case DOT1AS_PDELAY_REQ_RX_COUNT:
      *value = pStats->pDelayReqRxCount;
      break;
    case DOT1AS_PDELAY_RESP_TX_COUNT:
      *value = pStats->pDelayRespTxCount;
      break;
    case DOT1AS_PDELAY_RESP_RX_COUNT:
      *value = pStats->pDelayRespRxCount;
      break;
    case DOT1AS_PDELAY_RESP_FOLLOWUP_TX_COUNT:
      *value = pStats->pDelayRespFollowUpTxCount;
      break;
    case DOT1AS_PDELAY_RESP_FOLLOWUP_RX_COUNT:
      *value = pStats->pDelayRespFollowUpRxCount;
      break;
    case DOT1AS_SIGNALING_TX_COUNT:
      *value = pStats->signalingTxCount;
      break;
    case DOT1AS_SIGNALING_RX_COUNT:
      *value = pStats->signalingRxCount;
      break;
    case DOT1AS_SYNC_RX_TIMEOUTS:
      *value = pStats->syncReceiptTimeouts;
      break;
    case DOT1AS_SYNC_RX_DISCARDS: /* Sync + Followups discarded */
      *value = pStats->syncRxDiscards;
      break;
    case DOT1AS_ANNOUNCE_RX_TIMEOUTS:
      *value = pStats->announceReceiptTimeouts;
      break;
    case DOT1AS_ANNOUNCE_RX_DISCARDS:
      *value = pStats->announceRxDiscards;
      break;
    case DOT1AS_PDELAY_RX_TIMEOUTS:
      *value = pStats->pDelayReceiptTimeouts;
      break;
    case DOT1AS_PDELAY_RX_DISCARDS:
      *value = pStats->pDelayReqRxDiscards + pStats->pDelayRespRxDiscards;
      break;
    case DOT1AS_BAD_HEADER_COUNT:
      *value = pStats->badHeaderCount;
      break;
    case DOT1AS_PTP_DISCARD_COUNT:
      *value = pStats->ptpDiscardCount;
      break;
    case DOT1AS_PDELAY_LOST_RESP_EXCEEDED:
      *value = pStats->pDelayLostResponsesExceeded;
      break;
    case DOT1AS_SYNC_TOO_FAST:
      *value = pStats->syncTooFast;
      break;
    case DOT1AS_SIGNALING_RX_DISCARDS:
      *value = pStats->signalingRxDiscards;
      break;
    case DOT1AS_PDELAY_REQ_RX_DISCARDS:
      *value = pStats->pDelayReqRxDiscards;
      break;
    case DOT1AS_PDELAY_RESP_RX_DISCARDS:
      *value = pStats->pDelayRespRxDiscards;
      break;
    case DOT1AS_FOLLOWUP_RX_DISCARDS:
      *value = pStats->followupRxDiscards;
      break;
    case DOT1AS_SYNC_TX_TIMEOUTS:
      *value = pStats->syncTransmitTimeouts;
      break;
    case DOT1AS_FOLLOWUP_RX_TIMEOUTS:
      *value = pStats->followupReceiptTimeouts;
      break;
    case DOT1AS_BAD_PDELAY_VALUES:
      *value = pStats->badPdelayValues;
      break;
    case DOT1AS_TX_ERRORS:
      *value = pStats->txErrors;
      break;
    case DOT1AS_TIME_STAMP_ERROR:
      *value = pStats->tsErrors;
      break;
    case DOT1AS_COUNTER_ALL:
      sal_memcpy(as_stats, pStats, sizeof(DOT1AS_INTF_STATS_t));
      break;
    default:
      break;
  }

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Reset 802.1AS counters for the given interface
*
* @param    uint32  intIfNum  @b((input)) internal interface number
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfStatsReset(uint32_t intfNum)
{
  uint32_t index = 0;
  DOT1AS_INTF_STATS_t *pStats;
  uint32_t             port;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t             intf;
  uint32_t             port_bitmap;
  uint32_t             port_found;
#endif
  if (avb_intf_external_to_internal(intfNum, &port) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  } 
  if (port != 255) /* Clear given interface */
  {
#ifdef DOT1AS_LAG_SUPPORT
    port_bitmap = avb_bitmask_for_cfg_apply_get(port);
    port_found  = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      pStats = &pDot1asIntf_g[intf].stats;
      memset((uint8_t *)pStats, 0, sizeof(*pStats));
      port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
    }
#else
    pStats = &pDot1asIntf_g[port].stats;
    sal_memset((uint8_t *)pStats, 0, sizeof(*pStats));
#endif
  }
  else /* Clear all */
  {
    for (index = 0; index < dot1asMaxInterfaceCount_g; index++)
    {
      pStats = &pDot1asIntf_g[index].stats;
      sal_memset((uint8_t *)pStats, 0, sizeof(*pStats));
    }
  }

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Get the number of times the grandmaster has changed.
*
* @param    count - current value of gmChangeCount
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t dot1asGmChangeCountGet(uint32_t *count)
{
  *count = pDot1asBridge_g->gmChangeCount;

  return AVB_SUCCESS;
}

#ifdef DOT1AS_LAG_SUPPORT
/*@api
 * avb_lag_callback_dot1as
 *
 * @param=op            - Indicator of the LAG operation that just occurred
 * @param=lag           - The interface number for the lag
 * @param=member_bitmap - The internal bitmap for the lag member ports
 *
 * @brief
 * A lag has been added or removed,  make the required dot1as changes
 *
 *
 * @returns nothing
 */
void avb_lag_callback_dot1as(AVB_LAG_OP_t op, uint16_t lag, uint8_t member_bitmap)
{
  DOT1AS_INTF_STATE_t *pIntf;
  uint32_t             intf;
  AVB_RC_t             port_found;
  uint32_t             val32;
  uint8_t              val8;
  uint8_t              val8_lag;
  int8_t               ival8;

  if (!DOT1AS_IS_READY)
  {
    return;
  }

  switch(op) {

  case AVB_LAG_OP_ADD:

    /* Apply the lag config instead of the port config for all member ports */
    port_found = avb_port_next_get(AVB_TRUE, member_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      pIntf = &pDot1asIntf_g[intf];

      if (dot1asCfgxIntfAllowedLostResponsesGet(lag, &val32) == AVB_SUCCESS)
      {
        pIntf->allowedLostReponses = val32;
      }
      if (dot1asCfgxIntfNeighborPropDelayThreshGet(lag, &val32) == AVB_SUCCESS)
      {
        pIntf->neighborPropDelayThres = val32;
      }

      if (dot1asCfgxAVNUModeGet(&val8) != AVB_SUCCESS) { val8 = AVB_DISABLE; }
      if (val8 == AVB_ENABLE)
      {
        if (dot1asCfgxIntfAVNUDot1asCapableGet(lag, &val8) == AVB_SUCCESS)
        {
          pIntf->dot1asCapable = val8;
        }
        if (dot1asCfgxIntfAVNUPortRoleGet(lag, &val8) == AVB_SUCCESS)
        {
          pIntf->selectedRole = val8;
        }
        /* PDELAY configs */
        if (dot1asCfgxIntfAVNUNeighborPropDelayGet(lag, &val32) == AVB_SUCCESS)
        {
          pIntf->neighborPropDelay = val32;
        }
        if (dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet(lag, &ival8) == AVB_SUCCESS)
        {
          pIntf->currentLogPdelayReqInterval = (int32_t) ival8;
        }
        /* SYNC configs */
        if (dot1asCfgxIntfAVNUInitialLogSyncIntervalGet(lag, &ival8) == AVB_SUCCESS)
        {
          pIntf->currentLogSyncInterval = (int32_t) ival8;
        }
      }

      if (dot1asCfgxIntfSyncReceiptTimeoutGet(lag, &val8_lag) == AVB_SUCCESS)
      {
        if (dot1asCfgxIntfSyncReceiptTimeoutGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          if (val8 != val8_lag)
          {
            dot1asIntervalUpdate(intf, DOT1AS_SYNC_RX_TIMEOUT, (int32_t) val8_lag);
          }
        }
      }

      if (dot1asCfgxIntfAnnounceReceiptTimeoutGet(lag, &val8_lag) == AVB_SUCCESS)
      {
        if (dot1asCfgxIntfAnnounceReceiptTimeoutGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          if (val8 != val8_lag)
          {
            dot1asIntervalUpdate(intf, DOT1AS_ANNOUNCE_RX_TIMEOUT, (int32_t) val8_lag);
          }
        }
      }

      if (dot1asCfgxIntfPttPortEnabledGet(lag, &val8_lag) == AVB_SUCCESS)
      {
        if (dot1asCfgxIntfPttPortEnabledGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          if (val8 != val8_lag)
          {
            pIntf->pttPortEnabled = val8_lag;  /* Update state information */
            if (dot1asIntfUpdate(intf) != AVB_SUCCESS)
            {
              sal_printf("\navb_dot1as_lag_callback(AVB_LAG_OP_ADD) : dot1asIntfUpdate() failed for avb port %d\n",
                            avbCfgxIntfNum[intf]);/*TRGTRG */
            }
          }
        }
      }

      port_found = avb_port_next_get(AVB_FALSE, member_bitmap, &intf);
    }
    break; /* End AVB_LAG_OP_ADD */

  case AVB_LAG_OP_REMOVE:

    /* Apply the port config instead of the lag config for all member ports */
    port_found = avb_port_next_get(AVB_TRUE, member_bitmap, &intf);
    while (port_found == AVB_SUCCESS)
    {
      pIntf = &pDot1asIntf_g[intf];

      if (dot1asCfgxIntfAllowedLostResponsesGet((uint16_t) avbCfgxIntfNum[intf], &val32) == AVB_SUCCESS)
      {
        pIntf->allowedLostReponses = val32;
      }
      if (dot1asCfgxIntfNeighborPropDelayThreshGet((uint16_t) avbCfgxIntfNum[intf], &val32) == AVB_SUCCESS)
      {
        pIntf->neighborPropDelayThres = val32;
      }

      if (dot1asCfgxAVNUModeGet(&val8) != AVB_SUCCESS) { val8 = AVB_DISABLE; }
      if (val8 == AVB_ENABLE)
      {
        if (dot1asCfgxIntfAVNUDot1asCapableGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          pIntf->dot1asCapable = val8;
        }
        if (dot1asCfgxIntfAVNUPortRoleGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          pIntf->selectedRole = val8;
        }
        /* PDELAY configs */
        if (dot1asCfgxIntfAVNUNeighborPropDelayGet((uint16_t) avbCfgxIntfNum[intf], &val32) == AVB_SUCCESS)
        {
          pIntf->neighborPropDelay = val32;
        }
        if (dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet((uint16_t) avbCfgxIntfNum[intf], &ival8) == AVB_SUCCESS)
        {
          pIntf->currentLogPdelayReqInterval = (int32_t) ival8;
        }
        /* SYNC configs */
        if (dot1asCfgxIntfAVNUInitialLogSyncIntervalGet((uint16_t) avbCfgxIntfNum[intf], &ival8) == AVB_SUCCESS)
        {
          pIntf->currentLogSyncInterval = (int32_t) ival8;
        }
      }

      if (dot1asCfgxIntfSyncReceiptTimeoutGet(lag, &val8_lag) == AVB_SUCCESS)
      {
        if (dot1asCfgxIntfSyncReceiptTimeoutGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          if (val8 != val8_lag)
          {
            dot1asIntervalUpdate(intf, DOT1AS_SYNC_RX_TIMEOUT, (int32_t) val8);
          }
        }
      }

      if (dot1asCfgxIntfAnnounceReceiptTimeoutGet(lag, &val8_lag) == AVB_SUCCESS)
      {
        if (dot1asCfgxIntfAnnounceReceiptTimeoutGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          if (val8 != val8_lag)
          {
            dot1asIntervalUpdate(intf, DOT1AS_ANNOUNCE_RX_TIMEOUT, (int32_t) val8);
          }
        }
      }

      if (dot1asCfgxIntfPttPortEnabledGet(lag, &val8_lag) == AVB_SUCCESS)
      {
        if (dot1asCfgxIntfPttPortEnabledGet((uint16_t) avbCfgxIntfNum[intf], &val8) == AVB_SUCCESS)
        {
          if (val8 != val8_lag)
          {
            pIntf->pttPortEnabled = val8;  /* Update state information */
            if (dot1asIntfUpdate(intf) != AVB_SUCCESS)
            {
              sal_printf("\navb_dot1as_lag_callback(AVB_LAG_OP_REMOVE) : dot1asIntfUpdate() failed for avb port %d\n",
                         avbCfgxIntfNum[intf]);/*TRGTRG */
            }
          }
        }
      }

      port_found = avb_port_next_get(AVB_FALSE, member_bitmap, &intf);
    }

    break; /* End AVB_LAG_OP_REMOVE */

  default:
    break;
  }
}
#endif /* DOT1AS_LAG_SUPPORT */

/******************************************************************************
* @purpose  Set AVNU mode
*
* @param    avnu_mode @b{(input)} the new value of the AVNU mode
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuModeSet(uint32_t avnu_mode )
{
  uint8_t val8;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxAVNUModeGet(&val8) != AVB_SUCCESS))
  {
    return AVB_FAILURE;
  }

  if ((uint8_t) avnu_mode == val8)
  {
    return AVB_SUCCESS; /* Nothing to be done */
  }

  if (dot1asCfgxAVNUModeSet((uint8_t) avnu_mode) != AVB_SUCCESS)
  {
    return AVB_FAILURE;
  }

  pDot1asBridge_g->avnuMode = avnu_mode;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Get clock state
*
* @param    clockState @b{(fil)} the new value of clock state
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuClockStateGet(uint32_t *clockState)
{
  *clockState = pDot1asBridge_g->clkState;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Set clock state
*
* @param    clockState @b{(input)} the new value of clock state
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuClockStateSet(uint32_t clockState)
{
  uint8_t val8;
  if (!DOT1AS_IS_READY){ return AVB_FAILURE; }
  dot1asCfgxAVNUClockStateGet(&val8);
  if (val8 == (uint8_t)clockState) { return AVB_SUCCESS; }
  dot1asCfgxPriority1Get(&val8);
  if ((val8 == DOT1AS_MAX_PRIORITY_VALUE) && (clockState == DOT1AS_CLOCK_UPDATE_GM))
  {
    sal_printf("Cannot be configured AVnu device clock state as GM, since priority1 is 255\n");
    return AVB_FAILURE;
  }

  if (dot1asCfgxAVNUClockStateSet((uint8_t)clockState) != AVB_SUCCESS) { return AVB_FAILURE; }
  pDot1asBridge_g->clkState = clockState;
  return AVB_SUCCESS;
}
/******************************************************************************
* @purpose  Set Sync Absence Timeout
*
* @param    syncAbsenceTimeout @b{(input)} the new value of syncAbsenceTimeout
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuSyncAbsenceTimeoutSet(uint32_t syncAbsenceTimeout)
{
  uint8_t val8;
  if ((!DOT1AS_IS_READY) || (dot1asCfgxAVNUSyncAbsenceTimeoutGet(&val8) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (val8 == (uint8_t)syncAbsenceTimeout) { return AVB_SUCCESS; }
  if (dot1asCfgxAVNUSyncAbsenceTimeoutSet((uint8_t)syncAbsenceTimeout) != AVB_SUCCESS) { return AVB_FAILURE; }
  pDot1asBridge_g->avnuSyncAbsenceTimeout = DOT1AS_INTL_SEC_TO_US((uint8_t)syncAbsenceTimeout);
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Set port role
*
* @param    port_role @b{(input)} the new value of port role
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuPortRoleSet(uint32_t port, uint32_t port_role )
{
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t port_bitmap;
  AVB_RC_t port_found;
  uint32_t intf;
#endif
  uint8_t  val8;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxIntfAVNUPortRoleGet((uint16_t) port, &val8) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (val8 == (uint8_t) port_role) { return AVB_SUCCESS; }
  if (dot1asCfgxIntfAVNUPortRoleSet((uint16_t) port, (uint8_t) port_role) != AVB_SUCCESS) { return AVB_FAILURE; }

#ifdef DOT1AS_LAG_SUPPORT
  port_bitmap = avb_bitmask_for_cfg_apply_get(port);
  port_found = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
  while (port_found == AVB_SUCCESS)
  {
    pDot1asIntf_g[intf].selectedRole = port_role;
    port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
  }

#else
  pDot1asIntf_g[port].selectedRole = port_role;
#endif
  return AVB_SUCCESS;
}
/******************************************************************************
* @purpose  Set propagation delay
*
* @param    intfIndex @b{(input)}
* @param    pDelay @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuPDelaySet(uint32_t port, uint32_t pDelay)
{
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t port_bitmap;
  AVB_RC_t port_found;
  uint32_t intf;
#endif
  uint32_t val32;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxIntfAVNUNeighborPropDelayGet((uint16_t) port, &val32) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (val32 == pDelay) { return AVB_SUCCESS; }
  if (dot1asCfgxIntfAVNUNeighborPropDelaySet((uint16_t) port, pDelay) != AVB_SUCCESS) { return AVB_FAILURE; }

#ifdef DOT1AS_LAG_SUPPORT
  port_bitmap = avb_bitmask_for_cfg_apply_get(port);
  port_found = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
  while (port_found == AVB_SUCCESS)
  {
    pDot1asIntf_g[intf].neighborPropDelay = pDelay;
    port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
  }
#else
  pDot1asIntf_g[port].neighborPropDelay = pDelay;
#endif

  return AVB_SUCCESS;
}
/******************************************************************************
* @purpose  Set DOT1AS capable
*
* @param    port      @b{(input)} External Interface Number
* @param    asCapable @b{(input)} TRUE or FALSE
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuAsCapableSet(uint32_t port, uint32_t asCapable)
{
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t port_bitmap;
  AVB_RC_t port_found;
  uint32_t intf;
#endif
  uint8_t  val8;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxIntfAVNUDot1asCapableGet((uint16_t) port, &val8) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (val8 == (uint8_t) asCapable) { return AVB_SUCCESS; }
  if (dot1asCfgxIntfAVNUDot1asCapableSet((uint16_t) port, (uint8_t) asCapable) != AVB_SUCCESS) { return AVB_FAILURE; }

#ifdef DOT1AS_LAG_SUPPORT
  port_bitmap = avb_bitmask_for_cfg_apply_get(port);
  port_found = avb_port_next_get(AVB_TRUE, port_bitmap, &intf);
  while (port_found == AVB_SUCCESS)
  {
    pDot1asIntf_g[intf].dot1asCapable = asCapable;
    dot1asIntfUpdate(intf);
    port_found = avb_port_next_get(AVB_FALSE, port_bitmap, &intf);
  }
#else
  pDot1asIntf_g[port].dot1asCapable = asCapable;
  dot1asIntfUpdate(port);
#endif

  return AVB_SUCCESS;
}
/******************************************************************************
* @purpose  Set Initial Log PDelay Request Interval
*
* @param    intfIndex @b{(input)}
* @param    initialLogPdelayReqInterval @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuInitialLogPdelayReqIntervalSet(uint32_t port, int32_t initialLogPdelayReqInterval)
{
  int8_t   ival8;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet((uint16_t) port, &ival8) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (ival8 == (int8_t) initialLogPdelayReqInterval) { return AVB_SUCCESS; }
  if (dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalSet((uint16_t) port, (int8_t) initialLogPdelayReqInterval) != AVB_SUCCESS) { return AVB_FAILURE; }
  return AVB_SUCCESS;
}
/******************************************************************************
* @purpose  Set Operational Log PDelay Request Interval
*
* @param    intfIndex @b{(input)}
* @param    opearationalLogPdelayReqInterval @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuOperationalLogPdelayReqIntervalSet(uint32_t port, int32_t operationalLogPdelayReqInterval)
{
  int8_t   ival8;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalGet((uint16_t) port, &ival8) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (ival8 == (int8_t) operationalLogPdelayReqInterval) { return AVB_SUCCESS; }
  if (dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalSet((uint16_t) port, (int8_t) operationalLogPdelayReqInterval) != AVB_SUCCESS) { return AVB_FAILURE; }
  return AVB_SUCCESS;
}
/******************************************************************************
* @purpose  Set Initial Log Sync Interval
*
* @param    intfIndex @b{(input)}
* @param    initialLogSyncInterval @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t avnuInitialLogSyncIntervalSet(uint32_t port, int32_t initialLogSyncInterval)
{
  int8_t   ival8;

  if ((!DOT1AS_IS_READY) || (dot1asCfgxIntfAVNUInitialLogSyncIntervalGet((uint16_t) port, &ival8) != AVB_SUCCESS)) { return AVB_FAILURE; }
  if (ival8 == (int8_t) initialLogSyncInterval) { return AVB_SUCCESS; }
  if (dot1asCfgxIntfAVNUInitialLogSyncIntervalSet((uint16_t) port, (int8_t) initialLogSyncInterval) != AVB_SUCCESS) { return AVB_FAILURE; }
  return AVB_SUCCESS;
}

/******************************************************************************
*
* @purpose Function to determine avnu global status
*
* @param    dot1asStatus @b{(output)}
* @param    gmStatus @b{(output)}
* @param    slavePort @b{(output)}
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void avnuStatusGet(uint32_t *dot1asStatus, uint32_t *gmStatus, uint32_t *slavePortIndex)
{
  uint32_t intIfNum = dot1asMaxInterfaceCount_g;
  uint8_t avnuASCapable = FALSE;
  uint8_t dot1asCapable = FALSE;

  if (pDot1asBridge_g->avnuMode != TRUE)
  {
    if (pDot1asBridge_g->adminMode == FALSE)
    {
      *dot1asStatus = DOT1AS_DISABLED;
    }
    else
    {
      *dot1asStatus = DOT1AS_IEEE;
    }
    return;
  }
  else if (pDot1asBridge_g->adminMode == FALSE)
  {
    *dot1asStatus = DOT1AS_AVNU_DISABLED;
  }
  else
  {
    *dot1asStatus = DOT1AS_AVNU_AUTOMOTIVE;
  }

  if (pDot1asBridge_g->clkState == DOT1AS_CLOCK_UPDATE_SLAVE)
  {
    for (intIfNum = 0; intIfNum < dot1asMaxInterfaceCount_g; intIfNum++)
    {
      if (pDot1asIntf_g[intIfNum].selectedRole == DOT1AS_ROLE_SLAVE)
      {
        dot1asCfgxIntfAVNUDot1asCapableGet(avbCfgxIntfNum[intIfNum], &avnuASCapable);
        dot1asCfgxIntfPttPortEnabledGet(avbCfgxIntfNum[intIfNum], &dot1asCapable);
        break;
      }
    }
    if (intIfNum == dot1asMaxInterfaceCount_g)
    {
      *gmStatus = AVNU_GM_NO_SLAVE_PORT;
    }
    else if ((dot1asCapable == FALSE) || (avnuASCapable == FALSE))
    {
      *gmStatus = AVNU_GM_DISABLED;
    }
    else if (pDot1asBridge_g->avnuGMLost == TRUE)
    {
      if (pDot1asBridge_g->isSyncReceived == FALSE)
      {
        *gmStatus = AVNU_GM_STARTUP_ABSENT;
      }
      else
      {
        *gmStatus = AVNU_GM_OPERATIONAL_ABSENT;
      }
    }
    else if ((pDot1asBridge_g->isSyncReceived == FALSE) &&
             (sal_time_usecs() < pDot1asBridge_g->avnuSyncAbsenceTimeout))
    {
      *gmStatus = AVNU_GM_UNDETERMINED;
    }
    else
    {
      *gmStatus = AVNU_GM_PRESENT;
    }
  }

  *slavePortIndex = intIfNum;
}

/******************************************************************************
*
* @purpose Function to determine the reason for as capable false
*
* @param    extIfNum @b{(input)}
* @param    status @b{(output)}
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/

void avnuASCapableIntfStatusGet(uint32_t extIfNum, uint32_t *status)
{
  uint8_t asCapable;
  uint32_t intIfNum;
  uint32_t linkStatus;

  if (avb_intf_external_to_internal(extIfNum, &intIfNum) != AVB_SUCCESS)
  {
    return;
  }

  cbx_port_attribute_get(intIfNum, cbxPortAttrLinkStatus, &linkStatus);
  if (linkStatus == AVB_PORT_LINK_DOWN)
  {
    *status = AVNU_ASCAPABLE_STATUS_PORT_DOWN;
  }
  else if(pDot1asIntf_g[intIfNum].dot1asCapable == FALSE)
  {
    dot1asCfgxIntfAVNUDot1asCapableGet(extIfNum, &asCapable);
    if (asCapable == FALSE)
    {
      *status = AVNU_ASCAPABLE_STATUS_CONFIG_AVNU;
    }
    else
    {
      dot1asCfgxIntfPttPortEnabledGet(extIfNum, &asCapable);
      if (asCapable == FALSE)
      {
        *status = AVNU_ASCAPABLE_STATUS_CONFIG_DOT1AS;
      }
    }
  }
  else
  {
    *status = AVNU_ASCAPABLE_STATUS_TRUE;
  }
}

/******************************************************************************
*
* @purpose Function to get the AVnu device state
*
* @param    state @b{(output)}
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/

AVB_RC_t avnuDeviceStateGet(uint32_t *state)
{
  *state = pDot1asBridge_g->avnuDeviceState;
  return AVB_SUCCESS;
}

/******************************************************************************
*
* @purpose Function to set the AVnu device state
*
* @param    state @b{(input)}
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/

AVB_RC_t avnuDeviceStateSet(uint32_t state)
{
  pDot1asBridge_g->avnuDeviceState = state;
  return AVB_SUCCESS;
}

/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:    dot1as_api.h
 */

#ifndef DOT1AS_API_H
#define DOT1AS_API_H


/* Common typedefs */
#define SYNC_TX_LOWER_LIMIT -5
#define SYNC_TX_UPPER_LIMIT 5
#define BMCA_TX_LOWER_LIMIT -5
#define BMCA_TX_UPPER_LIMIT 5
#define PDELAY_TX_LOWER_LIMIT -5
#define PDELAY_TX_UPPER_LIMIT 5
#define PDELAY_TX_DISABLE_VAL 127
#define SYNC_RX_LOWER_LIMIT 2
#define SYNC_RX_UPPER_LIMIT 255
#define BMCA_RX_LOWER_LIMIT 2
#define BMCA_RX_UPPER_LIMIT 255
#define AVNU_OPER_PDELAY_LOWER_LIMIT 0
#define AVNU_OPER_PDELAY_UPPER_LIMIT 3

/* 802.1AS Admin modes */
typedef enum
{
  DOT1AS_DISABLE = 0,
  DOT1AS_ENABLE = 1
} DOT1AS_ADMIN_MODE_t;

/* 802.1AS Priority types */
typedef enum
{
  DOT1AS_CLOCK_PRIORITY1 = 0,
  DOT1AS_CLOCK_PRIORITY2
} DOT1AS_CLOCK_PRIORITY_t;

/* 802.1AS Port Roles - Match the values in Table 14-5 of the 802.1as Spec */
typedef enum
{
  DOT1AS_ROLE_DISABLED = 3,  /* Default value per the 802.1as Spec */
  DOT1AS_ROLE_MASTER   = 6,
  DOT1AS_ROLE_PASSIVE  = 7,
  DOT1AS_ROLE_SLAVE    = 9
} DOT1AS_PORT_ROLE_t;

#define AVNU_CLOCK_STATE_STR_GM     "gm"
#define AVNU_CLOCK_STATE_STR_SLAVE  "slave"
#define AVNU_CLOCK_STATE_STR_DEFAULT  AVNU_CLOCK_STATE_STR_SLAVE

#define DOT1AS_ROLE_STR_DISABLED    "disabled"
#define DOT1AS_ROLE_STR_MASTER      "master"
#define DOT1AS_ROLE_STR_SLAVE       "slave"
#define DOT1AS_ROLE_STR_PASSIVE     "passive"
#define DOT1AS_ROLE_STR_DEFAULT     DOT1AS_ROLE_STR_MASTER
#define AVNU_DOT1AS_ROLE_MASTER     DOT1AS_ROLE_MASTER

/* 802.1AS transmit and receive intervals */
typedef enum
{
  DOT1AS_INITIAL_SYNC_TX_INTERVAL = 0,
  DOT1AS_CURRENT_SYNC_TX_INTERVAL,
  DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL,
  DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL,
  DOT1AS_INITIAL_PDELAY_TX_INTERVAL,
  DOT1AS_CURRENT_PDELAY_TX_INTERVAL,
  DOT1AS_SYNC_RX_TIMEOUT,
  DOT1AS_ANNOUNCE_RX_TIMEOUT,
  DOT1AS_PDELAY_RX_TIMEOUT,
  DOT1AS_SYNC_RX_TIMEOUT_INTERVAL,
} DOT1AS_INTF_INTERVAL_t;

/* Interface counter types */
typedef enum
{
  DOT1AS_SYNC_TX_COUNT = 1,
  DOT1AS_SYNC_RX_COUNT,
  DOT1AS_FOLLOWUP_TX_COUNT,
  DOT1AS_FOLLOWUP_RX_COUNT,
  DOT1AS_ANNOUNCE_TX_COUNT,
  DOT1AS_ANNOUNCE_RX_COUNT,
  DOT1AS_PDELAY_REQ_TX_COUNT,
  DOT1AS_PDELAY_REQ_RX_COUNT,
  DOT1AS_PDELAY_RESP_TX_COUNT,
  DOT1AS_PDELAY_RESP_RX_COUNT,
  DOT1AS_PDELAY_RESP_FOLLOWUP_TX_COUNT,
  DOT1AS_PDELAY_RESP_FOLLOWUP_RX_COUNT,
  DOT1AS_SIGNALING_TX_COUNT,
  DOT1AS_SIGNALING_RX_COUNT,
  DOT1AS_SYNC_RX_TIMEOUTS,
  DOT1AS_SYNC_RX_DISCARDS,
  DOT1AS_ANNOUNCE_RX_TIMEOUTS,
  DOT1AS_ANNOUNCE_RX_DISCARDS,
  DOT1AS_PDELAY_RX_TIMEOUTS,
  DOT1AS_PDELAY_RX_DISCARDS,
  DOT1AS_BAD_HEADER_COUNT,
  DOT1AS_PTP_DISCARD_COUNT,
  DOT1AS_PDELAY_LOST_RESP_EXCEEDED,
  DOT1AS_FOLLOWUP_RX_DISCARDS,
  DOT1AS_FOLLOWUP_RX_TIMEOUTS,
  DOT1AS_PDELAY_REQ_RX_DISCARDS,
  DOT1AS_PDELAY_RESP_RX_DISCARDS,
  DOT1AS_BAD_PDELAY_VALUES,
  DOT1AS_SYNC_TX_TIMEOUTS,
  DOT1AS_SIGNALING_RX_DISCARDS,
  DOT1AS_SYNC_TOO_FAST,
  DOT1AS_TX_ERRORS,
  DOT1AS_TIME_STAMP_ERROR,
  DOT1AS_COUNTER_ALL

} DOT1AS_COUNTER_TYPE_t;


/******************************************************************************
* @purpose  Set the DOT1AS Admin Mode
*
* @param    mode @b{(input)} the new value of the DOT1AS mode
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asModeSet(DOT1AS_ADMIN_MODE_t mode);


/******************************************************************************
* @purpose  Get the DOT1AS Admin Mode
*
* @param    mode @b{(output)} the current value of the DOT1AS mode
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asModeGet(DOT1AS_ADMIN_MODE_t *mode);


/******************************************************************************
* @purpose  Set the DOT1AS Interface Mode
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    mode     @b{(input)} the new value of the DOT1AS mode
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfModeSet(uint32_t intfIndex, DOT1AS_ADMIN_MODE_t mode);


/******************************************************************************
* @purpose  Get the DOT1AS Interface Mode
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    mode @b{(output)} the current value of the DOT1AS mode
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfModeGet(uint32_t intIfNum, DOT1AS_ADMIN_MODE_t *mode);


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
AVB_RC_t dot1asIntfModeStatus(uint32_t port, DOT1AS_ADMIN_MODE_t *mode);


/******************************************************************************
* @purpose  Set the priority1/2 value of local clock in this 802.1AS system
*
* @param    type     @b{(input)} Priority1 or Priority2
* @param    priority @b{(input)} 0 - 255
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asBridgePrioritySet(DOT1AS_CLOCK_PRIORITY_t type,
                                        uint32_t priority);


/******************************************************************************
* @purpose  Get the priority1/2 value of local clock in this 802.1AS system
*
* @param    type     @b{(input)} Priority1 or Priority2
* @param    priority @b{(output)} 0 - 255
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
******************************************************************************/
extern AVB_RC_t dot1asBridgePriorityGet(DOT1AS_CLOCK_PRIORITY_t type,
                                        uint32_t *priority);


/******************************************************************************
* @purpose  Check if there is a Grand Master elected in the 802.1AS domain
*
* @param    isGmPresent @b{(output)} TRUE is GM is elected, else FALSE
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIsGrandmasterPresent(uint32_t *isGmPresent);


/******************************************************************************
* @purpose  Get the Best clock identity as detected by this time-aware system
*
* @param    id_upper @b{(output)} Clock identity (upper half)
* @param    id_lower @b{(output)} Clock identity (lower half)
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asBestClkIdGet(uint32_t *id_upper, uint32_t *id_lower);


/******************************************************************************
* @purpose  Get the priority1/2 value of Best clock in the 802.1AS domain
*
* @param    type     @b{(input)} Priority1 or Priority2
* @param    priority @b{(output)} 0 - 255
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asBestClkPriorityGet(DOT1AS_CLOCK_PRIORITY_t type,
                                         uint32_t *priority);


/******************************************************************************
* @purpose  Get the steps to Best clock as detected by this time-aware system
*
* @param    steps @b{(output)} Steps removed value for this time-aware system
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asBestClkStepsGet(uint32_t *steps);


/******************************************************************************
* @purpose  Get the clock identity of this bridge
*
* @param    id_upper @b{(output)} Clock identity (upper half)
* @param    id_lower @b{(output)} Clock identity (lower half)
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asBridgeClkIdGet(uint32_t *id_upper, uint32_t *id_lower);


/******************************************************************************
* @purpose  Check if the given interface 802.1AS capable (as determined by the
*           peer delay mechanism)
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    asCapable @b{(output)} TRUE if capable, else FALSE
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfIsCapable(uint32_t intIfNum, uint32_t *asCapable);


/******************************************************************************
* @purpose  Get the propagation delay on the given interface (as computed by the
*           peer delay mechanism)
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    delay @b{(output)} the current value of the propagation delay (ns)
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfPdelayGet(uint32_t intIfNum, uint32_t *delay);


/******************************************************************************
* @purpose  Get the neighbor rate ratio on the given interface (as computed by
*           the pdelay mechanism)
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    rateRatio @b{(output)} the current value of the rate ratio
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfNeighborRateRatioGet(uint32_t intIfNum, int32_t *rateRatio);


/******************************************************************************
* @purpose  Get the 'isMeasuringPdelay' property, which is set to TRUE if the
*           given interface is receiving PDELAY response messages from other
*           end of the link
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    isMeasuringPdelay @b{(output)} TRUE if other end if responding
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfIsMeasuringPdelay(uint32_t intIfNum,
                                            uint32_t *isMeasuringPdelay);

/******************************************************************************
* @purpose  Get the number of Pdelay_Req messages for which a valid response is
*           not received, above which a port is considered to not be exchanging
*            peer delay messages with its neighbor
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    numAllowedLostResp @b{(output)} Number of allowed lost responses
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
AVB_RC_t dot1asIntfAllowedLostRespStatus(uint32_t port, uint32_t *numAllowedLostResp);

/******************************************************************************
* @purpose  Set the number of Pdelay_Req messages for which a valid response is
*           not received, above which a port is considered to not be exchanging
*            peer delay messages with its neighbor
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    numAllowedLostResp @b{(input)} Number of allowed lost response
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfAllowedLostRespSet(uint32_t intIfNum,
                                  uint32_t numAllowedLostResp);


/******************************************************************************
* @purpose  Get the propagation delay threshold on the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    threshold @b{(output)} the current Pdelay threshold (nanosecs)
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments The threshold determines whether the port is capable of participating
*           in the 802.1AS protocol. The peer delay must be less than the
*           threshold value configured on the given interface.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfPdelayThresholdGet(uint32_t intIfNum, uint32_t *threshold);


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
AVB_RC_t dot1asIntfPdelayThresholdStatus(uint32_t port, uint32_t *threshold);

/******************************************************************************
* @purpose  Set the propagation delay threshold on the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    threshold @b{(input)} the Pdelay threshold (nanosecs)
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments The threshold determines whether the port is capable of participating
*           in the 802.1AS protocol. The peer delay must be less than the
*           threshold value configured on the given interface.
*
* @end
*******************************************************************************/
AVB_RC_t dot1asIntfPdelayThresholdSet(uint32_t intIfNum, uint32_t threshold);


/******************************************************************************
* @purpose  Get the 802.1AS role of the given interface
*
* @param    intIfNum @b{(input)} NIM Interface number
* @param    role @b{(output)} the current value of the interface's role
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfRoleGet(uint32_t intIfNum, DOT1AS_PORT_ROLE_t *role);


/******************************************************************************
* @purpose  Get the given interval on the given interface
*
* @param    type @b{(input)} DOT1AS interval type
* @param    intIfNum @b{(input)}  NIM Interface number
* @param    val @b{(output)} interval/timeout value
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @notes    Interval is specified in logarithm base 2 format. Timeout is
*           specified in number of transmission intervals. Timeout interval is
*           specified in 32-bit nanosecond value.
*
* @comments
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfIntervalGet(DOT1AS_INTF_INTERVAL_t type,
                                  uint32_t intIfNum,
                                  int32_t *val);


/******************************************************************************
* @purpose  Set the given interval on the given interface
*
* @param    type @b{(input)} DOT1AS interval type
* @param    intIfNum @b{(input)}  NIM Interface number
* @param    val @b{(input)} interval/timeout value
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @notes    Interval is specified in logarithm base 2 format. Timeout is
*           specified in number of transmission intervals.
*
* @comments
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfIntervalSet(DOT1AS_INTF_INTERVAL_t type,
                                  uint32_t intIfNum,
                                  int32_t val);




/******************************************************************************
* @purpose  Reset 802.1AS counters for the given interface
*
* @param    uint32  intIfNum  @b((input)) internal interface number
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asIntfStatsReset(uint32_t intIfNum);


/******************************************************************************
* @purpose  Get the parent clock identity and port number for this bridge
*
* @param    id @b{(output)} Clock identity of the parent clock
* @param    len @b{(output)} Length of parent clock identity
* @param    len @b{(output)} Port number of the parent clock
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asParentClkIdGet(uint8_t *id, uint32_t *len, uint16_t *port);


/******************************************************************************
* @purpose  Get the current estimate of the ratio of the frequency of the
*           grandmaster to the frequency of the local clock.
*
* @param    cumulativeRateRatio @b{(output)} ratio is expressed as the fractional
*           frequency offset multiplied by 2^41.
*           i.e., the quantity (rateRatio - 1.0)(2^41)
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
extern AVB_RC_t dot1asCumulativeRateRatioGet(int32_t *cumulativeRateRatio);


/******************************************************************************
* @purpose  Get the number of times the grandmaster has changed.
*
* @param    count - current value of gmChangeCount
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
AVB_RC_t dot1asGmChangeCountGet(uint32_t *count);

/******************************************************************************
* @purpose  Check if the interface is valid or not.
*
* @param    intIfNum - Interface number
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
uint8_t dot1asIsValidIntf(uint32_t intIfNum);

#ifdef DOT1AS_LAG_SUPPORT
/*@api
 * avb_dot1as_lag_callback
 *
 * @param=op            - Indicator of the LAG operation that just occurred
 * @param=lag           - The external interface number for the lag
 * @param=member_bitmap - The internal bitmap for the lag member ports
 *
 * @brief
 * A lag has been added or removed,  make the required dot1as changes
 *
 *
 * @returns nothing
 */
void avb_lag_callback_dot1as(AVB_LAG_OP_t op, uint16_t lag, uint8_t member_bitmap);
#endif

/******************************************************************************
* @purpose  Set the AVNU Mode
*
* @param    mode @b{(input)} the new value of the AVNU mode
*
* @returns  SUCCESS
* @returns  FAILURE
*
* @comments
*
*
* @end
*******************************************************************************/
AVB_RC_t avnuModeSet(uint32_t avnu_mode );

/******************************************************************************
* @purpose  Get clock state
*
* @param    clockState @b{(fill)} the new value clock state
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuClockStateGet(uint32_t *clockState);

/******************************************************************************
* @purpose  Set clock state
*
* @param    clockState @b{(input)} the new value clock state
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuClockStateSet(uint32_t clockState);
/******************************************************************************
* @purpose  Set Sync Absence Timeout
*
* @param    avnuSyncAbsenceTimeout @b{(input)} the new value Sync Absence Timeout
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuSyncAbsenceTimeoutSet(uint32_t avnuSyncAbsenceTimeout);
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

AVB_RC_t avnuPortRoleSet(uint32_t port, uint32_t port_role );
/******************************************************************************
* @purpose  Set DOT1AS capable
*
* @param    intfIndex @b{(input)}
* @param    asCapable @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuAsCapableSet(uint32_t port, uint32_t asCapable);
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

AVB_RC_t avnuPDelaySet(uint32_t port, uint32_t pDelay);

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

AVB_RC_t avnuInitialLogPdelayReqIntervalSet(uint32_t port, int32_t initialLogPdelayReqInterval);
/******************************************************************************
* @purpose  Set Operational Log PDelay Request Interval
*
* @param    intfIndex @b{(input)}
* @param    operationalLogPdelayReqInterval @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuOperationalLogPdelayReqIntervalSet(uint32_t port, int32_t operationalLogPdelayReqInterval);

/******************************************************************************
* @purpose  Set Initial Log Sync Request Interval
*
* @param    intfIndex @b{(input)}
* @param    initialLogSyncReqInterval @b{(input)}
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

AVB_RC_t avnuInitialLogSyncIntervalSet(uint32_t port, int32_t initialLogSyncInterval);

/******************************************************************************
* @purpose  Display AVnu related status
*
* @param   None
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/

void avnuStatusDisplay();

/******************************************************************************
*
* @purpose Function to determine avnu global status
*
* @param    dot1asStatus @b{(input)}
* @param    gmStatus @b{(input)}
* @param    slavePort @b{(input)}
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void avnuStatusGet(uint32_t *dot1asStatus, uint32_t *gmStatus, uint32_t *slavePort);

/******************************************************************************
*
* @purpose Function to determine reason for avnu ascapable false
*
* @param    intIfNum @b{(input)}
* @param    asFalseReason @b{(output)}
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
void avnuASCapableIntfStatusGet(uint32_t intIfNum, uint32_t *status);

/******************************************************************************
*
* @purpose Function to get the AVnu device state
*
* @param   *state @b{(output)}
*
* @returns AVB_SUCCESS/AVB_FAILURE
*
* @notes
*
* @end
*
*******************************************************************************/

AVB_RC_t avnuDeviceStateGet(uint32_t *state);

/******************************************************************************
*
* @purpose Function to set the AVnu device state
*
* @param   *state @b{(input)}
*
* @returns AVB_SUCCESS/AVB_FAILURE
*
* @notes
*
* @end
*
*******************************************************************************/

AVB_RC_t avnuDeviceStateSet(uint32_t state);
#endif /* DOT1AS_API_H */

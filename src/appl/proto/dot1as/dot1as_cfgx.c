/*
 * $Id$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:    dot1as_cfgx.c
 */

#include "dot1as.h"
#include "avb.h"
#include "dot1as_cfgx.h"
#include "sal_get_set_param.h"

dot1as_cfgx_t dot1as_cfgx_data = { .intfCfg = NULL };

uint8_t dot1asCfgxPortRoles[CFGX_DOT1AS_PORT_ROLE_LEN] = { DOT1AS_ROLE_DISABLED, DOT1AS_ROLE_MASTER, DOT1AS_ROLE_SLAVE, DOT1AS_ROLE_PASSIVE };
uint8_t avnuCfgxClockState[CFGX_AVNU_CLOCK_STATE_LEN] = { DOT1AS_AVNU_CLOCK_STATE_GM, DOT1AS_AVNU_CLOCK_STATE_SLAVE };

/******************************************************************************
* @purpose  Initialize the 802.1AS config structure
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxInit(void)
{
  uint8_t         flags;
  uint8_t         intfIndex;
  int             unit; 
  cbx_port_t      port;
  cbxi_pgid_t     lpgid;

  if (dot1as_cfgx_data.intfCfg == NULL)
  {
    dot1as_cfgx_data.intfCfg = (dot1as_intf_cfgx_t*)sal_alloc((sizeof(dot1as_intf_cfgx_t) * avbMaxIntfaces_g), "DOT1AS");
    if (dot1as_cfgx_data.intfCfg == NULL)
    {
      sal_printf("Failed to allocate memory for dot1as_cfgx_data.intfCfg component\n");
      return AVB_FAILURE;
    }

    sal_memset(dot1as_cfgx_data.intfCfg, 0x00, (sizeof(dot1as_intf_cfgx_t) * avbMaxIntfaces_g));
  }

  cfgxBoolInit(&dot1as_cfgx_data.globalAdmin, DOT1AS_GLOBAL_ADMIN_KEY, DOT1AS_GLOBAL_ADMIN_DEFAULT, DOT1AS_GLOBAL_ADMIN_DEFAULT);

  flags = 0;
  cfgxUint8Init(&dot1as_cfgx_data.priority1, DOT1AS_PRIORITY1_KEY, DOT1AS_PRIORITY1_DEFAULT, DOT1AS_PRIORITY1_MIN,
                DOT1AS_PRIORITY1_MAX, DOT1AS_PRIORITY1_DEFAULT, flags);

  flags = 0;
  cfgxUint8Init(&dot1as_cfgx_data.priority2, DOT1AS_PRIORITY2_KEY, DOT1AS_PRIORITY2_DEFAULT, DOT1AS_PRIORITY2_MIN,
                DOT1AS_PRIORITY2_MAX, DOT1AS_PRIORITY2_DEFAULT, flags);

  cfgxBoolInit(&dot1as_cfgx_data.avnuMode, DOT1AS_AVNU_MODE_KEY, DOT1AS_AVNU_MODE_DEFAULT, DOT1AS_AVNU_MODE_DEFAULT);

  flags = 0;
  cfgxEnum8Len16Init(&dot1as_cfgx_data.avnuClockState, DOT1AS_AVNU_CLOCK_STATE_KEY, DOT1AS_AVNU_CLOCK_STATE_DEFAULT, avnuCfgxClockState,
                     DOT1AS_AVNU_CLOCK_STATE_ENUM_LEN, DOT1AS_AVNU_CLOCK_STATE_DEFAULT, flags);
  flags = 0;
  cfgxUint8Init(&dot1as_cfgx_data.avnuSyncAbsenceTimeout, DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_KEY, DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_DEFAULT,
                DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_MIN, DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_MAX, DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_DEFAULT, flags);

  for (intfIndex=0; intfIndex < avbMaxIntfaces_g; intfIndex++)
  {
    /* Initialize only on configured/valid ports */
    if((cbxi_robo2_port_validate(intfIndex, &port, &lpgid, &unit)) != 0) {
      continue;
    }

    dot1as_cfgx_data.intfCfg[intfIndex].intf = avbCfgxIntfNum[intfIndex];
    cfgxBoolInit(&dot1as_cfgx_data.intfCfg[intfIndex].pttPortEnabled, DOT1AS_INTF_MODE_KEY, DOT1AS_INTF_MODE_DEFAULT, DOT1AS_INTF_MODE_DEFAULT);

    flags = 0;
    cfgxInt8Init(&dot1as_cfgx_data.intfCfg[intfIndex].initialLogNbrPDelayInterval, DOT1AS_INTF_INIT_NBR_PDELAY_KEY, DOT1AS_INTF_INIT_NBR_PDELAY_DEFAULT,
                 DOT1AS_INTF_INIT_NBR_PDELAY_MIN, DOT1AS_INTF_INIT_NBR_PDELAY_MAX, DOT1AS_INTF_INIT_NBR_PDELAY_DEFAULT, flags);
    flags = 0;
    cfgxInt8Init(&dot1as_cfgx_data.intfCfg[intfIndex].avnuOperationalLogPdelayReqInterval, DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_KEY,
                 DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_DEFAULT, DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_MIN,
                 DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_MAX, DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_DEFAULT, flags);

    flags = 0;
    cfgxInt8Init(&dot1as_cfgx_data.intfCfg[intfIndex].initialLogSyncInterval, DOT1AS_INTF_INIT_SYNC_KEY, DOT1AS_INTF_INIT_SYNC_DEFAULT,
                 DOT1AS_INTF_INIT_SYNC_MIN, DOT1AS_INTF_INIT_SYNC_MAX, DOT1AS_INTF_INIT_SYNC_DEFAULT, flags);

    flags = 0;
    cfgxInt8Init(&dot1as_cfgx_data.intfCfg[intfIndex].initialLogAnnounceInterval, DOT1AS_INTF_INIT_ANN_KEY, DOT1AS_INTF_INIT_ANN_DEFAULT,
                 DOT1AS_INTF_INIT_ANN_MIN, DOT1AS_INTF_INIT_ANN_MAX, DOT1AS_INTF_INIT_ANN_DEFAULT, flags);

    flags = 0;
    cfgxUint8Init(&dot1as_cfgx_data.intfCfg[intfIndex].announceReceiptTimeout, DOT1AS_INTF_ANN_TIMEOUT_KEY, DOT1AS_INTF_ANN_TIMEOUT_DEFAULT,
                  DOT1AS_INTF_ANN_TIMEOUT_MIN, DOT1AS_INTF_ANN_TIMEOUT_MAX, DOT1AS_INTF_ANN_TIMEOUT_DEFAULT, flags);

    flags = 0;
    cfgxUint8Init(&dot1as_cfgx_data.intfCfg[intfIndex].syncReceiptTimeout, DOT1AS_INTF_SYNC_TIMEOUT_KEY, DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT,
                  DOT1AS_INTF_SYNC_TIMEOUT_MIN, DOT1AS_INTF_SYNC_TIMEOUT_MAX, DOT1AS_INTF_SYNC_TIMEOUT_DEFAULT, flags);

    flags = 0;
    cfgxUint32Init(&dot1as_cfgx_data.intfCfg[intfIndex].neighborPropDelayThresh, DOT1AS_INTF_PDELAY_THRESH_KEY, DOT1AS_INTF_PDELAY_THRESH_DEFAULT,
                   DOT1AS_INTF_PDELAY_THRESH_MIN, DOT1AS_INTF_PDELAY_THRESH_MAX, DOT1AS_INTF_PDELAY_THRESH_DEFAULT, flags);

    flags = 0;
    cfgxUint32Init(&dot1as_cfgx_data.intfCfg[intfIndex].allowedLostResponses, DOT1AS_INTF_LOST_RESP_KEY, DOT1AS_INTF_LOST_RESP_DEFAULT,
                   DOT1AS_INTF_LOST_RESP_MIN, DOT1AS_INTF_LOST_RESP_MAX, DOT1AS_INTF_LOST_RESP_DEFAULT, flags);

    flags = 0;
    cfgxEnum8Len16Init(&dot1as_cfgx_data.intfCfg[intfIndex].avnuPortRole, DOT1AS_INTF_AVNU_ROLE_KEY, DOT1AS_INTF_AVNU_ROLE_DEFAULT,
                       dot1asCfgxPortRoles, DOT1AS_INTF_AVNU_ROLE_ENUM_LEN, DOT1AS_INTF_AVNU_ROLE_DEFAULT, flags);

    cfgxBoolInit(&dot1as_cfgx_data.intfCfg[intfIndex].avnuDot1asCapable, DOT1AS_INTF_AVNU_AS_CAP_KEY,
                 DOT1AS_INTF_AVNU_AS_CAP_DEFAULT, DOT1AS_INTF_AVNU_AS_CAP_DEFAULT);

    flags = 0;
    cfgxUint32Init(&dot1as_cfgx_data.intfCfg[intfIndex].avnuNeighborPropDelay, DOT1AS_INTF_AVNU_NBR_PDELAY_KEY, DOT1AS_INTF_AVNU_NBR_PDELAY_DEFAULT,
                   DOT1AS_INTF_AVNU_NBR_PDELAY_MIN, DOT1AS_INTF_AVNU_NBR_PDELAY_MAX, DOT1AS_INTF_AVNU_NBR_PDELAY_DEFAULT, flags);

    flags = 0;
    cfgxInt8Init(&dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogPdelayReqInterval, DOT1AS_INTF_AVNU_INIT_PDELAY_KEY, DOT1AS_INTF_AVNU_INIT_PDELAY_DEFAULT,
                 DOT1AS_INTF_AVNU_INIT_PDELAY_MIN, DOT1AS_INTF_AVNU_INIT_PDELAY_MAX, DOT1AS_INTF_AVNU_INIT_PDELAY_DEFAULT, flags);

    flags = 0;
    cfgxInt8Init(&dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogSyncInterval, DOT1AS_INTF_AVNU_INIT_SYNC_KEY, DOT1AS_INTF_AVNU_INIT_SYNC_DEFAULT,
                 DOT1AS_INTF_AVNU_INIT_SYNC_MIN, DOT1AS_INTF_AVNU_INIT_SYNC_MAX, DOT1AS_INTF_AVNU_INIT_SYNC_DEFAULT, flags);
  }

  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Show the current 802.1as configuration
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*******************************************************************************/
void dot1asCfgxShow()
{
  uint32_t index;

  sal_printf("\nDOT1AS Global Configuration:\n");
  sal_printf("\tAdmin mode : %s\n", dot1as_cfgx_data.globalAdmin.val == AVB_ENABLE ? "Enabled" : "Disabled");
  sal_printf("\tPriority1  : %d\n", dot1as_cfgx_data.priority1.val);
  sal_printf("\tPriority2  : %d\n", dot1as_cfgx_data.priority2.val);

  sal_printf("\nDOT1AS Interface Configuration:\n");

  CFGX_PRINT_LINE(77);
  sal_printf("%-4s\t%-9s%-8s%-8s%-9s%-8s%-9s%-10s%-10s\n",
               "Intf", "Mode", "SyncTx", "BmcaTx", "PdelayTx",
               "SyncRx", "BmcaRx", "Pthresh", "LostResp");
  CFGX_PRINT_LINE(77);

  for (index=0; index < avbMaxIntfaces_g; index++)
  {
    sal_printf("%u\t", dot1as_cfgx_data.intfCfg[index].intf);
    sal_printf("%-9s", dot1as_cfgx_data.intfCfg[index].pttPortEnabled.val == AVB_ENABLE ? "Enabled" : "Disabled");
    sal_printf("%-8d", dot1as_cfgx_data.intfCfg[index].initialLogSyncInterval.val);
    sal_printf("%-8d", dot1as_cfgx_data.intfCfg[index].initialLogAnnounceInterval.val);
    sal_printf("%-9d", dot1as_cfgx_data.intfCfg[index].initialLogNbrPDelayInterval.val);
    sal_printf("%-8d", dot1as_cfgx_data.intfCfg[index].syncReceiptTimeout.val);
    sal_printf("%-9d", dot1as_cfgx_data.intfCfg[index].announceReceiptTimeout.val);
    sal_printf("%-10d", dot1as_cfgx_data.intfCfg[index].neighborPropDelayThresh.val);
    sal_printf("%-10d\n", dot1as_cfgx_data.intfCfg[index].allowedLostResponses.val);
  }
  CFGX_PRINT_LINE(77);
}

/******************************************************************************
* @purpose  Show the current 802.1as AVNU configuration
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*******************************************************************************/
void avnuCfgxShow(void)
{
  uint32_t index = 0;

  sal_printf("\nAVNU Global Configuration:\n");
  sal_printf("AVnu Mode               : %s\n", dot1as_cfgx_data.avnuMode.val == AVB_TRUE ? "True" : "False");
  sal_printf("Clock State             : %s\n", dot1as_cfgx_data.avnuClockState.val == DOT1AS_CLOCK_UPDATE_GM ? "Grand Master" : "Slave");
  sal_printf("Sync Absence Timeout    : %d seconds\n", dot1as_cfgx_data.avnuSyncAbsenceTimeout.val);

  sal_printf("\nAVNU Interface Configuration:\n");
  CFGX_PRINT_LINE(100);
  sal_printf("%-8s%-10s%-12s%-12s%-18s%-18s%-18s\n", "Index", "Role", "asCapable", "PDelay", "PDelayLogInterval", "SyncLogInterval", "OperPDelayInterval");
  CFGX_PRINT_LINE(100);
  for (index=0; index < avbMaxIntfaces_g; index++)
  {
    sal_printf("%-8d%-10s%-12s%-12llu%-18d%-18d%-18d\n",
               dot1as_cfgx_data.intfCfg[index].intf,
               dot1asDebugRoleStringGet(dot1as_cfgx_data.intfCfg[index].avnuPortRole.val),
               dot1as_cfgx_data.intfCfg[index].avnuDot1asCapable.val == AVB_TRUE ? "True" : "False",
               (uint64_t)dot1as_cfgx_data.intfCfg[index].avnuNeighborPropDelay.val,
               dot1as_cfgx_data.intfCfg[index].avnuInitialLogPdelayReqInterval.val,
               dot1as_cfgx_data.intfCfg[index].avnuInitialLogSyncInterval.val,
               dot1as_cfgx_data.intfCfg[index].avnuOperationalLogPdelayReqInterval.val);
  }
  CFGX_PRINT_LINE(100);
}

#if 0  /* cfgxDump functionis not supported */
/******************************************************************************
* @purpose  Dump the 802.1AS configuration data structure
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*******************************************************************************/
void dot1asCfgxDump(void)
{
  uint8_t i;

  cfgxDump(CFGX_TYPE_HEADER, (void *) &dot1as_cfgx_data.hdr, CFG_HEADER_KEY_STR, 0);

  cfgxDump(CFGX_TYPE_BOOL,          (void *) &dot1as_cfgx_data.globalAdmin, DOT1AS_GLOBAL_ADMIN_KEY_STR, DOT1AS_GLOBAL_ADMIN_KEY);
  cfgxDump(CFGX_TYPE_UINT8,         (void *) &dot1as_cfgx_data.priority1, DOT1AS_PRIORITY1_KEY_STR, DOT1AS_PRIORITY1_KEY);
  cfgxDump(CFGX_TYPE_UINT8,         (void *) &dot1as_cfgx_data.priority2, DOT1AS_PRIORITY2_KEY_STR, DOT1AS_PRIORITY2_KEY);
  cfgxDump(CFGX_TYPE_BOOL,          (void *) &dot1as_cfgx_data.avnuMode, DOT1AS_AVNU_MODE_KEY_STR, DOT1AS_AVNU_MODE_KEY);
  cfgxDump(CFGX_TYPE_ENUM8LEN16,    (void *) &dot1as_cfgx_data.avnuClockState, DOT1AS_AVNU_CLOCK_STATE_KEY_STR, DOT1AS_AVNU_CLOCK_STATE_KEY);
  cfgxDump(CFGX_TYPE_UINT8,         (void *) &dot1as_cfgx_data.avnuSyncAbsenceTimeout, DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_KEY_STR, DOT1AS_AVNU_SYNC_ABSENCE_TIMEOUT_KEY);

  for (i=0; i < avbMaxIntfaces_g; i++)
  {
    /* TRGTRG TODO - Should I have a type dedicated to AVB Interfaces?? */
    sal_printf("\n\nConfiguration for interface index=%d and interface number %d :", i, dot1as_cfgx_data.intfCfg[i].intf);
    cfgxDump(CFGX_TYPE_BOOL,       (void *) &dot1as_cfgx_data.intfCfg[i].pttPortEnabled, DOT1AS_INTF_MODE_KEY_STR, DOT1AS_INTF_MODE_KEY);
    cfgxDump(CFGX_TYPE_INT8,       (void *) &dot1as_cfgx_data.intfCfg[i].initialLogNbrPDelayInterval, DOT1AS_INTF_INIT_NBR_PDELAY_KEY_STR, DOT1AS_INTF_INIT_NBR_PDELAY_KEY);
    cfgxDump(CFGX_TYPE_INT8,       (void *) &dot1as_cfgx_data.intfCfg[i].initialLogSyncInterval, DOT1AS_INTF_INIT_SYNC_KEY_STR, DOT1AS_INTF_INIT_SYNC_KEY);
    cfgxDump(CFGX_TYPE_UINT8,      (void *) &dot1as_cfgx_data.intfCfg[i].announceReceiptTimeout, DOT1AS_INTF_ANN_TIMEOUT_KEY_STR, DOT1AS_INTF_ANN_TIMEOUT_KEY);
    cfgxDump(CFGX_TYPE_UINT8,      (void *) &dot1as_cfgx_data.intfCfg[i].syncReceiptTimeout, DOT1AS_INTF_SYNC_TIMEOUT_KEY_STR, DOT1AS_INTF_SYNC_TIMEOUT_KEY);
    cfgxDump(CFGX_TYPE_UINT32,     (void *) &dot1as_cfgx_data.intfCfg[i].neighborPropDelayThresh, DOT1AS_INTF_PDELAY_THRESH_KEY_STR, DOT1AS_INTF_PDELAY_THRESH_KEY);
    cfgxDump(CFGX_TYPE_UINT32,     (void *) &dot1as_cfgx_data.intfCfg[i].allowedLostResponses, DOT1AS_INTF_LOST_RESP_KEY_STR, DOT1AS_INTF_LOST_RESP_KEY);
    cfgxDump(CFGX_TYPE_ENUM8LEN16, (void *) &dot1as_cfgx_data.intfCfg[i].avnuPortRole, DOT1AS_INTF_AVNU_ROLE_KEY_STR, DOT1AS_INTF_AVNU_ROLE_KEY);
    cfgxDump(CFGX_TYPE_BOOL,       (void *) &dot1as_cfgx_data.intfCfg[i].avnuDot1asCapable, DOT1AS_INTF_AVNU_AS_CAP_KEY_STR, DOT1AS_INTF_AVNU_AS_CAP_KEY);
    cfgxDump(CFGX_TYPE_UINT32,     (void *) &dot1as_cfgx_data.intfCfg[i].avnuNeighborPropDelay, DOT1AS_INTF_AVNU_NBR_PDELAY_KEY_STR, DOT1AS_INTF_AVNU_NBR_PDELAY_KEY);
    cfgxDump(CFGX_TYPE_INT8,       (void *) &dot1as_cfgx_data.intfCfg[i].avnuInitialLogPdelayReqInterval, DOT1AS_INTF_AVNU_INIT_PDELAY_KEY_STR, DOT1AS_INTF_AVNU_INIT_PDELAY_KEY);
    cfgxDump(CFGX_TYPE_INT8,       (void *) &dot1as_cfgx_data.intfCfg[i].avnuInitialLogSyncInterval, DOT1AS_INTF_AVNU_INIT_SYNC_KEY_STR, DOT1AS_INTF_AVNU_INIT_SYNC_KEY);
    cfgxDump(CFGX_TYPE_INT8,       (void *) &dot1as_cfgx_data.intfCfg[i].avnuOperationalLogPdelayReqInterval, DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_KEY_STR, DOT1AS_INTF_AVNU_OPER_PDELAY_INTL_KEY);

    sal_printf("\n");
  }
}
#endif


/******************************************************************************
* @purpose  Get the configured Global Admin Mode
*
* @param    val  @b{(output)} The Global Admin Mode AVB_ENABLE or AVB_DISABLE
*
* @returns  AVB_SUCCESS
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxGlobalAdminGet(uint8_t *val)
{
  *val = dot1as_cfgx_data.globalAdmin.val;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Set the configuration for Global Admin Mode
*
* @param    val  @b{(input)} The Global Admin Mode AVB_ENABLE or AVB_DISABLE
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxGlobalAdminSet(uint8_t val)
{
  return cfgxBoolSet(&dot1as_cfgx_data.globalAdmin, val);
}

/******************************************************************************
* @purpose  Get the configured priority1 value of local clock in this system
*
* @param    val  @b{(output)} The priority1 value
*
* @returns  AVB_SUCCESS
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxPriority1Get(uint8_t *val)
{
  *val = dot1as_cfgx_data.priority1.val;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Set the configuration for priority1 value of local clock in this system
*
* @param    val  @b{(input)} priority1 value
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxPriority1Set(uint8_t val)
{
  return cfgxUint8Set(&dot1as_cfgx_data.priority1, val);
}

/******************************************************************************
* @purpose  Get the configured priority2 value of local clock in this system
*
* @param    val  @b{(output)} The priority2 value
*
* @returns  AVB_SUCCESS
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxPriority2Get(uint8_t *val)
{
  *val = dot1as_cfgx_data.priority2.val;
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Set the configuration for priority2 value of local clock in this system
*
* @param    val  @b{(input)} priority2 value
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxPriority2Set(uint8_t val)
{
  return cfgxUint8Set(&dot1as_cfgx_data.priority2, val);
}

/******************************************************************************
* @purpose  Get the configured AVNU Mode
*
* @param    val  @b{(output)} AVNU Mode
*
* @returns  AVB_SUCCESS
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxAVNUModeGet(uint8_t *val)
{
  *val = dot1as_cfgx_data.avnuMode.val;
  return AVB_SUCCESS;
}


/******************************************************************************
* @purpose  Set the configuration for AVNU Mode
*
* @param    val  @b{(input)} AVNU Mode
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxAVNUModeSet(uint8_t val)
{
  return cfgxBoolSet(&dot1as_cfgx_data.avnuMode, val);
}

/******************************************************************************
* @purpose  Get the configured AVNU Clock State
*
* @param  clockState @b{(input)} AVNU Clock State
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxAVNUClockStateGet(uint8_t *clockState)
{
  *clockState = dot1as_cfgx_data.avnuClockState.val;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Set the configuration for AVNU Clock State
*
* @param    clockState  @b{(input)} AVNU Clock State
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxAVNUClockStateSet(uint8_t clockState)
{
  AVB_RC_t rc = AVB_FAILURE;

  rc = cfgxEnum8Len16Set(&dot1as_cfgx_data.avnuClockState, clockState);
  return rc;
}

/******************************************************************************
* @purpose  Get the configured AVNU Sync Absence Timeout
*
* @param  syncAbsenceTimeout @b{(input)} AVNU Sync Absence Timeout
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxAVNUSyncAbsenceTimeoutGet(uint8_t *syncAbsenceTimeout)
{
  *syncAbsenceTimeout = dot1as_cfgx_data.avnuSyncAbsenceTimeout.val;
  return AVB_SUCCESS;
}

/******************************************************************************
* @purpose  Configure AVNU Sync Absence Timeout
*
* @param    syncAbsenceTimeout  @b{(input)} AVNU Sync Absence Timeout
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxAVNUSyncAbsenceTimeoutSet(uint8_t syncAbsenceTimeout)
{
  AVB_RC_t rc = AVB_FAILURE;

  rc = cfgxUint8Set(&dot1as_cfgx_data.avnuSyncAbsenceTimeout, syncAbsenceTimeout);
  return rc;
}

/******************************************************************************
* @purpose  Get the configured 802.1as Admin Mode for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} 802.1as Admin Mode
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfPttPortEnabledGet(uint16_t intfNum, uint8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].pttPortEnabled.val;
    rc = AVB_SUCCESS;
  }

  return rc;
}

/******************************************************************************
* @purpose  Set the 802.1as Admin Mode configuration for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} 802.1as Admin Mode
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfPttPortEnabledSet(uint16_t intfNum, uint8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxBoolSet(&dot1as_cfgx_data.intfCfg[intfIndex].pttPortEnabled, val);
  }

  return rc;
}

/******************************************************************************
* @purpose  Get the configured Initial Log Neighbor PDelay Interval for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Neighbor Prop Delay
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfInitialLogNbrPDelayIntervalGet(uint16_t intfNum, int8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    if (pDot1asBridge_g->avnuMode == AVB_ENABLE)
    {
      *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogPdelayReqInterval.val;
    }
    else
    {
      *val = dot1as_cfgx_data.intfCfg[intfIndex].initialLogNbrPDelayInterval.val;
    }
    rc = AVB_SUCCESS;
  }

  return rc;
}

/******************************************************************************
* @purpose  Set the Initial Log Neighbor PDelay Interval configuration for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Neighbor Prop Delay
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfInitialLogNbrPDelayIntervalSet(uint16_t intfNum, int8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxInt8Set(&dot1as_cfgx_data.intfCfg[intfIndex].initialLogNbrPDelayInterval, val);
  }

  return rc;
}

/******************************************************************************
* @purpose  Get the configured Initial Log Sync Interval for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Initial Log Sync Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfInitialLogSyncIntervalGet(uint16_t intfNum, int8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    if (pDot1asBridge_g->avnuMode == AVB_ENABLE)
    {
      *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogSyncInterval.val;
    }
    else
    {
      *val = dot1as_cfgx_data.intfCfg[intfIndex].initialLogSyncInterval.val;
    }
    rc = AVB_SUCCESS;
  }

  return rc;
}

/******************************************************************************
* @purpose  Set the Initial Log Sync Interval configuration for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Initial Log Sync Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfInitialLogSyncIntervalSet(uint16_t intfNum, int8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxInt8Set(&dot1as_cfgx_data.intfCfg[intfIndex].initialLogSyncInterval, val);
  }

  return rc;
}

/******************************************************************************
* @purpose  Get the configured Initial Log Announce Interval for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Initial Log Announce Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfInitialLogAnnounceIntervalGet(uint16_t intfNum, int8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].initialLogAnnounceInterval.val;
    rc = AVB_SUCCESS;
  }

  return rc;
}

/******************************************************************************
* @purpose  Set the Initial Log Announce Interval configuration for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Initial Log Announce Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfInitialLogAnnounceIntervalSet(uint16_t intfNum, int8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxInt8Set(&dot1as_cfgx_data.intfCfg[intfIndex].initialLogAnnounceInterval, val);
  }

  return rc;
}

/******************************************************************************
* @purpose  Get the configured Announce Receipt Timeout for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Announce Receipt Timeout
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAnnounceReceiptTimeoutGet(uint16_t intfNum, uint8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].announceReceiptTimeout.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the Announce Receipt Timeout configuration for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Announce Receipt Timeout
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAnnounceReceiptTimeoutSet(uint16_t intfNum, uint8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxUint8Set(&dot1as_cfgx_data.intfCfg[intfIndex].announceReceiptTimeout, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured Sync Receipt Timeout for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Sync Receipt Timeout
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfSyncReceiptTimeoutGet(uint16_t intfNum, uint8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].syncReceiptTimeout.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the Sync Receipt Timeout configuration for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Sync Receipt Timeout
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfSyncReceiptTimeoutSet(uint16_t intfNum, uint8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxUint8Set(&dot1as_cfgx_data.intfCfg[intfIndex].syncReceiptTimeout, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured Neighbor Prop Delay Threshold for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Neighbor Prop Delay Threshold
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfNeighborPropDelayThreshGet(uint16_t intfNum, uint32_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].neighborPropDelayThresh.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the Neighbor Prop Delay Threshold for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Neighbor Prop Delay Threshold
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfNeighborPropDelayThreshSet(uint16_t intfNum, uint32_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxUint32Set(&dot1as_cfgx_data.intfCfg[intfIndex].neighborPropDelayThresh, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured Allowed Lost Responses for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} Allowed Lost Responses
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAllowedLostResponsesGet(uint16_t intfNum, uint32_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].allowedLostResponses.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the Neighbor Allowed Lost Responses for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} Allowed Lost Responses
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAllowedLostResponsesSet(uint16_t intfNum, uint32_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxUint32Set(&dot1as_cfgx_data.intfCfg[intfIndex].allowedLostResponses, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured AVNU Port Role for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} AVNU Port Role
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUPortRoleGet(uint16_t intfNum, uint8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuPortRole.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the AVNU Port Role for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} AVNU Port Role
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUPortRoleSet(uint16_t intfNum, uint8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxEnum8Len16Set(&dot1as_cfgx_data.intfCfg[intfIndex].avnuPortRole, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured AVNU Dot1as Capable Mode for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} AVNU Dot1as Capable Mode
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUDot1asCapableGet(uint16_t intfNum, uint8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuDot1asCapable.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the AVNU Dot1as Capable Mode for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} AVNU Dot1as Capable Mode
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUDot1asCapableSet(uint16_t intfNum, uint8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxBoolSet(&dot1as_cfgx_data.intfCfg[intfIndex].avnuDot1asCapable, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured AVNU Neighbor Prop Delay for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} AVNU Neighbor Prop Delay
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUNeighborPropDelayGet(uint16_t intfNum, uint32_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuNeighborPropDelay.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the AVNU Neighbor Prop Delay for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} AVNU Neighbor Prop Delay
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUNeighborPropDelaySet(uint16_t intfNum, uint32_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxUint32Set(&dot1as_cfgx_data.intfCfg[intfIndex].avnuNeighborPropDelay, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured AVNU Initial Log Pdelay Req Interval for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} AVNU Initial Log Pdelay Req Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalGet(uint16_t intfNum, int8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogPdelayReqInterval.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Get the configured AVNU Opearational Log Pdelay Req Interval for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} AVNU Opearation Log Pdelay Req Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalGet(uint16_t intfNum, int8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuOperationalLogPdelayReqInterval.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the AVNU Initial Log Pdelay Req Interval for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} AVNU Initial Log Pdelay Req Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUInitialLogPdelayReqIntervalSet(uint16_t intfNum, int8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxInt8Set(&dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogPdelayReqInterval, val);
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the AVNU Operational Log Pdelay Req Interval for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} AVNU Operational Log Pdelay Req Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUOperationalLogPdelayReqIntervalSet(uint16_t intfNum, int8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxInt8Set(&dot1as_cfgx_data.intfCfg[intfIndex].avnuOperationalLogPdelayReqInterval, val);
  }
  return rc;
}
/******************************************************************************
* @purpose  Get the configured AVNU Initial Log Sync Interval for the port
*
* @param    intfNum  @b{(input)}  The external interface number
* @param    val      @b{(output)} AVNU Initial Log Sync Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUInitialLogSyncIntervalGet(uint16_t intfNum, int8_t *val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    *val = dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogSyncInterval.val;
    rc = AVB_SUCCESS;
  }
  return rc;
}

/******************************************************************************
* @purpose  Set the AVNU Initial Log Sync Interval for the port
*
* @param    intfNum  @b{(input)} The external interface number
* @param    val      @b{(input)} AVNU Initial Log Sync Interval
*
* @returns  AVB_SUCCESS or AVB_FAILURE
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxIntfAVNUInitialLogSyncIntervalSet(uint16_t intfNum, int8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  intfIndex;

  if (cfgxIntfIndexGet(intfNum, &intfIndex) == AVB_SUCCESS)
  {
    rc = cfgxInt8Set(&dot1as_cfgx_data.intfCfg[intfIndex].avnuInitialLogSyncInterval, val);
  }
  return rc;
}


/******************************************************************************
* @purpose  Apply global 802.1AS configuration data for the bridge
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*******************************************************************************/
void dot1asCfgxGlobalApply(void)
{
  uint32_t  mac[AVB_MAC_ADDR_LEN];
  char mac_string[20];
  int rv;
  uint8_t i;

  pDot1asBridge_g->adminMode = dot1as_cfgx_data.globalAdmin.val;
  pDot1asBridge_g->priority1 = dot1as_cfgx_data.priority1.val;
  pDot1asBridge_g->priority2 = dot1as_cfgx_data.priority2.val;

  rv = osal_get_param(PARAMS_FILE, MAC_ADDRESS_PARAM_NAME, mac_string, 20);
  if ((rv == SOC_E_NONE) && (sal_sscanf(mac_string, "%x:%x:%x:%x:%x:%x",
      &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6)) {
      for (i = 0; i < 6; i++) {
          dot1asEthSrcAddr[i] = (uint8_t)mac[i];
      }
  }
  /* Clock Identity is an 8byte array in IEEE EUI-64 format */
  pDot1asBridge_g->thisClock.id[0] = mac[0];
  pDot1asBridge_g->thisClock.id[1] = mac[1];
  pDot1asBridge_g->thisClock.id[2] = mac[2];
  pDot1asBridge_g->thisClock.id[3] = 0xFF;
  pDot1asBridge_g->thisClock.id[4] = 0xFE;
  pDot1asBridge_g->thisClock.id[5] = mac[3];
  pDot1asBridge_g->thisClock.id[6] = mac[4];
  pDot1asBridge_g->thisClock.id[7] = mac[5];

  /* Set the Clock quality of thisClock */
  pDot1asBridge_g->thisClockQuality.class =  DOT1AS_CLOCK_CLASS_DEFAULT;
  pDot1asBridge_g->thisClockQuality.accuracy = DOT1AS_CLOCK_ACCURACY_DEFAULT;
  pDot1asBridge_g->thisClockQuality.variance = DOT1AS_CLOCK_VARIANCE_DEFAULT;

  /* Set the time property flags for thisClock. TBD. Defaults ok?  */
  pDot1asBridge_g->thisClockFlags &= ~(DOT1AS_CLK_FLG_LEAP59);
  pDot1asBridge_g->thisClockFlags &= ~(DOT1AS_CLK_FLG_LEAP61);
  pDot1asBridge_g->thisClockFlags &= ~(DOT1AS_CLK_FLG_TIME_TRACEABLE);
  pDot1asBridge_g->thisClockFlags &= ~(DOT1AS_CLK_FLG_FREQ_TRACEABLE);
  pDot1asBridge_g->thisClockFlags &= ~(DOT1AS_CLK_FLG_UTC_OFST_VALID);

  if (dot1as_cfgx_data.avnuMode.val == AVB_ENABLE)
  {
    /* set avnu mode */
    pDot1asBridge_g->avnuMode = dot1as_cfgx_data.avnuMode.val;
    /* Set GM clock identity to this time aware system*/
    pDot1asBridge_g->isGmPresent = TRUE;
    /* Initialize the clock state */
    pDot1asBridge_g->clkState = (DOT1AS_CLOCK_STATE_t)dot1as_cfgx_data.avnuClockState.val;
    pDot1asBridge_g->avnuSyncAbsenceTimeout = DOT1AS_INTL_SEC_TO_US(dot1as_cfgx_data.avnuSyncAbsenceTimeout.val);
  }
  else
  {
    /* Set default GM to this clock */
    pDot1asBridge_g->isGmPresent = FALSE;
    sal_memcpy(pDot1asBridge_g->gmClock.id, pDot1asBridge_g->thisClock.id,
           DOT1AS_CLOCK_ID_LEN);
    /* Set the parent port identity to this clock */
    sal_memcpy(pDot1asBridge_g->parentId.clock.id, pDot1asBridge_g->thisClock.id,
           DOT1AS_CLOCK_ID_LEN);
    pDot1asBridge_g->parentId.num = 0;
    /* Initialize the clock state */
    pDot1asBridge_g->clkState = DOT1AS_CLOCK_INIT_GM;
  }
  pDot1asBridge_g->gmStepsRemoved = 0xFF;
  pDot1asBridge_g->gmPriority1 = pDot1asBridge_g->priority1;
  pDot1asBridge_g->gmPriority2 = pDot1asBridge_g->priority2;
  pDot1asBridge_g->gmQuality = pDot1asBridge_g->thisClockQuality;
  pDot1asBridge_g->isSyncReceived = FALSE;
  pDot1asBridge_g->avnuGMLost = FALSE;
  pDot1asBridge_g->avnuSyncAbsenceOnStartUp = FALSE;
}


/******************************************************************************
* @purpose  Apply 802.1AS configuration data for the given interface
*
* @param    intfIndex    @b{(input)} Internal Interface index
* @param    intfCfgIndex @b{(input)} Interface configuration index to use for
*                                    the apply. May be different than intfIndex
*                                    if the port is a member of a lag.
*
* @returns  void
*
* @comments
*
* @end
*******************************************************************************/
void dot1asCfgxIntfApply(uint32_t intfIndex, uint32_t intfCfgIndex)
{
  DOT1AS_INTF_STATE_t *pIntf;

  if (intfIndex < dot1asMaxInterfaceCount_g)
  {
    pIntf = &pDot1asIntf_g[intfIndex];
    pIntf->pttPortEnabled = dot1as_cfgx_data.intfCfg[intfCfgIndex].pttPortEnabled.val;

    if (pDot1asBridge_g->avnuMode == AVB_ENABLE)
    {
#ifdef DOT1AS_LAG_SUPPORT
      /* Don't set avnu asCapable on LAG Members */
      if (AVB_INTERNAL_INTF_IS_LAG(intfCfgIndex))
      {
        pIntf->dot1asCapable = FALSE;
      }
      else
      {
        pIntf->dot1asCapable = dot1as_cfgx_data.intfCfg[intfCfgIndex].avnuDot1asCapable.val;
      }
#else
      pIntf->dot1asCapable = dot1as_cfgx_data.intfCfg[intfCfgIndex].avnuDot1asCapable.val;
#endif
      pIntf->selectedRole = dot1as_cfgx_data.intfCfg[intfCfgIndex].avnuPortRole.val;
      /* PDELAY configs */
      pIntf->neighborPropDelay = dot1as_cfgx_data.intfCfg[intfCfgIndex].avnuNeighborPropDelay.val;
      pIntf->currentLogPdelayReqInterval = dot1as_cfgx_data.intfCfg[intfCfgIndex].avnuInitialLogPdelayReqInterval.val;
      /* SYNC configs */
      pIntf->currentLogSyncInterval = dot1as_cfgx_data.intfCfg[intfCfgIndex].avnuInitialLogSyncInterval.val;
      if(pIntf->dot1asCapable == TRUE)
      {
        /* No need to set the link state */
      }
    }

    pIntf->neighborPropDelayThres = dot1as_cfgx_data.intfCfg[intfCfgIndex].neighborPropDelayThresh.val;
    pIntf->allowedLostReponses = dot1as_cfgx_data.intfCfg[intfCfgIndex].allowedLostResponses.val;
  }
}

/******************************************************************************
* @purpose  Apply 802.1AS configuration data
*
* @param    none
*
* @returns  AVB_SUCCESS - if configuration is successfully applied
* @returns  AVB_FAILURE - if failed to apply configuration
*
* @comments
*
* @end
*******************************************************************************/
AVB_RC_t dot1asCfgxApply(void)
{
  DOT1AS_INTF_STATE_t *pIntfState;
  uint32_t             index;
#ifdef DOT1AS_LAG_SUPPORT
  uint32_t             lagCfgIntf;
#endif
  uint32_t             intfCfgIndex = dot1asMaxInterfacesCfg_g;  /* Configuration index to use */

  /* Apply global config data */
  dot1asCfgxGlobalApply();

  /* Apply interface config data */
  for (index=0; index < dot1asMaxInterfacesCfg_g; index++)
  {
    /* State settings are only valid for the physical interfaces (not LAGs) */
    if (index < dot1asMaxInterfaceCount_g)
    {
      /* Apply Config */
      /* If the interface is a LAG member, apply the parent LAG config */
#ifdef DOT1AS_LAG_SUPPORT
      if (avb_lag_intf_for_member_port(index, &lagCfgIntf) == AVB_SUCCESS)
      {
        intfCfgIndex = lagCfgIntf;
      }
      else
      {
        intfCfgIndex = index;
      }
#else
      intfCfgIndex = index;
#endif
      pIntfState = &pDot1asIntf_g[index];
      sal_memset(pIntfState, 0, sizeof(DOT1AS_INTF_STATE_t));

      /* PDELAY defaults */
      pIntfState->isMeasuringPdelay = FALSE;
      pIntfState->isAVnuPdelayConfigSaved = FALSE;
      pIntfState->dot1asCapable = FALSE;
      pIntfState->pDelayReqState = DOT1AS_PDELAY_DISABLED;
      pIntfState->lagMemberDisabled = FALSE;

      /* BMCA defaults. */
      pIntfState->reselect = FALSE;
      pIntfState->selected = FALSE;
      pIntfState->updtInfo = FALSE;
      pIntfState->infoIs = DOT1AS_INFO_UNKNOWN;
      if (dot1as_cfgx_data.avnuMode.val == AVB_ENABLE)
      {
        pIntfState->selected = TRUE;
        pIntfState->reselect = FALSE;
        pIntfState->updtInfo = FALSE;
        pIntfState->infoIs = DOT1AS_INFO_MINE;
      }

      /* Timesync defaults */
      pIntfState->syncRxState = DOT1AS_SYNC_RX_DISABLED;
    }

    dot1asCfgxIntfApply(index, intfCfgIndex);

    if (index < dot1asMaxInterfaceCount_g)
    {
      /* Enable/disable state machines based on the configuration */
      dot1asIntfUpdate(index);
    }
  }

  return AVB_SUCCESS;
}


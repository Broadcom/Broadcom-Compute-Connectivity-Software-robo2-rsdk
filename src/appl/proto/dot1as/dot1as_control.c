/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    dot1as_control.c
 */

#include "osal/sal_task.h"
#include "osal/sal_sync.h"
#include "osal/sal_time.h"
#include "sal_get_set_param.h"
#include "shared/types.h"
#include "fsal/error.h"
#include "fsal/cosq.h"
#include "fsal_int/cosq.h"
#include "fsal/pktio.h"
#include "fsal/port.h"
#include "fsal/stg.h"
#include "fsal_int/vlan.h"
#include "fsal/exception.h"
#include "fsal/ptp.h"
#include "fsal_int/slic.h"
#include "soc/types.h"
#include "fsal_int/lag.h"
#include "portdb.h"
#include "dot1as.h"
#include "dot1as_cfgx.h"
#include "avb.h"

static int dot1asRxHandler(void *buffer, int buffer_size,
                           cbx_packet_attr_t *packet_attr,
                           cbx_packet_cb_info_t *cb_info);
void dot1asThread(void *arg);
void dot1asTimerThread(void *arg);

/* DOT1AS destination MAC */
uint8_t ETHER_MAC_DOT1AS_g[ETHER_ADDR_LEN] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};

/* DOT1AS Brige State */
DOT1AS_BRIDGE_STATE_t *pDot1asBridge_g = (DOT1AS_BRIDGE_STATE_t *)NULL;

/* DOT1AS Interface State */
DOT1AS_INTF_STATE_t   *pDot1asIntf_g = (DOT1AS_INTF_STATE_t *)NULL;

uint8_t dot1asEthSrcAddr[ETHER_ADDR_LEN];
uint8_t dot1asTxBuffer[DOT1AS_MAX_PDU_SIZE];
struct dot1asMsg dot1asMsgBuf[DOT1AS_MAX_QUEUE_MSGS];
static uint8_t dot1asMsgWrIdx;
static uint8_t dot1asMsgRdIdx;
static uint8_t dot1asTimerEvent;
static cbx_pktio_rx_cb_handle dot1asRxCbHdl = NULL;
static sal_thread_t dot1asTimerThreadId;
static sal_thread_t dot1asThreadId;
sal_sem_t dot1asSemId;
uint8_t dot1asMaxInterfaceCount_g; /* number of physical ports */
uint8_t avbMaxIntfPhysP8_g = CBX_MAX_PORT_PER_UNIT - 2;
uint8_t dot1asMaxInterfacesCfg_g;
uint8_t avbMaxIntfaces_g = CBX_MAX_PORT_PER_UNIT - 2;
uint8_t dot1asIsReady_g = AVB_FALSE;

/* Trace mask for DOT1AS and AVnu */
uint32_t dot1asTraceMask =  0;
uint32_t avnuTraceMask =  0;

/*@api
 * dot1asInit
 *
 * @brief
 *    Initialize 802.1AS application and allocate required resources.
 *
 * @param=arg - argument passed to func.
 *
 * @returns AVB_SUCCESS
 * @returns AVB_FAILURE
 *
 */
AVB_RC_t dot1asInit(void)
{
  uint32_t size = 0;
  uint32_t total_size = 0;
  cbx_packet_cb_info_t recv_info;
  uint8_t i;
  int rv;

  /* Alloc memory */
  size = (uint32_t)sizeof(DOT1AS_BRIDGE_STATE_t);
  if (pDot1asBridge_g == (DOT1AS_BRIDGE_STATE_t *)NULL)
  {
    pDot1asBridge_g = (DOT1AS_BRIDGE_STATE_t *)sal_alloc(size, "DOT1AS");
    if (pDot1asBridge_g == (DOT1AS_BRIDGE_STATE_t *)NULL)
    {
      DOT1AS_TRACE("Failed to allocate memory for DOT1AS\n");
      return AVB_FAILURE;
    }
    total_size += size;
  }

  sal_memset(pDot1asBridge_g, 0x0, size);
  dot1asSignalingStateInit();

  /* currently dot1as only runs on physical ports (0-13) */
  dot1asMaxInterfaceCount_g = CBX_MAX_PORT_PER_UNIT - 2;
  dot1asMaxInterfacesCfg_g = dot1asMaxInterfaceCount_g;

  size = (uint32_t)(sizeof(DOT1AS_INTF_STATE_t) * dot1asMaxInterfaceCount_g);
  if (pDot1asIntf_g == (DOT1AS_INTF_STATE_t *)NULL)
  {
    pDot1asIntf_g = (DOT1AS_INTF_STATE_t *)sal_alloc(size, "DOT1AS");
    if (pDot1asIntf_g == (DOT1AS_INTF_STATE_t *)NULL)
    {
      DOT1AS_TRACE("Failed to allocate memory for DOT1AS\n");
      return AVB_FAILURE;
    }
    total_size += size;
  }
  sal_memset(pDot1asIntf_g, 0x0, size);

  if (total_size)
  {
    DOT1AS_TRACE_DBG("DOT1AS: allocated memory of size %d\n", total_size);
  }

  /* Apply 802.1AS configuration data */
  avbCfgxIntfNum = (uint8_t*)sal_alloc(sizeof(uint8_t) * dot1asMaxInterfaceCount_g, "DOT1AS");
  sal_memset(avbCfgxIntfNum, 0x0, dot1asMaxInterfaceCount_g);
  for (i = 0; i < dot1asMaxInterfaceCount_g; i++)
  {
    avbCfgxIntfNum[i] = i;
  }
  dot1asCfgxInit();
  dot1asCfgxApply();

  /* Register Rx handler */
  sal_memset(&recv_info, 0, sizeof(cbx_packet_cb_info_t));
  recv_info.flags = CBX_PKTIO_CALLBACK_TRAP;
  recv_info.name = "DOT1AS";
  recv_info.priority = DOT1AS_PKT_RECEIVE_PRIORITY;
  recv_info.cookie = NULL;
  if (cbx_pktio_register(recv_info, dot1asRxHandler, &dot1asRxCbHdl) != CBX_E_NONE)
  {
    DOT1AS_TRACE("Failed to register with PKTIO module\n");
    goto pktioRegFail;
  }
  dot1asSemId = sal_sem_create("DOT1AS Ready", 1, 0);
  if (!dot1asSemId)
  {
    DOT1AS_TRACE("Failed to create semaphore \n");
    goto semCreateFail;
  }
  dot1asTimerThreadId = sal_thread_create("DOT1AS", DOT1AS_TASK_STACK_SIZE,
                                          DOT1AS_TASK_PRIO, dot1asTimerThread, NULL);
  if (dot1asTimerThreadId == SAL_THREAD_ERROR)
  {
    DOT1AS_TRACE("Failed to create Timer thread \n");
    goto threadCreateFail;
  }

  dot1asThreadId = sal_thread_create("DOT1AS", DOT1AS_TASK_STACK_SIZE,
                                     DOT1AS_TASK_PRIO, dot1asThread, NULL);
  if (dot1asThreadId == SAL_THREAD_ERROR)
  {
    DOT1AS_TRACE("Failed to create dot1as thread \n");
    goto threadCreateFail;
  }

  /* Enable PTP functionality on physical ports */
  for (i = 0; i < dot1asMaxInterfaceCount_g; i++)
  {
    rv = cbx_ptp_port_admin_set(i,CBX_PTP_PORT_ENABLE);
    if (rv == AVB_FAILURE)
    {
      DOT1AS_TRACE("Failed to enable PTP on the port %d \n", i);
      goto threadCreateFail;
    }
  }
  dot1asIsReady_g = AVB_TRUE;
  return AVB_SUCCESS;
threadCreateFail:
  if (dot1asTimerThreadId)
  {
    sal_thread_destroy(dot1asTimerThreadId);
  }
  if (dot1asThreadId)
  {
    sal_thread_destroy(dot1asThreadId);
  }
  sal_sem_destroy(dot1asSemId);
semCreateFail:
  cbx_pktio_unregister(dot1asRxCbHdl);
pktioRegFail:
  return AVB_FAILURE;
}

/*@api
 * dot1asTimerThread
 *
 * @brief
 *    802.1AS timer thread to generate ticks periodically
 *
 * @param=arg - argument passed to func.
 *
 * @returns void
 *
 */
void dot1asTimerThread(void *arg)
{
  while(1)
  {
    sal_nsleep(DOT1AS_TIMER_TICK_US * 1000);
    dot1asTimerEvent = TRUE;
    sal_sem_give(dot1asSemId);
  }
}

/*@api
 * dot1asTransmit
 *
 * @brief
 *    802.1AS transmit function handles message transmission intervals
 *    and receipt timeouts
 *
 * @param=arg - argument passed to func.
 *
 * @returns void
 *
 */
void dot1asTransmit(void *arg)
{
  /* Check Announce Timers */
  if (pDot1asBridge_g->avnuMode != TRUE)
  {
    dot1asCheckBmcaTimers();
  }
  else
  {
    dot1asCheckAvnuSignalingTimers();
  }

  /* Check Time Sync Timers */
  dot1asCheckTimesyncTimers();

  /* Check PDELAY timers */
  dot1asCheckPdelayTimers();
}

/*@api
 * dot1asThread
 *
 * @brief
 *    802.1AS thread waits for incoming messages to process.
 *    This thread also handles transmit side of 802.1as messages.
 *
 * @param=arg - argument passed to func.
 *
 * @returns void
 *
 */
void dot1asThread(void *arg)
{
  DOT1AS_PDU_t  pdu;
  struct dot1asMsg *msg;

  while (1)
  {
    sal_sem_take(dot1asSemId, sal_sem_FOREVER);
    while (dot1asMsgRdIdx != dot1asMsgWrIdx)
    {
      msg = &dot1asMsgBuf[dot1asMsgRdIdx];
      /* Parse the DOT1AS PDU */
      if (dot1asPduParse(msg->buffer, msg->bufferSize, &pdu))
      {
        DOT1AS_TRACE_DBG("dot1asReceiveThread: failed to parse  pdu\n");
      }
      else
      {
        /* DOT1AS PDU is successfully parsed, now process it */
        dot1asPduReceiveProcess(msg->port, &pdu, msg->ingTs);
      }
      /* free the buffer when the packet is handled */
      sal_free(msg->buffer);
      dot1asMsgRdIdx =
        (dot1asMsgRdIdx == (DOT1AS_MAX_QUEUE_MSGS - 1)) ? 0: dot1asMsgRdIdx + 1;
    }
    if (dot1asTimerEvent == TRUE)
    {
      /* process timer events */
      dot1asTransmit(NULL);
      dot1asTimerEvent = FALSE;
    }
  }
}


/******************************************************************************
* @purpose  Enable/Disable DOT1AS mechanism(s) on the given interface.
*
* @param    uint32  intfIndex  DOT1AS internal interface index
*
* @returns  AVB_SUCCESS
* @returns  AVB_FAILURE
*
* @comments Not intended to be called with LAG interfaces
*
* @end
*
*******************************************************************************/
AVB_RC_t dot1asIntfUpdate(uint32_t intfIndex)
{
  if (dot1asIsValidIntf(intfIndex) != TRUE)
  {
    DOT1AS_TRACE_ERR("%s: invalid internal interface %d\n", __FUNCTION__, intfIndex);
    return AVB_FAILURE;
  }

  /* Check if interface is PDELAY capable */
  if (dot1asIsPdelayCapable(intfIndex) == TRUE)
  {
    dot1asIntfPdelayInit(intfIndex);
  }
  else
  {
    /* Disable PDELAY mechanism */
    dot1asIntfPdelayDisable(intfIndex);
  }

  /*  Re-init BMCA state machine by generating a dummy interface event */
  dot1asInvokeBmca(intfIndex, DOT1AS_BMC_INTF_EVENT, NULL);

  /* Check if TimeSync mechanism can be enabled */
  if (dot1asIsTimesyncCapable(intfIndex) == TRUE)
  {
    dot1asIntfTimesyncInit(intfIndex);
  }
  else
  {
    /* Disable Timesync mechanism */
    dot1asIntfTimesyncDisable(intfIndex);
  }

  /* Re-init the signaling state machine */
  if (dot1asIsSignalingCapable(intfIndex) == TRUE)
  {
    dot1asIntfSignalingInit(intfIndex);
  }
  return AVB_SUCCESS;
}

/*@api
 * dot1asPktSend
 *
 * @brief
 *    Sends the 802.1as messages out on the given port
 *
 * @param=buffer - Packet buffer to be sent.
 * @param=bufLen - Buffer Length.
 * @param=intfIndex - Interface index.

 * @returns AVB_SUCCESS
 * @returns AVB_FAILURE
 &
 */
AVB_RC_t dot1asPktSend(void *buffer, uint16_t bufLen, uint32_t intfIndex)
{
  cbx_packet_attr_t attr;
  cbx_port_params_t portParams;
  cbx_stg_stp_state_t stpState;
  cbx_stgid_t stgId;
  int rc;

  /* Check the outgoing port STP state before sending the packet. In packet
   * send reseverd VLAN 0 needs to be used in CBX_PACKET_TX_FWD_UCAST mode
   * instead of pvid of the port. The VLAN 0 is part of STG0, and if the port
   * is up it is always in forward state. But if any other STG created and
   * the port pvid is associated with that STG, then if port is in discarding
   * state, do not send the pkt out */
  if ((rc = cbx_port_info_get(intfIndex, &portParams)) == CBX_E_NONE)
  {
    if ((rc = cbxi_vlan_stgid_get(portParams.default_vid, &stgId)) == CBX_E_NONE)
    {
      if ((rc = cbx_stg_stp_get(stgId, intfIndex, &stpState)) == CBX_E_NONE)
      {
        if (stpState == cbxStgStpStateDiscard)
        {
          return AVB_SUCCESS;
        }
      }
    }
  }
  if (rc != CBX_E_NONE)
  {
    return AVB_FAILURE;
  }

  cbx_packet_attr_t_init(&attr);
  attr.flags = CBX_PACKET_TX_FWD_UCAST;

  attr.dest_index = intfIndex;
  attr.tc.int_pri = CBXI_COSQ_MAX_INT_PRI;

  /* check minimum packet length */
  if (bufLen < 64)
  {
    bufLen = 64;
  }

  if (cbx_pktio_packet_send((void *)buffer, bufLen, &attr) != CBX_E_NONE)
  {
    DOT1AS_TRACE_ERR("DOT1AS: cbx_pktio_packet_send fail on port %d\n", intfIndex);
    return AVB_FAILURE;
  }
  return AVB_SUCCESS;
}

/*@api
 * dot1asRxHandler
 *
 * @brief
 *    802.1AS receive handler callback function. Receives the messages
 *    from PKTIO module and posts it to dot1as thread.
 *
 * @param=arg - argument passed to func.
 *
 * @returns CBX_RX_NOT_HANDLED
 * @returns CBX_RX_HANDLED
 *
 */
int dot1asRxHandler(void *buffer, int buffer_size,
                cbx_packet_attr_t *packet_attr,
                cbx_packet_cb_info_t *cb_info)
{
  struct dot1asMsg *msg;

  /* validate the packet is 802.1as or not */
  if ((packet_attr->trap_data < (CBXI_SLIC_ID_START + SLICID_PTP_ETH_PP0)) ||
     (packet_attr->trap_data > (CBXI_SLIC_ID_START + SLICID_PTP_UDP_PP15)))
  {
    return CBX_RX_NOT_HANDLED;
  }

  /* Check the destination MAC of the frame */
  if (sal_memcmp(buffer, ETHER_MAC_DOT1AS_g, ETHER_ADDR_LEN) != 0)
  {
    return CBX_RX_NOT_HANDLED;
  }

  /* Check for the free message buffer */
  if ((dot1asMsgRdIdx > dot1asMsgWrIdx) ||
    ((DOT1AS_MAX_QUEUE_MSGS - (dot1asMsgWrIdx - dot1asMsgRdIdx)) != 0))
  {
    msg = &dot1asMsgBuf[dot1asMsgWrIdx];
    msg->port = packet_attr->source_index;
    msg->ingTs = packet_attr->ing_ptp_ts;
    if (msg->ingTs == 0)
    {
      DOT1AS_TRACE_ERR("PTP packet with Invalid Ingress Time stamp\n");
    }
    msg->buffer = buffer;
    msg->bufferSize = buffer_size;
    dot1asMsgWrIdx = (dot1asMsgWrIdx == (DOT1AS_MAX_QUEUE_MSGS - 1)) ? 0:dot1asMsgWrIdx + 1;
    /* Free the pakcet attribute structure. Packet buffer will be freed later */
    sal_free(packet_attr);
    sal_sem_give(dot1asSemId);
  }
  else
  {
    sal_free(buffer);
    sal_free(packet_attr);
    DOT1AS_TRACE_ERR("DOT1AS packet dropped - Not enough buffers \n");
  }
  return CBX_RX_HANDLED;
}


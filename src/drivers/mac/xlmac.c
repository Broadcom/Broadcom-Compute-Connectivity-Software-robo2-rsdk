/*
 * *   xlmac.c
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * Purpose:
 * *     xlmac driver
 * *
 * */

#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/mac.h>
#include <fsal/error.h>
#include <soc/robo2/common/tables.h>
#include <cdk/cdk_symbols.h>
#include <avng_phy_ctrl.h>
#include "sal_get_set_param.h"

#define XLMAC_RUNT_THRESHOLD_GE    0x40 /* Runt threshold value for GE ports */
#define XLMAC_RUNT_THRESHOLD_XE    0x40 /* Runt threshold value for XE ports */

/*
 * XLMAC Register field definitions.
 */

mac_driver_t soc_mac_xl;


#define SOC_XLMAC_SPEED_10     0x0
#define SOC_XLMAC_SPEED_100    0x1
#define SOC_XLMAC_SPEED_1000   0x2
#define SOC_XLMAC_SPEED_2500   0x3
#define SOC_XLMAC_SPEED_10000  0x4

/* Transmit CRC Modes */
#define XLMAC_CRC_APPEND        0x0
#define XLMAC_CRC_KEEP          0x1
#define XLMAC_CRC_REPLACE       0x2
#define XLMAC_CRC_PER_PKT_MODE  0x3

struct {
    int speed;
    uint32 clock_rate;
}_mac_xl_clock_rate[] = {
    { 40000, 312 },
    { 20000, 156 },
    { 10000, 78  },
    { 5000,  78  },
    { 2500,  312 },
    { 1000,  125 },
    { 0,     25  },
};


STATIC int
mac_xl_speed_get(int unit, soc_port_t port, uint32 *speed);

STATIC int
mac_xl_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  uint32 value);

STATIC int
mac_xl_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  uint32 *value);

/*
 * Function:
 *      soc_link_mask_get
 * Purpose:
 *      Get link status bitmap
 * Parameters:
 *      unit -  unit #.
 *      mask - (OUT) Value.
 */

void
soc_link_mask_get(int unit, pbmp_t *mask)
{
    *mask = SOC_CONTROL(unit)->link_mask;
}

/*
 * Function:
 *      soc_link_mask_get
 * Purpose:
 *      Set link status bitmap
 * Parameters:
 *      unit -  unit #.
 *      mask - (OUT) Value.
 */
void
soc_link_mask_set(int unit, pbmp_t mask)
{
    SOC_CONTROL(unit)->link_mask = mask;
}

/*
 * Function:
 *      _mac_xl_speed_to_clock_rate
 * Purpose:
 *      get the clock rate for the speed
 */
void
_mac_xl_speed_to_clock_rate(int unit, soc_port_t port, int speed,
                            uint32 *clock_rate)
{
    int idx;


    for (idx = 0;
         idx < sizeof(_mac_xl_clock_rate) / sizeof(_mac_xl_clock_rate[0]);
         idx++) {
        if (speed >=_mac_xl_clock_rate[idx].speed) {
            *clock_rate = _mac_xl_clock_rate[idx].clock_rate;
            return;
        }
    }
    *clock_rate = 0;
}

/*
 * Function:
 *      _mac_xl_drain_cells
 * Purpose:
 *      Drain cells
 */
STATIC int
_mac_xl_drain_cells(int unit, soc_port_t port, int notify_phy, int queue_enable)
{
    int         rv;
    uint32      pause_tx = 0, pause_rx = 0, pfc_rx = 0, llfc_rx = 0;
    uint32      fval;
    uint32      mac_ctrl[2], rval64[2], mac_tx_ctrl[2];

    rv  = SOC_E_NONE;
    /* Disable pause/pfc function */
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_set(unit, port, pause_tx, 0));

    SOC_IF_ERROR_RETURN
        (mac_xl_control_get(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   &pfc_rx));
    SOC_IF_ERROR_RETURN
        (mac_xl_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   0));

    SOC_IF_ERROR_RETURN
        (mac_xl_control_get(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   &llfc_rx));
    SOC_IF_ERROR_RETURN
        (mac_xl_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   0));

    /* Assert SOFT_RESET before DISCARD just in case there is no credit left */
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &mac_ctrl));
    fval = 1;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&mac_ctrl, SOFT_RESETf, &fval);

    /* Drain data in TX FIFO without egressing at packet boundary */
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &mac_ctrl));

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_TX_CTRLr(unit, port, &mac_tx_ctrl));
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&mac_tx_ctrl, DISCARDf, &fval);
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&mac_tx_ctrl,
                                 EP_DISCARDf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_TX_CTRLr(unit, port, &mac_tx_ctrl));

    /* De-assert SOFT_RESET to let the drain start */
    fval = 0;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&mac_ctrl, SOFT_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &mac_ctrl));


    /* Wait until TX fifo cell count is 0 */
    for (;;) {
        rv = REG_READ_XLMAC_TXFIFO_CELL_CNTr(unit, port, &rval64);
        if (SOC_E_NONE != rv) {
            break;
        }
        SOC_IF_ERROR_RETURN(soc_XLMAC_TXFIFO_CELL_CNTr_field_get(unit,
                                         (uint32 *)&rval64, CELL_CNTf, &fval));
        if (fval == 0) {
            break;
        }
    }

    /* Stop TX FIFO drainging */
    fval = 0;
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&mac_tx_ctrl, DISCARDf, &fval);
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&mac_tx_ctrl,
                                 EP_DISCARDf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_TX_CTRLr(unit, port, &mac_tx_ctrl));

    /* Restore original pause/pfc/llfc configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_set(unit, port, pause_tx, pause_rx));
    SOC_IF_ERROR_RETURN
        (mac_xl_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   pfc_rx));
    SOC_IF_ERROR_RETURN
        (mac_xl_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   llfc_rx));

    return rv;
}

/*
 * Function:
 *      soc_port_credit_reset
 * Purpose:
 *      reset credit for the port
 */
static int
soc_port_credit_reset(int unit, soc_port_t port)
{
    uint32 rval[2];
    uint32 val;

    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_ENABLE_REGr(unit, &rval));
    val = 0;

    if (port == 10) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT2f, &val);
    } else if (port == 11) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT3f, &val);
    } else if (port == 12) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT1f, &val);
    } else if (port == 13) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT0f, &val);
    }

    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_ENABLE_REGr(unit, &rval));

    SOC_IF_ERROR_RETURN(REG_READ_NPA_NPA_CREDIT_CONTROLr(unit, &val));

    val &= ~(1 << port);
    SOC_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_CREDIT_CONTROLr(unit, &val));
    val |= (1 << port);
    SOC_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_CREDIT_CONTROLr(unit, &val));

    val = 1;

    if (port == 10) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT2f, &val);
    } else if (port == 11) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT3f, &val);
    } else if (port == 12) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT1f, &val);
    } else if (port == 13) {
        soc_XLPORT_ENABLE_REGr_field_set(unit, (uint32 *)&rval, PORT0f, &val);
    }

    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_ENABLE_REGr(unit, &rval));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_port_fifo_reset
 * Purpose:
 *      reset port fifo
 */
int
soc_port_fifo_reset(int unit, soc_port_t port)
{
    uint32 rval, orig_rval;
    uint32 fval = 1;

    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_SOFT_RESETr(unit, &rval));
    orig_rval = rval;
    soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&rval, PORT0f, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_SOFT_RESETr(unit, &rval));
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_SOFT_RESETr(unit, &orig_rval));
    return SOC_E_NONE;
}
/*
 * Function:
 *      mac_xl_init
 * Purpose:
 *      Initialize Xlmac into a known good state.
 * Parameters:
 *      unit - unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *
 */
STATIC int
mac_xl_init(int unit, soc_port_t port)
{
    uint32_t reg_data[2];
    uint32   fval = 0;
    uint32 value = 1;
    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    fval = 1;
    char mac_string[20];
    uint32_t mac_addr[6];
    int rv;

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_CTRLr(unit, port, (uint32 *)&reg_data));
    soc_XLMAC_RX_CTRLr_field_set(unit, (uint32 *)&reg_data,  STRIP_CRCf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_CTRLr(unit, port, (uint32 *)&reg_data));
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_CTRLr(unit, port, (uint32 *)&reg_data));

    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_TX_CTRLr(unit, port, (uint32 *)&reg_data));
    fval = 1;
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&reg_data,  PAD_ENf, &fval);
    fval = 0;
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&reg_data,  CRC_MODEf, &fval);
    fval = 2;
    soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&reg_data,  TX_THRESHOLDf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_TX_CTRLr(unit, port, (uint32 *)&reg_data));
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_TX_CTRLr(unit, port, (uint32 *)&reg_data));

    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_MAX_SIZEr(unit, port, (uint32 *)&reg_data));
    fval = (JUMBO_MAXSZ - 4); /* STRIP_CRC set in XLMAC_RX_CTRL. Length check is without CRC */;
    soc_XLMAC_RX_MAX_SIZEr_field_set(unit, (uint32 *)&reg_data, RX_MAX_SIZEf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_MAX_SIZEr(unit, port, (uint32 *)&reg_data));
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_MAX_SIZEr(unit, port, (uint32 *)&reg_data));


    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_CNTMAXSIZEr(unit, port,  (uint32 *)&reg_data));
    reg_data[0] = (JUMBO_MAXSZ - 4);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_CNTMAXSIZEr(unit, port, (uint32 *)&reg_data));
    reg_data[0] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_CNTMAXSIZEr(unit, port, (uint32 *)&reg_data));

    reg_data[0] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, (uint32 *)&reg_data));
    fval = 1;
    soc_XLPORT_FLOW_CONTROL_CONFIGr_field_set(unit, (uint32 *)&reg_data, MERGE_MODE_ENf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, (uint32 *)&reg_data));
    reg_data[0] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, (uint32 *)&reg_data));

    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PAUSE_CTRLr(unit, port, (uint32 *)&reg_data));
    fval = 1;
    soc_XLMAC_PAUSE_CTRLr_field_set(unit, (uint32 *)&reg_data, TX_PAUSE_ENf, &fval);
    soc_XLMAC_PAUSE_CTRLr_field_set(unit, (uint32 *)&reg_data, RX_PAUSE_ENf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PAUSE_CTRLr(unit, port,(uint32 *)&reg_data));
    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PAUSE_CTRLr(unit, port, (uint32 *)&reg_data));

    /* set SMAC for pause frame */
    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_TX_MAC_SAr(unit, port, (uint32 *)&reg_data));

    /* Get board MAC address and use that address */
    rv = osal_get_param(PARAMS_FILE, MAC_ADDRESS_PARAM_NAME, mac_string, 20);
    if (rv == SOC_E_NONE) {
        rv = sal_sscanf(mac_string, "%x:%x:%x:%x:%x:%x",
                         &mac_addr[0], &mac_addr[1], &mac_addr[2],
                         &mac_addr[3], &mac_addr[4], &mac_addr[5]);
        if (rv != 6) {
          sal_printf("ERROR: Invalid mac string\n");
          rv = SOC_E_FAIL;
        } else {
            reg_data[0] = ((uint8_t)mac_addr[2] << 24) | ((uint8_t)mac_addr[3] << 16) |
                     ((uint8_t)mac_addr[4] << 8) | (uint8_t)mac_addr[5] ;
            reg_data[1]  = ((uint8_t)mac_addr[0] << 8) | (uint8_t)mac_addr[1] ;
            SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_TX_MAC_SAr(unit, port, (uint32 *)&reg_data));
            rv = SOC_E_NONE;
        }
    }
    if (rv != SOC_E_NONE){
        /* SA_HI = 0x0200*/
        reg_data[0] = 0;
        reg_data[1] = 0x0200;
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_TX_MAC_SAr(unit, port, (uint32 *)&reg_data));
    }

    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, (uint32 *)&reg_data));
    fval = 1;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&reg_data, TX_ENf, &fval);
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&reg_data, RX_ENf, &fval);
    fval = 0;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&reg_data, SOFT_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, (uint32 *)&reg_data));
    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, (uint32 *)&reg_data));
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_data[0]));

    soc_XLMAC_RX_LSS_CTRLr_field_set(unit, (uint32 *)&reg_data[0],
                                     DROP_TX_DATA_ON_REMOTE_FAULTf, &value);
    soc_XLMAC_RX_LSS_CTRLr_field_set(unit, (uint32 *)&reg_data[0],
                                     DROP_TX_DATA_ON_LOCAL_FAULTf, &value);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_LSS_CTRLr(unit, port, &reg_data[0]));

    /* Disabling Frame length not out of range, but incorrect -- IEEE length check failed in order to
       make the behavior same as unimac and other devices */
    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_MAC_RSV_MASKr(unit, port, (uint32 *)&reg_data));
    reg_data[0] &= 0xFFFFFFDF;
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_RSV_MASKr(unit, port, &reg_data[0]));

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, (uint32 *)&reg_data));
    fval = 1;
    soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&reg_data, RX_PFC_ENf, &fval);
    soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&reg_data, TX_PFC_ENf, &fval);
    soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&reg_data, PFC_REFRESH_ENf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, (uint32 *)&reg_data));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_egress_queue_drain
 * Purpose:
 *      Drain the egress queues with out bringing down the port
 * Parameters:
 *      unit - unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_egress_queue_drain(int unit, soc_port_t port)
{
    uint32 ctrl[2], octrl[2];
    pbmp_t mask;
    uint32 rx_enable = 0;
    int is_active = 0;
    uint32 fval = 0;

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &ctrl));
    octrl[0] = ctrl[0];
    octrl[1] = ctrl[1];

    soc_XLMAC_CTRLr_field_get(unit, (uint32 *)&ctrl, RX_ENf, &rx_enable);

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, RX_ENf, &fval);
    fval = 1;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, TX_ENf, &fval);
    /* Disable RX */
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &ctrl));

       /* Remove port from LINK bitmap */
    soc_link_mask_get(unit, &mask);
    if (SOC_PBMP_MEMBER(mask, port)) {
        is_active = 1;
        SOC_PBMP_PORT_REMOVE(mask, port);
        soc_link_mask_set(unit, mask);
    }

        /* Drain cells */
    SOC_IF_ERROR_RETURN(_mac_xl_drain_cells(unit, port, 0, TRUE));

        /* Reset port FIFO */
    SOC_IF_ERROR_RETURN(soc_port_fifo_reset(unit, port));

        /* Put port into SOFT_RESET */
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, SOFT_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &ctrl));

        /* Reset EP credit before de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &ctrl));
    fval = rx_enable ? 1 : 0;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, RX_ENf, &fval);

      /* Enable both TX and RX, deassert SOFT_RESET */
    fval = 0;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, SOFT_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &ctrl));

        /* Restore XLMAC_CTRL to original value */
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &octrl));

        /* Add port to EPC_LINK */
    if(is_active) {
        soc_link_mask_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        soc_link_mask_set(unit, mask);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_enable_set(int unit, soc_port_t port, uint32 enable)
{
#if 1
    uint32 ctrl[2], octrl[2];
    pbmp_t mask;
    uint32 soft_reset = 0;
    uint32 fval = 1;

    if ((port == 12) || (port == 13)) {
        return CBX_E_NONE;
    }
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &ctrl));
    octrl[0] = ctrl[0];
    octrl[1] = ctrl[1];
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, TX_ENf, &fval);
    fval = enable ? 1 : 0;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, RX_ENf, &fval);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        soc_XLMAC_CTRLr_field_get(unit, (uint32 *)&ctrl,
                                  SOFT_RESETf, &soft_reset);
        if ((enable) || (!enable && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (enable) {
        /* Reset EP credit before de-assert SOFT_RESET */
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));

        /* Deassert SOFT_RESET */
        fval = 0;
        soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, SOFT_RESETf, &fval);

        /* Enable both TX and RX */
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &ctrl));

        /* Add port to LINK map */
        soc_link_mask_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        soc_link_mask_set(unit, mask);

    } else {
        /* Disable RX */
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &ctrl));

        /* Remove port from _LINK map */
        soc_link_mask_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        soc_link_mask_set(unit, mask);
        sal_usleep(1000);

        /* Drain cells */
        SOC_IF_ERROR_RETURN(_mac_xl_drain_cells(unit, port, 1, FALSE));

        /* Reset port FIFO */
        SOC_IF_ERROR_RETURN(soc_port_fifo_reset(unit, port));

        /* Put port into SOFT_RESET */
        fval = 1;
        soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&ctrl, SOFT_RESETf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &ctrl));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_enable_get(int unit, soc_port_t port, uint32 *enable)
{
    uint32 ctrl[2];

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &ctrl));

    soc_XLMAC_CTRLr_field_get(unit, (uint32 *)&ctrl, RX_ENf, enable);

    return SOC_E_NONE;
}



/*
 * Function:
 *      mac_xl_duplex_set
 * Purpose:
 *      Set XLMAC in the specified duplex mode.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_xl_duplex_set(int unit, soc_port_t port, uint32 duplex)
{
    /* XLAMC is always in full duplex mode */
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_duplex_get
 * Purpose:
 *      Get XLMAC duplex mode.
 * Parameters:
 *      unit - unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_duplex_get(int unit, soc_port_t port, uint32 *duplex)
{
    *duplex = TRUE; /* Always full duplex */
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_pause_set
 * Purpose:
 *      Configure XLMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_pause_set(int unit, soc_port_t port, uint32 pause_tx, uint32 pause_rx)
{
    uint32 value;
    uint32 ctrl[2];

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PAUSE_CTRLr(unit, port, &ctrl));
    if (pause_tx != (uint32)-1) {
        value = (pause_tx != FALSE ? 1 : 0);
        soc_XLMAC_PAUSE_CTRLr_field_set(unit, (uint32 *)&ctrl, TX_PAUSE_ENf, &value);
    }
    if (pause_rx != (uint32)-1) {
        value = (pause_rx != FALSE ? 1 : 0);
        soc_XLMAC_PAUSE_CTRLr_field_set(unit, (uint32 *)&ctrl, RX_PAUSE_ENf, &value);
    }
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PAUSE_CTRLr(unit, port, &ctrl));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_pause_get
 * Purpose:
 *      Return the pause ability of XLMAC
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      pause_tx - Boolean: transmit pause
 *      pause_rx - Boolean: receive pause
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_pause_get(int unit, soc_port_t port, uint32 *pause_tx, uint32 *pause_rx)
{
    uint32 rval[2];

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PAUSE_CTRLr(unit, port, &rval));
    soc_XLMAC_PAUSE_CTRLr_field_get(unit, (uint32 *)&rval,
                                    TX_PAUSE_ENf, pause_tx);
    soc_XLMAC_PAUSE_CTRLr_field_get(unit, (uint32 *)&rval,
                                    RX_PAUSE_ENf, pause_rx);
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_speed_set
 * Purpose:
 *      Set XLMAC in the specified speed.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      speed - 10, 100, 1000, 2500, 10000, ...
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_speed_set(int unit, soc_port_t port, uint32 speed)
{
    uint32 mode;
    uint32 enable = 0;
    uint32 val[2];
    uint32 value;
    uint32 rval = 0;
    uint32  wake_timer = 0, lpi_timer = 0;
#ifndef EMULATION_BUILD
    phy_ctrl_t *pc;
#endif
    soc_info_t *si;

    switch (speed) {
    case 10:
        mode = SOC_XLMAC_SPEED_10;
        break;
    case 100:
        mode = SOC_XLMAC_SPEED_100;
        break;
    case 1000:
        mode = SOC_XLMAC_SPEED_1000;
        break;
    case 2500:
        mode = SOC_XLMAC_SPEED_2500;
        break;
    case 5000:
        mode = SOC_XLMAC_SPEED_10000;
        break;
    case 0:
        return SOC_E_NONE;              /* Support NULL PHY */
    default:
        if (speed < 10000) {
            return SOC_E_PARAM;
        }
        mode = SOC_XLMAC_SPEED_10000;
        break;
    }

    SOC_IF_ERROR_RETURN(mac_xl_enable_get(unit, port, &enable));

    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_xl_enable_set(unit, port, 0));
    }

    /* Update the speed */
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_MODEr(unit, port, &val));
    soc_XLMAC_MODEr_field_set(unit, (uint32 *)&val, SPEED_MODEf, &mode);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_MODEr(unit, port, &val));

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_CTRLr(unit, port, &val));
    value = (speed >= 10000 ? 1 : 0);
    soc_XLMAC_RX_CTRLr_field_set(unit, (uint32 *)&val,
                                   STRICT_PREAMBLEf, &value);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_CTRLr(unit, port, &val));

    value = speed < 5000 ? 1 : 0;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &val));
    soc_XLMAC_RX_LSS_CTRLr_field_set(unit, (uint32 *)&val,
                                     LOCAL_FAULT_DISABLEf, &value);
    soc_XLMAC_RX_LSS_CTRLr_field_set(unit, (uint32 *)&val,
                                     REMOTE_FAULT_DISABLEf, &value);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_LSS_CTRLr(unit, port, &val));

    /*
     * Not set REMOTE_FAULT/LOCAL_FAULT for 1G ports,
     * else HW Linkscan interrupt would be suppressed.
     */
    value = speed <= 1000 ? 0 : 1;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_FAULT_LINK_STATUSr(unit, port, &val));
    soc_XLPORT_FAULT_LINK_STATUSr_field_set(unit, (uint32 *)&val,
                                            REMOTE_FAULTf, &value);
    soc_XLPORT_FAULT_LINK_STATUSr_field_set(unit, (uint32 *)&val,
                                            LOCAL_FAULTf, &value);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_FAULT_LINK_STATUSr(unit, port, &rval));
#ifndef EMULATION_BUILD
    pc = AVNG_PORT_PHY_CTRL(unit, port);
    SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(pc, PhyEvent_Speed, speed));
#endif /* EMULATION_BUILD */
    if (enable) {
        /* Re-enable transmitter and receiver */
        SOC_IF_ERROR_RETURN(mac_xl_enable_set(unit, port, 1));
    }

    si = &SOC_INFO(unit);
    /* assigning proper setting for Native EEE per speed */
    speed = si->port_speed_max[port];
    /* EEE wake timer (unit of uSec.) */
    wake_timer = ((speed >= 10000) ? 7 :
            ((speed >= 1000) ? 17 : 30));
    /* EEE dealy entry timer (unit of uSec.) */
    lpi_timer = ((speed >= 10000) ? 1 :
            ((speed >= 1000) ? 4 : 40));


    soc_XLMAC_EEE_TIMERSr_field_set(unit, (uint32 *)&rval,
                                    EEE_WAKE_TIMERf, &wake_timer);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_EEE_TIMERSr(unit, port, &rval));

    soc_XLMAC_EEE_TIMERSr_field_set(unit, (uint32 *)&rval,
                                    EEE_DELAY_ENTRY_TIMERf, &lpi_timer);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_EEE_TIMERSr(unit, port, &rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_speed_get
 * Purpose:
 *      Get XLMAC speed
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      speed - (OUT) speed in Mb
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_speed_get(int unit, soc_port_t port, uint32 *speed)
{
    uint32 rval[2];
    uint32 speed_mode;
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_MODEr(unit, port, &rval));
    soc_XLMAC_MODEr_field_get(unit, (uint32 *)&rval, SPEED_MODEf, &speed_mode);
    switch (speed_mode) {
    case SOC_XLMAC_SPEED_10:
        *speed = 10;
        break;
    case SOC_XLMAC_SPEED_100:
        *speed = 100;
        break;
    case SOC_XLMAC_SPEED_1000:
        *speed = 1000;
        break;
    case SOC_XLMAC_SPEED_2500:
        *speed = 2500;
        break;
    case SOC_XLMAC_SPEED_10000:
    default:
        *speed = 10000;
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_loopback_set
 * Purpose:
 *      Set a XLMAC into/out-of loopback mode
 * Parameters:
 *      unit - unit #.
 *      port - unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On Xlmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_loopback_set(int unit, soc_port_t port, uint32 lb)
{
    uint32 rval[2];
    uint32 value;

    SOC_IF_ERROR_RETURN
        (mac_xl_control_set(unit, port,
                                   SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE,
                                   lb ? 0 : 1));
    SOC_IF_ERROR_RETURN
        (mac_xl_control_set(unit, port,
                                   SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE,
                                   lb ? 0 : 1));

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &rval));
    value = lb ? 1 : 0;
    soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&rval, LOCAL_LPBKf, &value);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &rval));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_loopback_get
 * Purpose:
 *      Get current XLMAC loopback mode setting.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_loopback_get(int unit, soc_port_t port, uint32 *lb)
{
    uint32 ctrl[2];

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &ctrl));

    soc_XLMAC_CTRLr_field_get(unit, (uint32 *)&ctrl,
                              LOCAL_LPBKf, (uint32 *)&lb);

    return SOC_E_NONE;
}


/*
 * Function:
 *      mac_xl_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_frame_max_set(int unit, soc_port_t port, uint32 size)
{
    uint32 rval[2];
    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        size += 4;
    }
    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_MAX_SIZEr(unit, port, &rval));
    soc_XLMAC_RX_MAX_SIZEr_field_set(unit, (uint32 *)&rval,
                                     RX_MAX_SIZEf, &size);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_MAX_SIZEr(unit, port, &rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_frame_max_get(int unit, soc_port_t port, uint32 *size)
{
    uint32 rval[2];

    SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_MAX_SIZEr(unit, port, &rval));
    soc_XLMAC_RX_MAX_SIZEr_field_get(unit, (uint32 *)&rval, RX_MAX_SIZEf, size);
    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        *size -= 4;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      mac_xl_control_set
 * Purpose:
 *      To configure MAC control properties.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  uint32 value)
{
    uint32 rval[2], copy[2];
    uint32 fval;

    switch (type) {

    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &rval));
        copy[0] = rval[0];
        copy[1] = rval[1];
        fval = (value ? 1 : 0);
        soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&rval, RX_ENf, &fval);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &rval));
        }
        break;

    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &rval));
        copy[0] = rval[0];
        copy[1] = rval[1];
        fval = (value ? 1 : 0);
        soc_XLMAC_CTRLr_field_set(unit, (uint32 *)&rval, TX_ENf, &fval);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_CTRLr(unit, port, &rval));
        }
        break;

    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if ( value > 255) {
            return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(REG_READ_XLMAC_TX_CTRLr(unit, port, &rval));
            if (value >= 8) {
                soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&rval,
                                              THROT_DENOMf, &value);
                fval = 1;
                soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&rval,
                                              THROT_NUMf, &fval);
            } else {
                fval = 0;
                soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&rval,
                                              THROT_DENOMf, &fval);
                soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&rval,
                                              THROT_NUMf, &fval);
            }
            SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_TX_CTRLr(unit, port, &rval));
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PAUSE_CTRLr(unit, port, &rval));
        fval = value ? 0 : 1;
        soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&rval,
                                        RX_PAUSE_ENf, &fval);
        soc_XLMAC_TX_CTRLr_field_set(unit, (uint32 *)&rval,
                                        TX_PAUSE_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PAUSE_CTRLr(unit, port, &rval));

        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_TYPEr(unit, port, &rval));
        soc_XLMAC_PFC_TYPEr_field_set(unit, (uint32 *)&rval,
                                      PFC_ETH_TYPEf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_TYPEr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_OPCODEr(unit, port, &rval));
        soc_XLMAC_PFC_OPCODEr_field_set(unit, (uint32 *)&rval,
                                        PFC_ETH_TYPEf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_OPCODEr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (value != 8) {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_DAr(unit, port, &rval));
        soc_XLMAC_PFC_DAr_field_get(unit, (uint32 *)&rval,
                                    PFC_MACDA_LOf, &fval);
        fval &= 0x00ffffff;
        fval |= (value & 0xff) << 24;
        soc_XLMAC_PFC_DAr_field_set(unit, (uint32 *)&rval,
                                   PFC_MACDA_LOf, &fval);
        fval = value >> 8;
        soc_XLMAC_PFC_DAr_field_set(unit, (uint32 *)&rval,
                                   PFC_MACDA_HIf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_DAr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_DAr(unit, port, &rval));
        soc_XLMAC_PFC_DAr_field_get(unit, (uint32 *)&rval,
                                    PFC_MACDA_LOf, &fval);
        fval &= 0xff000000;
        fval |= value;
        soc_XLMAC_PFC_DAr_field_set(unit, (uint32 *)&rval,
                                     PFC_MACDA_LOf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_DAr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        /* this is always true */
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        fval = (value ? 1 : 0);
        soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&rval, RX_PFC_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        fval = (value ? 1 : 0);
        soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&rval, TX_PFC_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        fval = (value ? 1 : 0);
        soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&rval,
                                      FORCE_PFC_XONf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        fval = (value ? 1 : 0);
        soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&rval,
                                        PFC_STATS_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&rval,
                                       PFC_REFRESH_TIMERf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_set(unit, (uint32 *)&rval,
                                        PFC_XOFF_TIMERf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_LLFC_CTRLr(unit, port, &rval));
        fval = (value ? 1 : 0);
        soc_XLMAC_LLFC_CTRLr_field_set(unit, (uint32 *)&rval,
                                             RX_LLFC_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_LLFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_LLFC_CTRLr(unit, port, &rval));
        fval = (value ? 1 : 0);
        soc_XLMAC_LLFC_CTRLr_field_set(unit, (uint32 *)&rval,
                                       TX_LLFC_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_LLFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_EEE_CTRLr(unit, port, &rval));
        soc_XLMAC_EEE_CTRLr_field_set(unit, (uint32 *)&rval, EEE_ENf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_EEE_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_EEE_TIMERSr(unit, port, &rval));
        soc_XLMAC_EEE_TIMERSr_field_set(unit, (uint32 *)&rval,
                                        EEE_DELAY_ENTRY_TIMERf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_EEE_TIMERSr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_EEE_TIMERSr(unit, port, &rval));
        soc_XLMAC_EEE_TIMERSr_field_set(unit, (uint32 *)&rval,
                                        EEE_WAKE_TIMERf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_EEE_TIMERSr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        fval = value ? 0 : 1;
        soc_XLMAC_RX_LSS_CTRLr_field_set(unit, (uint32 *)&rval,
                                        LOCAL_FAULT_DISABLEf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        fval = value ? 0 : 1;
        soc_XLMAC_RX_LSS_CTRLr_field_set(unit, (uint32 *)&rval,
                                        REMOTE_FAULT_DISABLEf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_FAILOVER_RX_SET:
        break;

    case SOC_MAC_CONTROL_EGRESS_DRAIN:
        SOC_IF_ERROR_RETURN(mac_xl_egress_queue_drain(unit, port));
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_XLMAC_RX_VLAN_TAGr_field_set(unit, (uint32 *)&rval,
                                        OUTER_VLAN_TAGf, &value);
        soc_XLMAC_RX_VLAN_TAGr_field_set(unit, (uint32 *)&rval,
                                        OUTER_VLAN_TAG_ENABLEf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_XLMAC_RX_VLAN_TAGr_field_set(unit, (uint32 *)&rval,
                                        INNER_VLAN_TAGf, &value);
        soc_XLMAC_RX_VLAN_TAGr_field_set(unit, (uint32 *)&rval,
                                        INNER_VLAN_TAG_ENABLEf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_control_get
 * Purpose:
 *      To get current MAC control setting.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  uint32 *value)
{
    int rv;
    uint32 rval[2];
    uint32 fval0, fval1;

    if (value == NULL) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &rval));
        soc_XLMAC_CTRLr_field_get(unit, (uint32 *)&rval, RX_ENf, value);
        break;

    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_CTRLr(unit, port, &rval));
        soc_XLMAC_CTRLr_field_get(unit, (uint32 *)&rval, TX_ENf, value);
        break;

    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_TX_CTRLr(unit, port, &rval));
        soc_XLMAC_TX_CTRLr_field_get(unit, (uint32 *)&rval,
                                      THROT_DENOMf, value);
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        *value = TRUE;
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_TYPEr(unit, port, &rval));
        soc_XLMAC_PFC_TYPEr_field_get(unit, (uint32 *)&rval,
                                      PFC_ETH_TYPEf, value);
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_OPCODEr(unit, port, &rval));
        soc_XLMAC_PFC_OPCODEr_field_get(unit, (uint32 *)&rval,
                                        PFC_OPCODEf, value);
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        *value = 8;
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_DAr(unit, port, &rval));
        soc_XLMAC_PFC_DAr_field_get(unit, (uint32 *)&rval,
                                    PFC_MACDA_LOf, &fval0);
        soc_XLMAC_PFC_DAr_field_get(unit, (uint32 *)&rval,
                                    PFC_MACDA_HIf, &fval1);
        *value = (fval0 >> 24) | (fval1 << 8);
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_DAr(unit, port, &rval));
        soc_XLMAC_PFC_DAr_field_get(unit, (uint32 *)&rval,
                                       PFC_MACDA_LOf, value);
        *value &= 0x00ffffff;
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        *value = TRUE;
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_get(unit, (uint32 *)&rval, RX_PFC_ENf, value);
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_get(unit, (uint32 *)&rval, TX_PFC_ENf, value);
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_get(unit, (uint32 *)&rval,
                                      FORCE_PFC_XONf, value);
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_get(unit, (uint32 *)&rval,
                                      PFC_STATS_ENf, value);
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_get(unit, (uint32 *)&rval,
                                      PFC_REFRESH_TIMERf, value);
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        soc_XLMAC_PFC_CTRLr_field_get(unit, (uint32 *)&rval,
                                      PFC_XOFF_TIMERf, value);
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_LLFC_CTRLr(unit, port, &rval));
        soc_XLMAC_LLFC_CTRLr_field_get(unit, (uint32 *)&rval,
                                       RX_LLFC_ENf, value);
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_LLFC_CTRLr(unit, port, &rval));
        soc_XLMAC_LLFC_CTRLr_field_get(unit, (uint32 *)&rval,
                                       TX_LLFC_ENf, value);
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_EEE_CTRLr(unit, port, &rval));
        soc_XLMAC_EEE_CTRLr_field_get(unit, (uint32 *)&rval,
                                        EEE_ENf, value);
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_EEE_TIMERSr(unit, port, &rval));
        soc_XLMAC_EEE_TIMERSr_field_get(unit, (uint32 *)&rval,
                                        EEE_DELAY_ENTRY_TIMERf, value);
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_EEE_TIMERSr(unit, port, &rval));
        soc_XLMAC_EEE_TIMERSr_field_get(unit, (uint32 *)&rval,
                                       EEE_WAKE_TIMERf, value);
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        soc_XLMAC_RX_LSS_CTRLr_field_get(unit, (uint32 *)&rval,
                                         LOCAL_FAULT_DISABLEf, &fval0);
        *value = fval0 ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        soc_XLMAC_RX_LSS_CTRLr_field_get(unit, (uint32 *)&rval,
                                         LOCAL_FAULT_DISABLEf, &fval0);
        if (fval0 == 1) {
            *value = 0;
        } else {
            SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_STATUSr(unit,
                                                              port, &rval));
            soc_XLMAC_RX_LSS_STATUSr_field_get(unit, (uint32 *)&rval,
                                               LOCAL_FAULT_STATUSf, value);
        }
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        soc_XLMAC_RX_LSS_CTRLr_field_get(unit, (uint32 *)&rval,
                                         REMOTE_FAULT_DISABLEf, &fval0);
        *value = fval0 ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        soc_XLMAC_RX_LSS_CTRLr_field_get(unit, (uint32 *)&rval,
                                         REMOTE_FAULT_DISABLEf, &fval0);
        if (fval0 == 1) {
            *value = 0;
        } else {
            SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_LSS_STATUSr(unit,
                                                              port, &rval));
            soc_XLMAC_RX_LSS_STATUSr_field_get(unit, (uint32 *)&rval,
                                               REMOTE_FAULT_STATUSf, value);
        }
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        soc_XLMAC_RX_VLAN_TAGr_field_get(unit, (uint32 *)&rval,
                                          OUTER_VLAN_TAGf, value);
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN(REG_READ_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        soc_XLMAC_RX_VLAN_TAGr_field_get(unit, (uint32 *)&rval,
                                        INNER_VLAN_TAGf, value);
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    return rv;
}

/* Exported XLMAC driver structure */
mac_driver_t soc_mac_xl = {
    "XLMAC Driver",               /* drv_name */
    mac_xl_init,                  /* md_init  */
    mac_xl_enable_set,            /* md_enable_set */
    mac_xl_enable_get,            /* md_enable_get */
    mac_xl_duplex_set,            /* md_duplex_set */
    mac_xl_duplex_get,            /* md_duplex_get */
    mac_xl_speed_set,             /* md_speed_set */
    mac_xl_speed_get,             /* md_speed_get */
    mac_xl_pause_set,             /* md_pause_set */
    mac_xl_pause_get,             /* md_pause_get */
    mac_xl_loopback_set,          /* md_lb_set */
    mac_xl_loopback_get,          /* md_lb_get */
    mac_xl_frame_max_set,         /* md_frame_max_set */
    mac_xl_frame_max_get          /* md_frame_max_get */
 };


/*
 * *   unimac.c
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * Purpose:
 * *     10/100/1000/2500 Megabit Media Access Controller Driver (unimac)
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
#include "fsal_int/port.h"
#include "sal_get_set_param.h"

/*
 * Forward Declarations
 */
mac_driver_t soc_mac_uni;

#define SOC_UNIMAC_SPEED_10     0x0
#define SOC_UNIMAC_SPEED_100    0x1
#define SOC_UNIMAC_SPEED_1000   0x2
#define SOC_UNIMAC_SPEED_2500   0x3

/* EEE related defination */
#define SOC_UNIMAC_MAX_EEE_SPEED    1000

/*
 * Function:
 *      _mac_uni_drain_cells
 * Purpose:
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number being examined.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */

STATIC int
_mac_uni_drain_cells(int unit, soc_port_t port)
{
    int                 rv = SOC_E_NONE;
    uint32              flush_ctrl;
    uint32              pause_tx, pause_rx;
    uint32              fval = 1;

    /* Disable pause function */
    SOC_IF_ERROR_RETURN
        (soc_mac_uni.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_uni.md_pause_set(unit, port, 0, 0));

    /* Drop out all packets in TX FIFO without egressing any packets */
    SOC_IF_ERROR_RETURN(REG_READ_UMAC_FLUSH_CONTROLr(unit, port, &flush_ctrl));
    soc_UMAC_FLUSH_CONTROLr_field_set(unit, &flush_ctrl, FLUSHf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_FLUSH_CONTROLr(unit, port, &flush_ctrl));


    /* Soft-reset is recommended here.
     * SOC_IF_ERROR_RETURN
     *     (mac_uni_control_set(unit, port, SOC_MAC_CONTROL_SW_RESET, TRUE));
     * SOC_IF_ERROR_RETURN
     *     (mac_uni_control_set(unit, port, SOC_MAC_CONTROL_SW_RESET, FALSE));
     */

    /* Bring the TxFifo out of flush */
    fval = 0;
    soc_UMAC_FLUSH_CONTROLr_field_set(unit, &flush_ctrl, FLUSHf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_FLUSH_CONTROLr(unit, port, &flush_ctrl));

    /* Restore original pause configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_uni.md_pause_set(unit, port, pause_tx, pause_rx));

    return rv;
}

/*
 * Function:
 *      _mac_uni_sw_reset
 * Purpose:
 *      Do unimac soft reset
 */
STATIC int
_mac_uni_sw_reset(int unit, soc_port_t port, int reset_assert)
{
    uint32 command_config;
    int reset_sleep_usec;
    reset_sleep_usec = 2;  /* choose max delay */
    uint32  fval = 0;

    SOC_IF_ERROR_RETURN
        (REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    if (reset_assert) {
        /* SIDE EFFECT: TX and RX are enabled when SW_RESET is set. */
        /* Assert SW_RESET */
        fval = 1;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           SW_RESETf, &fval);
    } else {
        /* Deassert SW_RESET */
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           SW_RESETf, &fval);
    }

    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    sal_usleep(reset_sleep_usec);

    return SOC_E_NONE;
}
/*
 * Function:
 *      mac_uni_init
 * Purpose:
 *      Initialize UniMAC into a known good state.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port # on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The initialization speed/duplex is arbitrary and must be
 *      updated by linkscan before enabling the MAC.
 */

STATIC  int
mac_uni_init(int unit, soc_port_t port)
{
    uint32              command_config, ocommand_config;
    uint32              rval32, speed;
    int                 frame_max;
    uint32              fval = 1;
    char                mac_string[20];
    uint32_t            mac_addr[6];
    int                 rv;

    frame_max = JUMBO_MAXSZ;

    soc_mac_uni.md_frame_max_set(unit, port, frame_max);
    /* First put the MAC in reset and sleep */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));

    /* Do the initialization */
    SOC_IF_ERROR_RETURN
        (REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;
    fval = 0;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config, TX_ENAf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config, RX_ENAf, &fval);
    speed = SOC_UNIMAC_SPEED_1000;
#if !defined(CONFIG_1000X_1000) &&  !defined(CONFIG_QSGMII) && (CONFIG_SGMII_SPEED == 2500)
    if ((port == 8) || (port == 9)) {
        speed = SOC_UNIMAC_SPEED_2500;
    }
#endif
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                       ETH_SPEEDf, &speed);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                       PAUSE_FWDf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, TX_ADDR_INSf, &fval);
    fval = 1;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                       PROMIS_ENf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, SW_OVERRIDE_TXf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, SW_OVERRIDE_RXf, &fval);
    /* Discard packets with CRC errors */
    fval = 1;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, RX_ERR_DISCf, &fval);
    if (port >= 10) { /* include all unimac ports 0 - 9 */
        fval = 0;
    } else {
        fval = 1;
    }
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, ENA_EXT_CONFIGf, &fval);
    fval = 1;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, NO_LGTH_CHECKf, &fval);
    fval = 0;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                     &ocommand_config, IGNORE_TX_PAUSEf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                      PAUSE_IGNOREf, &fval);
    if ((port == 8) || (port == 9)) {
        fval = 0;
    } else {
        fval = 1;
    }
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config, CRC_FWDf, &fval);
#if 0
    ocommand_config = 0x11460018;
    if ((port >= 8) || (port == 0)) {
        fval = 0;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit,
                                       &ocommand_config, ENA_EXT_CONFIGf, &fval);
    }
#endif    
    if (ocommand_config != command_config) {
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port,
                                                           &ocommand_config));
    }
    /* Bring the UniMAC out of reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, FALSE));
    rval32 = 0;
    fval = 1;
    soc_UMAC_PAUSE_CONTROLr_field_set(unit, &rval32, ENABLEf, &fval);
    fval = 0x08000;
    soc_UMAC_PAUSE_CONTROLr_field_set(unit, &rval32, VALUEf, &fval);

    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_PAUSE_CONTROLr(unit, port, &rval32));

    if (port < CBX_PORT_GPHY_MAX) { 
        rval32 = 0x5000;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_PAUSE_QUANTr(unit, port, &rval32));
    }

    rval32 = 0xc0000001;  /* PFC_REFRESH_EN=1 and PFC_REFRESH_TIMER=0xc000  */
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_REFRESH_CTRLr(unit, port, &rval32));

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval32));
    fval = 1;
    soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval32,
                                          PFC_TX_ENBLf, &fval);
    soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval32,
                                          PFC_RX_ENBLf, &fval);
    soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval32,
                                          PFC_STATS_ENf, &fval);
    soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval32,
                                          RX_PASS_PFC_FRMf, &fval);
    soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval32,
                                          FORCE_PFC_XONf, &fval);
    rval32 = 0xFF;
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &rval32));
    SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval32));
    fval = 1;

    fval = 0x101;
    soc_UMAC_MAC_PFC_OPCODEr_field_set(unit, &rval32,
                                           PFC_OPCODEf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_OPCODEr(unit, port, &rval32));

    rval32 = 12;
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_TX_IPG_LENGTHr(unit, port, &rval32));

    /* assigning proper setting for EEE feature :
     * Note :
     *  - GE speed force assigned for timer setting
     */
    rval32 = 125;
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_UMAC_EEE_REF_COUNTr(unit, port, &rval32));

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_FLUSH_CONTROLr(unit, port, &rval32));
    fval = 1;
    soc_UMAC_FLUSH_CONTROLr_field_set(unit, &rval32, FLUSHf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_FLUSH_CONTROLr(unit, port, &rval32));

    /* Enable drop for CRC error in gport_rsv_mask[4] */
    /* Enable drop for packet len greated than Max len set, gport_rsv_mask[6] */
    rval32 = 0x50;
    SOC_IF_ERROR_RETURN(REG_WRITE_STAT_R_GPORT_RSVr(unit, port, &rval32));

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
            rval32 = ((uint8_t)mac_addr[0] << 24) | ((uint8_t)mac_addr[1] << 16) |
                     ((uint8_t)mac_addr[2] << 8) | (uint8_t)mac_addr[3] ;
            SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_0r(unit, port, &rval32));

            rval32 = ((uint8_t)mac_addr[4] << 8) | (uint8_t)mac_addr[5] ;
            SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_1r(unit, port, &rval32));
            rv = SOC_E_NONE;
        }
    }
    if (rv != SOC_E_NONE){
        /* Add SMAC 02:00:00:00:00:00 ;  Only Global bit set
             * SMAC_HI = 0x02000000 */
        rval32 = 0x02000000;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_0r(unit, port, &rval32));
        rval32 = 0x00000000;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_1r(unit, port, &rval32));
    }

    /* Enable Half duplex Back pressure - Only applicable for ports 0 to 7 */
    if (port < 8) {
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_IPG_HD_BKP_CNTLr(unit, port, &rval32));
        fval = 1;
        SOC_IF_ERROR_RETURN(soc_UMAC_IPG_HD_BKP_CNTLr_field_set(unit, &rval32, HD_FC_ENAf, &fval));
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_IPG_HD_BKP_CNTLr(unit, port, &rval32));
    }
    /* Enable PFC */
    SOC_IF_ERROR_RETURN(
        REG_READ_STAT_MAC_CONTROLr(unit, port,  &rval32));
    fval = 0;
    SOC_IF_ERROR_RETURN(soc_STAT_MAC_CONTROLr_field_set(unit, &rval32,
                                                        PPP_DISABLE_STRAPf, &fval));
    SOC_IF_ERROR_RETURN(
        REG_WRITE_STAT_MAC_CONTROLr(unit, port, &rval32));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_enable_set(int unit, soc_port_t port, uint32 enable)
{
    uint32              command_config, tx_ena, rx_ena, pfc_ctrl ;
    uint32              fval = 0;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                                        &command_config, TX_ENAf, &tx_ena));
    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                                        &command_config, RX_ENAf, &rx_ena));

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));


    /* de-assert RX_ENA and TX_ENA */
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config, SW_RESETf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config, TX_ENAf, &fval);
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config, RX_ENAf, &fval);

    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    sal_usleep(2);

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, FALSE));

    if (!enable) {
        SOC_IF_ERROR_RETURN(_mac_uni_drain_cells(unit, port));
        SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));

    } else {
         /* Flush MMU XOFF state with toggle bit */
         fval = 1;
         SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &pfc_ctrl));
         soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &pfc_ctrl,
                                          FORCE_PFC_XONf, &fval);
         SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &pfc_ctrl));
         fval = 0;
         soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &pfc_ctrl,
                                          FORCE_PFC_XONf, &fval);
         SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &pfc_ctrl));

        /* if it is to enable, assert RX_ENA and TX_ENA */
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           SW_RESETf, &fval);
        fval = 1;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                            TX_ENAf, &fval);
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                            RX_ENAf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port,
                                                           &command_config));
        sal_usleep(2);

    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_enable_get
 * Purpose:
 *      Get UniMAC enable state
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_enable_get(int unit, soc_port_t port, uint32 *enable)
{
    uint32        command_config;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                                  &command_config, RX_ENAf, (uint32 *)enable));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_duplex_set
 * Purpose:
 *      Set UniMAC in the specified duplex mode.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Set duplex.
 */

STATIC int
mac_uni_duplex_set(int unit, soc_port_t port, uint32 duplex)
{
    uint32     command_config, ocommand_config;
    uint32     speed;
#ifndef EMULATION_BUILD
    phy_ctrl_t *pc;
#endif
    uint32     fval = 1;

    SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));

    if (speed >= 1000) {
        /*
         * program the bit full-duplex for speed 1000 or 2500Mbps.
         */
        duplex = 1;
    }

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                       HD_ENAf, (uint32 *)&duplex);

    if (command_config == ocommand_config) {
        return SOC_E_NONE;
    }

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));

    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                       SW_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &ocommand_config));

#ifndef EMULATION_BUILD
    /*
     * Notify internal PHY driver of duplex change in case it is being
     * used as pass-through to an external PHY.
     */
    pc = AVNG_PORT_PHY_CTRL(unit, port);
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(pc, PhyEvent_Duplex, duplex));
#endif /*EMULATION_BUILD */
    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, FALSE));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_duplex_get
 * Purpose:
 *      Get UniMAC duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_duplex_get(int unit, soc_port_t port, uint32 *duplex)
{
    uint32    command_config;
    uint32    speed;

    SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));

    if ((1000 == speed) || (2500 == speed)) {
        *duplex = TRUE;
    } else {
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port,
                                                          &command_config));
        SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                                   &command_config, HD_ENAf, (uint32 *)duplex));

        if (TRUE == *duplex) {
            *duplex = FALSE;
        } else {
            *duplex = TRUE;
        }

    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_speed_set
 * Purpose:
 *      Set UniMAC in the specified speed.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number on unit.
 *      speed - 10,100,1000, 2500 for speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 */

STATIC int
mac_uni_speed_set(int unit, soc_port_t port, uint32 speed)
{
    uint32  command_config;
    uint32  speed_select;
    uint32  cur_speed;
    uint32  fval = 1;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                                     &command_config, ETH_SPEEDf, &cur_speed));

    switch (speed) {
    case 10:
        speed_select = SOC_UNIMAC_SPEED_10;
    break;
    /* support non-standard speed in Broadreach mode */
    case 20:
    case 25:
    case 33:
    case 50:
    /* fall through to case 100 */
    case 100:
        speed_select = SOC_UNIMAC_SPEED_100;
    break;
    case 1000:
        speed_select = SOC_UNIMAC_SPEED_1000;
        break;
    case 2500:
        speed_select = SOC_UNIMAC_SPEED_2500;
        break;
    case 0:
        return (SOC_E_NONE);              /* Support NULL PHY */
    default:
        return (SOC_E_CONFIG);
    }

    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                       ETH_SPEEDf, &speed_select);

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));

    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                       SW_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, FALSE));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_uni_speed_get
 * Purpose:
 *      Get GE MAC speed
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number on unit.
 *      speed - (OUT) speed in Mb (10/100/1000)
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
mac_uni_speed_get(int unit, soc_port_t port, uint32 *speed)
{
    uint32         mac_mode, mac_speed;
    int            rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_MODEr(unit, port, &mac_mode));

    SOC_IF_ERROR_RETURN(soc_UMAC_MAC_MODEr_field_get(unit,
                                     &mac_mode, MAC_SPEEDf, &mac_speed));

    switch(mac_speed) {
    case SOC_UNIMAC_SPEED_10:
    *speed = 10;
    break;
    case SOC_UNIMAC_SPEED_100:
    *speed = 100;
    break;
    case SOC_UNIMAC_SPEED_1000:
    *speed = 1000;
    break;
    case SOC_UNIMAC_SPEED_2500:
    *speed = 2500;
    break;
    default:
        rv = SOC_E_INTERNAL;
    }
    return(rv);
}

/*
 * Function:
 *      mac_uni_pause_set
 * Purpose:
 *      Configure UniMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port # on unit.
 *      pause_tx - Boolean: transmit pause, or -1 (don't change)
 *      pause_rx - Boolean: receive pause, or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_uni_pause_set(int unit, soc_port_t port, uint32 pause_tx, uint32 pause_rx)
{
    uint32         command_config, ocommand_config;
    uint32         fval = 0;

    if ((pause_tx == (uint32)-1) && (pause_rx == (uint32)-1)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    if (pause_rx != (uint32)-1) {
        fval = pause_rx ? 0 : 1;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           PAUSE_IGNOREf, &fval);
    }

    if (pause_tx != (uint32)-1) {
        fval = pause_tx ? 0 : 1;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           IGNORE_TX_PAUSEf, &fval);
    }

    if (command_config == ocommand_config) {
        return SOC_E_NONE;
    }

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));
    fval = 1;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                       SW_RESETf, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    /* Add 2usec delay before deasserting SW_RESET */
    sal_usleep(2);

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, FALSE));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_pause_get
 * Purpose:
 *      Return current GE MAC transmit/receive pause frame configuration.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port # on unit.
 *      pause_tx - (OUT) TRUE is TX pause enabled.
 *      pause_rx - (OUT) TRUE if obey RX pause enabled.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_pause_get(int unit, soc_port_t port, uint32 *pause_tx, uint32 *pause_rx)
{
    uint32         command_config;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                          &command_config, PAUSE_IGNOREf, (uint32 *)pause_rx));
    if (*pause_rx > 0) {
        *pause_rx = FALSE;
    } else {
        *pause_rx = TRUE;
    }
    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                        &command_config, IGNORE_TX_PAUSEf, (uint32 *)pause_tx));
    if (*pause_tx > 0) {
        *pause_tx = FALSE;
    } else {
        *pause_tx = TRUE;
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      mac_uni_loopback_set
 * Purpose:
 *      Set GE MAC into/out-of loopback mode
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_loopback_set(int unit, soc_port_t port, uint32 loopback)
{
    uint32      command_config, ocommand_config;
    uint32      fval = 0;
    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, TRUE));
    fval = loopback ? 1 : 0;
    soc_UMAC_COMMAND_CONFIGr_field_set(unit, &ocommand_config,
                                       LOOP_ENAf, &fval);

    if (command_config != ocommand_config) {
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port,
                                                           &ocommand_config));
    }

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(_mac_uni_sw_reset(unit, port, FALSE));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_loopback_get
 * Purpose:
 *      Get current GE MAC loopback mode setting.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port # on unit
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_loopback_get(int unit, soc_port_t port, uint32 *loopback)
{
    uint32         command_config;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                              &command_config, LOOP_ENAf, (uint32 *)loopback));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      CBX_E_XXX
 */

STATIC int
mac_uni_frame_max_set(int unit, soc_port_t port, uint32 size)
{
    uint32 command_config, rx_ena;
    uint32 speed = 0;
    uint32 fval = 0;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &command_config));

    SOC_IF_ERROR_RETURN(soc_UMAC_COMMAND_CONFIGr_field_get(unit,
                                  &command_config, RX_ENAf, &rx_ena));
    /* If Rx is enabled then disable RX */
    if (rx_ena) {
        /* Disable RX */
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           RX_ENAf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port,
                                                           &command_config));

        /* Wait for maximum frame receiption time(for 16K) based on speed */
        SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));
        switch (speed) {
        case 2500:
            sal_usleep(55);
            break;
        case 1000:
            sal_usleep(131);
            break;
        case 100:
            sal_usleep(1310);
            break;
        case 10:
            sal_usleep(13100);
            break;
        default:
            break;
        }
    }

    SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_FRM_LENGTHr(unit, port, &size));

    /* if Rx was enabled before, restore it */
    if (rx_ena) {
        /* Enable RX */
        fval = 1;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &command_config,
                                           RX_ENAf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port,
                                                           &command_config));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      CBX_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size
 *      might be slightly higher.
 */
STATIC int
mac_uni_frame_max_get(int unit, soc_port_t port, uint32 *size)
{
    uint32  frame_length;

    SOC_IF_ERROR_RETURN(REG_READ_UMAC_FRM_LENGTHr(unit, port, &frame_length));

    *size = (int) frame_length;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_control_set
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
mac_uni_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                    uint32 value)
{
    uint32 rval;
#ifdef MAC_UNI
    uint32 fval, copy;
#endif /* MAC_UNI */

    switch(type) {
#ifdef MAC_UNI
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN
            (REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &rval));
        copy = rval;
        fval = value ? 1 : 0;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &rval, RX_ENAf, &fval);

        if (copy != rval) {
            SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &rval));
        }
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN
            (REG_READ_UMAC_COMMAND_CONFIGr(unit, port, &rval));
        copy = rval;
        fval = value ? 1 : 0;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &rval, TX_ENAf, &fval);
        if (copy != rval) {
            SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_COMMAND_CONFIGr(unit, port, &rval));
        }
        break;
    case SOC_MAC_CONTROL_SW_RESET:
    case SOC_MAC_CONTROL_DISABLE_PHY:
        return _mac_uni_sw_reset(unit, port, value);

    case SOC_MAC_CONTROL_PFC_TYPE:
        rval = 0;
        soc_UMAC_MAC_PFC_TYPEr_field_set(unit, &rval,
                                         PFC_ETH_TYPEf, (uint32 *)&value);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_TYPEr(unit, port, &value));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        rval = 0;
        soc_UMAC_MAC_PFC_OPCODEr_field_set(unit, &rval,
                                           PFC_OPCODEf, (uint32 *)&value);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_OPCODEr(unit, port, &value));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_DA_0r(unit, port, &rval));
        rval &= 0x00ffffff;
        rval |= (value & 0x0000ff) << 24;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_DA_0r(unit, port, &rval));

        rval = ((value >> 8) & 0xffff);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_DA_1r(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_DA_0r(unit, port, &rval));
        rval &= 0xff000000;
        rval |= value & 0x00ffffff;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_DA_0r(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval,
                                           RX_PASS_PFC_FRMf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_UMAC_COMMAND_CONFIGr_field_set(unit, &rval,
                                           PFC_RX_ENBLf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval,
                                           PFC_TX_ENBLf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval,
                                           FORCE_PFC_XONf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        fval = value ? 1 : 0;
        soc_UMAC_MAC_PFC_CTRLr_field_set(unit, &rval,
                                           PFC_STATS_ENf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_MAC_PFC_REFRESH_CTRLr(unit, port,
                                                         &rval));
        soc_UMAC_MAC_PFC_REFRESH_CTRLr_field_set(unit, &rval,
                                           PFC_REFRESH_TIMERf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MAC_PFC_REFRESH_CTRLr(unit, port, &rval));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        value &= 0xffff;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_PFC_XOFF_TIMERr(unit, port, &value));
        break;
#endif /* MAC_UNI */

    case SOC_MAC_CONTROL_EEE_ENABLE:
        SOC_IF_ERROR_RETURN(REG_READ_UMAC_UMAC_EEE_CTRLr(unit, port,
                                                         &rval));
        soc_UMAC_UMAC_EEE_CTRLr_field_set(unit, &rval,
                                           EEE_ENf, &value);
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_UMAC_EEE_CTRLr(unit, port, &rval));
        break;

#ifdef MAC_UNI
    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MII_EEE_DELAY_ENTRY_TIMERr(unit, port,
                                                                     &value));
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_GMII_EEE_DELAY_ENTRY_TIMERr(unit, port,
                                                                     &value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        value &= 0xffff;
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_MII_EEE_WAKE_TIMERr(unit, port, &value));
        SOC_IF_ERROR_RETURN(REG_WRITE_UMAC_GMII_EEE_WAKE_TIMERr(unit, port, &value));
        break;
#endif /* MAC_UNI */

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/* Exported UniMAC driver structure */
mac_driver_t soc_mac_uni = {
    "UniMAC Driver",                 /* drv_name */
    mac_uni_init,                    /* md_init  */
    mac_uni_enable_set,              /* md_enable_set */
    mac_uni_enable_get,              /* md_enable_get */
    mac_uni_duplex_set,              /* md_duplex_set */
    mac_uni_duplex_get,              /* md_duplex_get */
    mac_uni_speed_set,               /* md_speed_set */
    mac_uni_speed_get,               /* md_speed_get */
    mac_uni_pause_set,               /* md_pause_set */
    mac_uni_pause_get,               /* md_pause_get */
    mac_uni_loopback_set,            /* md_lb_set */
    mac_uni_loopback_get,            /* md_lb_get */
    mac_uni_frame_max_set,           /* md_frame_max_set */
    mac_uni_frame_max_get,           /* md_frame_max_get */
    mac_uni_control_set              /* md_control_set */
};

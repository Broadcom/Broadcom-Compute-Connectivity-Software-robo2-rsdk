/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     ptp.c
 * * Purpose:
 * *     Robo2 precision time protocol module
 * *
 * * Note:
 * * FSAL APIs to initialize 1588 PTP hardware interface.
 * *
 * */

#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal/l2.h>
#include <fsal/ptp.h>
#include <fsal/vlan.h>
#include <fsal_int/port.h>
#include <fsal_int/slic.h>
#include <fsal_int/lin.h>
#include <fsal_int/encap.h>
#include <fsal_int/mcast.h>
#include <fsal_int/vlan.h>
#include <fsal_int/trap.h>
#include <fsal_int/extender.h>
#include <soc/robo2/common/tables.h>
#include <sal_alloc.h>

extern int cbxi_slicid_map(int unit, cbxi_slic_rule_id_t rule_number,
                           cbxi_slicid_t slicid, cbxi_slic_trap_t slic_trap);
extern int cbxi_port_egress_info_set(int unit, int port, int lpgid,
                                     cbx_port_params_t *port_params );
/**************************************************************************
 *                    PTP FSAL API IMPLEMENTATION                         *
 **************************************************************************/
/* LINs */
typedef struct cbxi_ptp_lin_id_e {
    int eth_linid;
    int udp_linid;
} cbxi_ptp_lin_id_t;

cbxi_ptp_lin_id_t ptp_lin_id[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT];

/* MAC address of Ethernet PTP messages */
uint8_t cbxi_ptp_eth_mac_addr[6] = {0x1, 0x1b, 0x19, 0x0, 0x0, 0x0};
/* MAC address of Ethernet PTP peer messages */
uint8_t cbxi_ptp_eth_p2p_mac_addr[6] = {0x1, 0x80, 0xc2, 0x0, 0x0, 0xe};
/* MAC address of UDP PTP messages */
uint8_t cbxi_ptp_udp_mac_addr[6] = {0x1, 0x0, 0x5e, 0x0, 0x1, 0x81};
/* MAC address of UDP PTP peer messages */
uint8_t cbxi_ptp_udp_p2p_mac_addr[6] = {0x1, 0x0, 0x5e, 0x0, 0x0, 0x6b};

/* PTP clock mode */
cbx_ptp_clk_mode_t ptp_clock_mode = cbxPtpClkTwoStep;
/* PTP enabled port bitmap */
pbmp_t ptp_enable_pbmp[CBX_MAX_UNITS];

/* Time stamp FIFO to store the time stamp information per port */
CBX_TS_FIFO_t ts_fifo[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT];

/* Multicast id  for the ethernet PTP */
cbx_mcastid_t ptp_eth_mcastid;
/* Multicast id  for the UDP PTP */
cbx_mcastid_t ptp_udp_mcastid;

/**
 * Initializes the PTP subsystem
 * - setup of Precision Time Protocol module
 *
 * @param ptp_params   (IN)  PTP configuration parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
void cb_convert_pmap_to_plist(uint32_t pmap,uint32_t *plist,uint32_t *no_ports) {
    uint32_t i;
    uint32_t N=0;
    for(i=0;i<16;i++) {
        if((pmap >> i) & 0x1) {
            *(plist)=i;
            plist ++;
            *(no_ports) = ++N;
        }
    }
}

/**
 * Initialize PTP: Private function
 * Use a dedicated VSI # 4095 to create a multi cast group
 * containing as many LINs as there are physical ports.
 *
 * @param ptp_params   (IN)  PTP parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
static int cbxi_ptp_init(int unit)
{
    uint32_t reg_data;

    reg_data = 0x0;
    CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_TLV_CONTROLr(unit, &reg_data));
    CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_TS_MACC_TLV_CONTROLr(unit, &reg_data));

    /*************** Create MCAST, DLI and VSI ********************* */
    cbx_mcast_params_t mcast_params;
    cbx_vlan_params_t vlan_params;
    cbx_vlanid_t vsiid;

    /* Create a dedicated Multicast group for Flooding PTP messages */
    mcast_params.lilt_mode = cbxMcastLiltModeArray;
    CBX_IF_ERROR_RETURN(cbx_mcast_create(&mcast_params, &ptp_eth_mcastid));

    /* Create VSI to flood on all DLIs */
    cbx_vlan_params_t_init(&vlan_params);
    vlan_params.vlan = CBXI_PTP_VSI;
    CBX_IF_ERROR_RETURN(cbx_vlan_create(&vlan_params, &vsiid));

    /* Create a dedicated Multicast group for Flooding PTP messages */
    mcast_params.lilt_mode = cbxMcastLiltModeArray;
    CBX_IF_ERROR_RETURN(cbx_mcast_create(&mcast_params, &ptp_udp_mcastid));
    /* Disable PTP message correction updates and timestamp updates
     * on General messages */
    reg_data = 0xfff0;
    REG_WRITE_CB_EPP_DISABLE_PTP_MSG_TYPEr(0, &reg_data);

    return CBX_E_NONE;
}

/**
 * Initialize PTP: Public API to Initialize PTP functionality
 *
 * @param ptp_params   (IN)  PTP parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_init(void)
{
    int result;
    result = cbxi_ptp_init(0);
    return result;
}

/**
 * Detach PTP previously initialized by cbx_ptp_init()
 * - shut down the PTP subsystem
 *
 * @param ptp_params   (IN)  PTP parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_ptp_detach ( cbx_ptp_params_t *ptp_params ) {
    return CBX_E_NONE;
}

/**
 * PTP register
 * This routine is used to register a PTP callback routine.
 * If multiple handlers are registered, they are invoked on
 * in the same order as they were registered.
 *
 * @param ptp_info (IN)  PTP callback information
 * @param cb       (IN)  Callback function pointer
 *                       see: cbx_ptp_callback_t
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_ptp_register ( cbx_ptp_info_t      ptp_info,
                       cbx_ptp_callback_t  cb ) {
    return CBX_E_NONE;
}

/**
 * PTP unregister
 * This routine is used to unregister a PTP callback routine.
 *
 * @param cb           (IN)  PTP callback routine to unregister
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_unregister ( cbx_ptp_callback_t  cb ) {
    return CBX_E_NONE;
}

/**
 * cbxi_ptp_port_admin_set
 * Set up the slic rule mappings for ethernet and UDP packets.
 * For one step clock, enables the correction field updates,
 * and does lin encap settings. For two step clock sets up
 * the PET table for the timestamping of the egress PTP packets.
 *
 * @param unit      (IN)  Unit number
 * @param portid_in (IN)  Port identifier
 * @param port      (IN)  Port number
 * @param lpg_id    (IN)  Local PG identifier
 * @param admin     (IN)  Admin value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbxi_ptp_port_admin_set(uint8 unit, cbx_portid_t portid_in,
                           cbx_port_t port, cbxi_pgid_t lpg_id, uint8_t admin) {

    uint32_t umac_control;
    uint32_t umac_ts_adjust;
    uint32_t xmac_mode;
    cbx_portid_t portid;
    pbmp_t pg_map;
    int linid;
    cbxi_encap_record_t encap_record;
    cbx_port_params_t port_params;

    if ((admin == CBX_PTP_PORT_ENABLE)&&(ptp_clock_mode == cbxPtpClkOneStep)) {
        if (IS_XL_PORT(unit, port)) {
            REG_READ_XLPORT_MODE_REGr(unit, &xmac_mode);
            xmac_mode = xmac_mode | (0x1 << 6);
            REG_WRITE_XLPORT_MODE_REGr(unit, &xmac_mode);
        } else {
            REG_READ_STAT_MAC_CONTROLr(unit, port, &umac_control);
            umac_control = umac_control | (0x1 << 2);
            REG_WRITE_STAT_MAC_CONTROLr(unit,port, &umac_control);

            REG_READ_UMAC_UMAC_TIMESTAMP_ADJUSTr(unit, port, &umac_ts_adjust);
            umac_ts_adjust = umac_ts_adjust | (0x1 << 9);
            umac_ts_adjust = umac_ts_adjust | (0x1 << 10);
            REG_WRITE_UMAC_UMAC_TIMESTAMP_ADJUSTr(unit, port, &umac_ts_adjust);
        }
        /* Prepare encap record for Ethernet PTP
         * 1. CBX_EGR_DIRECTIVE_UPDATE_PTP_CORRECTION_FIELD (0x0F)
         * 2a. CBX_EGR_DIRECTIVE_EGRESS_TIMESTAMP_CONTROL_TLV (0x10)
         * 2b. Enable OSTS: 0x40
         */
        sal_memset(&encap_record, 0, sizeof(cbxi_encap_record_t));
        encap_record.data[0] = 0x0F;
        encap_record.data[1] = 0x10;
        encap_record.data[2] = 0x40;
        encap_record.len     = 3;

        CBX_EPP_INSERT_EOR(&encap_record);

        /* get a free LIN value and configure */
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&linid));

        /* Configure Ingress Part. Assign this LIN to the default SLI
         * for the SLIC id. The same LIN is used for both source and
         * destination physical port, so that hardware may be able to
         * source port knock out in this case of flooding.
         */
        CBX_IF_ERROR_RETURN(cbxi_ptp_lin_source_set(unit, linid, port,
                                                    CBXI_PTP_VSI, 1));

        /* Configure the Egress Part */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, linid, port));
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(port, linid, 0, &encap_record,
                                               CBXI_ENCAP_AVG_SYS));
        /* Add the DLI to Multicast group */
        portid = 0;
        CBX_PORTID_VLAN_PORT_ID_SET(portid, linid);
        CBX_IF_ERROR_RETURN(cbx_mcast_member_add(ptp_eth_mcastid, portid));
        ptp_lin_id[unit][port].eth_linid = linid;

        /* Prepare encap record UDP PTP
         * 1. CBX_EGR_DIRECTIVE_UPDATE_PTP_CORRECTION_FIELD (0x0F)
         * 2a. CBX_EGR_DIRECTIVE_EGRESS_TIMESTAMP_CONTROL_TLV (0x10)
         * 2b. Enable OSTS: 0x60
         */
        sal_memset(&encap_record, 0, sizeof(cbxi_encap_record_t));
        encap_record.data[0] = 0x0F;
        encap_record.data[1] = 0x10;
        encap_record.data[2] = 0x60;
        encap_record.len     = 3;

        CBX_EPP_INSERT_EOR(&encap_record);

        /* get a free LIN value and configure */
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&linid));

        /* Configure Ingress Part. Assign this LIN to the default SLI
         * for the SLIC id. The same LIN is used for both source and
         * destination physical port, so that hardware may be able to
         * source port knock out in this case of flooding.
         */
        CBX_IF_ERROR_RETURN(cbxi_ptp_lin_source_set(unit, linid, port,
                                                    CBXI_PTP_VSI, 2));

        /* Configure the Egress Part */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, linid, port));
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(port, linid, 0, &encap_record,
                                               CBXI_ENCAP_AVG_SYS));

        /* Add the DLI to Multicast group */
        portid = 0;
        CBX_PORTID_VLAN_PORT_ID_SET(portid, linid);
        CBX_IF_ERROR_RETURN(cbx_mcast_member_add(ptp_udp_mcastid, portid));
        ptp_lin_id[unit][port].udp_linid = linid;

        /* Add the physical port to VLAN */
        CBX_IF_ERROR_RETURN(cbx_vlan_port_add(CBXI_PTP_VSI,
                                              port, CBX_VLAN_PORT_MODE_TAGGED));
    } else if ((admin == CBX_PTP_PORT_DISABLE) &&
        (ptp_clock_mode == cbxPtpClkOneStep)) {
        portid = 0;
        CBX_PORTID_VLAN_PORT_ID_SET(portid, ptp_lin_id[unit][port].eth_linid);
        CBX_IF_ERROR_RETURN(cbx_mcast_member_remove(ptp_eth_mcastid, portid));
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_release(
                                           ptp_lin_id[unit][port].eth_linid));
        CBX_IF_ERROR_RETURN(cbxi_ptp_lin_source_release(unit, linid, port,
                                                    CBXI_PTP_VSI, 1));
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_release(
                                           ptp_lin_id[unit][port].eth_linid));
        portid = 0;
        CBX_PORTID_VLAN_PORT_ID_SET(portid, ptp_lin_id[unit][port].udp_linid);
        CBX_IF_ERROR_RETURN(cbx_mcast_member_remove(ptp_udp_mcastid, portid));
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_release(
                                           ptp_lin_id[unit][port].udp_linid));
        CBX_IF_ERROR_RETURN(cbxi_ptp_lin_source_release(unit, linid, port,
                                                    CBXI_PTP_VSI, 2));
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_release(
                                           ptp_lin_id[unit][port].udp_linid));
        CBX_IF_ERROR_RETURN(cbx_vlan_port_remove(CBXI_PTP_VSI, port));
    }
    if (admin == CBX_PTP_PORT_ENABLE) {
        /* slic MAP */
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit,
                                   (SLIC_PTP_ETH_P2P_PORT_0 + port), &pg_map));
        CBX_PBMP_PORT_ADD(pg_map, port);
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit,
                                    (SLIC_PTP_ETH_P2P_PORT_0 + port), pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit,
                                (SLIC_PTP_ETH_P2P_PORT_0 + port),
                                (SLICID_PTP_ETH_PP0 + port), SLIC_TRAP_GROUP5));

        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit,
                                   (SLIC_PTP_UDP_P2P_PORT_0 + port), &pg_map));
        CBX_PBMP_PORT_ADD(pg_map, port);
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit,
                                    (SLIC_PTP_UDP_P2P_PORT_0 + port), pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit,
                               (SLIC_PTP_UDP_P2P_PORT_0 + port),
                               (SLICID_PTP_UDP_PP0 + port), SLIC_TRAP_GROUP5));

        CBX_PBMP_PORT_ADD(ptp_enable_pbmp[unit], port);
    } else {
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit,
                                  (SLIC_PTP_ETH_P2P_PORT_0 + port), &pg_map));
        CBX_PBMP_PORT_REMOVE(pg_map, port);
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit,
                                   (SLIC_PTP_ETH_P2P_PORT_0 + port), pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit,
                               (SLIC_PTP_ETH_P2P_PORT_0 + port),
                               (SLICID_PTP_ETH_PP0 + port), SLIC_TRAP_GROUP0));

        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit,
                                   (SLIC_PTP_UDP_P2P_PORT_0 + port), &pg_map));
        CBX_PBMP_PORT_REMOVE(pg_map, port);
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit,
                                    (SLIC_PTP_UDP_P2P_PORT_0 + port), pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit,
                               (SLIC_PTP_UDP_P2P_PORT_0 + port),
                               (SLICID_PTP_UDP_PP0 + port), SLIC_TRAP_GROUP0));
        CBX_PBMP_PORT_REMOVE(ptp_enable_pbmp[unit], port);
    }
    if (ptp_clock_mode == cbxPtpClkTwoStep) {
        CBX_IF_ERROR_RETURN(cbx_port_info_get(portid_in, &port_params));
        CBX_IF_ERROR_RETURN(cbxi_port_egress_info_set(unit, port, lpg_id,
                                                      &port_params));
    }
    return CBX_E_NONE;
}

/**
 * cbx_ptp_port_admin_set
 * Enables/Disables the port PTP configuration. This will configure the
 * port to do PTP correction field updates and timestamping of packets
 *
 * @param portid    (IN)  Port identifier
 * @param admin     (IN)  Admin value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_port_admin_set(cbx_portid_t portid, uint8_t admin) {
    cbx_port_t port;
    cbxi_pgid_t lpgid;
    int unit;

    /* validate the portid passed */
    if(CBX_FAILURE(cbxi_robo2_port_validate(portid, &port, &lpgid, &unit))) {
        return CBX_E_PORT;
    }
    if ((port == CBX_PORT_ICPU) || (port == CBX_PORT_ECPU)) {
        return CBX_E_NONE;
    }
    return (cbxi_ptp_port_admin_set(unit, portid, port, lpgid, admin));
}

/**
 * cbx_ptp_port_admin_get
 * Gets the admin state of port PTP configurtion
 *
 * @param portid    (IN)  Port identifier
 * @param admin     (IN/OUT)  Admin value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_port_admin_get(cbx_portid_t portid, uint8_t *admin) {
    cbx_port_t port;
    cbxi_pgid_t lpgid;
    int unit;

    /* validate the portid passed */
    if(CBX_FAILURE(cbxi_robo2_port_validate(portid, &port, &lpgid, &unit))) {
        return CBX_E_PORT;
    }
    if (CBX_PBMP_MEMBER(ptp_enable_pbmp[unit], port)) {
        *admin = CBX_PTP_PORT_ENABLE;
    } else {
        *admin = CBX_PTP_PORT_DISABLE;
    }
    return CBX_E_NONE;
}

/**
 * cbx_ptp_clock_mode_set
 * Sets the PTP clock mode to either one step or two step.
 *
 * @param clk_mode   (IN) PTP clock mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_clock_mode_set(cbx_ptp_clk_mode_t clk_mode) {
    cbx_l2_entry_t l2_entry;

    if (clk_mode == cbxPtpClkOneStep) {
        sal_memset(&l2_entry, 0, sizeof(cbx_l2_entry_t));
        l2_entry.src_mac[0] = cbxi_ptp_eth_mac_addr[0];
        l2_entry.src_mac[1] = cbxi_ptp_eth_mac_addr[1];
        l2_entry.src_mac[2] = cbxi_ptp_eth_mac_addr[2];
        l2_entry.src_mac[3] = cbxi_ptp_eth_mac_addr[3];
        l2_entry.src_mac[4] = cbxi_ptp_eth_mac_addr[4];
        l2_entry.src_mac[5] = cbxi_ptp_eth_mac_addr[5];

        l2_entry.vlanid = CBXI_PTP_VSI;
        l2_entry.params.flags = CBX_L2_ADDR_STATIC;
        l2_entry.params.dest_type = cbxL2AddrDestMcast;
        l2_entry.params.portid = ptp_eth_mcastid;
        CBX_IF_ERROR_RETURN(cbx_l2_entry_add(&l2_entry));

        sal_memset(&l2_entry, 0, sizeof(cbx_l2_entry_t));
        l2_entry.src_mac[0] = cbxi_ptp_udp_mac_addr[0];
        l2_entry.src_mac[1] = cbxi_ptp_udp_mac_addr[1];
        l2_entry.src_mac[2] = cbxi_ptp_udp_mac_addr[2];
        l2_entry.src_mac[3] = cbxi_ptp_udp_mac_addr[3];
        l2_entry.src_mac[4] = cbxi_ptp_udp_mac_addr[4];
        l2_entry.src_mac[5] = cbxi_ptp_udp_mac_addr[5];

        l2_entry.vlanid = CBXI_PTP_VSI;
        l2_entry.params.flags = CBX_L2_ADDR_STATIC;
        l2_entry.params.dest_type = cbxL2AddrDestMcast;
        l2_entry.params.portid = ptp_udp_mcastid;
        CBX_IF_ERROR_RETURN(cbx_l2_entry_add(&l2_entry));

    }
    ptp_clock_mode = clk_mode;
    return CBX_E_NONE;
}

/**
 * cbx_ptp_clock_mode_get
 * Gets the PTP clock mode.
 *
 * @param clk_mode   (IN) PTP clock mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_clock_mode_get(cbx_ptp_clk_mode_t *clk_mode) {
    *clk_mode = ptp_clock_mode;
    return CBX_E_NONE;
}

/**
 * cbx_ptp_timestamp_get
 * Gets the timestamp and sequence id of egress PTP packet on the port
 *
 * @param portid    (IN)  Port identifier
 * @param timestamp (IN/OUT) timestamp value
 * @param seq_id    (IN/OUT) Sequence id of the packet
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_timestamp_get(cbx_portid_t portid, uint32_t *timestamp,
                          uint32_t *seq_id) {
    cbx_port_t port;
    cbxi_pgid_t lpgid;
    int unit;
    uint64_t xlmac_reg;
    uint32_t unimac_reg;
    uint8_t index;

    /* validate the portid passed */
    if(CBX_FAILURE(cbxi_robo2_port_validate(portid, &port, &lpgid, &unit))) {
        return CBX_E_PORT;
    }
    *timestamp = 0;
    *seq_id = 0;
    if (IS_XL_PORT(unit, port)) {
        for (index = 0; index < CBX_TS_FIFO_DEPTH; index++) {
            REG_READ_XLMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &xlmac_reg);
            if (xlmac_reg != 0) {
                REG_READ_XLMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &xlmac_reg);
                *timestamp = xlmac_reg & 0xffffffff;
                *seq_id = (xlmac_reg & 0xffff00000000) >> 32;
                break;
            } else {
                break;
            }
        }
    } else {
        for (index = 0; index < CBX_TS_FIFO_DEPTH; index++) {
            REG_READ_UMAC_TS_STATUS_CNTRLr(unit, port, &unimac_reg);
            if (unimac_reg != 0x02) {
                REG_READ_UMAC_TX_TS_SEQ_IDr(unit, port, &unimac_reg);
                *seq_id = unimac_reg & 0xffff;
                REG_READ_UMAC_TX_TS_DATAr(unit, port, &unimac_reg);
                *timestamp = unimac_reg;
                break;
            } else {
                break;
            }
        };
    }
    return CBX_E_NONE;
}

/**
 * cbx_ptp_seqid_timestamp_get
 * Gets the timestamp of egress PTP packet for the matching sequence id.
 * This function reads the FIFO entries and checks for the matching
 * sequence id.
 *
 * @param portid    (IN)  Port identifier
 * @param seq_id    (IN) Sequence id of the packet
 * @param timestamp (IN/OUT) timestamp value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_seqid_timestamp_get(cbx_portid_t portid, uint32_t seq_id,
                                uint32_t *timestamp) {
    CBX_TS_FIFO_t *port_fifo;
    CBX_TS_INFO_t *ts_info;
    uint8_t index;
    uint8_t fifo_idx;
    uint32_t pkt_ts;
    uint32_t pkt_seq_id;
    cbx_port_t port;
    cbxi_pgid_t lpgid;
    int unit;
    int rc = CBX_E_NOT_FOUND;

    /* validate the portid passed */
    if(CBX_FAILURE(cbxi_robo2_port_validate(portid, &port, &lpgid, &unit))) {
        return CBX_E_PORT;
    }

    port_fifo = &ts_fifo[unit][port];
    for (index = 0; index < CBX_TS_FIFO_DEPTH; index++) {
        /* Read the time stamp and sequence ID and store in local FIFO */
        rc = cbx_ptp_timestamp_get(portid, &pkt_ts, &pkt_seq_id);
        if (rc != CBX_E_NONE) {
            sal_printf("Time stamp get failed for interface %d\n", port);
        }
        if ((pkt_ts == 0) || (pkt_seq_id == 0)) {
            break;
        } else {
            fifo_idx = port_fifo->cur_ts_idx;
            ts_info = &port_fifo->ts_info[fifo_idx];
            ts_info->nanoseconds = pkt_ts;
            ts_info->seq_id = pkt_seq_id;
            ts_info->is_ts_read = FALSE;

            port_fifo->cur_ts_idx++;
            if (port_fifo->cur_ts_idx >= CBX_TS_FIFO_DEPTH) {
                port_fifo->cur_ts_idx = 0;
            }
        }
    }

    for (index = 0; index < CBX_TS_FIFO_DEPTH; index++) {
        /* This FIFO is circular, read from the oldest to latest update */
        if(port_fifo->cur_ts_idx + index >= CBX_TS_FIFO_DEPTH) {
           ts_info = &port_fifo->ts_info[port_fifo->cur_ts_idx + index - CBX_TS_FIFO_DEPTH];
        } else {
           ts_info = &port_fifo->ts_info[port_fifo->cur_ts_idx + index];
        }
        if ((ts_info->seq_id == seq_id) && !(ts_info->is_ts_read)) {
            *timestamp = ts_info->nanoseconds;
            ts_info->is_ts_read = TRUE;
            rc = CBX_E_NONE;
            break;
        }
    }
    return rc;
}

/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     cable_diag.c
 * * Purpose:
 * *     Run Cable diag on single or all ports
 * *
 * * Note:
 * *  Implementation of Cable diag FSAL APIs.
 * *
 * */


#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal_int/types.h>
#include <fsal_int/port.h>
#include <sal_alloc.h>
#include <fsal/link.h>
#include <fsal/cable_diag.h>
#include <fsal_int/cable_diag.h>
#include <avng_phy.h>

#define CBXI_LINKSCAN_PAUSE_DELAY   100000000; /*Value in usec; 100 seconds */

const char *cdiag_result[] = {
    "Nill",
    "Busy",
    "Invalid",
    "Pass",
    "Fail"
};

const char *fault_display[] = {
    "ECD incomplete   ",
    "OK.Terminated    ",
    "Fault:Open       ",
    "Fault:Intra Short",
    "Fault:Inter Short",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Pair busy        ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
    "Invalid / Resv   ",
};

/*
 * Function:
 *  cbxi_cdiag_print_results
 * Purpose:
 *  Print cable diag result
 */

int cbxi_cdiag_print_results(cbx_port_t port, cbx_port_cable_diag_t *port_res)
{
    int pair_no = 0;

    sal_printf("---------------------------------------------------------\n");
    sal_printf("Port %d Cable diag result... ", port);

    sal_printf(": %s\n", cdiag_result[port_res->port_cdiag_result]);

    if ((port_res->port_cdiag_result != cbxCdiagResultPass) &&
                (port_res->port_cdiag_result != cbxCdiagResultFail)) {
        return CBX_E_NONE;
    }

    for (pair_no = 0; pair_no < 4; pair_no++) {
        sal_printf("\tPair %c   Result : ", ('A' + pair_no));
        sal_printf(" %s;  ", fault_display[port_res->pair_state[pair_no]]);
        sal_printf("Length : %d m;\n", port_res->pair_len[pair_no]);
    }
    sal_printf("---------------------------------------------------------\n\n");
    return CBX_E_NONE;
}

/**
 * Run cable diag on given port.
 *
 * @param portid       (IN)  portid on which Cable diag has to run
 *                           This always represent physical port.
 *
 * @param port_result  (OUT) Port cable diag result
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_cable_diag (
                cbx_portid_t portid, cbx_port_cable_diag_t *port_result)
{
    cbx_link_scan_params_t link_params;
    int            link_interval = 0;
    cbx_port_t     port;
    cbxi_pgid_t    port_lpg;
    uint32_t       reg_val, c_length;
    int            unit = 0;
    uint8_t        timeout = PHY_REG_READ_WRITE_TIMEOUT_COUNT;

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                            portid, &port, &port_lpg, &unit));

    if (port > 7) {
        /* Cable diag is supported only on GPHY ports 0 - 7 */
        return CBX_E_PARAM;
    }

    /* Store current Link scan interval and pause the linkscan by configuring
     * high value of interval time (100 seconds)*/
    cbx_link_scan_enable_get(&link_params);
    link_interval = link_params.usec;
    link_params.usec = CBXI_LINKSCAN_PAUSE_DELAY; /* 100 secs */
    cbx_link_scan_enable_set(&link_params);
    /* Allow linkscan to set a large value of interval: Delay 100ms */
    sal_usleep(100000);

    sal_memset(port_result, 0, sizeof(cbx_port_cable_diag_t));

    /* Enable RDB Access mode */
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                        unit, port, PHY_ADDR_REG, (0xF00 | RDB_EN_REG)));
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                        unit, port, PHY_DATA_REG, 0x0));

    /* no ECD execution yet, Autoneg ECD disabled */
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_DATA_REG, (ECD_CTRL & 0x0FFF)));
    /* Read ECD_control_and_status reg to flush sticky bit */
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
    CBX_IF_ERROR_RETURN(avng_phy_reg_read(unit, port, RDB_DATA_REG, &reg_val));

    /* Check Error condition - ECD engine still busy */
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
    CBX_IF_ERROR_RETURN(avng_phy_reg_read(unit, port, RDB_DATA_REG, &reg_val));
    if ((reg_val & 0x0800) != 0) {
        /* Clear RUN state in ECD_control_and_status reg */
        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_DATA_REG, (ECD_CTRL & 0x7FFF)));
        port_result->port_cdiag_result = cbxCdiagResultBusy;
#if 0
        CBX_IF_ERROR_RETURN(cbxi_cdiag_print_results(port, port_result));
#endif

        /* Enable link scan */
        link_params.usec = link_interval;
        cbx_link_scan_enable_set(&link_params);

        return CBX_E_BUSY;
    }

    /* Force break link and RUN ECD */
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_DATA_REG, (ECD_CTRL | 0x9000)));

    /* Delay 1 second  */
    sal_sleep(1);

    while (timeout > 1) {
        timeout--;
        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                unit, port, RDB_DATA_REG, &reg_val));
        if ((reg_val & 0x800) == 0) {
            break;
        }
        /* Delay 10ms */
        sal_usleep(10000);
    }

    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
    CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                unit, port, RDB_DATA_REG, &reg_val));
    /* Check 0x0800 for BUSY, 0x0008 for invalid result */
    if ((reg_val & 0x0800) != 0) {
        port_result->port_cdiag_result = cbxCdiagResultBusy;
    }else if ((reg_val & 0x8) != 0) {
        port_result->port_cdiag_result = cbxCdiagResultInvalid;
    } else {
        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                    unit, port, RDB_ADDR_REG, ECD_FAULT_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                    unit, port, RDB_DATA_REG, &reg_val));
    /* Test for No fault (0x1) on Pair A, Pair B, Pair C, Pair D cables */
        if (reg_val == 0x1111) {
            port_result->port_cdiag_result = cbxCdiagResultPass;
        } else {
            port_result->port_cdiag_result = cbxCdiagResultFail;
        }

        /* Pair wise results */
        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_PAIR_A_LEN_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                unit, port, RDB_DATA_REG, &c_length));
        port_result->pair_state[0]    = ((reg_val >> 12) & 0xF);
        port_result->pair_len[0]      = c_length;

        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_PAIR_B_LEN_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                unit, port, RDB_DATA_REG, &c_length));
        port_result->pair_state[1]    = ((reg_val >> 8) & 0xF);
        port_result->pair_len[1]      = c_length;

        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_PAIR_C_LEN_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                unit, port, RDB_DATA_REG, &c_length));
        port_result->pair_state[2]    = ((reg_val >> 4) & 0xF);
        port_result->pair_len[2]      = c_length;

        CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_PAIR_D_LEN_REG));
        CBX_IF_ERROR_RETURN(avng_phy_reg_read(
                                unit, port, RDB_DATA_REG, &c_length));
        port_result->pair_state[3]    = ((reg_val) & 0xF);
        port_result->pair_len[3]      = c_length;
    }
    /* Restore */
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_ADDR_REG, ECD_CONTROL_REG));
    CBX_IF_ERROR_RETURN(avng_phy_reg_write(
                                unit, port, RDB_DATA_REG, (ECD_CTRL & 0x0FFF)));

#if 0
    CBX_IF_ERROR_RETURN(cbxi_cdiag_print_results(port, port_result));
#endif

    /* Enable link scan */
    link_params.usec = link_interval;
    cbx_link_scan_enable_set(&link_params);

    return CBX_E_NONE;
}


/**
 * Run cable diag on all ports.
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbxi_cdiag_run_all (void)
{
    cbx_port_cable_diag_t port_result;
    cbx_port_t     port;
    int            unit;
    uint8          num_units = 1;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    sal_printf("Run full cable diag....\n");
    for (unit = 0; unit < num_units; unit++) {
        for (port = 0; port < 8; port++) {
            CBX_IF_ERROR_RETURN(cbx_port_cable_diag(
                                CBXI_GLOBAL_PORT(port, unit), &port_result));
            CBX_IF_ERROR_RETURN(cbxi_cdiag_print_results(
                                CBXI_GLOBAL_PORT(port, unit), &port_result));
        }
    }

    return CBX_E_NONE;
}

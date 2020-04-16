/*
 * *   mac.c
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * Purpose:
 * *     Robo2 MAC modules
 * *     contains MAC interface functions.
 * *
 * */

#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/mac.h>
#include <fsal/error.h>
#include <fsal_int/port.h>

int
xlmac_init(int unit, int no_xlport) {
    if (no_xlport) {
        return SOC_E_NONE;
    }
    uint32_t reg_data[2];
    uint32_t regval;
    uint32 fval = 0;
    reg_data[0] = 0 ;
    reg_data[1] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_SOFT_RESETr(unit, &regval));
    soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&regval, PORT2f, &fval);
    soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&regval, PORT3f, &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_SOFT_RESETr(unit, &regval));
#if 0
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_MAC_CONTROLr(unit, (uint32 *)&regval));
    regval = regval | 1;
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_CONTROLr(unit, &regval));
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_MAC_CONTROLr(unit, (uint32 *)&regval));
    regval = regval & 0xFFFFFFFE;
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_CONTROLr(unit, &regval));

    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_XGXS0_CTRL_REGr(unit, &regval));
    /* reset RSTB_HW (bit 0) */
    regval = regval & 0xFFFFFFFE;
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_XGXS0_CTRL_REGr(unit, &regval));
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_XGXS0_CTRL_REGr(unit, &regval));
    regval = regval | 1;
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_XGXS0_CTRL_REGr(unit, &regval));
#endif
    reg_data[0] = 0 ;
    fval = 1;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_ENABLE_REGr(unit, (uint32 *)&reg_data[0]));
    soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT0f , &fval);
    soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT1f , &fval);
    soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT2f , &fval);
    soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT3f , &fval);
    SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_ENABLE_REGr(unit, (uint32 *)&reg_data[0]));
    reg_data[0] = 0 ;
    SOC_IF_ERROR_RETURN(REG_READ_XLPORT_ENABLE_REGr(unit,  (uint32 *)&reg_data[0]));
    return SOC_E_NONE;
}

int
mac_reset(int unit, int no_xlport) {
    uint32_t regval = 0;
    uint32_t reg_data[2];
    uint32_t fval= 1;

    REG_READ_CRU_CRU_IP_SYSTEM_RESET5r(0, &regval);
    regval = regval & ~(0x1FF << 0);  // QSGMII Reset, Unimac
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(0, &regval);

    REG_READ_CRU_CRU_IP_SYSTEM_RESET5r(0, &regval);
    regval = regval | (0x7 << 0);  // QSGMII Reset
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(0, &regval);

    if (no_xlport) {
        /* NO XLPORT */
    } else {
        reg_data[0] = 0 ;
        reg_data[1] = 0 ;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_SOFT_RESETr(unit, &reg_data));
        soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&reg_data, PORT2f, &fval);
        soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&reg_data, PORT3f, &fval);
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_SOFT_RESETr(unit, &reg_data));
    }
    return SOC_E_NONE;
}

int
unimac_init(int unit) {
    uint32_t regval;
    REG_READ_CRU_CRU_IP_SYSTEM_RESET5r(0, &regval);
    regval = regval | (0x1FF << 0);  // QSGMII Reset, Unimac
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(0, &regval);
    return SOC_E_NONE;
}
/*
 * Function:
 *  soc_mac_pre_init
 * Purpose:
 *  Global Initialization for mac driver.
 *
 */
int soc_mac_pre_init(int no_xlport) {
#ifdef CONFIG_EXTERNAL_HOST
    int             unit = SOC_AVENGER_PRIMARY;
    mac_reset(unit, no_xlport);
    if (SOC_IS_CASCADED(unit)){
        unit = SOC_AVENGER_SECONDARY;
        mac_reset(unit, no_xlport);
    }
#endif
    return SOC_E_NONE;
}
/*
 * Function:
 *  soc_mac_init
 * Purpose:
 *  Global Initialization for mac driver.
 *
 */
int soc_mac_init(int no_xlport) {
    pbmp_t          pbmp;
    int             port;
    int             unit = SOC_AVENGER_PRIMARY;
    int             skip = 0;
    uint32          speed = 2500;
    unimac_init(unit);
    xlmac_init(unit, no_xlport);
    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    sal_printf("Unit %d Mac init Ports: ", unit);
    SOC_PBMP_ITER(pbmp, port) {
        sal_printf("%d ", port);
        if (port > SOC_ROBO2_MAX_NUM_PORTS) {
            continue;
        }
        if(IS_LOCAL_CPU_PORT(unit, port)) {
           /* do nothing */
            skip = 1;
            port_info[unit][port].p_mac = NULL;
        } else if (IS_XL_PORT(unit, port)) {
            port_info[unit][port].p_mac = &soc_mac_xl;
        } else {
            port_info[unit][port].p_mac = &soc_mac_uni;
        }
        if (skip == 0) {
            /* init mac */
            SOC_IF_ERROR_RETURN(MAC_INIT(port_info[unit][port].p_mac,
                                     unit, port));
            /* Probe function should leave port disabled */
            SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(port_info[unit][port].p_mac,
                                        unit, port, FALSE));
            skip = 0;
            if (IS_XL_PORT(unit, port)) {
#if defined(CONFIG_QSGMII) || defined(CONFIG_1000X_1000) || (CONFIG_SGMII_SPEED == 1000)
                speed = 1000;
#endif
                if ((port == 12) || (port == 13)) {
                    speed = 10000;
#if (CONFIG_XFI_SPEED == 1000)
                    speed = 1000;
#endif
#if (CONFIG_XFI_SPEED == 2500)
                    speed = 2500;
#endif
#if defined(CONFIG_10G_INTF_SGMII) || defined(CONFIG_10G_INTF_KX)
                    speed = 1000;
#endif
                }
                SOC_IF_ERROR_RETURN(MAC_SPEED_SET(port_info[unit][port].p_mac,
                                        unit, port, speed));
            }
        }
    }
    sal_printf("\n", port);
    skip = 0;
    if (SOC_IS_CASCADED(unit)){
        unit = SOC_AVENGER_SECONDARY;
        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        SOC_PBMP_AND(pbmp, 0xFFFF);
        sal_printf("Unit %d Mac init Ports: ", unit);
        unimac_init(unit);
        xlmac_init(unit, no_xlport);
        SOC_PBMP_ITER(pbmp, port) {
            sal_printf("%d ", port);
            if (port > SOC_ROBO2_MAX_NUM_PORTS) {
                continue;
            }
            if(IS_LOCAL_CPU_PORT(unit, port)) {
               /* do nothing */
               skip = 1;
               port_info[unit][port].p_mac = NULL;
            } else if (IS_XL_PORT(unit, port)) {
                port_info[unit][port].p_mac = &soc_mac_xl;
            } else {
                port_info[unit][port].p_mac = &soc_mac_uni;
            }
            if (skip == 0) {
                /* init mac */
                SOC_IF_ERROR_RETURN(MAC_INIT(port_info[unit][port].p_mac,
                                                                    unit, port));
                /* Probe function should leave port disabled */
                SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(port_info[unit][port].p_mac,
                                                              unit, port, FALSE));
                skip = 0;
                if (IS_XL_PORT(unit, port)) {
#if defined(CONFIG_QSGMII) || defined(CONFIG_1000X_1000) || (CONFIG_SGMII_SPEED == 1000)
                    speed = 2500;
#endif
                    if ((port == 12) || (port == 13)) {
                        speed = 10000;
#if (CONFIG_XFI_SPEED == 1000)
                        speed = 1000;
#endif
#if (CONFIG_XFI_SPEED == 2500)
                        speed = 2500;
#endif
                    }
                    SOC_IF_ERROR_RETURN(MAC_SPEED_SET(port_info[unit][port].p_mac,
                                                             unit, port, speed));
                }
            }
        }
        sal_printf("\n", port);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *  mac_duplex_set
 * Purpose:
 *  Set duplex.
 *
 */
int mac_duplex_set(int unit, cbx_port_t port, int duplex) {
    int  rv = SOC_E_NONE;
    rv = MAC_DUPLEX_SET(port_info[unit][port].p_mac, unit,
                                       port, duplex);
    return rv;
}

/*
 * Function:
 *  mac_duplex_get
 * Purpose:
 *  Get duplex setting
 *
 */
int mac_duplex_get(int unit, cbx_port_t port, int *duplex) {
    int  rv = SOC_E_NONE;
    rv = MAC_DUPLEX_GET(port_info[unit][port].p_mac, unit,
                                       port, (uint32 *)duplex);
    return rv;
}

/*
 * Function:
 *  mac_speed_set
 * Purpose:
 *  Set MAC speed for the port.
 *
 */
int mac_speed_set(int unit, cbx_port_t port, int speed) {
    int  rv = SOC_E_NONE;
    rv = MAC_SPEED_SET(port_info[unit][port].p_mac, unit,
                                       port, (uint32)speed);
    return rv;
}

/*
 * Function:
 *  mac_speed_get
 * Purpose:
 *  Get MAC speed for the port.
 *
 */
int mac_speed_get(int unit, cbx_port_t port, int *speed) {
    int  rv = SOC_E_NONE;
    rv = MAC_SPEED_GET(port_info[unit][port].p_mac, unit,
                                       port, (uint32 *)speed);
    return rv;
}

/*
 * Function:
 *  mac_max_frame_size_get
 * Purpose:
 *  Get maximum frame size set for the port.
 *
 */
int mac_max_frame_size_get(int unit, cbx_port_t port, uint32 *size) {
    int  rv = SOC_E_NONE;
    rv = MAC_FRAME_MAX_GET(port_info[unit][port].p_mac, unit,
                                       port, size);
    return rv;

}

/*
 * Function:
 *  mac_max_frame_size_set
 * Purpose:
 *  Set maximum frame size for the port.
 *
 */
int mac_max_frame_size_set(int unit, cbx_port_t port, uint32_t size) {
    int  rv = SOC_E_NONE;
    rv = MAC_FRAME_MAX_SET(port_info[unit][port].p_mac, unit,
                                       port, size);
    return rv;

}

/*
 * Function:
 *  mac_loopback_set
 * Purpose:
 *  Set the port in MAC loopback.
 *
 */
int mac_loopback_set(int unit, cbx_port_t port, int loopback) {
    int  rv = SOC_E_NONE;
    rv = MAC_LOOPBACK_SET(port_info[unit][port].p_mac, unit,
                                       port, loopback);
    return rv;

}

/*
 * Function:
 *  mac_loopback_get
 * Purpose:
 *  Get port MAC loopback status.
 *
 */
int mac_loopback_get(int unit, cbx_port_t port, int *loopback) {
    int  rv = SOC_E_NONE;
    rv = MAC_LOOPBACK_GET(port_info[unit][port].p_mac, unit,
                                       port, (uint32 *)loopback);
    return rv;
}

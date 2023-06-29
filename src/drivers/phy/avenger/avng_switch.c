/*
 * $Id: avng_switch.c,v 1.0 2016/03/18 06:45:04 mohanm Exp $
 *
 * $Copyright: (c) 2016 Broadcom Ltd.
 * All Rights Reserved.$
 *
 */
#include <soc/types.h>
#include <soc/drv.h>
#include <avng_miim.h>
#include <cdk/cdk_symbols.h>
#include <avng_phy.h>
#include <avng_phy_ctrl.h>
#include <sal_util.h>
#include <cdk/cdk_chip.h>
#include <cdk/cdk_error.h>
#include <phy/phy.h>
#include <drv_if.h>
#include <common/memregs.h>
#ifdef CONFIG_8488X
#include <bcm8488x.h>
#endif

#if EAGLE_LOAD_EXTERNAL
int
_firmware_helper(void *ctx, uint32_t offset, uint32_t size, void *data)
{
    int ioerr = 0;
    uint32 val;
    uint32 wbuf[4], ucmem_data[4];
    uint32 ucmem_ctrl;
    uint32 *fw_data;
    uint32 *fw_entry;
    uint32 fw_size;
    uint32 idx, wdx;
    phy_ctrl_t *pc = (phy_ctrl_t *)ctx;
    int lport;


    /* Check if PHY driver requests optimized MDC clock */
    if (data == NULL) {
        uint32 rate_adjust;
        val = 1;

        /* Offset value is MDC clock in kHz (or zero for default) */
        if (offset) {
            val = offset / 1500;
        }

        ioerr = REG_READ_CMIC_RATE_ADJUSTr(0, (void *)&rate_adjust);
        rate_adjust &= ~ (0xFFFF0000);
        rate_adjust |= val << 16;
        ioerr += REG_WRITE_CMIC_RATE_ADJUSTr(0, (void *)&rate_adjust);
        return ioerr ? SYS_ERR_STATE : SYS_OK;
    }
    if (sal_strcmp(pc->drv->drv_name, "bcmi_tsce_xgxs") != 0) {
        return SOC_E_NONE;
    }
    if (size == 0) {
        return SYS_ERR_PARAMETER;
    }
    /* Aligned firmware size */
    fw_size = (size + FW_ALIGN_MASK) & ~FW_ALIGN_MASK;

    /* Enable parallel bus access, ACCESS_MODE [Bit 0] = 0x1 */
    ioerr += REG_READ_XLPORT_WC_UCMEM_CTRLr(0, &ucmem_ctrl);
    ucmem_ctrl |= 1;
    ioerr += REG_WRITE_XLPORT_WC_UCMEM_CTRLr(0, (void *)&ucmem_ctrl);

    /* DMA buffer needs 32-bit words in little endian order */
    fw_data = (uint32 *)data;
    for (idx = 0; idx < fw_size; idx += 16) {
        if (idx + 15 < size) {
            fw_entry = &fw_data[idx >> 2];
        } else {
            /* Use staging buffer for modulo bytes */
            sal_memset(wbuf, 0, sizeof(wbuf));
            sal_memcpy(wbuf, &fw_data[idx >> 2], 16 - (fw_size - size));
            fw_entry = wbuf;
        }
        for (wdx = 0; wdx < 4; wdx++) {
            ucmem_data[wdx] = sal_htonl(fw_entry[wdx]);
        }
#if 0 /* TBD - Memory doesn't seem to exist in Avenger  */
        bcm5340x_mem_set(pc->unit, SOC_PORT_BLOCK(lport),
                         M_XLPORT_WC_UCMEM_DATA(idx >> 4), ucmem_data, 4);
#endif
    }
    /* Disable parallel bus access */
    ucmem_ctrl = 0;
    ioerr += REG_WRITE_XLPORT_WC_UCMEM_CTRLr(0, (void *)&ucmem_ctrl);
    return ioerr ? SYS_ERR_STATE : SYS_OK;
}
#endif

//Test code
pbmp_t skip_pbmp;
void set_skip_pbmp(uint32 bitmap)
{
    int i;
    if (bitmap == 0) {
        SOC_PBMP_CLEAR(skip_pbmp);
    } else {
        for(i=0; i<32; i++) {
            if (bitmap & (1 << i)) {
                SOC_PBMP_PORT_ADD(skip_pbmp, i);
            }
        }
    }
}

int soc_phy_probe(void)
{
    pbmp_t gphy_pbmp;
    pbmp_t sgmii_pbmp;
    pbmp_t eagle_pbmp;
    int rv = 0;
    int unit = 0;
    uint32_t ability= 0x0;
    int port;
    int orig_port = 0;
    int i=0;
#if EAGLE_LOAD_EXTERNAL
    int speed_max = 0;
#endif
    int num_units = 1;
    int port_13_present = 0;
    phy_ctrl_t *pc;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(SOC_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    for (unit = 0; unit < num_units; unit++ ) {

       SOC_PBMP_CLEAR(gphy_pbmp);
       SOC_PBMP_CLEAR(sgmii_pbmp);
       SOC_PBMP_CLEAR(eagle_pbmp);

       for (i=0; i<8; i++) {
           SOC_PBMP_PORT_ADD(gphy_pbmp,i);
       }
       for (i=8; i<12; i++) {
           SOC_PBMP_PORT_ADD(sgmii_pbmp,i);
       }
       for (i=12; i<14; i++) {
           SOC_PBMP_PORT_ADD(eagle_pbmp,i);
       }

       /* AND with actual ports enabled by OTP */
       SOC_PBMP_AND(gphy_pbmp, PBMP_ALL(unit));
       SOC_PBMP_AND(sgmii_pbmp, PBMP_ALL(unit));
       SOC_PBMP_AND(eagle_pbmp, PBMP_ALL(unit));

//Test code
       if (SOC_PBMP_NOT_NULL(skip_pbmp)) {
           SOC_PBMP_REMOVE(gphy_pbmp, skip_pbmp);
           SOC_PBMP_REMOVE(sgmii_pbmp, skip_pbmp);
           SOC_PBMP_REMOVE(eagle_pbmp, skip_pbmp);
       }

       SOC_PBMP_ITER(sgmii_pbmp, port) {
           /* PHY init */
           rv = avng_phy_probe(unit, port);
           if (SOC_SUCCESS(rv)) {
               rv = avng_phy_mode_set(unit, port, "viper",
                                           AVNG_PHY_MODE_SERDES, 1);
               rv = avng_phy_mode_set(unit, port, "viper",
                                          AVNG_PHY_MODE_2LANE, 0);
#ifndef CONFIG_QSGMII
               rv = avng_phy_mode_set(unit, port, "viper",
                                          AVNG_PHY_MODE_FIBER, 1);
#endif
           }
           ability |= AVNG_PHY_ABIL_2500MB;
           /* Set default as not fiber mode */
           pc = AVNG_PORT_PHY_CTRL(unit, port);
           if (SOC_SUCCESS(rv)) {
               PHY_CTRL_INTF(pc) = phymodInterface1000X;
#ifdef CONFIG_QSGMII
               PHY_CTRL_INTF(pc) = phymodInterfaceQSGMII;
#endif
#if  defined(CONFIG_SGMII)
               PHY_CTRL_INTF(pc) = phymodInterfaceSGMII;
#endif
               rv = avng_phy_init(unit, port);
               if ((port == 8) && (PHY_CTRL_INTF(pc) == phymodInterfaceQSGMII)) {
                   qsgmii_init(unit);
               }
            }
        } /* end of SGMII PHY init */

        if ((PBMP_ALL(unit) & 0x2000)) { /* port 13 present */
            port_13_present = 1;
        }
        SOC_PBMP_ITER(eagle_pbmp, port) {
            ability= 0x0;
            orig_port = port;
            if (port_13_present == 1) { /* if 13 is present probe 13 first */
                if(port % 2) {
                    port = port - 1;
                } else {
                    port = port + 1;
                }
            }
            rv = avng_phy_probe(unit, port);
            if (SOC_SUCCESS(rv)) {
                    rv = avng_phy_mode_set(unit, port, "eagle",
                                            AVNG_PHY_MODE_SERDES, 1);
                    rv = avng_phy_mode_set(unit, port, "eagle",
                                            AVNG_PHY_MODE_2LANE, 0);
            }
            ability |= (AVNG_PHY_ABIL_10GB | AVNG_PHY_ABIL_2500MB);
            pc = AVNG_PORT_PHY_CTRL(unit, port);
#if EAGLE_LOAD_EXTERNAL
            speed_max = 10312;
            if (SOC_SUCCESS(rv)) {
                rv = avng_phy_fw_helper_set(unit, port, _firmware_helper);
            }
#endif
            if (SOC_SUCCESS(rv)) {
                PHY_CTRL_INTF(pc) = phymodInterfaceXFI;
#if defined(CONFIG_10G_INTF_SGMII)
                PHY_CTRL_INTF(pc) = phymodInterfaceSGMII;
#endif
#if defined(CONFIG_10G_INTF_KR)
                PHY_CTRL_INTF(pc) = phymodInterfaceKR;
#endif
#if defined(CONFIG_10G_INTF_KX)
                PHY_CTRL_INTF(pc) = phymodInterfaceKX;
#endif
                rv = avng_phy_init(unit, port);
            }
            port = orig_port;
        } /* end of 10G PHY init */
        SOC_PBMP_ITER(gphy_pbmp, port) {
            rv = avng_phy_probe(unit, port);
            if (SOC_SUCCESS(rv)) {
                rv = avng_phy_init(unit, port);
            }
        }
        /* Probe for 8488x */
#ifdef CONFIG_8488X
        rv = plp_orca_phy_8481_probe(unit);
        if (rv == SOC_E_TIMEOUT) {
            sal_printf("mdio_timeout\n");
        }
        if (SOC_SUCCESS(rv)) {
            rv = plp_orca_phy_8481_init(unit);
        } else {
            sal_printf(" plp_orca_phy_8481_probe failed \n");
        }
#endif
    }
    return SYS_OK;
}

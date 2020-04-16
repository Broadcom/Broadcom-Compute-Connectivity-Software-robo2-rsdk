/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:
 *    led.c
 * Description:
 *    This file contains initialization code for LED uprocessor
 */

#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/robo2/common/tables.h>
#include <fsal/error.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/led.h>


#define LED_PROGRAM_MAX_SIZE 256


#ifdef CONFIG_EXTERNAL_HOST
#define LED_DEBUG   sal_printf
#else
#define LED_DEBUG   sysprintf
#endif /* CONFIG_EXTERNAL_HOST */

void led_refresh_cycle_config(int unit)
{
    uint8 i;
    uint32 remap_data;
    uint32 address;
    uint32 reg_len;
    uint8 temp_data;

    remap_data = 40;
    REG_WRITE_LED_LEDUP0_CLK_DIVr(unit, &remap_data);
    address = DRV_REG_ADDR(unit,LED_LEDUP0_PORT_ORDER_REMAP_0_3r_ROBO2,
                           REG_PORT_ANY, 0);
    reg_len = DRV_REG_LENGTH_GET(unit,LED_LEDUP0_PORT_ORDER_REMAP_0_3r_ROBO2);

    for (i = 0; i < 16; i++) {
        /* idea is to configure remap[port] = port */
        temp_data = ( i * 4 ) + 3;
        /* size of remap_data in register is 6bit per port */
        remap_data = temp_data;
        temp_data = temp_data - 1;
        remap_data = (remap_data << 6) | temp_data;
        temp_data = temp_data - 1;
        remap_data = (remap_data << 6) | temp_data;
        temp_data = temp_data - 1;
        remap_data = (remap_data << 6) | temp_data;

        drv_robo2_reg_write(unit, address, &remap_data, reg_len);
        address = address + 4;
    }
}

void led_delay_config(int unit)
{
    uint32 val;
    /* Set GPHY intra port delay */
    val = 0x11111111;
    REG_WRITE_CRU_CRU_LED_CONTROLLER_CFG0r(unit, &val);

    /* Set QSGMII intra port delay */
    val = 0x1111;
    REG_WRITE_CRU_CRU_LED_CONTROLLER_CFG1r(unit, &val);

    /* Set PMACRO intra port delay */
    val = 0x1;
    REG_WRITE_XLPORT_LED_CHAIN_CONFIGr(unit, &val);

}

void led_load_strap_to_data_ram(int unit)
{
    uint32 strap_data;
    uint8  led_strap_data;
    uint32 scan_chain_start_addr;

    REG_READ_CRU_CRU_STRAP_STATUS_REGr(unit, &strap_data);
#if 0
    LED_DEBUG("Strap Data:0x%x\n", strap_data);
#endif

    /* led 7 : bit 3 in strap */
    led_strap_data = (strap_data & 0x8) << 4;
    /* led 6 : bit 0 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x1) << 6);
    /* led 5 : bit 1 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x2) << 4);
    /* led 4 : bit 13 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x2000) >> 9);
    /* led 3 : bit 2 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x4) << 1);
    /* led 2 : bit 4 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x10) >> 2);
    /* led 1 : bit 9 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x200) >> 8);
    /* led 0 : bit 10 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x400) >> 10);

    /* Write the strap_data into LED_DATA_RAM location 0xFE.
	 * Multiplying by 4 to match the avenger addressing mechanism for
     * LED data ram
  	 */
    REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, 0xFE, &led_strap_data);


    /*  config SCAN CHAIN ASSEMBLY AREA
     *  start address = 128;
     *  depth = 32
     *  Bits 32 * 8 = 256 bits max
     */
    scan_chain_start_addr = 128;
    REG_WRITE_LED_LEDUP0_SCANCHAIN_ASSEMBLY_ST_ADDRr(unit,
                                                     &scan_chain_start_addr);

}

void led_firmware_load(int unit)
{
    uint16 count;
    uint32 firmware_data;

    LED_DEBUG("Loading LED Firmware\n");
    for (count = 0; count < LED_PROGRAM_MAX_SIZE; count = count + 1)
    {
        firmware_data = led_firmware[count];
        REG_WRITE_LED_LEDUP0_PROGRAM_RAMr(unit, count, &firmware_data);
        if ((count % 40) == 0) {
            LED_DEBUG(".");
        }
    }
    LED_DEBUG("\n");
}

void led_start(int unit)
{
    uint32 reg_data;

    /* Refresh cycle =  33.33 ms */
    reg_data = 0xCB7355;
    REG_WRITE_LED_LEDUP0_CLK_PARAMSr(unit, &reg_data);

    /* { led_scan_start_dly = 0, led_intra_port_dly = 0, ledup_enable = 1} */
    reg_data = 1;
    REG_WRITE_LED_LEDUP0_CTRLr(unit, &reg_data);
}

void bcm53158_initialize_led(int unit)
{

    LED_DEBUG("Unit %d: LED Refresh cycle config\n", unit);
    led_refresh_cycle_config(unit);

    LED_DEBUG("Unit %d: LED Delay config\n", unit);
    led_delay_config(unit);

    LED_DEBUG("Unit %d: LED Strap load\n", unit);
    led_load_strap_to_data_ram(unit);

    led_firmware_load(unit);

    LED_DEBUG("Unit %d: LED Start\n", unit);
    led_start(unit);

}

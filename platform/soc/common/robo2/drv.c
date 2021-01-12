/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:
 *     drv.c
 * Purpose:
 *     Robo2 switch driver
 */


#include <sal_types.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/wdog.h>
#include "driver.h"
#include "robo2_drv.h"
#include <bslenable.h>
#include "bsl.h"
#include <mac.h>
#include <avng_phy.h>
#include <fsal_int/port.h>
#ifdef CONFIG_USE_UART_AS_CPU_INTF
#include "soc/robo2/bcm53158/uart_spi.h"
#else
#include "soc/robo2/bcm53158/spi.h"
#endif

#ifdef CONFIG_EXTERNAL_HOST
#define dbg_printf sal_printf
#else
#define dbg_printf sysprintf
#endif

#define PRINT_PER_QUEUE_PKT_CNT(unit, port, queue) \
{ \
    REG_READ_CB_PQS_PER_QUEUE_PKT_COUNT_PORT ## port ## _QUEUE ## queue ##r(unit, &regval);    \
    if (regval != 0) {    \
         sal_printf("CB_PQS_per_queue_pkt_count_port%d_queue%d => %08x\n", port, queue, regval);    \
    } \
}

#define PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, port) \
{ \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 0) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 1) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 2) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 3) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 4) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 5) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 6) \
    PRINT_PER_QUEUE_PKT_CNT(unit, port, 7) \
}

/* [Murali]: Updates to this file is ongoing  #if 0 code is retained and will be
 * updated/deleted as per sequence provided by hardware.
 */

extern  void bcm53158_initialize_led(int unit);
#ifndef CONFIG_BOOTLOADER
  extern int soc_robo2_arl_table_init(void);
#endif
/*
 * Driver global variables
 *
 *   soc_control: per-unit driver control structure
 *   soc_ndev: the number of units created
 *   soc_ndev_attached: the number of units attached
 */

int soc_eth_unit = -1;
int soc_eth_ndev = 0;

uint32_t otp_flags = 0;

int soc_robo2_mgmt_init(int unit);
int _soc_robo2_device_created(int unit);

/* Start Stub functions replace with actual implementation */
int
soc_cm_get_id_driver(uint16 dev_id, uint8 rev_id,
                     uint16 *dev_id_driver, uint8 *rev_id_driver)
{
    return SOC_E_NONE;
}

int
soc_cm_get_id(int dev, uint16 *dev_id, uint8 *rev_id)
{
    return SOC_E_NONE;
}

uint32_t
soc_cm_get_dev_type(int dev)
{
    return 0;
}

const char *
soc_dev_name(int unit)
{
    return  "<UNKNOWN>";
}


char *
soc_block_port_name_lookup_ext(soc_block_t blk, int unit)
{
    return  "?";
}

int
soc_misc_init(int unit)
{
    return SOC_E_NONE;
}

void
soc_feature_init(int unit)
{
    return;
}

int
soc_robo_counter_attach(int unit)
{
    return(soc_robo2_counter_attach(unit));
}

int
soc_robo_counter_detach(int unit)
{
    return(soc_robo2_counter_detach(unit));
}

/* End Stub functions */

/*
 * @par Function:
 *    soc_robo2_reboot
 * @par Purpose:
 *    Hard reset the Soc
 * @params [in] unit - switch unit #.
 * @returns  SOC_E_XXXX on failure, Does not return on success
 */
int
soc_robo2_reboot(int unit)
{
#ifndef CONFIG_EXTERNAL_HOST
    /*
     * In External host mode, SPI Chip reset will be issued by Host.
     * In Internal host mode, the only way to reboot the system
     * is by forcing watchdog to expire
     * This routine does not return
     */
    watchdog_reboot();
#endif
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *  soc_timesync_init
 * Purpose:
 *  Initialization of clock for modules.
 *
 */
int
soc_timesync_init(int unit) {
    int           i = unit;
    uint32        fval = 0;
    uint32        reg_data;

        // Remove Reset for Timesync
        (
          REG_READ_CRU_CRU_IP_RESET_REG_CRUr(i, &reg_data));
        reg_data = reg_data | (0x1 << 5);
        reg_data = reg_data | (0x1 << 6);
        (
          REG_WRITE_CRU_CRU_IP_RESET_REG_CRUr(i, &reg_data));

        // Enable Common Controls for TS
        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_COUNTER_CONFIG_SELECTr_field_set(i, &reg_data,
                                       ENABLE_COMMON_CONTROLf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_COUNTER_CONFIG_SELECTr(i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_TS0_COUNTER_ENABLEr_field_set(i, &reg_data,
                                       ENABLEf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_COUNTER_ENABLEr(i, &reg_data));


        // We use 500 Mhz in Avenger
        reg_data = 0x0;
        fval = 0x40000000;
        soc_CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_FRACr_field_set(i, &reg_data,
                                           FRACf, &fval);
        (
              REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_FRACr(i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr_field_set(
                               i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
            REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr(
                               i, &reg_data));


        reg_data = 0x0;
        fval = 0xf2d;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_UP_EVENT_CTRLr_field_set(
                               i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_UP_EVENT_CTRLr(
                               i, &reg_data));

        reg_data = 0x0;
        fval = 0xf2d;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_DOWN_EVENT_CTRLr_field_set(
                                         i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_DOWN_EVENT_CTRLr(
                                                                 i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_field_set(i,
                                                    &reg_data, ENABLEf, &fval);
        (
              REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr(i,
                                                                &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_OUTPUT_ENABLEr_field_set(
                                      i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_OUTPUT_ENABLEr(i,
                                                                   &reg_data));


        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_UP_EVENT_CTRLr_field_set(
                                       i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_UP_EVENT_CTRLr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_DOWN_EVENT_CTRLr_field_set(
                                        i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_DOWN_EVENT_CTRLr(
                                        i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_CTRLr_field_set(i,
                                                  &reg_data, ENABLEf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_CTRLr(i,
                                                             &reg_data));

        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_OUTPUT_ENABLEr_field_set(
                                      i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_OUTPUT_ENABLEr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        /* 0xf42 is 3906 Decimal. This is the half-period so 3906*2 = 7812 ns is the full period.
         * Shaper Clock rate as (1/(7812*10^-9) i.e. 1/0.000,007,812 or 128008.192 Hz, the clock
         * needed to be configured (128 kHz)
         */
        if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
            fval = 0xf42;
        }
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_UP_EVENT_CTRLr_field_set(
                                      i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_UP_EVENT_CTRLr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        /* 0xf42 is 3906 Decimal. This is the half-period so 3906*2 = 7812 ns is the full period.
         * Shaper Clock rate as (1/(7812*10^-9) i.e. 1/0.000,007,812 or 128008.192 Hz, the clock
         * needed to be configured (128 kHz)
         */
        if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
            fval = 0xf42;
        }
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_DOWN_EVENT_CTRLr_field_set(
                                     i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_DOWN_EVENT_CTRLr(
                                                             i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_CTRLr_field_set(i,
                                                &reg_data, ENABLEf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_CTRLr(i,
                                                              &reg_data));

        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_OUTPUT_ENABLEr_field_set(
                                i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
            REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_OUTPUT_ENABLEr(
                                i, &reg_data));


        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_UP_EVENT_CTRLr_field_set(
                                i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_UP_EVENT_CTRLr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_DOWN_EVENT_CTRLr_field_set(
                                   i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_DOWN_EVENT_CTRLr(
                                                              i, &reg_data));

        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_CTRLr_field_set(i,
                                      &reg_data, ENABLEf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_CTRLr(
                                                      i, &reg_data));
    return SOC_E_NONE;
}
int
soc_lcpll2_init(int unit, int is_50mhz)
{

#define LCPLL_STATUS_TS_LCPLL_LOCK            (1 << 12)
#define LCPLL_CONTROL_TS_LCPLL_RESETB         (1 << 31)
#define LCPLL_CONTROL_TS_LCPLL_POSTDIV_RESETB (1 << 30)

    int  timeout_count;
    uint32 lcpll_control = 0x300;
    uint32 lcpll_status = 0;
    uint32 regval = 0;
    uint32 reg_data = 0x0;
    uint32 fval = 1;
    soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_field_set(unit,
                                                    &reg_data, ENABLEf, &fval);
    REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr(unit, &reg_data);

    /* Program LCPLL Holdover bypass os that CRU controls LCPLL directly */
    regval = 1;
    REG_WRITE_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_BYPASSr(unit, &regval);

    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL0r(unit, &lcpll_control);

    regval = 0x80000000;
    if (is_50mhz) {
        /* PDIV = 2 for 50Mhz (default) */
        regval |= 0x8000000;
    } else {
        /* Pdiv = 1 for 25Mhz */
        regval |= 0x4000000;
    }
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL1r(unit, &regval);

    /* Program LCPLL FDIV Upper Bits */
    regval = 0x1F40;
    REG_WRITE_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_UPPERr(unit, &regval);

    /* Program LCPLL FDIV Lower Bits so that entire FDIV write takes effect */
    regval = 0;
    REG_WRITE_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_LOWERr(unit, &regval);

    /* Program CH1 MDIV and CH2 MDIV to get 25M */
    regval = 0x07d1f414;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL3r(unit, &regval);

    /* Program bit 62 of {ts_lcpll_pll_ctrl_h, ts_lcpll_pll_ctrl_l}
        which is bit 30 of ts_lcpll_pll_ctrl_h to 0
        to forward the refclk as this bit acts as mux select
     */
    regval = 0x0010047D;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL4r(unit, &regval);
    regval = 0x00805804;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL5r(unit, &regval);
    regval = 0x40000014;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL6r(unit, &regval);
    regval = 0xF1F0203D;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL7r(unit, &regval);

    /* ts_lcpll_msc_ctrl[8:7] should be enabled so that QSGMII get clocks.
       Hence reset value of 0x80 is changed to 0x180.
     */
    regval =  0x00260180;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL8r(unit, &regval);
    regval =  0x0;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL9r(unit, &regval);
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL10r(unit, &regval);

    /* Out of reset */
    lcpll_control |= LCPLL_CONTROL_TS_LCPLL_RESETB;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL0r(unit, &lcpll_control);

    timeout_count = 100;
    /* Wait for PLL Lock */
    do {
        /* Read the LCPLL Status */
        REG_READ_CRU_TS_TOP_TS_LCPLL_STATUSr(unit, &lcpll_status);
        if (lcpll_status & LCPLL_STATUS_TS_LCPLL_LOCK) {
            break;
        }
        sal_usleep(100);
    } while(timeout_count--);

#ifndef EMULATION_BUILD
    if (timeout_count < 0) {
        return SOC_E_INIT;
    }
#endif

    lcpll_control |= LCPLL_CONTROL_TS_LCPLL_POSTDIV_RESETB;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL0r(unit, &lcpll_control);
    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_spi_master_enable
 * Purpose:
 *    Enable the SPI master
 * Parameters:
 *    None
 * Returns:
 *    None
 */
void
soc_spi_master_enable()
{
    uint32 regval = 0;
    uint32 fval   = 1;
    int    unit   = 0;

    /* Reset SPIM */
    REG_WRITE_M7SS_IDM_SPIM_IDM_RESET_CONTROLr(unit, &fval);
    fval = 0;
    REG_WRITE_M7SS_IDM_SPIM_IDM_RESET_CONTROLr(unit, &fval);

    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
    /* Default in B0 is selected as GPIO unlike in A0.
     * In order to use SPI as same way as A0, MFIO_control_register_2 has to be
     * programmed to select the SS coming from SPIM.
     * The selection is in MFIO_control_register_2 bit 0*/
        fval   = 1;
        REG_WRITE_CRU_CRU_MFIO_CONTROL_REGISTER_2r(unit, &fval);
    }
    /* Initialize SPI Master */
    REG_READ_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r(unit, &regval);
    soc_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r_field_get(unit, &regval, POST_DIV_FOR_CH1f, &fval);
    if (fval == 0xA) {
        fval = 0x887;
    } else {
        fval = 0x587;
    }
    dbg_printf("Cascade Fval=%x\n", fval);
    regval = 0;
    REG_WRITE_M7SS_M7SC_RACRr(unit, &regval);

    /* Spi clock pre-divisor */
    regval = 2;
    REG_WRITE_M7SS_SPI0_SSPCPSRr(unit, &regval);

    /* Spi clock divisor, Data size and Spi mode */
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
    /* The cpol/cpha of avenger has changed from A0 to B0.
     * For SPI-S:
     *     A0 is CPOL=0; CPHA=1
     *     B0 is CPOL=1; CPHA=1
     * Change the bit 6 of SSPCR0 in order for SPI master to talk to the SPIS */
        fval |= 0x40;
    }
    REG_WRITE_M7SS_SPI0_SSPCR0r(unit, &fval);

    /* Spi SSE enable */
    regval = 2;
    REG_WRITE_M7SS_SPI0_SSPCR1r(unit, &regval);

    /* Remote access */
    regval = 7;
    REG_WRITE_M7SS_M7SC_RACRr(unit, &regval);
}

/*
 * Function:
 *    soc_dev_check
 * Purpose:
 *    Check the Chip ID
 * Parameters:
 *    ChipID, RevID
 * Returns:
 *    SOC_E_NONE - Found valid device
 *    SOC_E_NOTFOUND - Not valid device
 */
static int
soc_dev_check(int unit, uint32 chipid, uint32 revid)
{
    dbg_printf("Unit %d: ChipID: %d Rev %d\n", unit, chipid, revid);
    switch (chipid) {
    case BCM53112_DEVICE_ID:
    case BCM53154_DEVICE_ID:
    case BCM53156_DEVICE_ID:
    case BCM53157_DEVICE_ID:
    case BCM53158_DEVICE_ID:
    case BCM53161_DEVICE_ID:
    case BCM53162_DEVICE_ID:
        return 0;
    }
    dbg_printf("Unit %d: ChipID verfication failed\n", unit);
    return 1;
}

int
soc_cb_init(int unit)
{
    uint32 regval = 0;
    uint32 fval = 0;
    uint32 done;
    uint8_t timeout = 200;

    /* Reset CB */
    REG_READ_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    regval &=  ~(0x1 << 2);  /* CB Reset */
    regval &=  ~(0x1 << 7);  /* CB Reset */
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    regval = regval | (0x1 << 2);  /* CB Reset */
    regval = regval | (0x1 << 7);  /* CB Reset */
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);

    /* Initialize LLC free page list */
    regval = 0x0;
    fval = 1;
    soc_CB_BMU_LLC_SELF_INITr_field_set(unit, (uint32 *)&regval, SELF_INITf, &fval);
    soc_CB_BMU_LLC_SELF_INITr_field_set(unit, (uint32 *)&regval, START_PAGEf, &fval);
    fval = 4095;
    soc_CB_BMU_LLC_SELF_INITr_field_set(unit, (uint32 *)&regval, END_PAGEf, &fval);
    REG_WRITE_CB_BMU_LLC_SELF_INITr(unit, (uint32 *)&regval);

    /* Initialize LLC control */
    regval = 0x0;
    fval = 1;
    soc_CB_BMU_LLC_CONTROLr_field_set(unit, (uint32 *)&regval, LLC_ENABLEf, &fval);
    REG_WRITE_CB_BMU_LLC_CONTROLr(unit, (uint32 *)&regval);

    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
        regval = 0x0;
        REG_READ_CB_BMU_LLC_STATUSr(unit, (uint32 *)&regval);
        soc_CB_BMU_LLC_STATUSr_field_get(unit , (uint32 *)&regval, SELF_INIT_DONEf, &done);
        timeout--;
    }
    /* Write back to clear self init done */
    REG_WRITE_CB_BMU_LLC_STATUSr(unit, (uint32 *)&regval);

    return SOC_E_NONE;
}

int
soc_pll1_init(int unit)
{
    uint32 clkmux, regval, fval;

    REG_READ_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r(unit, &regval);
    soc_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r_field_get(
            unit, &regval, POST_DIV_FOR_CH1f, &fval);
    dbg_printf("Unit %d: PLL1 CH1 POSTDIV %x\n", unit, fval);
    if (unit == 0) {
        /* Set NIC to 250Mhz on Master, Fval=16 */
        fval = 16;
        //fval = 10;
        soc_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r_field_set(
                unit, &regval, POST_DIV_FOR_CH1f, &fval);
        REG_READ_CRU_CRU_CLK_MUX_SELr(unit, &clkmux);
        /* Turn off Channel 1 */
        /* Workaround to fix ROBO-389 */
   /*     clkmux &= ~0x2; */
        SOC_IF_ERROR_RETURN(
            REG_WRITE_CRU_CRU_CLK_MUX_SELr(unit, &clkmux));
        SOC_IF_ERROR_RETURN(
            REG_WRITE_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r(unit, &regval));
        /* Turn on Channel 1 */
        clkmux |= 0x2; /* Channel 1 */
        SOC_IF_ERROR_RETURN(
            REG_WRITE_CRU_CRU_CLK_MUX_SELr(unit, &clkmux));
    }
    return SOC_E_NONE;
}

int soc_otp_status_read(int unit)
{
    uint32 regval = 0;

#if 0
    /* OTP override for software testing only */
    regval = 0x218;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE0_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN0_REGISTERr(unit, &regval);

    regval = 0x00100000;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE3_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN3_REGISTERr(unit, &regval);

    regval = 0x8;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE1_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN1_REGISTERr(unit, &regval);

#endif

    /* ****  Read OTP values ***** */
    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER0r(unit, &regval);
    otp_flags |= ((regval & 0x1) ? SOC_OTP_FLAG_GPHY_1G_DIS: 0);
    otp_flags |= ((regval & 0x4) ? SOC_OTP_FLAG_RGMII_DIS: 0);
    otp_flags |= ((regval & 0x8) ? SOC_OTP_FLAG_GPHY_0_3_DIS: 0);
    otp_flags |= ((regval & 0x10) ? SOC_OTP_FLAG_GPHY_4_7_DIS: 0);
    otp_flags |= ((regval & 0x20) ? SOC_OTP_FLAG_XFI_13_DIS: 0);
    otp_flags |= ((regval & 0x40) ? SOC_OTP_FLAG_XFI_12_DIS: 0);
    otp_flags |= (((regval & 0x600) == 0x000) ? SOC_OTP_FLAG_QSG_MODE_QSG: 0);
    otp_flags |= (((regval & 0x600) == 0x200) ? SOC_OTP_FLAG_QSG_MODE_SGM: 0);
    otp_flags |= (((regval & 0x600) == 0x600) ? SOC_OTP_FLAG_QSGMII_DIS: 0);
    otp_flags |= ((regval & 0x1000) ? SOC_OTP_FLAG_SGMII_2G5_DIS: 0);
    otp_flags |= ((regval & 0x2000) ? SOC_OTP_FLAG_SGMII_1G_DIS: 0);

    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER1r(unit, &regval);
    otp_flags |= ((regval & 0x8) ? SOC_OTP_FLAG_ARL_SIZE_8K: 0);
    otp_flags |= ((regval & 0x10) ? SOC_OTP_FLAG_CFP_DIS: 0);

    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER2r(unit, &regval);
    if (regval == 0x0C00CFA2) {
        otp_flags |= SOC_OTP_FLAG_PORT_9_DISABLE;
    }
    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER3r(unit, &regval);
    otp_flags |= ((regval & 0x00400000) ? SOC_OTP_FLAG_XLPORT_DISABLE: 0);
    otp_flags |= ((regval & 0x00800000) ? SOC_OTP_FLAG_XFI_13_MAX_2G5: 0);
    otp_flags |= ((regval & 0x01000000) ? SOC_OTP_FLAG_XFI_12_MAX_2G5: 0);

    sal_printf("OTP_FLAGS = 0x%x\n", otp_flags);

    return SOC_E_NONE;
}

int
soc_bootstrap_init(int unit)
{
    uint32 regval = 0;
    uint32 straps = 0;
    uint32   fval = 1;
    int is_50mhz;

    REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval);
    regval &= 0xFFFFFF; /* 24 bit chipid */
    REG_READ_CRU_CRU_REVISION_ID_REGr(unit, &fval);
    fval &= 0xFF; /* 8 bit revid */
    SOC_IF_ERROR_RETURN(soc_dev_check(unit, regval, fval));

    REG_READ_CRU_CRU_STRAP_STATUS_REGr(unit, &straps);
    dbg_printf("Unit %d: Straps: %08x\n", unit, straps);

    /* Set PLL1 */
    soc_pll1_init(unit);

    /* Enable lcpll2 */
    is_50mhz = (straps >> 4) & 1;
    SOC_IF_ERROR_RETURN(soc_lcpll2_init(unit, is_50mhz));

#ifdef CONFIG_EXTERNAL_HOST
    regval = 0x0;
#else
    /* Do not reset CB */
    regval = 0x84;
#endif
    /* Disable all peripherals */
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(unit, &regval);

#ifdef CONFIG_EXTERNAL_HOST
    /* External Host, init CB regardless of unit */
    soc_cb_init(unit);
#else
    /* Internal Host, init CB only on cascade */
    if ((unit == 1) && (SOC_IS_CASCADED(0))) {
        dbg_printf("Unit 1 cb init\n");
        soc_cb_init(1);
    }
#endif

    /* set  CRU_CRU_MFIO_control_register */
    regval = 0x59996655;
    REG_WRITE_CRU_CRU_MFIO_CONTROL_REGISTERr(unit, &regval);

    REG_READ_CRU_CRU_PAD_CONTROL_PUP_REGISTER_4r(unit, (uint32 *)&regval);
    regval = regval | (0x1 << 15);
    REG_WRITE_CRU_CRU_PAD_CONTROL_PUP_REGISTER_4r(unit, (uint32 *)&regval);
    regval = 0x59992655;
    REG_WRITE_CRU_CRU_MFIO_CONTROL_REGISTERr(unit, &regval);

   // Hack for Avenger-A0 : Issue with Return FIFO LLC Error
    REG_READ_CB_BMU_LLC_CONTROLr(unit, (uint32 *)&regval);
    fval = 1;
    soc_CB_BMU_LLC_CONTROLr_field_set(unit , (uint32 *)&regval, LLC_MC_MODEf , &fval);
    REG_WRITE_CB_BMU_LLC_CONTROLr(unit, (uint32 *)&regval);

#ifdef CONFIG_EXTERNAL_HOST
    regval = 0x000C23FF;
#else
    regval = 0xFFFFFFFF;
#endif
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESETr(unit, &regval);
    regval = 0;
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET2r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET3r(unit, &regval);
    regval = 0xFFFFFFFF;
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET2r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET3r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(unit, &regval);

    /* ARL table init is done here as a work around to avoid the
       port ARL count getting to max value immediately after the power on */
#ifndef CONFIG_BOOTLOADER
    soc_robo2_arl_table_init();
#endif
    SOC_IF_ERROR_RETURN(soc_timesync_init(unit));

    /* reset XLMAC */
    if (otp_flags & SOC_OTP_FLAG_XLPORT_DISABLE) {
        /* NO XLMAC */
    } else {
        uint32 xl_reg_data[2];
        xl_reg_data[0] = 0 ;
        xl_reg_data[1] = 0 ;
        fval = 1;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_MAC_CONTROLr(unit, (uint32 *)&xl_reg_data));
        soc_XLPORT_MAC_CONTROLr_field_set(unit , (uint32 *)&xl_reg_data, XMAC0_RESETf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_CONTROLr(unit, &xl_reg_data));
        fval = 0;
        soc_XLPORT_MAC_CONTROLr_field_set(unit , (uint32 *)&xl_reg_data, XMAC0_RESETf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_CONTROLr(unit, &xl_reg_data));

        fval = 0;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_XGXS0_CTRL_REGr(unit, &xl_reg_data));
        soc_XLPORT_XGXS0_CTRL_REGr_field_set(unit, (uint32 *)&xl_reg_data, RSTB_HWf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_XGXS0_CTRL_REGr(unit, &xl_reg_data));
        fval = 1;
        soc_XLPORT_XGXS0_CTRL_REGr_field_set(unit, (uint32 *)&xl_reg_data, RSTB_HWf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_XGXS0_CTRL_REGr(unit, &xl_reg_data));
    }
    // PVT mon
    regval = 0x58;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x78;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x68;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x48;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x68;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0xe8;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0xa8;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);

    if (otp_flags & SOC_OTP_FLAG_XFI_13_DIS) {
        REG_READ_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
        fval = 1;
        soc_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr_field_set(unit, &regval,
                                                     PM_TSC_CLK_MUX_SELECTf, &fval);
        REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    }

    REG_READ_AVR_PORTMACRO_PORT_MACRO_SPEED_CONFIGr(unit, &regval);
    /* Lane0 * [bit * 2:0] * = * 0x4, * lane1 * [bit * 6:4] * = * 4 * ;
    * configure * 10G 10M : 'b000 100M : 'b001 1G : 'b010 2.5G : 'b011 10G : 'b100 */
    regval = (regval & 0xFFFFFF00) | 0x44;
#if (CONFIG_XFI_SPEED == 1000)
    regval = (regval & 0xFFFFFF00) | 0x22;
#endif
#if (CONFIG_XFI_SPEED == 2500)
    regval = (regval & 0xFFFFFF00) | 0x33;
#endif
    REG_WRITE_AVR_PORTMACRO_PORT_MACRO_SPEED_CONFIGr(unit, &regval);

    regval = 0;
    if (straps & 1) {
        REG_READ_M7SS_MDIO_CMIC_MIIM_SCAN_CTRLr(unit, &regval);
        regval |= (1 << 28);
        REG_WRITE_M7SS_MDIO_CMIC_MIIM_SCAN_CTRLr(unit, &regval);
    }

#ifdef CONFIG_CASCADED_MODE
    if (unit == 0) {
/*
#ifndef CONFIG_EXTERNAL_HOST
*/
#if 0
        /* M7 Bootfrom flash can process straps */
        if (1 == ((straps >> 7) & 0x3)) {
             /* Cascaded Primary device  */
             dbg_printf("Unit %d: Cascade Configuration: Primary device \n", unit);
             soc_spi_master_enable();
             /* Attempt to access the Slave before confirming cascade operation */
             if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
                 SOC_INFO(unit).cascaded_mode = 1;
                 dbg_printf("Cascade Configuration Enabled\n");
                 SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
             } else {
                 dbg_printf("Cascade Config Not Enabled (Failed to detect Slave)\n");
             }
        }
#else
        /* External CPU mode cannot process straps */
        soc_spi_master_enable();
        /* Attempt to access the Slave before confirming cascade operation */
        if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
            SOC_INFO(unit).cascaded_mode = 1;
            dbg_printf("Cascade Configuration Enabled\n");
            SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
        } else {
            dbg_printf("Cascade Configuration Not Enabled (Failed to detect Slave)\n");
        }
#endif
        regval = 0;
        REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
    } else {
        regval = 1;
        REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
        SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_SLAVE;
    }

    //
    // Cannot Use Straps in A0:
    //
    // if (1 == ((straps >> 7) & 0x3)) {
    //     /* Cascaded Primary device  */
    //     dbg_printf("Unit %d: Cascade Configuration: Primary device \n", unit);
    //     soc_spi_master_enable();
    //     /* Attempt to access the Slave before confirming cascade operation */
    //     if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
    //          SOC_INFO(unit).cascaded_mode = 1;
    //          dbg_printf("Cascade Configuration Enabled\n");
    //          SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
    //     } else {
    //          dbg_printf("Cascade Configuration Not Enabled (Failed to detect Slave)\n");
    //     }
    //     regval = 0;
    //     REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
    //     }
    // }
    // if (2 == ((straps >> 7) & 0x3)) {
    //     /* Cascaded Secondary device */
    //     dbg_printf("Unit %d: Cascade Configuration: Secondary device \n", unit);
    //     regval = 1;
    //     REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
    //     SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_SLAVE;
    // }
    //
#endif /* CONFIG_CASCADE_MODE */
    return SOC_E_NONE;
}


/*
 * Function:
 *  soc_robo2_power_down_config
 * Purpose:
 *      Disable/Enable PORTX power down mode
 * Parameters:
 *  valid_pbmp - pbmp to disable power down mode (PHY is enable)
 */
int
soc_robo2_power_down_config(int unit, soc_pbmp_t valid_pbmp)
{

#if 0 /* Update power down sequence for robo2 */
    int rv;
    uint32 reg_val;
    soc_pbmp_t pbmp;

    /* Get power down support pbmp from chip specific dev_prop
     * Do nothing if the return value is unavail.
     */
    rv = DRV_DEV_PROP_GET(unit,
                          DRV_DEV_PROP_POWER_DOWN_SUPPORT_PBMP, (uint32 *)&pbmp);

    if (SOC_SUCCESS(rv)) {
        rv = REG_READ_PWR_DOWN_MODEr(unit, &reg_val);
        if (rv == SOC_E_UNAVAIL) {
            return SOC_E_NONE;
        }
        SOC_PBMP_NEGATE(valid_pbmp, valid_pbmp);
        SOC_PBMP_AND(pbmp, valid_pbmp);
        soc_PWR_DOWN_MODEr_field_set(unit, &reg_val, PORTX_PWR_DOWNf, (uint32 *)&pbmp);
        rv = REG_WRITE_PWR_DOWN_MODEr(unit, &reg_val);
        if (rv == SOC_E_UNAVAIL) {
            return SOC_E_NONE;
        }
    }
#endif
    return SOC_E_NONE;
}


static int
soc_robo2_board_led_init(int unit)
{
    int rv = SOC_E_NONE;
#if 0 /* Update led init sequence for robo2 */
    uint16      dev_id, chip_dev_id;
    uint8       rev_id, chip_rev_id;
    int         dma_unit = 1;
    uint32      data;

    /* check if the robo chip is BCM53242 */
    soc_cm_get_id(dma_unit, &dev_id, &rev_id);

    /* Keystone GMAC core ID */
    if (dev_id == BCM53000_GMAC_DEVICE_ID) {
        soc_cm_get_id(unit, &chip_dev_id, &chip_rev_id);
        if ((SOC_IS_ROBO53242(unit)) ||
                (SOC_IS_TB(unit) && (chip_dev_id == BCM53284_DEVICE_ID))) {
            data = 0x104;
            SOC_IF_ERROR_RETURN(REG_WRITE_LED_FUNC0_CTLr(unit,&data));
            data = 0x106;
            SOC_IF_ERROR_RETURN(REG_WRITE_LED_FUNC1_CTLr(unit,&data));
            data = 0x6000000;
            if((rv = REG_READ_LED_FUNC_MAPr(unit, (void *)&reg_value64)) < 0) {
                return rv;
            }
            soc_LED_FUNC_MAPr_field_set(unit, (void *)&reg_value64,
                                        LED_FUNC_MAPf, &data);
            if((rv = REG_WRITE_LED_FUNC_MAPr(unit, (void *)&reg_value64)) < 0) {
                return rv;
            }
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *  robo2_soc_chip_driver_find
 * Purpose:
 *      Return the soc_driver for a chip if found.
 *      If not found, return NULL.
 * Parameters:
 *  pci_dev_id - PCI dev ID to find (exact match)
 *  pci_rev_id - PCI dev ID to find (exact match)
 * Returns:
 *  Pointer to static driver structure
 */
static soc_driver_t *
soc_robo2_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id)
{
    int         i;
    soc_driver_t    *d;
    uint16      driver_dev_id = 0;
    uint8       driver_rev_id = 0;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id,
                             &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }

    /*
     * Find driver in table.  In theory any IDs returned by
     * soc_cm_id_to_driver_id() should have a driver in the table.
     */
    for (i = 0; i < SOC_ROBO2_NUM_SUPPORTED_CHIPS; i++) {
        d = soc_robo2_base_driver_table[i];
        if ((d != NULL) &&
                (d->block_info != NULL) &&
                (d->pci_device == driver_dev_id) &&
                (d->pci_revision == driver_rev_id)) {
            return d;
        }
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("soc_chip_driver_find: driver in devid table "
                        "not in soc_robo_base_driver_table\n")));

    return NULL;
}

/*
 * Function:
 *  soc_info_config
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  soc  - soc_control_t associated with this unit
 * Purpose:
 *  Fill in soc_info structure for a newly attached unit.
 *  Generates bitmaps and various arrays based on block and
 *  ports that the hardware has enabled.
 */

static void
soc_robo2_info_config(int unit, soc_control_t *soc)
{
    soc_info_t      *si;
    soc_robo2_control_t   *robo;
    soc_pbmp_t      pbmp_valid = 0;
    uint16      dev_id;
    uint8       rev_id;
    uint16      drv_dev_id = 0;
    uint8       drv_rev_id = 0;
    int         port, blk = 0, bindex = 0, pno = 0;
#if 0
    int         mem;
#endif
    char        *bname;
    int         blktype;
    int         disabled_port;

    si = &soc->info;
    sal_memset((void *)si, 0, sizeof(soc_info_t));

    robo = SOC_ROBO2_CONTROL(unit);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);
#if 0 /* TBD CM to be added */
    if (CMDEV(unit).dev.info->dev_type & SOC_SPI_DEV_TYPE) {
        si->spi_device = TRUE;
    }
#endif

    si->driver_type = soc->chip_driver->type;
#if 0
    si->driver_group = soc_chip_type_map[si->driver_type];
#endif
    si->num_cpu_cosq = 1;
    si->port_addr_max = 31;
    si->modid_count = 1;
    /* there is no Stacking solution for ROBO device so far */
    si->modid_max = 1;    /* See SOC_MODID_MAX(unit) */

    SOC_PBMP_CLEAR(si->s_pbm);  /* 10/100/1000/2500 Mbps comboserdes */
    SOC_PBMP_CLEAR(si->gmii_pbm);
#if 0
    /*
     * pbmp_valid is a bitmap of all ports that exist on the unit.
     */
    pbmp_valid = soc_property_get_pbmp(unit, spn_PBMP_VALID, 1);
#endif

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    switch (drv_dev_id) {
    case BCM53158_DEVICE_ID:
        robo->chip_type = SOC_ROBO2_CHIP_53158;
        robo->arch_type = SOC_ROBO2_ARCH_ROBO2;
        si->num_cpu_cosq = 8;
        /* Port 4 & 7 is not present in BCM53134 */
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 4);
        SOC_PBMP_PORT_REMOVE(pbmp_valid, 7);
        break;
    default:
        si->chip = 0;
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_info_config: driver device %04x unexpected\n"),
                  drv_dev_id));
        break;
    }

    si->ipic_port = -1;
    si->ipic_block = -1;
    si->exp_port = -1;
    si->exp_block = -1;
    si->spi_port = -1;
    si->spi_block = -1;
    si->fe.min = si->fe.max = -1;
    si->ge.min = si->ge.max = -1;
    si->xe.min = si->xe.max = -1;
    si->ether.min = si->ether.max = -1;
    si->port.min = si->port.max = -1;
    si->all.min = si->all.max = -1;
    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    for (blk = 0; blk < SOC_ROBO2_MAX_NUM_BLKS; blk++) {
        si->block_port[blk] = REG_PORT_ANY;
    }

    for (port = 0; ; port++) {
        disabled_port = FALSE;
        blk = SOC_PORT_INFO(unit, port).blk;
        bindex = SOC_PORT_INFO(unit, port).bindex;
        if (blk < 0 && bindex < 0) {            /* end of list */
            break;
        }
        if (blk < 0) {                  /* empty slot */
            disabled_port = TRUE;
            blktype = 0;
        } else {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            if (!SOC_PBMP_MEMBER(pbmp_valid, port)) {   /* disabled port */
#if 0
                if (SOC_BLOCK_IN_LIST(&blktype, SOC_BLK_CPU)) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit,
                                         "soc_info_config: "
                                         "cannot disable cpu port\n")));
                } else {
                    disabled_port = TRUE;
                }
#endif
            }
        }

        if (disabled_port) {
#if 0
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "?%d", port);
#endif
            si->port_offset[port] = port;
            continue;
        }

#define ADD_PORT(ptype, port) \
            si->ptype.port[si->ptype.num++] = port; \
            if (si->ptype.min < 0) { \
            si->ptype.min = port; \
            } \
            if (port > si->ptype.max) { \
            si->ptype.max = port; \
            } \
            SOC_PBMP_PORT_ADD(si->ptype.bitmap, port);

        bname = soc_block_port_name_lookup_ext(blktype, unit);
        switch (blktype) {
#if 0
        case SOC_BLK_EPIC:
            pno = si->fe.num;
            ADD_PORT(fe, port);
            ADD_PORT(ether, port);
            ADD_PORT(port, port);
            ADD_PORT(all, port);
            break;
        case SOC_BLK_GPIC:
            pno = si->ge.num;
            ADD_PORT(ge, port);
            ADD_PORT(ether, port);
            ADD_PORT(port, port);
            ADD_PORT(all, port);
            break;
        case SOC_BLK_XPIC:
            pno = si->xe.num;
            ADD_PORT(xe, port);
            ADD_PORT(ether, port);
            ADD_PORT(port, port);
            ADD_PORT(all, port);
            break;
#endif
        case SOC_BLK_SPI:
            pno = 0;
            si->spi_port = port;
            si->spi_block = blk;
            SOC_PBMP_PORT_ADD(si->spi_bitmap, port);
            ADD_PORT(all, port);
            break;
        default:
            pno = 0;
            break;
        }
#undef  ADD_PORT
        if (bname[0] == '?') {
            pno = port;
        }
#if 0
        sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "%s%d", bname, pno);
#endif
        si->port_type[port] = blktype;
        si->port_offset[port] = pno;
        si->block_valid[blk] += 1;
        if (si->block_port[blk] < 0) {
            si->block_port[blk] = port;
        }
        SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
    }
    si->port_num = port;

    /* some things need to be found in the block table */
    si->arl_block = -1;
    si->mmu_block = -1;
    si->mcu_block = -1;
    si->inter_block = -1;
    si->exter_block = -1;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        si->has_block[blk] = blktype;
#if 0
        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%d",
                     soc_block_name_lookup_ext(blktype, unit),
                     SOC_BLOCK_INFO(unit, blk).number);
#endif
    }
    si->block_num = blk;

#if 0
    /*
     * Calculate the mem_block_any array for this configuration
     * The "any" block is just the first one enabled
     */
    for (mem = 0; mem < NUM_SOC_ROBO2_MEM; mem++) {
        si->mem_block_any[mem] = -1;
        if(&SOC_MEM_INFO(unit, mem)==NULL) {
            continue;
        }
        if (SOC_MEM_IS_VALID(unit, mem)) {
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                si->mem_block_any[mem] = blk;
                break;
            }
        }
    }
#endif
}

int
soc_robo2_chip_reset(int unit)
{

    int rv = SOC_E_NONE;
    int i;
    uint32 temp;
#if 0
    uint32 data;
    uint32  model = 0;

    SOC_IF_ERROR_RETURN(REG_READ_WATCH_DOG_CTRLr(unit, &data));
    temp = 1;
    soc_WATCH_DOG_CTRLr_field_set(unit, &data, EN_SW_RESETf, &temp);
    soc_WATCH_DOG_CTRLr_field_set(unit, &data, SOFTWARE_RESETf, &temp);
    SOC_IF_ERROR_RETURN(REG_WRITE_WATCH_DOG_CTRLr(unit, &data));
#endif


    /* Wait for chip reset complete */
    for (i=0; i<100000; i++) {
#if 0
        data = 0;
        if ((rv = REG_READ_WATCH_DOG_CTRLr(unit, &data)) < 0) {
            /*
             * Can't check reset status from register.
             * Anyway, sleep for 3 seconds
             */
            sal_sleep(3);
            break;
        }
#endif
        temp = 0;
#if 0
        soc_WATCH_DOG_CTRLr_field_get(unit, &data, SOFTWARE_RESETf, &temp);
#endif
        if (!temp) {
            /* Reset is complete */
            break;
        }
    }
#if 0
    if (soc_feature(unit, soc_feature_int_cpu_arbiter)) {
        model = BCM53158_MODEL_ID;

        /* wait internal cpu boot up completely */
        do {
            REG_READ_MODEL_IDr(unit, &data);
            if(SOC_IS_POLAR(unit)) {
                data &= 0xFF000;
            }
        } while (data != model);

#if defined (BCM_STARFIGHTER3_SUPPORT)
        if (SOC_IS_STARFIGHTER3(unit) ) {
            /* Check if the internal 8051 is enabled or not */
            REG_READ_STRAP_PIN_STATUSr(unit, &data);
            if (data & BCM53134_STRAP_PIN_8051_MASK) {
                /* Bit 3 is en_8051 */
                SOC_CONTROL(unit)->int_cpu_enabled = TRUE;
            } else {
                SOC_CONTROL(unit)->int_cpu_enabled = FALSE;
            }
        }
#endif /* BCM_STARFIGHTER3_SUPPORT */
    }
#endif
    return rv;
}

/*
 * @par Function:
 *      soc_robo2_do_init
 * @par Purpose:
 *      Initialize a robo2 switch without resetting it.
 *
 * @params [in] unit - unit #
 * @return SOC_E_XXX
 */
int
soc_robo2_do_init(int unit, int reset)
{
    soc_control_t        *soc;
    uint32  enable;

    SAL_ASSERT(SOC_UNIT_VALID(unit));
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    soc = SOC_CONTROL(unit);

    if (!(soc->soc_flags & SOC_F_ATTACHED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_robo2_do_init: Unit %d not attached\n"), unit));
        return(SOC_E_UNIT);
    }


    /* Backwards compatible default */
    SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) = 1;

    if (soc->soc_flags & SOC_F_INITED) {
        /********************************************************************/
        /* If the device has already been initialized before, perform some     */
        /* de-initialization to avoid stomping on existing activities.         */
        /********************************************************************/

        soc->soc_flags &= ~SOC_F_INITED;
    }

    /* Set bitmaps according to which type of device it is */
    soc_robo2_info_config(unit, soc);

    /* Chip HW Reset */
    if (reset) {
        SOC_IF_ERROR_RETURN(soc_robo2_chip_reset(unit));
    }

    /*
     * Configure DMA channels.
     */
    if (soc_robo2_mgmt_init(unit) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unit %d: DMA initialization failed, unavailable\n"),
                   unit));
        return(SOC_E_INTERNAL);
    }

    /*
     * Chip depended misc init.
     */
    if (soc_misc_init(unit) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unit %d: Chip Misc initialization failed, unavailable\n"),
                   unit));
        return(SOC_E_INTERNAL);
    }

    /* Set feature cache */
    soc_feature_init(unit);


    /***********************************************************************/
    /* Begin initialization from a known state (reset).                    */
    /***********************************************************************/
    /*
     * PHY drivers and ID map
     */
#if 0
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_init(unit));
#endif

    /*
     * Update saved chip state to reflect values after reset.
     */

    soc->soc_flags &= SOC_F_RESET;
    /*
    Will fill more needed fields in soc_control_t.
    Ex. soc->xxx = xxx;
    */

    /*
    Any SPI Register needed be set ?
    */

    if (reset) {
        SOC_IF_ERROR_RETURN(soc_robo2_reset(unit));
        /* It will be implemented when receive chip
        init default setting from DVT. */
    }

    /* Configure board LED */
    soc_robo2_board_led_init(unit);


#ifdef IMP_SW_PROTECT
    soc_imp_prot_init(unit);

#endif /* IMP_SW_PROTECT */

    /* Check the MAC LOW POWER MODE enable */
    enable = 1;/*TBD Fix hardcoded enable value */
    SOC_AUTO_MAC_LOW_POWER(unit) = enable;
    if (!enable) {
        return SOC_E_NONE;
    }

    uint32 temp = 0;
#if 0
    /* Get current Low Power Mode */
    SOC_IF_ERROR_RETURN(
        DRV_DEV_PROP_GET(unit, DRV_DEV_PROP_LOW_POWER_ENABLE, &temp));
#endif

#if 0
    MAC_LOW_POWER_LOCK(unit);
#endif

    if (temp) {
        SOC_MAC_LOW_POWER_ENABLED(unit) = 1;
    } else {
        SOC_MAC_LOW_POWER_ENABLED(unit) = 0;
        soc->all_link_down_detected = 0;

    }

#if 0
    MAC_LOW_POWER_UNLOCK(unit);
#endif

    return(SOC_E_NONE);
}


/*
 * @par Function:
 *      soc_robo2_deinit
 * @par Purpose:
 *      DeInitialize a Device.
 *
 * @params unit - Device unit #
 * @return SOC_E_XXX
 */
int
soc_robo2_deinit(int unit)
{
    return SOC_E_NONE;
}

/*
 * @par Function:
 *    soc_robo2_reset
 * @par Purpose:
 *    Reset some registers in unit to default working values.
 *
 * @params [in] unit - unit number.
 */
int
soc_robo2_reset(int unit)
{
    /* It will be implemented when receive chip
       init default setting from DVT. */
    int rv = SOC_E_NONE;
    soc_port_t port;
    pbmp_t bm;
#if 0
    uint32 data;
    uint32 temp;
#endif

    /* Reset register */
    SOC_PBMP_ASSIGN(bm, PBMP_ALL(unit));
    PBMP_ITER(bm, port)


    /* Enable Frame Forwarding, Set Managed Mode */
#if 0

    /* LED initialization for Avenger (BCM53158)??? */
    data = 0x4320;
    SOC_IF_ERROR_RETURN(REG_WRITE_LED_FUNC1_CTLr(unit, &data));

    data = SOC_PBMP_WORD_GET(PBMP_PORT_ALL(unit), 0);
    SOC_IF_ERROR_RETURN(REG_WRITE_LED_EN_MAPr(unit, &data));
#endif

    /* power down the invalid port*/
    soc_robo2_power_down_config(unit, bm);

    return rv;
}

/*
 * @par Function:
 *    soc_robo2_reset_init
 * @par Purpose:
 *    Reset and Initialize the switch
 *
 * @params [in] unit - unit number.
 */
int
soc_robo2_reset_init(int unit)
{
    return(soc_robo2_do_init(unit, TRUE));
}


/*
 * @par Function:
 *    soc_robo2_attach
 * @par Purpose:
 *    Initialize the soc_control_t structure for a device, allocating all memory
 *    and semaphores required.
 * @params [in] unit - switch unit #.
 * @params [in] detach - Callback function called on detach.
 * @returns  SOC_E_XXX
 */
int
soc_robo2_attach(int unit)
{
    soc_control_t        *soc;
#if 0
    soc_mem_t        mem;
#endif

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "soc_robo2_attach: unit=%d\n"), unit));

    _soc_robo2_device_created(unit);

    soc = SOC_CONTROL(unit);

    /*
     *  Robo control structure
     */
    if(SOC_ROBO2_CONTROL(unit) == NULL) {
        SOC_CONTROL(unit)->drv = (soc_robo2_control_t *)
                                 sal_alloc(sizeof(soc_robo2_control_t),
                                           "soc_robo2_control");

        if (SOC_ROBO2_CONTROL(unit) == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_ROBO2_CONTROL(unit), 0, sizeof (soc_robo2_control_t));
    }

    /* Install the Interrupt Handler */
    /* Make sure interrupts are masked before connecting line. */

    /*
     * Attached flag must be true during initialization.
     * If initialization fails, the flag is cleared by soc_detach (below).
     */

    soc->soc_flags |= SOC_F_ATTACHED;

#if 0 /* [VKC Not supported ] */
    if (soc_ndev_attached++ == 0) {
        int            chip;

        /* Work to be done before the first SOC device is attached. */
        for (chip = 0; chip < SOC_ROBO2_NUM_SUPPORTED_CHIPS; chip++) {

            /* Call each chip driver's init function */
            if (soc_robo_base_driver_table[chip]->init) {

                (soc_robo_base_driver_table[chip]->init)();
            }

        }
    }
#endif


    /*
     * Set up port bitmaps.  They are also set up on each soc_robo2_init so
     * they can be changed from a CLI without rebooting.
     */
    soc_robo2_info_config(unit, soc);

    /* Set supported feature cache */
    soc_feature_init(unit);
#if 0
    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL) {
        goto error;
    }
#endif


    /* Allocate counter module resources */
    if (soc_robo_counter_attach(unit)) {
        goto error;
    }

#if 0
    /*
     * Create mutices
     */
    for (mem = 0; mem < NUM_SOC_ROBO2_MEM; mem++) {
        /*
         * should only create mutexes for valid memories.
         */
        if ((soc->memState[mem].lock =
                    sal_mutex_create(SOC_ROBO2_MEM_NAME(unit, mem))) == NULL) {
            goto error;
        }
    }

    if ((soc->arl_table_Mutex = sal_mutex_create("ARL_SW_TABLE")) == NULL) {
        goto error;
    }

    if ((soc->arl_mem_search_Mutex = sal_mutex_create("ARL_MEM_SEARCH")) ==
            NULL) {
        goto error;
    }

    if ((soc->arl_notify =
                sal_sem_create("ARL interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

    if ((soc->mac_low_power_mutex = sal_mutex_create("MAC_LOW_POWER")) ==
            NULL) {
        goto error;
    }

    if ((soc->arbiter_mutex = sal_mutex_create("ARBITER")) ==
            NULL) {
        goto error;
    }

    soc->arl_pid = SAL_THREAD_ERROR;
    soc->arl_exit = _ROBO_SEARCH_LOCK | _ROBO_SCAN_LOCK;

#endif

    /* Clear statistics */
    sal_memset(&soc->stat, 0, sizeof(soc->stat));


    if (soc_cm_get_dev_type(unit) & SOC_SPI_DEV_TYPE) {
        LOG_CLI((BSL_META_U(unit,
                            "SPI device %s attached as unit %d.\n"),
                 soc_dev_name(unit), unit));
    }

    return(SOC_E_NONE);

error:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_robo2_attach: unit %d failed\n"), unit));

    soc_robo2_detach(unit);        /* Perform necessary clean-ups on error */
    return SOC_E_MEMORY;
    return SOC_E_NONE;
}

/*
 * @par Function:
 *    soc_robo2_detach
 * @par Purpose:
 *    Clean up allocated memories and release sems
 *    Free up device access
 * @params [in] unit - switch unit #.
 * @params [in] detach - Callback function called on detach.
 * @returns  SOC_E_XXX
 */
int
soc_robo2_detach(int unit)
{
    soc_control_t   *soc;
    soc_robo2_control_t *robo;
#if 0
    soc_mem_t       mem;
#endif

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "Detaching\n")));

    soc = SOC_CONTROL(unit);

    if (NULL == soc) {
        return SOC_E_NONE;
    }

    if (0 == (soc->soc_flags & SOC_F_ATTACHED)) {
        return SOC_E_NONE;
    }

    robo = SOC_ROBO2_CONTROL(unit);

    /*
     * Call soc_robo_init to cancel link scan task, counter DMA task,
     * outstanding DMAs, interrupt generation, and anything else the
     * driver or chip may be doing.
     */

    SOC_IF_ERROR_RETURN(soc_robo2_reset_init(unit));

#if 0
    /*
     * PHY drivers and ID map
     */
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_deinit(unit));
#endif

    /* Terminate counter module; frees allocated space */
    if(soc_robo_counter_detach(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_detach: could not detach counter thread!\n")));
        return SOC_E_INTERNAL;
    }

#if 0
    if (soc->miimMutex) {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }


    SOC_FLAGS_CLR(soc, SOC_F_ATTACHED);

    if(soc_robo_arl_mode_set(unit,ARL_MODE_NONE)) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_detach: could not detach arl thread!\n")));
        return SOC_E_INTERNAL;
    }

    if (soc->arl_table_Mutex) {
        sal_mutex_destroy(soc->arl_table_Mutex);
        soc->arl_table_Mutex = NULL;
    }

    if (soc->arl_mem_search_Mutex) {
        sal_mutex_destroy(soc->arl_mem_search_Mutex);
        soc->arl_mem_search_Mutex = NULL;
    }

    for (mem = 0; mem < NUM_SOC_ROBO2_MEM; mem++) {
        if (SOC_MEM_IS_VALID(unit, mem)) {

            if (soc->memState[mem].lock != NULL) {
                sal_mutex_destroy(soc->memState[mem].lock);
                soc->memState[mem].lock = NULL;
            }
        }
    }
#endif

    if (robo!= NULL) {
        sal_free(robo);
        robo = soc->drv = NULL;
    }

    sal_free(SOC_CONTROL(unit));
    SOC_CONTROL(unit) = NULL;

    if (--soc_ndev_attached == 0) {
        /* Work done after the last SOC device is detached. */
        /* (currently nothing) */
#if 0
        if (spiMutex) {
            sal_mutex_destroy(spiMutex);
            spiMutex = NULL;
        }
#endif
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    _soc_functions_find
 * Purpose:
 *    Return function array corresponding to the driver
 * Returns:
 *    Pointer to static function array soc_functions_t
 */

static soc_functions_t *
_soc_functions_find(soc_driver_t *drv)
{
    switch (drv->type) {

#ifdef BCM_ROBO_SUPPORT
    case SOC_CHIP_BCM53158_A0:
        return &soc_robo_drv_funs;
#endif

    default:
        break;
    }

    LOG_CLI((BSL_META("_soc_functions_find: no functions for this chip\n")));
    SAL_ASSERT(0);
    return NULL;
}

/*
 * Function:
 *    _soc_device_created
 * Purpose:
 *    Called by soc_cm_device_create() to tell the driver a
 *    device is being created.
 * Notes:
 *    This function may perform management initialization only.
 *    It may not touch the chip, as its accessors will not
 *    have been setup yet.
 *
 *    This function is a result of the prior organization,
 *    and is here to make the transition easier.
 *    and this should all probably be restructured to match
 *    the new configuration code.
 */

int
_soc_robo2_device_created(int unit)
{
    soc_control_t        *soc;
    int         chip;
    uint16        dev_id = 0;
    uint8        rev_id = 0;

    if (SOC_CONTROL(unit) != NULL) {
        return SOC_E_EXISTS;
    }
    soc = SOC_CONTROL(unit) = sal_alloc(sizeof(soc_control_t), "soc_control");
    if (soc == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(soc, 0, sizeof(soc_control_t));

    if (soc_cm_get_dev_type(unit) & SOC_ETHER_DEV_TYPE) {
        if (soc_eth_unit == -1) {
            soc_eth_unit = unit;
        }
        soc_eth_ndev++;

#ifdef ETH_MII_DEBUG
        /*
         * Instantiate the driver -- Verify chip revision matches driver
         * compilation revision.
         */

        soc_cm_get_id(unit, &dev_id, &rev_id);

        soc->chip_driver = soc_robo2_chip_driver_find(dev_id, rev_id);
        soc->soc_functions = NULL;

#endif
        return 0;
    }

    /*
     * Instantiate the driver -- Verify chip revision matches driver
     * compilation revision.
     */

    soc_cm_get_id(unit, &dev_id, &rev_id);

    soc->chip_driver = soc_robo2_chip_driver_find(dev_id, rev_id);
    if (soc->chip_driver == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "_soc_robo2_device_created: unit %d has no driver "
                              "(device 0x%04x rev 0x%02x)\n"),
                   unit, dev_id, rev_id));
        return SOC_E_UNAVAIL;
    }
    soc->soc_functions = _soc_functions_find(soc->chip_driver);

    if (soc->chip_driver == NULL) {
        SOC_ROBO_FIRST_ACTIVE_DRIVER(chip);
        soc->chip_driver = soc_robo2_base_driver_table[chip];
        LOG_CLI((BSL_META_U(unit,
                            "#### \n")));
        LOG_CLI((BSL_META_U(unit,
                            "#### Chip not recognized. \n")));
        LOG_CLI((BSL_META_U(unit,
                            "#### Device id:  0x%04x.  Rev id:  0x%02x\n"),
                 dev_id, rev_id));
        LOG_CLI((BSL_META_U(unit,
                            "#### Installing default driver for device %d.\n"),
                 (soc->chip_driver->type)));
        /* SOC_CHIP_NAME(soc->chip_driver->type))); */
        LOG_CLI((BSL_META_U(unit,
                            "#### Unpredictable behavior may result.\n")));
        LOG_CLI((BSL_META_U(unit,
                            "#### \n")));
    }

    return 0;
}

/*
 * Function:
 *  soc_robo2_mgmt_init
 * Purpose:
 *  Initialize the management mode/IMP configuration.
 * Parameters:
 *  unit - SOC unit #
 * Returns:
 *  SOC_E_NONE - Success
 *  SOC_E_XXX
 */

int
soc_robo2_mgmt_init(int unit)
{

#if 0
    uint32 temp, reg_value;
    uint32  rgmii = 0;
    uint32 imp1_rgmii = 0;

    /* Enable Frame Forwarding, Set Managed Mode */
    reg_value = 0;

    REG_READ_SWMODEr(unit, &reg_value);
    temp = 1;
    soc_SWMODEr_field_set(unit, &reg_value, SW_FWDG_ENf, &temp);
    soc_SWMODEr_field_set(unit, &reg_value, SW_FWDG_MODEf, &temp);
    REG_WRITE_SWMODEr(unit, &reg_value);

    if (unit == soc_mii_unit) {

        soc_dma_initialized = 1;

        if (SOC_IS_ROBO_ARCH_VULCAN(unit) || SOC_IS_DINO(unit)) {

            /* force enabling the BRCM header tag :
             *  - 53115/53118 allowed user to disable the BRCM header
             *
             */
            if (SOC_IS_ROBO_ARCH_VULCAN(unit)) {
                reg_value = 0;
                {
                    temp = 1;
                }

                soc_BRCM_HDR_CTRLr_field_set(unit, &reg_value,
                                             BRCM_HDR_ENf, &temp);
                REG_WRITE_BRCM_HDR_CTRLr(unit, &reg_value);
            }

            if (SOC_IS_LOTUS(unit)) {
                REG_READ_STRAP_VALUEr(unit, &reg_value);
                soc_STRAP_VALUEr_field_get(unit, &reg_value,
                                           FINAL_MII1_MODEf, &temp);

                /* check if RGMII mode */
                if (temp == 0x5) {
                    rgmii = 1;
                }

                if (rgmii) {
                    /* Select 2.5V as MII voltage */
                    REG_READ_SWITCH_CTRLr(unit, &reg_value);
                    temp = 1; /* 2.5V */
                    soc_SWITCH_CTRLr_field_set(unit,
                                               &reg_value, MII1_VOL_SELf, &temp);
                    REG_WRITE_SWITCH_CTRLr(unit, &reg_value);

                    /* Enable RGMII tx/rx clock delay mode */
                    REG_READ_IMP_RGMII_CTL_GPr(unit,
                                               CMIC_PORT(unit), &reg_value);
                    temp = 1;
                    soc_IMP_RGMII_CTL_GPr_field_set(unit,
                                                    &reg_value, RXC_DLL_DLY_ENf, &temp);
                    soc_IMP_RGMII_CTL_GPr_field_set(unit,
                                                    &reg_value, TXC_DLL_DLY_ENf, &temp);
                    REG_WRITE_IMP_RGMII_CTL_GPr(unit,
                                                CMIC_PORT(unit), &reg_value);
                }

            } else if (SOC_IS_STARFIGHTER(unit) || SOC_IS_POLAR(unit)) {
                if ((reg_value & 0xc0) == 0) {
                    rgmii = 1;
                }

                if (rgmii) {

                    /* Enable RGMII tx/rx clock delay mode */
                    REG_READ_IMP_RGMII_CTL_GPr(unit,
                                               CMIC_PORT(unit), &reg_value);
                    temp = 1;
                    soc_IMP_RGMII_CTL_GPr_field_set(unit,
                                                    &reg_value, EN_RGMII_DLL_RXCf, &temp);
                    soc_IMP_RGMII_CTL_GPr_field_set(unit,
                                                    &reg_value, EN_RGMII_DLL_TXCf, &temp);
                    REG_WRITE_IMP_RGMII_CTL_GPr(unit,
                                                CMIC_PORT(unit), &reg_value);
                }

                REG_READ_STRAP_PIN_STATUSr(unit, &reg_value);
                /* check if IMP1 is RGMII mode */
                {
                    if ((reg_value & 0x3000) == 0) {
                        imp1_rgmii = 1;
                    }
                }
                if (imp1_rgmii) {

                    /* Enable RGMII tx/rx clock delay mode */
                    REG_READ_PORT5_RGMII_CTL_GPr(unit,
                                                 CMIC_PORT(unit), &reg_value);
                    temp = 1;
                    soc_PORT5_RGMII_CTL_GPr_field_set(unit,
                                                      &reg_value, EN_RGMII_DLL_RXCf, &temp);
                    soc_PORT5_RGMII_CTL_GPr_field_set(unit,
                                                      &reg_value, EN_RGMII_DLL_TXCf, &temp);
                    REG_WRITE_PORT5_RGMII_CTL_GPr(unit,
                                                  CMIC_PORT(unit), &reg_value);
                }
            } else if (SOC_IS_STARFIGHTER3(unit)) {
#ifdef BCM_STARFIGHTER3_SUPPORT
                REG_READ_STRAP_PIN_STATUSr(unit, &reg_value);
                if ((reg_value & BCM53134_STRAP_PIN_IMP_MODE_MASK)
                        == BCM53134_STRAP_PIN_IMP_MODE_RGMII) {
                    rgmii = 1;
                }
                if (rgmii) {

                    /* Enable RGMII tx/rx clock delay mode */
                    REG_READ_IMP_RGMII_CTL_GPr(unit,
                                               CMIC_PORT(unit), &reg_value);
                    temp = 1;
                    soc_IMP_RGMII_CTL_GPr_field_set(unit,
                                                    &reg_value, EN_RGMII_DLL_RXCf, &temp);
                    soc_IMP_RGMII_CTL_GPr_field_set(unit,
                                                    &reg_value, EN_RGMII_DLL_TXCf, &temp);
                    REG_WRITE_IMP_RGMII_CTL_GPr(unit,
                                                CMIC_PORT(unit), &reg_value);
                }

                REG_READ_STRAP_PIN_STATUSr(unit, &reg_value);
                /* check if IMP1 is RGMII mode */
                if ((reg_value & BCM53134_STRAP_PIN_WAN_MODE_MASK)
                        == BCM53134_STRAP_PIN_WAN_MODE_RGMII) {
                    imp1_rgmii = 1;
                }
                if (imp1_rgmii) {

                    /* Enable RGMII tx/rx clock delay mode */
                    REG_READ_PORT5_RGMII_CTL_GPr(unit,
                                                 CMIC_PORT(unit), &reg_value);
                    temp = 1;
                    soc_PORT5_RGMII_CTL_GPr_field_set(unit,
                                                      &reg_value, EN_RGMII_DLL_RXCf, &temp);
                    soc_PORT5_RGMII_CTL_GPr_field_set(unit,
                                                      &reg_value, EN_RGMII_DLL_TXCf, &temp);
                    REG_WRITE_PORT5_RGMII_CTL_GPr(unit,
                                                  CMIC_PORT(unit), &reg_value);
                }
#endif /* BCM_STARFIGHTER3_SUPPORT */
            }

            /*
             * Force MII Software Override, set 100 Full Link up
             * in MII port of mgnt chip
             */
            reg_value = 0;
            temp = 1;
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, MII_SW_ORf, &temp);
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, DUPLX_MODEf, &temp);
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, LINK_STSf, &temp);
            if (SOC_IS_LOTUS(unit) || SOC_IS_STARFIGHTER(unit) ||
                    SOC_IS_BLACKBIRD2(unit) || SOC_IS_POLAR(unit) ||
                    SOC_IS_STARFIGHTER3(unit)) {
                if (rgmii) {
                    /* Speed 1000MB */
                    temp = 0x2;
                }
                soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, SPEEDf,&temp);
            } else {
                if (rgmii) {
                    /* Speed 1000MB */
                    temp = 0x2;
                }
                soc_STS_OVERRIDE_IMPr_field_set
                (unit, &reg_value, SPEED_Rf, &temp);
            }
            REG_WRITE_STS_OVERRIDE_IMPr
            (unit, CMIC_PORT(soc_mii_unit), &reg_value);
        }

        if(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            /*
             * Force MII Software Override, set 100 Full Link up
             * in MII port of mgnt chip
             */
            reg_value = 0;
            temp = 1;
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, SW_ORDf, &temp);
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, DUPLX_MODEf, &temp);
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, LINK_STSf, &temp);
            soc_STS_OVERRIDE_IMPr_field_set(unit, &reg_value, GIGA_SPEEDf, &temp);
            REG_WRITE_STS_OVERRIDE_IMPr(unit, CMIC_PORT(soc_mii_unit), &reg_value);
        }

        /*
         * Enable All flow (unicast, multicast, broadcast)
         * in MII port of mgmt chip
         */
        if (SOC_IS_ROBO_ARCH_VULCAN(unit)) {
            reg_value = 0;
            temp = 1;
            soc_IMP_CTLr_field_set(unit, &reg_value, RX_UCST_ENf, &temp);
            soc_IMP_CTLr_field_set(unit, &reg_value, RX_MCST_ENf, &temp);
            soc_IMP_CTLr_field_set(unit, &reg_value, RX_BCST_ENf, &temp);
            REG_WRITE_IMP_CTLr(unit, CMIC_PORT(soc_mii_unit), &reg_value);
        } else {
            reg_value = 0;
            temp = 1;
            soc_MII_PCTLr_field_set(unit, &reg_value, MIRX_UC_ENf, &temp);
            soc_MII_PCTLr_field_set(unit, &reg_value, MIRX_MC_ENf, &temp);
            soc_MII_PCTLr_field_set(unit, &reg_value, MIRX_BC_ENf,&temp);
            REG_WRITE_MII_PCTLr(unit, CMIC_PORT(soc_mii_unit), &reg_value);
        }
    }

    /* Identify mgmt port is MII port */
    reg_value = 0;
    temp = 2;
    soc_GMNGCFGr_field_set(unit, &reg_value, FRM_MNGPf, &temp);
    temp = 1;
    if (SOC_IS_TBX(unit)) {
        soc_GMNGCFGr_field_set(unit, &reg_value, RX_BPDU_ENf, &temp);
    } else {
        soc_GMNGCFGr_field_set(unit, &reg_value, RXBPDU_ENf, &temp);
    }
    REG_WRITE_GMNGCFGr(unit, &reg_value);
#endif
    return SOC_E_NONE;
}

#ifndef CONFIG_BOOTLOADER
/*
 * @Fixme - Temp changes
 */
#include <fsal/cosq.h>
#include <fsal/pktio.h>

int soc_pkt_loop_callback(void *buffer, int buffer_size,
                     cbx_packet_attr_t  *packet_attr,
                     cbx_packet_cb_info_t *cb_info)
{
    cbx_packet_attr_t txattr;
    cbx_packet_attr_t_init(&txattr);
    txattr.vlanid = packet_attr->vlanid;
    txattr.dest_index = packet_attr->source_index;
    //txattr.flags = CBX_PACKET_TX_FWD_UCAST;
    txattr.flags = CBX_PACKET_TX_FWD_RAW;
    cbx_pktio_packet_send(buffer, buffer_size, &txattr);
    //Buffer will be freed by send
    sal_free(packet_attr);
    return CBX_RX_HANDLED;
}
int soc_pkt_callback(void *buffer, int buffer_size,
                     cbx_packet_attr_t  *packet_attr,
                     cbx_packet_cb_info_t *cb_info)
{
    int i;
    uint8 *ptr= (uint8*)buffer;
    sal_printf("Packet Callback\n");
    sal_printf("Attr: flags=%04x,vlanid=%d,tc=%d,source_index=%d,dest_index=%d\n",
               packet_attr->flags, packet_attr->vlanid, packet_attr->tc,
               packet_attr->source_index,packet_attr->dest_index);
    sal_printf("Attr: source_port=%x,dest_port=%x,mcastid=%x,trapid=%x,trap_data=%x\n",
               packet_attr->source_port, packet_attr->dest_port, packet_attr->mcastid,
               packet_attr->trapid,packet_attr->trap_data);
    for (i = 0; i < buffer_size; i++) {
        if (i%16==0) sal_printf("\n%04x: ", i);
        sal_printf(" %02x", ptr[i]);
    }
    sal_printf("\n");
    sal_free(buffer);
    sal_free(packet_attr);
    return CBX_RX_HANDLED;
}

uint8 data_sram_tagged[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbb, 0x81, 0x00, 0x00, 0x64,
    0x08, 0x99, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
};
uint8 data_sram_untagged[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbb, 0x08, 0x99, 0xaa, 0xab,
    0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb,
    0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb,
    0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
};

uint8 data_arp[72] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0xAA, 0xB1, 0x00, 0x02,
    0x00, 0x01, 0xE0, 0x01, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x11,
    0x22, 0x33, 0x44, 0x55, 0xc0, 0xa8, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8,
    0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/*
 * soc_pkt_send
 *    Port - Port index or Port ID
 *    Vlan - Vlan id for packet
 *    MGID - Multicast ID
 *    Type - Tagged (1) Untagged (0)
 *    Mode - CBX_PACKET_TX_FWD_xx
 */
int soc_pkt_send(uint32 port, int vlan, uint32 mgid, int type, int mode)
{
    int rc = SOC_E_NONE;
    cbx_packet_attr_t attr;
    cbx_packet_attr_t_init(&attr);
    if ((vlan < 0) || (vlan > 4096)) {
        sal_printf("Invalid Vlan id: %d", vlan);
        return SOC_E_PARAM;
    }
    switch (mode) {
    case CBX_PACKET_TX_FWD_RAW:
        sal_printf("Sending L2 Packet RAW mode");
        attr.vlanid = vlan;
        attr.dest_index = port;
        attr.flags = CBX_PACKET_TX_FWD_RAW;
        break;
    case CBX_PACKET_TX_FWD_UCAST:
        sal_printf("Sending L2 Packet Directed UCAST mode");
        attr.vlanid = vlan;
        attr.dest_port = port;
        attr.flags = CBX_PACKET_TX_FWD_UCAST;
        break;
    case CBX_PACKET_TX_FWD_MCAST:
        sal_printf("Sending L2 Packet MCAST mode");
        attr.vlanid = vlan;
        attr.mcastid = mgid;
        attr.flags = CBX_PACKET_TX_FWD_MCAST;
        break;
    case CBX_PACKET_TX_FWD_SWITCH:
        sal_printf("Sending L2 Packet SWITCHED mode");
        attr.flags = CBX_PACKET_TX_FWD_SWITCH;
        break;
    default:
        sal_printf("Invalid Packet Transmit type %d", type);
        return SOC_E_PARAM;
    }
    if (type == 2) {
        rc = cbx_pktio_packet_send(data_arp, sizeof(data_arp), &attr);
    } else if (type == 1) {
        rc = cbx_pktio_packet_send(data_sram_tagged, sizeof(data_sram_tagged), &attr);
    } else {
        rc = cbx_pktio_packet_send(data_sram_untagged, sizeof(data_sram_untagged), &attr);
    }
    return rc;
}

int pktio_init = 0;
static cbx_pktio_rx_cb_handle rx_cb_handle = NULL;
static cbx_pktio_rx_cb_handle rx_lcb_handle = NULL;

int
soc_pktio_init(int unit)
{
    cbx_packet_cb_info_t cbinfo;
    if ((unit < 0) || (unit >= 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    if ((pktio_init == 0) && (unit == 0)) {
        sal_memset(&cbinfo, 0, sizeof(cbinfo));
        cbinfo.flags = CBX_PKTIO_CALLBACK_TRAP;
        cbinfo.name = "Trap packets handler";
        cbx_pktio_register(cbinfo, soc_pkt_callback, &rx_cb_handle);
        cbinfo.flags = CBX_PKTIO_CALLBACK_IP;
        cbinfo.name = "IP packets handler";
        cbx_pktio_register(cbinfo, soc_pkt_loop_callback, &rx_lcb_handle);
        pktio_init = 1;
    }
    return 0;
}

/*
 *    xlreg_read
 * @par Purpose:
 *    read 4 byte value from registers for debug
 * @params [in] unit  unit number
 * @params [in] port  -1 (control reg) 0-3 (port reg)
 * @params [in] addr - register address
 * @returns  SOC_E_XXX
 */
int
xlreg_read(int unit, int port, uint32 addr)
{
    int value[2], rv = 0;
    int len = 8;
    if ((unit < 0) || (unit >= 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    if (port < 0) {
        port = REG_PORT_ANY;
    }
    if ((addr >= 0x200) && (addr <= 0x230)) {
        len = 4;
    }
    rv = drv_robo2_apb2pbus_reg_read(unit, port, addr, &value[0], len);
    if( rv != 0)
        return rv;
    if (port < 0) {
        if (len == 4)
            sal_printf("XlReg Addr 0x%x = %d (0x%x)\n", addr, value[0], value[0]);
        else
            sal_printf("XlReg Addr 0x%x = %d%d (0x%x%08x)\n", addr, value[1], value[0], value[1], value[0]);
    } else {
        if (len == 4)
            sal_printf("XlReg Port %d Addr 0x%x = %d (0x%x)\n", port, addr, value[0], value[0]);
        else
            sal_printf("XlReg Port %d Addr 0x%x = %d%d (0x%x%08x)\n", port, addr, value[1], value[0], value[1], value[0]);
    }
    return rv;
}

/*
 *     xlreg_write
 * @par Purpose:
 *     write  4 byte value into the register for debug
 * @params [in] unit  unit number
 * @params [in] port  -1 (control reg) 0-3 (port reg)
 * @params [in] addr  register address
 * @params [in] value  4 byte data to write
 * @returns  SOC_E_XXX
 */
int
xlreg_write(int unit, int port, uint32 addr, uint32 valuelo, uint32 valuehi)
{
    int rv = 0;
    int len = 8;
    uint32 value[2];
    if ((unit < 0) || (unit >= 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    if (port < 0) {
        port = REG_PORT_ANY;
    }
    if ((addr >= 0x200) && (addr <= 0x230)) {
        len = 4;
        value[0] = valuelo;
    } else {
        value[0] = valuelo;
        value[1] = valuehi;
    }
    rv = drv_robo2_apb2pbus_reg_write(unit, port, addr, &value, len);
    return rv;
}

/*
 *
 *    reg_read
 * @par Purpose:
 *    read 4 byte value from registers for debug
 * @params [in] unit  unit number
 * @params [in] addr - register address
 * @returns  SOC_E_XXX
 */
int
reg_read(int unit, uint32 addr)
{
    int value = 0, rv = 0;
    if ((unit < 0) || (unit >= 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    rv = drv_robo2_reg_read(unit, addr, &value, 4);
    if( rv != 0)
        return rv;
    sal_printf("Reg Addr 0x%x = %d (0x%x)\n", addr, value, value);
    return rv;
}
#if 0
typedef enum soc_block_e {
  ipp = 0,
  epp = 1,
  bmu = 2,
  pqm = 3,
  pqs = 4,
  itm = 5,
  etm = 6
} soc_block_t;
#endif
int
reg_block_read(int unit, uint32_t block_name)
{
    int value = 0, rv = 0;
    int start = 0;
    int end = 0;
    int index = 0;
    uint32_t addr = 0;
    int i = 0;
    switch (block_name) {
        case 0:
           start = CB_IPP_BA_QUEUE_CONFIGr_ROBO2;
           end  =  CB_IPP_VSIT_IA_WDATA_PART3r_ROBO2;
           break;
        case 1:
           start = CB_EPP_BIMC_CMD0r_ROBO2;
           end  =  CB_EPP_TXREQ_CM_STATUSr;
           break;
        case 2:
           start = CB_BMU_BIMC_CMD0r_ROBO2;
           end  =  CB_BMU_SPG_PP_BMP_CONFIG_IA_WDATA_PART0r_ROBO2;
           break;
        case 3:
           start = CB_PQM_ADMCTRLCFGr_ROBO2;
           end  =  CB_PQM_WPT_IA_WDATA_PART0r_ROBO2;
           break;
        case 4:
           start = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A0r_ROBO2;
           end  =  CB_PQS_WEIGHT_LOW_QUEUE_PORT_A15r_ROBO2;
           break;
        case 5:
           start = CB_ITM_ARLFM0_IA_CAPABILITYr_ROBO2;
           end  =  CB_ITM_SPARE_OUTr_ROBO2;
           break;
        case 6:
           start = CB_ETM_BIMC_CMD0r_ROBO2;
           end  = CB_ETM_VTCT_IA_WDATA_PART1r_ROBO2;
           break;
        default:
          sal_printf( " dumping for all blocks \n");
          start = CB_IPP_BA_QUEUE_CONFIGr_ROBO2;
          end  =  (NUM_SOC_ROBO2_REG - 1);
          break;
    }
    for (i = start; i <= end; i++) {
        addr =  drv_robo2_reg_addr_get(unit, i, REG_PORT_ANY, index);
        if ((i >= M7SS_SPI0_SSPCPSRr_ROBO2) &&
                (i <= M7SS_SPI0_SSPTDRr_ROBO2)) {
               if (SOC_IS_CASCADED(0)) {
                   /* Spi registers are locked cannot access them */
                   continue;
               }
        }
        if (i >= XLMAC_CTRLr_ROBO2) {
            rv = drv_robo2_apb2pbus_reg_read(unit, REG_PORT_ANY, addr, &value, 4);
        } else {
            rv = drv_robo2_reg_read(unit, addr, &value, 4);
        }
        if( rv != 0)
            return rv;
#if !defined(SOC_NO_NAMES)
        sal_printf(" %s Reg Addr 0x%x = %d (0x%x)\n", soc_robo2_reg_name[i],addr, value, value);
#else
        sal_printf("Reg Addr 0x%x = %d (0x%x)\n", addr, value, value);
#endif
    }
    return rv;
}

/*
 *
 *    reg_write
 * @par Purpose:
 *    write  4 byte value into the register for debug
 * @params [in] unit  unit number
 * @params [in] addr - register address
 * @params [in] value - 4 byte data to write
 * @returns  SOC_E_XXX
 */
int
reg_write(int unit, uint32 addr, int value)
{
    int rv = 0;
    if ((unit < 0) || (unit >= 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    rv = drv_robo2_reg_write(unit, addr, &value, 4);
    return rv;
}

/*
 *
 *    dump_table
 * @par Purpose:
 *    dump table tid from index idx till idx + len
 * @params [in] tid - table id
 * @params [in] idx - index of the table
 * @params [in] len - number of entries to dump
 * @returns  SOC_E_XXX
 */
int
dump_table(soc_robo2_table_id_t tid, int idx, int len)
{
    int rv = 0;
    int i = 0;
    int unit = 0;
    int num_units = 1;

    if (SOC_IS_CASCADED(0)) {
        num_units = 2;
    }

    for (i = idx; i < (idx + len); i++) {
        for (unit = 0; unit < num_units; unit++) {
            if (!unit) {
                sal_printf("\t\t PRIMARY AVENGER\n");
            } else {
                sal_printf("\t\tSECONDARY AVENGER\n");
            }

            rv = soc_robo2_table_dump(unit, tid, i);
            if(rv < 0) {
                if (rv == SOC_E_INIT) {
                    sal_printf("dump_table ERROR: Check if table is enabled\n");
                }
                return rv;
            }
        }
    }
    return rv;
}

char*
ipp_counter_name(int index)
{
    char *name = "UNKNWON";
    switch (index) {
    case 0:
        name = "FWD_OK";
        break;
    case 1:
        name = "DST_DENIED";
        break;
    case 2:
        name = "SRC_DENIED";
        break;
    case 3:
        name = "TRAP_DROP";
        break;
    case 4:
        name = "CFP_DROP";
        break;
    case 5:
        name = "STP_DROP";
        break;
    case 6:
        name = "SLF_DROP";
        break;
    case 7:
        name = "ULF_DROP";
        break;
    case 8:
        name = "MLF_DROP";
        break;
    case 9:
        name = "BC_DROP";
        break;
    case 10:
        name = "SLI_DROP";
        break;
    }
    return name;
}

void
dump_cb_counters(int unit)
{
    uint32 regval = 0;
    int i=0;
    if ((unit < 0) || (unit >= 2)) return;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return;
    for (i=0; i < 15; i++) {
        REG_READ_CB_SIA_NISI_DROP_COUNT_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
            sal_printf("CB_SIA_nisi_drop_count_count_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_SIA_HISI_DROP_COUNT_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SIA_hisi_drop_count_count_a => %08x\n", regval);
    }
    for (i=0; i < 15; i++) {
        REG_READ_CB_SIA_MIRR_DROP_COUNT_COUNT_Ar(unit,i , &regval);
        if (regval != 0) {
             sal_printf("CB_SIA_mirr_drop_count_count_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_SIA_HISI_OCTET_COUNT_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SIA_hisi_octet_count_count_a => %08x\n", regval);
    }
    for (i=0; i < 11; i++) {
        REG_READ_CB_IPP_FORWARD_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_IPP_forward_count_a[%s] => %08x\n", ipp_counter_name(i), regval);
        }
    }
    for (i =0; i < 2; i++)  {
        REG_READ_CB_IPP_SIA_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_IPP_sia_count_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_IPP_SBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_IPP_sbe_count_a => %08x\n", regval);
    }
    REG_READ_CB_IPP_MBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_IPP_mbe_count_a => %08x\n", regval);
    }
    for (i =0; i < 4; i++)  {
        REG_READ_CB_IPP_TST_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_IPP_tst_a[%d] => %08x\n", i, regval);
             REG_WRITE_CB_IPP_TST_Ar(unit, i, &regval);
        }
    }
    REG_READ_CB_CFP_SBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_CFP_sbe_count_a => %08x\n", regval);
    }
    REG_READ_CB_CFP_MBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_CFP_mbe_count_a => %08x\n", regval);
    }
    REG_READ_CB_BMU_BMU_QUAL_DROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_BMU_bmu_qual_drop_count_a => %08x\n", regval);
    }
    REG_READ_CB_BMU_BMU_CH_PKT_DROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_BMU_bmu_ch_pkt_drop_count_a => %08x\n", regval);
    }
    REG_READ_CB_BMU_BMU_STM_PKT_DROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_BMU_bmu_stm_pkt_drop_count_a => %08x\n", regval);
    }
    for (i=0; i<31; i++) {
        REG_READ_CB_BMU_METER_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_meter_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_DOMAIN_ADMISSION_DROP_ALL_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_domain_admission_drop_all_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_DOMAIN_ADMISSION_DROP_YELLOW_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_domain_admission_drop_yellow_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_DOMAIN_ADMISSION_DROP_RED_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_domain_admission_drop_red_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_GLOBAL_ADMISSION_DROP_ALL_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_global_admission_drop_all_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_GLOBAL_ADMISSION_DROP_YELLOW_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_global_admission_drop_yellow_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_GLOBAL_ADMISSION_DROP_RED_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_global_admission_drop_red_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_IPP_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_ipp_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_CNTXT_RESOURCE_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_cntxt_resource_drop_count_a[%d] => %08x\n", i, regval);
        }
    }

    for (i=0; i < 16; i++) {
        REG_READ_CB_PQS_PER_QUEUE_EN_QUEUE_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQS_per_queue_en_queue_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQS_PER_PORT_STATUS_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQS_per_port_status_a[%d] => %08x\n", i, regval);
        }
    }

    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 0);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 1);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 2);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 3);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 4);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 5);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 6);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 7);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 8);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 9);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 10);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 11);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 12);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 13);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 14);
    PRINT_PER_QUEUE_PKT_CNT_FOR_PORT(unit, 15);


    REG_READ_CB_PQM_PMAPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_pmapdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_DLIDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_dlidrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_DPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_dpdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcenq_count_a => %08x\n", regval);
    }

    REG_READ_CB_PQM_MRRENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrrenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TGENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tgenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCQADDENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcqaddenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MRRQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrrqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MRRQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrrqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TGQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tgqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TGQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tgqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UC_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_uc_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MC_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mc_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MRR_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrr_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TG_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tg_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCREP_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcrep_deq_count_a => %08x\n", regval);
    }

    REG_READ_CB_PQM_UCSLIDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucslidrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCSPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucspdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCSTGETDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucstgetdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCPGMAPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucpgmapdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCCASDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_uccasdrop_count_a => %08x\n", regval);
    }

    for (i=0; i < 128; i++) {
        REG_READ_CB_PQM_ADM_ENQ_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_enq_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQM_ADM_WRED_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_wred_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQM_ADM_THRSH_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_thrsh_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQM_ADM_DEQ_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_deq_count_a[%d] => %08x\n", i, regval);
        }
    }

    for (i=0; i < 16; i++) {
        REG_READ_CB_EPP_COUNT_EPP_OK_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_EPP_count_epp_ok_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_EPP_COUNT_EPP_ERR_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_EPP_count_epp_err_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_EPP_COUNT_EPP_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_EPP_count_epp_drop_count_a[%d] => %08x\n", i, regval);
        }
    }

    REG_READ_CB_SID_MIRR_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SID_mirr_count_a => %08x\n", regval);
    }
    REG_READ_CB_SID_HESI_OCTET_CNT_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SID_hesi_octet_cnt_count_a => %08x\n", regval);
    }
}

void
dump_hpa_regs(int unit)
{
    uint32 regval = 0;
    if ((unit < 0) || (unit >= 2)) return;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return;
    REG_READ_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC0_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC1_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC2_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC3_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC4_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC5_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC6_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_TX_DESC7_HPA_TX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC0_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC1_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC2_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC3_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC4_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC5_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC6_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_31_0r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_31_0 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_63_32r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_63_32 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_127_96r(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_RX_DESC7_HPA_RX_DESCRIPTOR_127_96 => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROLr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_CONTROL => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_AXI_CONTROLr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_AXI_CONTROL => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_DESC_SIZEr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_RX_DESC_SIZE => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_MAX_BURST_LENr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_MAX_BURST_LEN => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_TX_HPA_CUR_DESC_PTRr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_TX_HPA_CUR_DESC_PTR => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_TX_CPU_CUR_DESC_PTRr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_TX_CPU_CUR_DESC_PTR => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_HPA_CUR_DESC_PTRr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_RX_HPA_CUR_DESC_PTR => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_RX_CPU_CUR_DESC_PTRr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_RX_CPU_CUR_DESC_PTR => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_HESI_CREDIT_LIMITr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_HESI_CREDIT_LIMIT => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_STSr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_INT_STS => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_INT_ENr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_INT_EN => %08x\n",regval);
    }
    REG_READ_AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_STSr(unit, &regval);
    if (regval != 0) {
        sal_printf("AVR_HPA_BLOCK_CTRL_STS_HPA_DMA_STS => %08x\n",regval);
    }
}
#endif /* CONFIG_BOOTLOADER */

#if defined(CONFIG_BOOTLOADER) && defined(CONFIG_CASCADED_MODE)
int soc_cascaded_mode_enable(int unit)
{
    int rv = SOC_E_NONE;
    uint32 regval = 0;
    if (unit == 0) {
        soc_spi_master_enable();
        /* Attempt to access the Slave before confirming cascade operation */
        if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
            SOC_INFO(unit).cascaded_mode = 1;
            dbg_printf("Cascade Configuration Enabled\n");
            SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
        }
    }
    return rv;
}
#endif /* defined(CONFIG_BOOTLOADER) && defined(CONFIG_CASCADED_MODE) */


int
soc_robo2_essential_init(int unit)
{
    soc_control_t *soc;
    int idx = 0;
    uint8_t rev_id;
    int rv;

    if (SOC_CONTROL(unit) != NULL) {
        return SOC_E_EXISTS;
    }
    soc = SOC_CONTROL(unit) = (soc_control_t*)sal_alloc(sizeof(soc_control_t), "soc_control");
    if (soc == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(soc, 0, sizeof(soc_control_t));
    soc->apb2pbus_access = sal_mutex_create("APB2PBUS");
    if (soc->apb2pbus_access == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(BCM_53158_A0) || defined(BCM_53158_B0) || defined(BCM_53158_B1)
#ifdef CONFIG_EXTERNAL_HOST
#ifdef CONFIG_USE_UART_AS_CPU_INTF
    rv = soc_uart_spi_read(0, BCM53158_CHIP_REV_REG, &rev_id, 1);
    SAL_ASSERT(rv == 0);
#else
    rv = soc_spi_read(0, BCM53158_CHIP_REV_REG, &rev_id, 1);
    SAL_ASSERT(rv == 0);
#endif
#else
    rev_id = *(uint32 *)BCM53158_CHIP_REV_REG & 0xFF;
#endif
#endif

    switch (rev_id) {
        case BCM53158_A0_REV_ID:
            idx = soc_robo2_chip_type_to_index(SOC_CHIP_BCM53158_A0);
            soc->info.driver_type = SOC_CHIP_BCM53158_A0;
            break;

        case BCM53158_B0_REV_ID:
            idx = soc_robo2_chip_type_to_index(SOC_CHIP_BCM53158_B0);
            soc->info.driver_type = SOC_CHIP_BCM53158_B0;
            break;

        case BCM53158_B1_REV_ID:
            idx = soc_robo2_chip_type_to_index(SOC_CHIP_BCM53158_B1);
            soc->info.driver_type = SOC_CHIP_BCM53158_B1;
            break;

        default:
            idx = SOC_CHIP_TYPES_COUNT;
            break;
    }
    SAL_ASSERT(idx < SOC_CHIP_TYPES_COUNT);
    soc->chip_driver = soc_robo2_base_driver_table[idx];
    if (!unit) {
        /* This assumes that same OTP values on both Avengers in cascade setup*/
        rv = soc_otp_status_read(unit);
        if (rv != SOC_E_NONE) {
            return rv;
        }
    }

#if defined(CONFIG_BOOTLOADER) && defined(CONFIG_CASCADED_MODE)
    rv = soc_cascaded_mode_enable(unit);
    if (rv != SOC_E_NONE) {
        sal_free(soc);
        return rv;
    }
#else /* !CONFIG_BOOTLOADER && !CONFIG_CASCADED_MODE */
    rv = soc_bootstrap_init(unit);
    if (rv != SOC_E_NONE) {
        sal_free(soc);
        return rv;
    }
    /* LED_FIRMWARE_LOAD_ENABLE */
    bcm53158_initialize_led(unit);
#endif
    soc->soc_flags |= SOC_F_ATTACHED;

    return SOC_E_NONE;
}


void
dump_port_status(int unit)
{
    if ((unit < 0) || (unit >= 2)) return;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return;
    soc_port_t port;
    uint32 maclb = 0, phylb = 0, linkstatus = 0, physpeed = 0, macspeed = 0;
    uint32 regval = 0;
    REG_READ_AVR_PORTMACRO_PORT_MACRO_STATUS1r(unit, &regval);
    sal_printf("PortMacro Status: %x\n", regval);
    sal_printf("+-----------------------------------------------------+\n");
    sal_printf("| Port |  MacSpeed |  PhySpeed | Link | MacLb | PhyLB |\n");
    sal_printf("+-----------------------------------------------------+\n");
    SOC_PBMP_ITER(PBMP_ALL(unit), port) {
        maclb = 0; phylb = 0; linkstatus = 0; physpeed = 0; macspeed = 0;
        mac_speed_get(unit, port, (int*)&macspeed);
        mac_loopback_get(unit, port, (int*)&maclb);
#ifndef EMULATION_BUILD
        avng_phy_speed_get(unit, port, &physpeed);
        avng_phy_loopback_get(unit, port, &phylb);
        if (port != 15) {
            cbxi_port_link_status_get(unit, port, &linkstatus);
        }
#endif

        if (maclb) {linkstatus = 1;}
        if (port == 15) {macspeed = 1000;}
        sal_printf("|  %3d |   %5d   |   %5d   | %4s |  %3s  |  %3s  |\n",
                    port, macspeed, physpeed,
                    linkstatus ? "UP  " : "DOWN",
                    maclb ? "LB" : "NO",
                    phylb ? "LB" : "NO");
    }
    sal_printf("+-----------------------------------------------------+\n");
}

void
dump_l2_table(int unit)
{
    int rv, i, max, max1 = 0;
    arlfm0_t entry0;
    arlfm1_ght_h0_t entry1;
    arlfm_t entry2;

    if ((unit < 0) || (unit >= 2)) return;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return;
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        max = soc_robo2_arlfm_max_index(unit);
        sal_printf("|  IDX  |");
        sal_printf(" VALID |");
        sal_printf("  DST  |");
        sal_printf(" DST_TYPE |");
        sal_printf(" HIT |");
        sal_printf("      MAC       |");
        sal_printf("  VLAN  |");
        sal_printf(" UPD_CTRL |");
        sal_printf(" FWD_CTRL |\n");

    } else {
        max = soc_robo2_arlfm0_max_index(unit);
        max1 = soc_robo2_arlfm1_ght_h0_max_index(unit);
    }
    for (i=0; i < max; i++) {
        if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
            rv = soc_robo2_arlfm_get(unit, i, &entry2);
            if (rv == SOC_E_NONE) {
                if (entry2.valid != 0) {
                    sal_printf("| %5d |",   i);
                    sal_printf("  0x%1x  |", entry2.valid);
                    sal_printf(" 0x%02x  |", entry2.dst);
                    sal_printf("   0x%1x   | ", entry2.dst_type);
                    sal_printf(" 0x%1x |", entry2.hit);
                    sal_printf(" 0x%04x%08x |", entry2.mac_hi, entry2.mac_lo);
                    sal_printf(" 0x%04x |", entry2.fid);
                    sal_printf("    0x%1x   |", entry2.upd_ctrl);
                    sal_printf("    0x%1x   |\n", entry2.fwd_ctrl);
                }
            }
        } else {
            rv = soc_robo2_arlfm0_get(unit, i, &entry0);
            if (rv == SOC_E_NONE) {
                if (entry0.valid != 0) {
                    sal_printf("ARMFM0 Index = %d\n",   i);
                    sal_printf("VALID=0x%1x",      entry0.valid);
                    sal_printf(" DST=0x%x",        entry0.dst);
                    sal_printf(" DST_TYPE=0x%1x",  entry0.dst_type);
                    sal_printf(" HIT=0x%x\n",      entry0.hit);
                    if (i/2 > max1) {
                        sal_printf("Invalid Index for arlfm1 %d\n", i/1);
                        continue;
                    }
                    rv = soc_robo2_arlfm1_ght_h0_get(unit, i/2, &entry1);
                    if (rv == SOC_E_NONE) {
                        if ((entry1.ght_key_47_32_arlfm1_mac_hi_15_0 |
                              entry1.ght_key_31_0_arlfm1_mac_lo) != 0) {
                           sal_printf("1:MAC=0x%04x%08x",   entry1.ght_key_47_32_arlfm1_mac_hi_15_0,
                                                            entry1.ght_key_31_0_arlfm1_mac_lo);
                           sal_printf(" VLAN=0x%x",         entry1.ght_key_59_48_arlfm1_fid_11_0);
                           sal_printf(" UPD_CTRL=0x%x",    entry1.ght_key_63_62_arlfm1_upd_ctrl_1_0);
                           sal_printf(" FWD_CTRL=0x%x\n",  entry1.ght_key_61_60_arlfm1_fwd_ctrl_1_0);
                        }

                        if ((entry1.ght_key_111_96_arlfm1_mac_hi | entry1.ght_key_95_64_arlfm1_mac_lo) != 0) {
                            sal_printf("2:MAC=0x%04x%08x",   entry1.ght_key_111_96_arlfm1_mac_hi,
                                                             entry1.ght_key_95_64_arlfm1_mac_lo);
                            sal_printf(" VLAN=0x%x",         entry1.ght_n_vsi_9_0_key_113_112_arlfm1_fid);
                            sal_printf(" UPD_CTRL=0x%x",    entry1.ght_valid_litype_arlfm1_upd_ctrl_1_0);
                            sal_printf(" FWD_CTRL=0x%x\n",  entry1.ght_n_vsi_11_10_arlfm1_fwd_ctrl_1_0);
                        }
                    }
                }
            }
        }
    }
}

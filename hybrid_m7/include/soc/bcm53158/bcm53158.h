/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/*
 * bcm53158.h
 */

#ifndef _BCM53158_H_
#define _BCM53158_H_

#ifdef BCM53158_B0
#include "soc/bcm53158/socregs_b0.h"
#else
#include "soc/bcm53158/socregs_a0.h"
#endif

/* Switch characteristics */

#define BCM53158_PORT_COUNT                     16
#define BCM53158_PORT_COUNT_INTERNAL             8
#define BCM53158_UMAC_PORTS_MAX                 10                     
#define BCM53158_8051_CPU_PORT                  15
#define BCM53158_SGMII_PORT0                     8
#define BCM53158_SGMII_PORT1                     9
#define BCM53158_SGMII_PORT2                    10
#define BCM53158_SGMII_PORT3                    11
#define BCM53158_10G_PORT0                      12
#define BCM53158_10G_PORT1                      13
#define BCM53158_PORT0_TO_3_MASK                0xF
#define BCM53158_PORT4_TO_7_MASK                0xF0
#define BCM53158_SGMII_PORT_MASK                0xF00

#define BCM53158_SGMII_PORT2_XLMAC               2
#define BCM53158_SGMII_PORT3_XLMAC               3
#define BCM53158_10G_PORT0_XLMAC                 0
#define BCM53158_10G_PORT1_XLMAC                 1


/* Max port id of internal PHY */
#define BCM53158_INT_PHY_MAX    7
#define BCM53158_PHY_GROUP_MAX  2

#define BCM53158_MAX_UNIT       2
#define BCM53158_MAX_CSD_PORTS  2

/* MTU size allowed in UM */
#define BCM53158_MAX_FRM_LENGTH  9720

typedef struct
{
    uint8   revision;
    uint32  sysclk;
    uint8   link[BCM53158_PORT_COUNT]; /* link status */
    uint8   phy_present[BCM53158_PORT_COUNT]; /* phy type if present */
    BOOL    port_enabled[BCM53158_PORT_COUNT];
    BOOL    eee_enable;
    BOOL    gig_dis;  /* 1G disable. Ports will not advertise 1G */
    BOOL    gphy0to3_dis;
    BOOL    gphy4to7_dis;
    BOOL    rgmii_dis;
    uint8   rom_flags;
    uint16  valid_pbmp;
} bcm53158_sw_info_t;

typedef struct
{
    BOOL        link;
    uint16      speed;
    BOOL        duplex;
} bcm53158_phy_status_t;

enum {
    PORT_LINK_DOWN = 0,
    PORT_LINK_UP
};


typedef struct
{
    int8 ld_gpio;    /* loop detect */
    int8 ld_gpio_value;
    int8 igmp_gpio;
    int8 igmp_gpio_value;
    int8 auto_dos_gpio;
    int8 auto_dos_gpio_value;
    int8 auto_voip_gpio;
    int8 auto_voip_gpio_value;
    int8 cd_gpio;  /* cable diagnosis */
    int8 cd_gpio_value;
} bcm53158_gpio_map_t;

extern uint8 is_cascaded;
#define SOC_ROBO2_CASCADE_MASK    (0x04000000) /* (1 << 26) */
#define SOC_ROBO2_CASCADE_MASK__R 26
#define SOC_IS_CASCADED is_cascaded
#define CASCADE_ADDR_GEN(addr)      (addr | SOC_ROBO2_CASCADE_MASK)
#define UNIT_FROM_ADDRESS(addr)     ((addr & SOC_ROBO2_CASCADE_MASK) >> SOC_ROBO2_CASCADE_MASK__R)

#ifdef M7_COMPILE

#define PACKET_BUFFER_START_PAGE 256

#else /* M7_COMPILE */

#ifdef RAM_ALLOC_32KB
/* 32 KB used for 8051 */
#define PMIPAGE_IA_CONFIG_WDATA_VALUE 0x7000007f
#define PACKET_BUFFER_START_PAGE 128
#else  /* RAM_ALLOC_32KB */
/* 8 KB used for 8051 */
#define PMIPAGE_IA_CONFIG_WDATA_VALUE 0x7000001f
#define PACKET_BUFFER_START_PAGE 32
#endif
#endif /* M7_COMPILE */
#define PACKET_BUFFER_END_PAGE_512KB 2047
#define PACKET_BUFFER_END_PAGE_1MB 4095



/********************************************************
 *  8051 ADDRESS MAPPING
 *      Base Reg 0  : 0x00000 to 0x07fff    (  0  to  32K-1)
 *      Base Reg 1  : 0x08000 to 0x0ffff    ( 32K to  64K-1)
 *      Base Reg 2  : 0x10000 to 0x17fff    ( 64K to  96K-1)
 *      Base Reg 3  : 0x18000 to 0x1ffff    ( 96K to 128K-1)
 *      Base Reg 4  : 0x20000 to 0x27fff    (128K to 160K-1)
 *      Base Reg 5  : 0x28000 to 0x2ffff    (160K to 192K-1)
 *      Base Reg 6  : 0x30000 to 0x37fff    (192K to 224K-1)
 *      Base Reg 7  : 0x38000 to 0x3ffff    (224K to 256K-1)
 ********************************************************/

#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_0         0x100
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_1         0x104
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_2         0x108
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_3         0x10C
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_4         0x110
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_5         0x114
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_6         0x118
#define MC8051_PROGRAM_ADDRESS_BASE_REGISTER_7         0x11C

#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_0   0x200
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_1   0x204
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_2   0x208
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_3   0x20C
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_4   0x210
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_5   0x214
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_6   0x218
#define MC8051_EXTERNAL_DATA_ADDRESS_BASE_REGISTER_7   0x21C


#define VSIT_PORTGROUP_MAP_ADDRESS CB_IPP_vsit_ia_wdata_part1
#define DEFAULT_PORTGROUP_MAP      0x3fff

#define DEFAULT_FAST_AGING_5M      0x805C3370


/* Avenger Registers used in initialization */

#define PACKET_RAM_BASE           0x20800000
#define QSPI_FLASH_BASE           0x10000000
#define QSPI_FLASH_128K           0x10020000
#define QSPI_FLASH_160K           0x10028000
#define QSPI_FLASH_192K           0x10030000
#define QSPI_FLASH_224K           0x10038000


#define REG_CRU_CRU_PLL1_RESET_CTRL        CRU_CRU_Pll1_reset_ctrl
#define REG_CRU_CRU_PLL1_LOOP_CTRL1        CRU_CRU_Pll1_loop_ctrl1
#define REG_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2 \
                                           CRU_CRU_PLL1_Post_divider_CH_ctrl2
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_RATIO_FOR_CH3_R \
                   CRU_CRU_PLL1_Post_divider_CH_ctrl2__post_div_ratio_for_ch3_R
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_RATIO_FOR_CH3_MASK 0x03FC0000
 /* Clock divisor value for 62.5Mhz */
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_RATIO_FOR_CH3_VALUE 0x40
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_FOR_CH1_R \
                   CRU_CRU_PLL1_Post_divider_CH_ctrl2__post_div_for_ch1_R
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_FOR_CH1_MASK     0xFF
 /* clock divisor value for 210Mhz */
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_FOR_CH1_VALUE_210MHZ 0x13
#define PLL1_POST_DIVIDER_CH_CTRL2__POST_DIV_FOR_CH1_VALUE_400MHZ 0x0A
 
#define REG_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL3 \
                                           CRU_CRU_PLL1_Post_divider_CH_ctrl3

#define REG_CRU_CRU_PLL1_STATUS            CRU_CRU_Pll1_status
#define PLL1_RESET_CTRL__GLOBAL_RESET_MASK    \
                            (1 << CRU_CRU_Pll1_reset_ctrl__global_reset_R)
#define PLL1_RESET_CTRL__POST_RESET_MASK      \
                            (1 << CRU_CRU_Pll1_reset_ctrl__post_reset_R)
#define PLL1_STATUS__PLL1_LOCK_MASK           \
                            (1 << CRU_CRU_Pll1_status__Pll1_lock_R)

#define REG_CRU_CRU_CLK_MUX_SEL            CRU_CRU_Clk_mux_sel

#define REG_CRU_CRU_IP_RESET_REG_CRU       CRU_CRU_ip_reset_reg_cru
#define REG_CRU_CRU_IP_SYSTEM_RESET        CRU_CRU_IP_System_reset
#define REG_CRU_CRU_IP_SYSTEM_RESET2       CRU_CRU_IP_System_reset2
#define REG_CRU_CRU_IP_SYSTEM_RESET3       CRU_CRU_IP_System_reset3
#define REG_CRU_CRU_IP_SYSTEM_RESET4       CRU_CRU_IP_System_reset4
#define REG_CRU_CRU_IP_SYSTEM_RESET5       CRU_CRU_IP_System_reset5

#define REG_CRU_CRU_STRAP_LATCH_ENABLE_CONTROL           \
                                            CRU_CRU_Strap_latch_enable_control
#define CRU_STRAP_LATCH_ENABLE_CONTROL__STRAP_LATCH_EN_MASK \
                   (1 << CRU_CRU_Strap_latch_enable_control__Strap_latch_en_R)

#define REG_CRU_CRU_OEB_ENABLE             CRU_CRU_OEB_Enable_Register
#define CRU_OEB_ENABLE__OEB_EN_CONTROL_MASK   \
                   (1 << CRU_CRU_OEB_Enable_Register__OEB_EN_control_R)
#define REG_CRU_CRU_STRAP_STATUS           CRU_CRU_Strap_Status_Reg

#define REG_CRU_CRU_OTP_STATUS_VALUE_0     CRU_CRU_OTP_Status_value_register0
#define REG_CRU_CRU_OTP_STATUS_VALUE_1     CRU_CRU_OTP_Status_value_register1
#define REG_CRU_CRU_OTP_STATUS             CRU_CRU_OTP_status_register

#define REG_CRU_CRU_EEPROM_CONTROL         CRU_CRU_eeprom_control_register
#define REG_CRU_CRU_EEPROM_STATUS          CRU_CRU_eeprom_status_register
#define REG_CRU_CRU_LED_CONTROLLER_CFG0    CRU_CRU_led_controller_cfg0
#define REG_CRU_CRU_LED_CONTROLLER_CFG1    CRU_CRU_led_controller_cfg1

#define REG_CRU_CRU_GPHY_PLL_CONTROL       CRU_CRU_gphy_pll_control_register

#define CRU_CRU_TESTMODE_STATUS            CRU_CRU_testmode_status_register
#define REG_CRU_CRU_IMP_VOL_SEL_CONTROL    CRU_CRU_imp_vol_sel_control_register
#define REG_CRU_CRU_RESET_SEQUENCE         CRU_CRU_reset_sequence_register
#define REG_CRU_CRU_MDIO_VOL_SEL_CONTROL \
                                        CRU_CRU_mdio_vol_sel_control_register

#define EEPROM_CONFIG_START_ADDRESS        0x40208004

#define FLASH_CONFIG_START_ADDRESS         0x10080000

#define REG_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRL    \
                CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRL
#define TIMESYNC_INTERVAL_GENERATOR_0_CTRL__ENABLE_MASK        \
                (1<<CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRL__ENABLE_R)
#define REG_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_LOWER \
                CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_LOWER
#define REG_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_UPPER \
                CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_UPPER
#define REG_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_BYPASS           \
                CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_BYPASS
#define TIMESYNC_LCPLL_HOLDOVER_BYPASS__HOLDOVER_BYPASS_MASK    \
                (1<<CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_BYPASS__Holdover_bypass_R)
#define REG_CRU_TS_TOP_TS_LCPLL_CONTROL0      CRU_TS_TOP_ts_lcpll_CONTROL0

#define LCPLL_CONTROL0__TS_LCPLL_POST_RESETB_R  \
                CRU_TS_TOP_ts_lcpll_CONTROL0__ts_lcpll_POST_RESETB_R
#define LCPLL_CONTROL0__TS_LCPLL_RESETB_R       \
                CRU_TS_TOP_ts_lcpll_CONTROL0__ts_lcpll_RESETB_R
#define REG_CRU_TS_TOP_TS_LCPLL_CONTROL1   CRU_TS_TOP_ts_lcpll_CONTROL1
#define REG_CRU_TS_TOP_TS_LCPLL_CONTROL3   CRU_TS_TOP_ts_lcpll_CONTROL3
#define REG_CRU_TS_TOP_TS_LCPLL_CONTROL5   CRU_TS_TOP_ts_lcpll_CONTROL5
#define REG_CRU_TS_TOP_TS_LCPLL_CONTROL8   CRU_TS_TOP_ts_lcpll_CONTROL8
#define REG_CRU_TS_TOP_TS_LCPLL_STATUS     CRU_TS_TOP_ts_lcpll_STATUS

/*
#define LCPLL_STATUS__TS_LCPLL_LOCK_MASK \
         (1<< CRU_TS_TOP_ts_lcpll_STATUS__ts_lcpll_LOCK_R)
*/
#define LCPLL_STATUS__TS_LCPLL_LOCK_MASK     0x1000


#define REG_UMAC0_BASE                                                0x41000000
#define REG_UMAC_COMMAND_CONFIG_OFFSET     UNIMAC0_UMAC_COMMAND_CONFIG_OFFSET
#define REG_UMAC_MAC_MODE_OFFSET           UNIMAC0_UMAC_MAC_MODE_OFFSET
#define REG_MIB_TX_COUNTER_OFFSET          UNIMAC0_MIB_TX_GD_OCTETS_LO_OFFSET
#define REG_MIB_RX_COUNTER_OFFSET          UNIMAC0_MIB_RX_GD_OCTETS_LO_OFFSET
#define REG_STAT_MIB_EN_OFFSET             UNIMAC0_STAT_mib_en_OFFSET
#define REG_UMAC_UMAC_EEE_CTRL_OFFSET      UNIMAC0_UMAC_UMAC_EEE_CTRL_OFFSET
#define REG_UMAC_FLUSH_CONTROL_OFFSET      UNIMAC0_UMAC_FLUSH_CONTROL_OFFSET
#define FLD_UMAC_FLUSH_CONTROL__FLUSH_R    UNIMAC0_UMAC_FLUSH_CONTROL__FLUSH_R
#define FLD_UMAC_UMAC_EEE_CTRL_EEE_EN_MASK               (1<<3)
#define FLD_UMAC_MAC_MODE_LINK_STS_MASK                  (1<<5)
#define FLD_UMAC_COMMAND_CONFIG_RX_ENA_MASK              (1<<1)
#define FLD_UMAC_COMMAND_CONFIG_CRC_FWD_MASK             0x40
#define FLD_UMAC_COMMAND_CONFIG_PAUSE_FWD_MASK           0x80

#define MIB_TX_COUNTER_MAX                                      43
#define MIB_RX_COUNTER_MAX                                      30
#define REG_UMAC4_BASE                                                0x41008000
#define REG_UMAC8_BASE                                                0x41010000
#define REG_UMAC9_BASE                                                0x41011000
#define REG_UMAC10_BASE                                               0x41018000
#define RGMII_PORT                                           14


#define REG_LED_LEDUP0_CLK_DIV                  LED_LEDUP0_CLK_DIV
#define REG_LED_LEDUP0_DATA_RAM0                LED_LEDUP0_DATA_RAM0
#define REG_LED_LEDUP0_PROGRAM_RAM              LED_LEDUP0_PROGRAM_RAM0
#define REG_LED_LEDUP0_CTRL                     LED_LEDUP0_CTRL
#define REG_LED_LEDUP0_SCANCHAIN_ASSEMBLY_ST_ADDR  \
                        LED_LEDUP0_SCANCHAIN_ASSMEBLY_ST_ADDR
#define REG_LED_LEDUP0_PORT_ORDER_REMAP_0_3     LED_LEDUP0_PORT_ORDER_REMAP_0_3
#define REG_LED_LEDUP0_CLK_PARAMS               LED_LEDUP0_CLK_PARAMS

#define REG_PBUSBRIDGE_APB2PBUS_CTRL            PBUSBRIDGE_APB2PBUS_CTRL
#define REG_PBUSBRIDGE_APB2PBUS_WADDR           PBUSBRIDGE_APB2PBUS_WADDR
#define REG_PBUSBRIDGE_APB2PBUS_WDATA0          PBUSBRIDGE_APB2PBUS_WDATA0
#define REG_PBUSBRIDGE_APB2PBUS_WCTRL           PBUSBRIDGE_APB2PBUS_WCTRL
#define REG_PBUSBRIDGE_APB2PBUS_WR_GO           PBUSBRIDGE_APB2PBUS_WR_GO
#define REG_PBUSBRIDGE_APB2PBUS_RD_WR_STATUS    PBUSBRIDGE_APB2PBUS_RD_WR_STATUS



#define REG_NPA_NPA_EEE_CONTROL            NPA_npa_eee_control
#define REG_NPA_NPA_ENG_EEE_CONTROL        NPA_npa_eng_eee_control
#define REG_NPA_NPA_ENG_EEE_STATUS         NPA_npa_eng_eee_status
#define REG_NPA_NPA_ING_EEE_STATUS         NPA_npa_ing_eee_status
#define REG_NPA_NPA_RX_LINK_STATUS         NPA_npa_rx_link_status
#define REG_NPA_NPA_LNK_CHG_INT_STS        NPA_npa_lnk_chg_int_sts
#define REG_NPA_NPA_PFC_CONTROL            NPA_npa_pfc_control

#define REG_M7SS_M7SC_VTOR                 M7SS_M7SC_VTOR
#define REG_M7SS_M7SC_VTVR                 M7SS_M7SC_VTVR

#define REG_M7SS_MDIO_CMIC_MIIM_PARAM      M7SS_MDIO_CMIC_MIIM_PARAM
#define REG_M7SS_MDIO_CMIC_MIIM_READ_DATA  M7SS_MDIO_CMIC_MIIM_READ_DATA
#define REG_M7SS_MDIO_CMIC_MIIM_ADDRESS    M7SS_MDIO_CMIC_MIIM_ADDRESS
#define REG_M7SS_MDIO_CMIC_MIIM_CTRL       M7SS_MDIO_CMIC_MIIM_CTRL
#define REG_M7SS_MDIO_CMIC_MIIM_STAT       M7SS_MDIO_CMIC_MIIM_STAT
#define REG_M7SS_MDIO_CMIC_MIIM_SCAN_CTRL  M7SS_MDIO_CMIC_MIIM_SCAN_CTRL
						
#define REG_M7SS_UART_UART_RBR_THR_DLL     M7SS_UART_UART_RBR_THR_DLL
#define REG_M7SS_UART_UART_DLH_IER         M7SS_UART_UART_DLH_IER
#define REG_M7SS_UART_UART_IIR_FCR         M7SS_UART_UART_IIR_FCR
#define REG_M7SS_UART_UART_LCR             M7SS_UART_UART_LCR
#define REG_M7SS_UART_UART_MCR             M7SS_UART_UART_MCR
#define REG_M7SS_UART_UART_LSR             M7SS_UART_UART_LSR

#define REG_CB_SIA_NISI_SI_CONFIG          CB_SIA_nisi_si_config
#define REG_CB_EPP_EPP_SID_PORT_EN_CONFIG  CB_EPP_Epp_Sid_Port_En_Config

#define REG_CB_BMU_LLC_CONTROL             CB_BMU_LLC_CONTROL
#define REG_CB_BMU_LLC_SELF_INIT           CB_BMU_LLC_SELF_INIT
#define REG_CB_BMU_LLC_STATUS              CB_BMU_LLC_STATUS

#define REG_CB_CMM_INIT_CTRL               CB_CMM_INIT_CTRL
#define REG_CB_CMM_INIT_STAT               CB_CMM_INIT_STAT

#define REG_CB_PQM_QFC_MEM_CFG             CB_PQM_qfc_mem_cfg
#define REG_CB_PQM_QFC_MEM_STAT            CB_PQM_qfc_mem_stat

#define REG_CB_PQM_QFC_MEM_STAT__QFC_CFG_DONE_MASK 0x1


#define REG_CB_PQM_EEECFG_IA_STATUS        CB_PQM_eeecfg_ia_status
#define REG_CB_PQM_EEECFG_IA_WDATA_PART0   CB_PQM_eeecfg_ia_wdata_part0
#define REG_CB_PQM_EEECFG_IA_CONFIG        CB_PQM_eeecfg_ia_config
#define REG_CB_PQM_EEECFG_IA_RDATA_PART0   CB_PQM_eeecfg_ia_rdata_part0

#define REG_CB_PMI_PMIPAGE_CONTROL         CB_PMI_PMIPAGE_CONTROL
#define REG_CB_PMI_PMIPAGE_IA_STATUS       CB_PMI_pmipage_ia_status
#define REG_CB_PMI_PMIPAGE_IA_WDATA        CB_PMI_pmipage_ia_wdata_part0
#define REG_CB_PMI_PMIPAGE_IA_CONFIG       CB_PMI_pmipage_ia_config
#define REG_CB_PMI_PMIPAGE_STATUS          CB_PMI_PMIPAGE_STATUS



/***************************************/
/* TABLE INDIRECT ADDRESSING REGISTERS */
/***************************************/

#define REG_CB_IPP_VSIT_IA_STATUS          CB_IPP_vsit_ia_status
#define REG_CB_IPP_VSIT_IA_CONFIG          CB_IPP_vsit_ia_config
#define REG_CB_IPP_PP2LPG_IA_STATUS        CB_IPP_pp2lpg_ia_status
#define REG_CB_IPP_PP2LPG_IA_CONFIG        CB_IPP_pp2lpg_ia_config
#define REG_CB_IPP_PGT_IA_STATUS           CB_IPP_pgt_ia_status
#define REG_CB_IPP_PGT_IA_CONFIG           CB_IPP_pgt_ia_config
#define REG_CB_IPP_CPMT_IA_STATUS          CB_IPP_cpmt_ia_status
#define REG_CB_IPP_CPMT_IA_WDATA_PART0     CB_IPP_cpmt_ia_wdata_part0
#define REG_CB_IPP_CPMT_IA_CONFIG          CB_IPP_cpmt_ia_config
#define REG_CB_IPP_STT_IA_WDATA_PART1      CB_IPP_stt_ia_wdata_part1
#define REG_CB_IPP_STT_IA_WDATA_PART0      CB_IPP_stt_ia_wdata_part0
#define REG_CB_IPP_STT_IA_CONFIG           CB_IPP_stt_ia_config
#define REG_CB_IPP_STT_IA_STATUS           CB_IPP_stt_ia_status

#define REG_CB_ITM_ARLFM0_IA_STATUS        CB_ITM_arlfm0_ia_status
#define REG_CB_ITM_ARLFM0_IA_CONFIG        CB_ITM_arlfm0_ia_config
#define REG_CB_ITM_PGLCT_IA_STATUS         CB_ITM_pglct_ia_status
#define REG_CB_ITM_PGLCT_IA_CONFIG         CB_ITM_pglct_ia_config
#define REG_CB_ITM_ARLFM1_GHT_H0_IA_STATUS CB_ITM_arlfm1_ght_h0_ia_status
#define REG_CB_ITM_ARLFM1_GHT_H0_IA_CONFIG CB_ITM_arlfm1_ght_h0_ia_config
#define REG_CB_ITM_ARLFM1_GHT_H1_IA_STATUS CB_ITM_arlfm1_ght_h1_ia_status
#define REG_CB_ITM_ARLFM1_GHT_H1_IA_CONFIG CB_ITM_arlfm1_ght_h1_ia_config

#define REG_CB_PQS_MAX_CONFIG_IA_STATUS    CB_PQS_max_config_ia_status
#define REG_CB_PQS_MAX_CONFIG_IA_CONFIG    CB_PQS_max_config_ia_config
#define REG_CB_PQS_PER_QUEUE_EN_QUEUE_A    CB_PQS_per_queue_en_queue_a0

#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A0    CB_PQS_queue_to_group_map_port_a0
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A1    CB_PQS_queue_to_group_map_port_a1
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A2    CB_PQS_queue_to_group_map_port_a2
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A3    CB_PQS_queue_to_group_map_port_a3
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A4    CB_PQS_queue_to_group_map_port_a4
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A5    CB_PQS_queue_to_group_map_port_a5
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A6    CB_PQS_queue_to_group_map_port_a6
#define REG_CB_PQS_QUEUE_TO_GROUP_MAP_PORT_A7    CB_PQS_queue_to_group_map_port_a7

#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A0     CB_PQS_weight_high_queue_port_a0
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A0      CB_PQS_weight_low_queue_port_a0
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A1     CB_PQS_weight_high_queue_port_a1
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A1      CB_PQS_weight_low_queue_port_a1
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A2     CB_PQS_weight_high_queue_port_a2
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A2      CB_PQS_weight_low_queue_port_a2
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A3     CB_PQS_weight_high_queue_port_a3
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A3      CB_PQS_weight_low_queue_port_a3
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A4     CB_PQS_weight_high_queue_port_a4
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A4      CB_PQS_weight_low_queue_port_a4
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A5     CB_PQS_weight_high_queue_port_a5
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A5      CB_PQS_weight_low_queue_port_a5
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A6     CB_PQS_weight_high_queue_port_a6
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A6      CB_PQS_weight_low_queue_port_a6
#define REG_CB_PQS_WEIGHT_HIGH_QUEUE_PORT_A7     CB_PQS_weight_high_queue_port_a7
#define REG_CB_PQS_WEIGHT_LOW_QUEUE_PORT_A7      CB_PQS_weight_low_queue_port_a7



#define REG_CB_PQM_TC2QD_IA_STATUS         CB_PQM_tc2qd_ia_status
#define REG_CB_PQM_TC2QD_IA_CONFIG         CB_PQM_tc2qd_ia_config
#define REG_CB_PQM_LPG2PPFOV_IA_STATUS     CB_PQM_lpg2ppfov_ia_status
#define REG_CB_PQM_LPG2PPFOV_IA_CONFIG     CB_PQM_lpg2ppfov_ia_config
#define REG_CB_PQM_PP2LPG_IA_STATUS        CB_PQM_pp2lpg_ia_status
#define REG_CB_PQM_PP2LPG_IA_CONFIG        CB_PQM_pp2lpg_ia_config
#define REG_CB_PQM_LPG2PG_IA_STATUS        CB_PQM_lpg2pg_ia_status
#define REG_CB_PQM_LPG2PG_IA_CONFIG        CB_PQM_lpg2pg_ia_config
#define REG_CB_PQM_PG2LPG_IA_STATUS        CB_PQM_pg2lpg_ia_status
#define REG_CB_PQM_PG2LPG_IA_CONFIG        CB_PQM_pg2lpg_ia_config
#define REG_CB_PQM_QFCQDADDR_IA_STATUS     CB_PQM_qfcqdaddr_ia_status
#define REG_CB_PQM_QFCQDADDR_IA_CONFIG     CB_PQM_qfcqdaddr_ia_config
#define REG_CB_PQM_QFCCREDIT_IA_STATUS     CB_PQM_qfccredit_ia_status
#define REG_CB_PQM_QFCCREDIT_IA_CONFIG     CB_PQM_qfccredit_ia_config
#define REG_CB_PQM_LPG2AP_IA_STATUS        CB_PQM_lpg2ap_ia_status
#define REG_CB_PQM_LPG2AP_IA_CONFIG        CB_PQM_lpg2ap_ia_config
#define REG_CB_PQM_APT_IA_STATUS           CB_PQM_apt_ia_status
#define REG_CB_PQM_APT_IA_CONFIG           CB_PQM_apt_ia_config
#define REG_CB_PQM_QDSCHCFG_IA_STATUS      CB_PQM_qdschcfg_ia_status
#define REG_CB_PQM_QDSCHCFG_IA_CONFIG      CB_PQM_qdschcfg_ia_config

#define REG_CB_PQM_SVT_IA_STATUS           CB_PQM_svt_ia_status
#define REG_CB_PQM_SVT_IA_WDATA_PART0      CB_PQM_svt_ia_wdata_part0
#define REG_CB_PQM_SVT_IA_CONFIG           CB_PQM_svt_ia_config

#define REG_CB_PQM_TC2QD_IA_STATUS         CB_PQM_tc2qd_ia_status
#define REG_CB_PQM_TC2QD_IA_WDATA_PART0    CB_PQM_tc2qd_ia_wdata_part0
#define REG_CB_PQM_TC2QD_IA_CONFIG         CB_PQM_tc2qd_ia_config
			
#define AVR_LSR_TXRDY (0x40)
#define AVR_LSR_RXRDY (0x01)



/* TODO: Check the strap position */




#define MDIO_RD_START 2
#define MDIO_WR_START 1




#define TABLE_STATUS_MASK            0xE0000000
#define TABLE_STATUS_POWER_DOWN      0xE0000000
#define TABLE_STATUS_READY           0x0
#define TABLE_STATUS_BUSY            0x20000000
#define TABLE_CONFIG_READ            0x10000000
#define TABLE_CONFIG_ENABLE          0x30000000
#define TABLE_CONFIG_WRITE           0x20000000
#define TABLE_CONFIG_RESET           0x50000000
#define TABLE_CONFIG_INDEX_MASK      0xf;


/*TODO: Check the bit positions */
#define STRAP_STATUS_MDIO_VOL_SEL_MASK             (1 << 13)
#define STRAP_STATUS_EN_EEE_MASK                   (1 << 12)
#define STRAP_STATUS_IMP_VOL_SEL_MASK              (1 << 11)
#define STRAP_STATUS_ATE_VARIABLE_TIME_BYPASS_MASK (1 << 9)
#define STRAP_STATUS_CASCADING_CONFIG_MASK         (3 << 7)
#define STRAP_STATUS_BOOT_SRC                      (3 << 5)
#define STRAP_STATUS_XTAL_FREQ_SEL_MASK            (1 << 4)
#define STRAP_STATUS_ENABLE_EEPROM_MASK            (1 << 2)
#define STRAP_STATUS_BOOT_SRC_MASK                 (3 << 5)
#define BOOT_SRC_8051_ROM                          (0 << 5)
#define BOOT_SRC_8051_FLASH                        (1 << 5)
#define BOOT_SRC_M7_FLASH                          (2 << 5)
#define BOOT_SRC_M7_INTERNAL                       (3 << 5)
#define CASCADING_CONFIG_STANDALONE_HW_FWD         (0 << 7)
#define CASCADING_CONFIG_CASCADE_PRIMARY           (1 << 7)
#define CASCADING_CONFIG_CASCADE_SECONDARY         (0x100) /* (2 << 7) */ 
#define CASCADING_CONFIG_STANDALONE_NO_HW_FWD      (0x180) /* (3 << 7) */


#define OTP_STATUS_OTP_DONE                                  0x1 /* MASK VALUE */
#define OTP_STATUS_BISR_DONE                                 0x2 /* MASK VALUE */
#define OTP_STATUS_VALUE_REGISTER0__G_DIS_MASK               (1 << 0)
#define OTP_STATUS_VALUE_REGISTER0__RGMII_DIS_MASK           (1 << 2)
#define OTP_STATUS_VALUE_REGISTER0__GPHY0TO3_DIS_MASK        (1 << 3)
#define OTP_STATUS_VALUE_REGISTER0__GPHY4TO7_DIS_MASK        (1 << 4)
#define OTP_STATUS_VALUE_REGISTER0__XFI1_DIS_MASK            (1 << 5)
#define OTP_STATUS_VALUE_REGISTER0__XFI0_DIS_MASK            (1 << 6)
#define OTP_STATUS_VALUE_REGISTER0__M7_DIS_MASK              (1 << 11)
#define OTP_STATUS_VALUE_REGISTER0__QSGMII_DIS_MASK          (3 << 9)

#define OTP_STATUS_VALUE_REGISTER1__CB_BUFFER_SIZE_MASK      0x4
#define OTP_STATUS_VALUE_REGISTER1__ROM_FLAGS_MASK           0x1E0000
#define OTP_STATUS_VALUE_REGISTER1__ROM_FLAGS_R              17
#define OTP_STATUS_VALUE_REGISTER1__ROM_FLAGS_EEE_DIS        0x020000
#define ROM_FLAGS__EEE_DIS                                   1
#define OTP_STATUS_VALUE_REGISTER1__ROM_FLAGS_AVS_EN        0x040000

#define OTP_STATUS_VALUE_REGISTER2__53154_ID                 0x0C00CFA2
#define OTP_STATUS_VALUE_REGISTER3__XLPORT_ENABLE_MASK       0x400000
      /* TODO: Check the bit position */
#define OTP_STATUS_FLD_BISR_DONE  (1 << 1)


#define CB_BMU_LLC_SELF_INIT__SELF_INIT_R                    30
#define CB_BMU_LLC_SELF_INIT__END_PAGE_R                     15
#define CB_BMU_LLC_SELF_INIT__START_PAGE_R                   0
#define CB_BMU_LLC_SELF_INIT__PAGE_MASK                     0x1FFF

#define CB_BMU_LLC_CONTROL__FREE_PAGE_THRESHOLD_R            0
#define CB_BMU_LLC_CONTROL__FREE_PAGE_THRESHOLD_MASK         0x3FF
#define CB_BMU_LLC_CONTROL__LLC_ENABLE_R                     10

#define CB_BMU_LLC_STATUS__SELF_INIT_DONE_BIT_MASK           0x400   
#define CB_BMU_LLC_STATUS__SELF_INIT_ERR_BIT_MASK            0x200
#define CB_BMU_LLC_STATUS__ERROR_MASK                        0x3FF   

#define REG_XLPORT_LED_CHAIN_CONFIG             XLPORT_LED_CHAIN_CONFIG

/*
 * System registers (on 8051 space)
 */


/***************************************************
 * 8051 Indirect Access Register for Switch memory 
 * 
 *    SYS -> 8051, IA -> Indirect Access
 ***************************************************/    

/* CMD Reg
   Bit: 1:0 Operation  (0 - noop, 1 - resv, 2- read, 3 - write)
   Bit: 3:2 Reserved
   Bit: 7:4 WStrobe
 */ 
#define SYSREG_IA_CMD			0x1000
#define SYSFLD_IA_CMD_REG_WRITE   0x3
#define SYSFLD_IA_CMD_REG_READ    0x2

/* Status Reg
    Bit 0   - RBusy (0- no active read, 1 - Read in progress)
		Bit 1   - WBusy
		Bit 7-2 - Resv
 */
#define SYSREG_IA_STATUS  0x1001
#define SYSFLD_IA_STATUS_REG_RBUSY 1
#define SYSFLD_IA_STATUS_REG_WBUSY 2
#define SYSFLD_IA_STATUS_BUSY 3

#define SYSREG_IA_WDATA0  0x1010
#define SYSREG_IA_WDATA1  0x1011
#define SYSREG_IA_WDATA2  0x1012
#define SYSREG_IA_WDATA3  0x1013
#define SYSREG_IA_ADDR0   0x1020
#define SYSREG_IA_ADDR1   0x1021
#define SYSREG_IA_ADDR2   0x1022
#define SYSREG_IA_ADDR3   0x1023
#define SYSREG_IA_RDATA0  0x1030
#define SYSREG_IA_RDATA1  0x1031
#define SYSREG_IA_RDATA2  0x1032
#define SYSREG_IA_RDATA3  0x1033

#define MC8051_FLASH_JUMP_ADDRESS 0x8000
#define BCM53158_FLASH_ADDRESS_FOR_8051_MAPPING 0x10008000
#if DV_SIM
#define BCM53158_FLASH_START_ADDRESS 0x00404000
#else /* DV_SIM */
#define BCM53158_FLASH_START_ADDRESS 0x10010000
#endif /* DV_SIM */

#define BCM53158_FIELD_SET(data, field, val)\
    do {\
        data &= ~((uint32)1 << field##_L |\
                (((uint32)1 << field##_L) - ((uint32)1 << field##_R)));\
        data |= ((uint32)val << field##_R);\
    } while(0)

#define SOC_IF_ERROR_RETURN(op) \
    do { int __rv__;\
        if ((__rv__ = (op)) < 0) { return(__rv__); }\
    } while(0)

extern sys_error_t bcm53158_umac_base_address_get(uint8 port, uint32 *address);
extern sys_error_t bcm53158_reg_get(uint8 unit, uint32 addr, uint32 *rdata);
extern sys_error_t bcm53158_reg_set(uint8 unit, uint32 addr, uint32 wdata);
extern sys_error_t bcm53158_port_link_get(uint8 unit, uint8 port, uint8 *link);
extern sys_error_t bcm53158_port_link_set(uint8 unit, uint8 port, uint8 link);
extern sys_error_t bcm53158_port_mac_flush_set(uint8 unit, uint8 port, BOOL flush_enable);
extern sys_error_t bcm53158_port_eee_enable_get(uint8 unit, uint8 port, uint8 *enable);
extern sys_error_t bcm53158_port_eee_enable_set(uint8 unit, uint8 port, uint8 enable);
extern void        bcm53158_eeecfg_table_enable(uint8 unit);
extern sys_error_t bcm53158_port_eee_config_set(uint8 unit, uint8 port, BOOL enable, uint16 threshold, uint16 timer);
extern sys_error_t bcm53158_port_npa_eee_enable_set(uint8 unit, uint8 port, BOOL enable);
extern sys_error_t bcm53158_port_npa_pfc_enable_set(uint8 unit, uint8 port, BOOL enable);
extern sys_error_t bcm53158_port_config_crc_pause_fwd(uint8 unit, uint8 port, BOOL crc_fwd, BOOL pause_fwd);
extern sys_error_t bcm53158_phy_1gig_adv_disable(uint8 unit, uint8 port);

extern void eeprom_config();

#ifdef CFG_CASCADE_SUPPORT
extern sys_error_t bcm53158_cascade_set(
        int8 unit0_p0, int8 unit0_p1, int8 unit1_p0, int8 unit1_p1);
#endif

extern void bcm53158_led_software_override_init(uint8 unit);
extern void bcm53158_led_set_override(uint8 unit, uint8 port, uint8 led0, uint8 led1);

extern int bcm53158_l2_dump(int unit);
#endif /* _BCM53158_H_ */

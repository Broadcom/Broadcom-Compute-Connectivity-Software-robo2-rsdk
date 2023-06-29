/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 * $Id: $
 *
 * File:
 *    intr.h
 * Description:
 *    Avenger Interrupt and Resets
 *
 */

#ifndef __INTR_H
#define __INTR_H


#ifdef BCM_53158_A0

/*   Interrupts    */
#define BCM53158_A0_8051_TO_M7_SW_INTR             (  0 )       /* 0x00000001  W0 */
#define BCM53158_A0_8051_TO_M7_MB_INTR             (  1 )       /* 0x00000002  W0 */
#define BCM53158_A0_CPU_TO_M7_SW_INTR              (  2 )       /* 0x00000004  W0 */
#define BCM53158_A0_CPU_TO_M7_MB_INTR              (  3 )       /* 0x00000008  W0 */
#define BCM53158_A0_8051_TO_CPU_SW_INTR            (  4 )       /* 0x00000010  W0 */
#define BCM53158_A0_8051_TO_CPU_MB_INTR            (  5 )       /* 0x00000020  W0 */
#define BCM53158_A0_M7_TO_CPU_SW_INTR              (  6 )       /* 0x00000040  W0 */
#define BCM53158_A0_M7_TO_CPU_MB_INTR              (  7 )       /* 0x00000080  W0 */
#define BCM53158_A0_M7_TO_8051_SW_INTR             (  8 )       /* 0x00000100  W0 */
#define BCM53158_A0_M7_TO_8051_MB_INTR             (  9 )       /* 0x00000200  W0 */
#define BCM53158_A0_CPU_TO_8051_SW_INTR            ( 10 )       /* 0x00000400  W0 */
#define BCM53158_A0_CPU_TO_8051_MB_INTR            ( 11 )       /* 0x00000800  W0 */
#define BCM53158_A0_QSPI_INTR                      ( 12 )       /* 0x00001000  W0 */
#define BCM53158_A0_SPI_INTR                       ( 13 )       /* 0x00002000  W0 */
#define BCM53158_A0_GPIO_INTR                      ( 14 )       /* 0x00004000  W0 */
#define BCM53158_A0_UART_INTR                      ( 15 )       /* 0x00008000  W0 */
#define BCM53158_A0_TIMER_INTR                     ( 16 )       /* 0x00010000  W0 */
#define BCM53158_A0_WDOG_INTR                      ( 17 )       /* 0x00020000  W0 */
#define BCM53158_A0_WDOG_RESET_INTR                ( 18 )       /* 0x00040000  W0 */
#define BCM53158_A0_SMBUS_I2C_INTR                 ( 19 )       /* 0x00080000  W0 */
#define BCM53158_A0_MIIM_OP_DONE_INTR              ( 20 )       /* 0x00100000  W0 */
#define BCM53158_A0_SPI_MASTER_INTR                ( 21 )       /* 0x00200000  W0 */
#define BCM53158_A0_CB_IDM_INTR                    ( 22 )       /* 0x00400000  W0 */
#define BCM53158_A0_HPA_INTR                       ( 23 )       /* 0x00800000  W0 */
#define BCM53158_A0_NPA_INTR                       ( 24 )       /* 0x01000000  W0 */
#define BCM53158_A0_RSVD_INTR                      ( 25 )       /* 0x02000000  W0 */
#define BCM53158_A0_PM_INTR                        ( 26 )       /* 0x04000000  W0 */
#define BCM53158_A0_APB2PBUS_INTR                  ( 27 )       /* 0x08000000  W0 */
#define BCM53158_A0_GPHY0_INTR                     ( 28 )       /* 0x10000000  W0 */
#define BCM53158_A0_GPHY1_INTR                     ( 29 )       /* 0x20000000  W0 */
#define BCM53158_A0_GPHY2_INTR                     ( 30 )       /* 0x40000000  W0 */
#define BCM53158_A0_GPHY3_INTR                     ( 31 )       /* 0x80000000  W0 */
#define BCM53158_A0_GPHY4_INTR                     ( 32 )       /* 0x00000001  W1 */
#define BCM53158_A0_GPHY5_INTR                     ( 33 )       /* 0x00000002  W1 */
#define BCM53158_A0_GPHY6_INTR                     ( 34 )       /* 0x00000004  W1 */
#define BCM53158_A0_GPHY7_INTR                     ( 35 )       /* 0x00000008  W1 */
#define BCM53158_A0_UNIMAC0_INTR                   ( 36 )       /* 0x00000010  W1 */
#define BCM53158_A0_UNIMAC1_INTR                   ( 37 )       /* 0x00000020  W1 */
#define BCM53158_A0_UNIMAC2_INTR                   ( 38 )       /* 0x00000040  W1 */
#define BCM53158_A0_UNIMAC3_INTR                   ( 39 )       /* 0x00000080  W1 */
#define BCM53158_A0_UNIMAC4_INTR                   ( 40 )       /* 0x00000100  W1 */
#define BCM53158_A0_UNIMAC5_INTR                   ( 41 )       /* 0x00000200  W1 */
#define BCM53158_A0_UNIMAC6_INTR                   ( 42 )       /* 0x00000400  W1 */
#define BCM53158_A0_UNIMAC7_INTR                   ( 43 )       /* 0x00000800  W1 */
#define BCM53158_A0_UNIMAC8_INTR                   ( 44 )       /* 0x00001000  W1 */
#define BCM53158_A0_UNIMAC9_INTR                   ( 45 )       /* 0x00002000  W1 */
#define BCM53158_A0_UNIMAC14_INTR                  ( 46 )       /* 0x00004000  W1 */
#define BCM53158_A0_CB_INTR                        ( 47 )       /* 0x00008000  W1 */
#define BCM53158_A0_M7_RESET_INTR                  ( 48 )       /* 0x00010000  W1 */
#define BCM53158_A0_M7_LOCKUP_INTR                 ( 49 )       /* 0x00020000  W1 */
#define BCM53158_A0_GPHY03_ENRGY_DET_INTR          ( 50 )       /* 0x00040000  W1 */
#define BCM53158_A0_GPHY47_ENRGY_DET_INTR          ( 51 )       /* 0x00080000  W1 */
#define BCM53158_A0_XFP0_INTR                      ( 52 )       /* 0x00100000  W1 */
#define BCM53158_A0_XFP1_INTR                      ( 53 )       /* 0x00200000  W1 */
#define BCM53158_A0_RANDOM_NG_INTR                 ( 54 )       /* 0x00400000  W1 */
#define BCM53158_A0_TIMESYNC_INTR                  ( 55 )       /* 0x00800000  W1 */
#define BCM53158_A0_NIC_IDM_INTR                   ( 56 )       /* 0x01000000  W1 */
#define BCM53158_A0_AVS_POWER_WDOG_INTR            ( 57 )       /* 0x02000000  W1 */
#define BCM53158_A0_AVS_ROCS_THRESH1_INTR          ( 58 )       /* 0x04000000  W1 */
#define BCM53158_A0_AVS_ROCS_THRESH2_INTR          ( 59 )       /* 0x08000000  W1 */
#define BCM53158_A0_AVS_SW_DONE_INTR               ( 60 )       /* 0x10000000  W1 */
#define BCM53158_A0_AVS_PM_INTR                    ( 61 )       /* 0x20000000  W1 */
#define BCM53158_A0_MAX_INTR                       ( 62 )       /* 0x40000000  W1 */

#define BCM53158_A0_ANY_INTR_MASK              ((1 << SOC_ROBO2_MAX_INTR) - 1)



/*   Software Reset for components    */
#define BCM53158_A0_R1_SMBUS_RESET_N           0x000001
#define BCM53158_A0_R1_UART_RESET_N            0x000002
#define BCM53158_A0_R1_MDIO_RESET_N            0x000004
#define BCM53158_A0_R1_GPIO_RESET_N            0x000008
#define BCM53158_A0_R1_WDT_RESET_N             0x000010
#define BCM53158_A0_R1_TIMER_RESET_N           0x000020
#define BCM53158_A0_R1_SPI_MASTER_RESET_N      0x000100
#define BCM53158_A0_R1_SPI_SLAVE_RESET_N       0x000200
#define BCM53158_A0_R1_QSPI_RESET_N            0x000400
#define BCM53158_A0_R1_HPA_RESET_N             0x000800
#define BCM53158_A0_R1_M7_SYS_RESET_N          0x008000
#define BCM53158_A0_R1_M7_CRSIGHT_RESET_N      0x020000
#define BCM53158_A0_R1_M7_PWM_RESET_N          0x040000
#define BCM53158_A0_R1_M7_RNG_RESET_N          0x080000


#define BCM53158_A0_R2_EGPHY0_APB_RESETN            0x0001 
#define BCM53158_A0_R2_EGPHY0_IP_RESETN             0x0002
#define BCM53158_A0_R2_GPHY0_SW_RESETN              0x0004
#define BCM53158_A0_R2_UMAC0_RESETN                 0x0008
#define BCM53158_A0_R2_UMAC0_SYNC_SW_RESETN         0x0010
#define BCM53158_A0_R2_UMAC0_MIB_SYNC_SW_RESETN     0x0020
#define BCM53158_A0_R2_UMAC1_RESETN                 0x0040
#define BCM53158_A0_R2_UMAC1_SYNC_SW_RESETN         0x0080
#define BCM53158_A0_R2_UMAC1_MIB_SYNC_SW_RESETN     0x0100
#define BCM53158_A0_R2_UMAC2_RESETN                 0x0200
#define BCM53158_A0_R2_UMAC2_SYNC_SW_RESETN         0x0400
#define BCM53158_A0_R2_UMAC2_MIB_SYNC_SW_RESETN     0x0800
#define BCM53158_A0_R2_UMAC3_RESETN                 0x1000
#define BCM53158_A0_R2_UMAC3_SYNC_SW_RESETN         0x2000
#define BCM53158_A0_R2_UMAC3_MIB_SYNC_SW_RESETN     0x4000

#define BCM53158_A0_R3_EGPHY1_APB_RESETN            0x0001
#define BCM53158_A0_R3_EGPHY1_IP_RESETN             0x0002
#define BCM53158_A0_R3_GPHY1_SW_RESETN              0x0004
#define BCM53158_A0_R3_UMAC4_RESETN                 0x0008
#define BCM53158_A0_R3_UMAC4_SYNC_SW_RESETN         0x0010
#define BCM53158_A0_R3_UMAC4_MIB_SYNC_SW_RESETN     0x0020
#define BCM53158_A0_R3_UMAC5_RESETN                 0x0040
#define BCM53158_A0_R3_UMAC5_SYNC_SW_RESETN         0x0080
#define BCM53158_A0_R3_UMAC5_MIB_SYNC_SW_RESETN     0x0100
#define BCM53158_A0_R3_UMAC6_RESETN                 0x0200
#define BCM53158_A0_R3_UMAC6_SYNC_SW_RESETN         0x0400
#define BCM53158_A0_R3_UMAC6_MIB_SYNC_SW_RESETN     0x0800
#define BCM53158_A0_R3_UMAC7_RESETN                 0x1000
#define BCM53158_A0_R3_UMAC7_SYNC_SW_RESETN         0x2000
#define BCM53158_A0_R3_UMAC7_MIB_SYNC_SW_RESETN     0x4000


#define BCM53158_A0_R4_NPA_RESETN                    0x0001
#define BCM53158_A0_R4_NPA_CSR_RESETN                0x0002
#define BCM53158_A0_R4_CB_RESETN                     0x0004
#define BCM53158_A0_R4_UMAC14_RESETN                 0x0008
#define BCM53158_A0_R4_PORT14_GMII_IOMUX_RESETN      0x0010
#define BCM53158_A0_R4_UMAC14_SYNC_SW_RESETN         0x0020
#define BCM53158_A0_R4_UMAC14_MIB_SYNC_SW_RESETN     0x0040
#define BCM53158_A0_R4_SW_TOP_APB_INTERFACE_RESETN   0x0080

#define BCM53158_A0_R5_QSGMII_RSTB_HW                    0x000001
#define BCM53158_A0_R5_QSGMII_RSTB_MDIOREGS              0x000002
#define BCM53158_A0_R5_QSGMII_RSTB_PLL                   0x000004
#define BCM53158_A0_R5_UMAC8_RESETN                      0x000008
#define BCM53158_A0_R5_UMAC8_SYNC_SW_RESETN              0x000010
#define BCM53158_A0_R5_UMAC8_MIB_SYNC_SW_RESETN          0x000020
#define BCM53158_A0_R5_UMAC9_RESETN                      0x000040
#define BCM53158_A0_R5_UMAC9_SYNC_SW_RESETN              0x000080
#define BCM53158_A0_R5_UMAC9_MIB_SYNC_SW_RESETN          0x000100
#define BCM53158_A0_R5_QSGMII_COMBO_WRAPPER_APB_RESETN   0x000200
#define BCM53158_A0_R5_AVR_PORTMACRO_APB_RESETN          0x010000
#define BCM53158_A0_R5_PORTMACRO_RESETN                  0x020000
#define BCM53158_A0_R5_LED_CONTROLLER_RESETN             0x040000
#define BCM53158_A0_R5_PORTMACRO_EAGLE0_RESETN           0x080000
#define BCM53158_A0_R5_PORTMACRO_EAGLE1_RESETN           0x100000

/* Declartions for Handlers */
extern void uc_to_m7_sw_handler(void);
extern void uc_to_m7_mb_handler(void);
extern void cpu_to_m7_sw_handler(void);
extern void cpu_to_m7_mb_handler(void);
extern void uc_to_cpu_sw_handler(void);
extern void uc_to_cpu_mb_handler(void);
extern void m7_to_cpu_sw_handler(void);
extern void m7_to_cpu_mb_handler(void);
extern void m7_to_uc_sw_handler(void);
extern void m7_to_uc_mb_handler(void);
extern void cpu_to_uc_sw_handler(void);
extern void cpu_to_uc_mb_handler(void);
extern void qspi_handler(void);
extern void spi_handler(void);
extern void gpio_handler(void);
extern void uart_handler(void);
extern void timer_handler(void);
extern void wdog_handler(void);
extern void wdog_reset_handler(void);
extern void smbus_i2c_handler(void);
extern void miim_op_done_handler(void);
extern void spi_master_handler(void);
extern void cb_idm_handler(void);
extern void hpa_handler(void);
extern void npa_handler(void);
extern void rsvd10_handler(void);
extern void pm_handler(void);
extern void apb2pbus_handler(void);
extern void gphy0_handler(void);
extern void gphy1_handler(void);
extern void gphy2_handler(void);
extern void gphy3_handler(void);
extern void gphy4_handler(void);
extern void gphy5_handler(void);
extern void gphy6_handler(void);
extern void gphy7_handler(void);
extern void unimac0_handler(void);
extern void unimac1_handler(void);
extern void unimac2_handler(void);
extern void unimac3_handler(void);
extern void unimac4_handler(void);
extern void unimac5_handler(void);
extern void unimac6_handler(void);
extern void unimac7_handler(void);
extern void unimac8_handler(void);
extern void unimac9_handler(void);
extern void unimac14_handler(void);
extern void cb_handler(void);
extern void m7_reset_handler(void);
extern void m7_lockup_handler(void);
extern void gphy03_enrgy_det_handler(void);
extern void gphy47_enrgy_det_handler(void);
extern void xfp0_handler(void);
extern void xfp1_handler(void);
extern void random_ng_handler(void);
extern void rimesync_handler(void);
extern void nic_idm_handler(void);
extern void avs_power_wdog_handler(void);
extern void avs_rocs_thresh1_handler(void);
extern void avs_rocs_thresh2_handler(void);

#endif /* BCM53158_A0 */

#endif /* __INTR_H */

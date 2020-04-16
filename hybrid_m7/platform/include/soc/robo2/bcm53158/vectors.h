/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 * $Id: $
 *
 * File:
 *     vectors.h   
 * Description:
 *     Avenger Irq Handlers 
 */

#ifndef __VECTORS_H
#define __VECTORS_H



/* Vector table handlers */
typedef void (*handler_t)(void);

typedef struct DeviceVectors_s {

    uint32      Stack_Base;               /* Top of Stack  */
 
    /* Cortex-M handlers */
    handler_t   Reset_Handler;            /* Reset Handler  */
    handler_t   NMI_Handler;              /* NMI Handler  */
    handler_t   HardFault_Handler;        /* Hard Fault Handler  */
    handler_t   MemManage_Handler;        /* MPU Fault Handler  */
    handler_t   BusFault_Handler;         /* Bus Fault Handler  */
    handler_t   UsageFault_Handler;       /* Usage Fault Handler  */
    handler_t   Rsvd0_Handler;            /* Reserved  */
    handler_t   Rsvd1_Handler;            /* Reserved  */
    handler_t   Rsvd2_Handler;            /* Reserved  */
    handler_t   Rsvd3_Handler;            /* Reserved  */
    handler_t   SVC_Handler;              /* SVCall Handler  */
    handler_t   DebugMon_Handler;         /* Debug Monitor Handler  */
    handler_t   Rsvd4_Handler;            /* Reserved  */
    handler_t   PendSV_Handler;           /* PendSV Handler  */
    handler_t   SysTick_Handler;          /* SysTick Handler  */


#ifdef BCM_53158_A0
    /* BCM_53158_A0 Handlers */

    handler_t   Uc_to_m7_sw_Handler;       /*   8051 To M7 Software Interrupt Handler       */ 
    handler_t   Uc_to_m7_mb_Handler;       /*   8051 To M7 Mailbox Interrupt Handler        */ 
    handler_t   Cpu_to_m7_sw_Handler;      /*   Cpu To M7 Software Interrupt Handler        */ 
    handler_t   Cpu_to_m7_mb_Handler;      /*   Cpu To M7 Mailbox Interrupt Handler         */ 
    handler_t   Uc_to_cpu_sw_Handler;      /*   8051 To Cpu Software Interrupt Handler      */ 
    handler_t   Uc_to_cpu_mb_Handler;      /*   8051 To Cpu Mailbox Interrupt Handler       */ 
    handler_t   M7_to_cpu_sw_Handler;      /*   M7 To Cpu Software Interrupt Handler        */ 
    handler_t   M7_to_cpu_mb_Handler;      /*   M7 To Cpu Mailbox Interrupt Handler         */ 
    handler_t   M7_to_uc_sw_Handler;       /*   M7 To 8051 Software Interrupt Handler       */ 
    handler_t   M7_to_uc_mb_Handler;       /*   M7 To 8051 Mailbox Interrupt Handler        */ 
    handler_t   Cpu_to_uc_sw_Handler;      /*   Cpu To 8051 Software Interrupt Handler      */ 
    handler_t   Cpu_to_uc_mb_Handler;      /*   Cpu To 8051 Mailbox Interrupt Handler       */ 
    handler_t   Qspi_Handler;              /*   Qspi Interrupt Handler                      */ 
    handler_t   Spi_Handler;               /*   Spi Interrupt Handler                       */ 
    handler_t   Gpio_Handler;              /*   Gpio Interrupt Handler                      */ 
    handler_t   Uart_Handler;              /*   Uart Interrupt Handler                      */ 
    handler_t   Timer_Handler;             /*   Timer Interrupt Handler                     */ 
    handler_t   Wdog_Handler;              /*   Wdog Interrupt Handler                      */ 
    handler_t   Wdog_reset_Handler;        /*   Wdog Reset Interrupt Handler                */ 
    handler_t   I2c_Handler;               /*   Smbus I2c Interrupt Handler                 */ 
    handler_t   Miim_op_done_Handler;      /*   Miim Op Done Interrupt Handler              */ 
    handler_t   Spi_master_Handler;        /*   Spi Master Interrupt Handler                */ 
    handler_t   Cb_idm_Handler;            /*   Cb Idm Interrupt Handler                    */ 
    handler_t   Hpa_Handler;               /*   Hpa Interrupt Handler                       */ 
    handler_t   Npa_Handler;               /*   Npa Interrupt Handler                       */ 
    handler_t   Rsvd_Handler;              /*   Rsvd Interrupt Handler                      */ 
    handler_t   Pm_Handler;                /*   Pm Interrupt Handler                        */ 
    handler_t   Apb2pbus_Handler;          /*   Apb2pbus Interrupt Handler                  */ 
    handler_t   Gphy0_Handler;             /*   Gphy0 Interrupt Handler                     */ 
    handler_t   Gphy1_Handler;             /*   Gphy1 Interrupt Handler                     */ 
    handler_t   Gphy2_Handler;             /*   Gphy2 Interrupt Handler                     */ 
    handler_t   Gphy3_Handler;             /*   Gphy3 Interrupt Handler                     */ 
    handler_t   Gphy4_Handler;             /*   Gphy4 Interrupt Handler                     */ 
    handler_t   Gphy5_Handler;             /*   Gphy5 Interrupt Handler                     */ 
    handler_t   Gphy6_Handler;             /*   Gphy6 Interrupt Handler                     */ 
    handler_t   Gphy7_Handler;             /*   Gphy7 Interrupt Handler                     */ 
    handler_t   Unimac0_Handler;           /*   Unimac0 Interrupt Handler                   */ 
    handler_t   Unimac1_Handler;           /*   Unimac1 Interrupt Handler                   */ 
    handler_t   Unimac2_Handler;           /*   Unimac2 Interrupt Handler                   */ 
    handler_t   Unimac3_Handler;           /*   Unimac3 Interrupt Handler                   */ 
    handler_t   Unimac4_Handler;           /*   Unimac4 Interrupt Handler                   */ 
    handler_t   Unimac5_Handler;           /*   Unimac5 Interrupt Handler                   */ 
    handler_t   Unimac6_Handler;           /*   Unimac6 Interrupt Handler                   */ 
    handler_t   Unimac7_Handler;           /*   Unimac7 Interrupt Handler                   */ 
    handler_t   Unimac8_Handler;           /*   Unimac8 Interrupt Handler                   */ 
    handler_t   Unimac9_Handler;           /*   Unimac9 Interrupt Handler                   */ 
    handler_t   Unimac14_Handler;          /*   Unimac14 Interrupt Handler                  */ 
    handler_t   Cb_Handler;                /*   Coronado Bridge Interrupt Handler           */ 
    handler_t   M7_reset_Handler;          /*   M7 Reset Interrupt Handler                  */ 
    handler_t   M7_lockup_Handler;         /*   M7 Lockup Interrupt Handler                 */ 
    handler_t   Gphy03_enrgy_det_Handler;  /*   Gphy03 Energy Det Interrupt Handler         */ 
    handler_t   Gphy47_enrgy_det_Handler;  /*   Gphy47 Energy Det Interrupt Handler         */ 
    handler_t   Xfp0_Handler;              /*   Xfp0 Interrupt Handler                      */ 
    handler_t   Xfp1_Handler;              /*   Xfp1 Interrupt Handler                      */ 
    handler_t   Random_ng_Handler;         /*   Random Number Generator Interrupt Handler   */ 
    handler_t   Timesync_Handler;          /*   Timesync Interrupt Handler                  */ 
    handler_t   Nic_idm_Handler;           /*   Nic Idm Interrupt Handler                   */ 
    handler_t   Avs_power_wdog_Handler;    /*   Avs Power Wdog Interrupt Handler            */ 
    handler_t   Avs_rocs_thresh1_Handler;  /*   Avs Rocs Thresh1 Interrupt Handler          */ 
    handler_t   Avs_rocs_thresh2_Handler;  /*   Avs Rocs Thresh2 Interrupt Handler          */ 
    handler_t   Avs_sw_done_Handler;       /*   Avs Software Done Interrupt Handler         */ 
    handler_t   Avs_pm_Handler;            /*   Avs Pm Interrupt Handler                    */ 
#endif /* BCM_53158_A0 */

} DeviceVectors_t;



#endif /* __VECTORS_H */

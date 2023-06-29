/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 * $Id: $
 *
 * File:
 *     irq.h   
 * Description:
 *     Avenger IRQ types in CMSIS model
 */

#ifndef __IRQ_H
#define __IRQ_H


#ifdef BCM_53158_A0

#include <sal_types.h>
#include <soc/robo2/bcm53158/intr.h>

/* Device Vectors */
#include <soc/robo2/bcm53158/vectors.h>

#define BCM53158_A0_IRQ_DEFAULT_PRIORITY  4
#define BCM53158_A0_IRQ_DEFAULT_GROUPING  0

typedef enum IRQn {

  /* M7 Core IRQ */
  NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
  HardFault_IRQn        = -13, /**<  3 HardFault Interrupt                   */
  MemoryManagement_IRQn = -12, /**<  4 Cortex-M7 Memory Management Interrupt */
  BusFault_IRQn         = -11, /**<  5 Cortex-M7 Bus Fault Interrupt         */
  UsageFault_IRQn       = -10, /**<  6 Cortex-M7 Usage Fault Interrupt       */
  SVCall_IRQn           = -5,  /**< 11 Cortex-M7 SV Call Interrupt           */
  DebugMonitor_IRQn     = -4,  /**< 12 Cortex-M7 Debug Monitor Interrupt     */
  PendSV_IRQn           = -2,  /**< 14 Cortex-M7 Pend SV Interrupt           */
  SysTick_IRQn          = -1,  /**< 15 Cortex-M7 System Tick Interrupt       */

  /* Avenger Specific IRQ */
  UC_TO_M7_SW_IRQn      = BCM53158_A0_8051_TO_M7_SW_INTR,
  UC_TO_M7_MB_IRQn      = BCM53158_A0_8051_TO_M7_MB_INTR,
  CPU_TO_M7_SW_IRQn     = BCM53158_A0_CPU_TO_M7_SW_INTR,
  CPU_TO_M7_MB_IRQn     = BCM53158_A0_CPU_TO_M7_MB_INTR,
  UC_TO_CPU_SW_IRQn     = BCM53158_A0_8051_TO_CPU_SW_INTR,
  UC_TO_CPU_MB_IRQn     = BCM53158_A0_8051_TO_CPU_MB_INTR,
  M7_TO_CPU_SW_IRQn     = BCM53158_A0_M7_TO_CPU_SW_INTR,
  M7_TO_CPU_MB_IRQn     = BCM53158_A0_M7_TO_CPU_MB_INTR,
  M7_TO_UC_SW_IRQn      = BCM53158_A0_M7_TO_8051_SW_INTR,
  M7_TO_UC_MB_IRQn      = BCM53158_A0_M7_TO_8051_MB_INTR,
  CPU_TO_UC_SW_IRQn     = BCM53158_A0_CPU_TO_8051_SW_INTR,
  CPU_TO_UC_MB_IRQn     = BCM53158_A0_CPU_TO_8051_MB_INTR,
  QSPI_IRQn             = BCM53158_A0_QSPI_INTR,
  SPI_IRQn              = BCM53158_A0_SPI_INTR,
  GPIO_IRQn             = BCM53158_A0_GPIO_INTR,
  UART_IRQn             = BCM53158_A0_UART_INTR,
  TIMER_IRQn            = BCM53158_A0_TIMER_INTR,
  WDOG_IRQn             = BCM53158_A0_WDOG_INTR,
  WDOG_RESET_IRQn       = BCM53158_A0_WDOG_RESET_INTR,
  SMBUS_I2C_IRQn        = BCM53158_A0_SMBUS_I2C_INTR,
  MIIM_OP_DONE_IRQn     = BCM53158_A0_MIIM_OP_DONE_INTR,
  SPI_MASTER_IRQn       = BCM53158_A0_SPI_MASTER_INTR,
  CB_IDM_IRQn           = BCM53158_A0_CB_IDM_INTR,
  HPA_IRQn              = BCM53158_A0_HPA_INTR,
  NPA_IRQn              = BCM53158_A0_NPA_INTR,
  RSVD_IRQn             = BCM53158_A0_RSVD_INTR,
  PM_IRQn               = BCM53158_A0_PM_INTR,
  APB2PBUS_IRQn         = BCM53158_A0_APB2PBUS_INTR,
  GPHY0_IRQn            = BCM53158_A0_GPHY0_INTR,
  GPHY1_IRQn            = BCM53158_A0_GPHY1_INTR,
  GPHY2_IRQn            = BCM53158_A0_GPHY2_INTR,
  GPHY3_IRQn            = BCM53158_A0_GPHY3_INTR,
  GPHY4_IRQn            = BCM53158_A0_GPHY4_INTR,
  GPHY5_IRQn            = BCM53158_A0_GPHY5_INTR,
  GPHY6_IRQn            = BCM53158_A0_GPHY6_INTR,
  GPHY7_IRQn            = BCM53158_A0_GPHY7_INTR,
  UNIMAC0_IRQn          = BCM53158_A0_UNIMAC0_INTR,
  UNIMAC1_IRQn          = BCM53158_A0_UNIMAC1_INTR,
  UNIMAC2_IRQn          = BCM53158_A0_UNIMAC2_INTR,
  UNIMAC3_IRQn          = BCM53158_A0_UNIMAC3_INTR,
  UNIMAC4_IRQn          = BCM53158_A0_UNIMAC4_INTR,
  UNIMAC5_IRQn          = BCM53158_A0_UNIMAC5_INTR,
  UNIMAC6_IRQn          = BCM53158_A0_UNIMAC6_INTR,
  UNIMAC7_IRQn          = BCM53158_A0_UNIMAC7_INTR,
  UNIMAC8_IRQn          = BCM53158_A0_UNIMAC8_INTR,
  UNIMAC9_IRQn          = BCM53158_A0_UNIMAC9_INTR,
  UNIMAC14_IRQn         = BCM53158_A0_UNIMAC14_INTR,
  CB_IRQn               = BCM53158_A0_CB_INTR,
  M7_RESET_IRQn         = BCM53158_A0_M7_RESET_INTR,
  M7_LOCKUP_IRQn        = BCM53158_A0_M7_LOCKUP_INTR,
  GPHY03_ENRGY_DET_IRQn = BCM53158_A0_GPHY03_ENRGY_DET_INTR,
  GPHY47_ENRGY_DET_IRQn = BCM53158_A0_GPHY47_ENRGY_DET_INTR,
  XFP0_IRQn             = BCM53158_A0_XFP0_INTR,
  XFP1_IRQn             = BCM53158_A0_XFP1_INTR,
  RANDOM_NG_IRQn        = BCM53158_A0_RANDOM_NG_INTR,
  TIMESYNC_IRQn         = BCM53158_A0_TIMESYNC_INTR,
  NIC_IDM_IRQn          = BCM53158_A0_NIC_IDM_INTR,
  AVS_POWER_WDOG_IRQn   = BCM53158_A0_AVS_POWER_WDOG_INTR,
  AVS_ROCS_THRESH1_IRQn = BCM53158_A0_AVS_ROCS_THRESH1_INTR,
  AVS_ROCS_THRESH2_IRQn = BCM53158_A0_AVS_ROCS_THRESH2_INTR,
  AVS_SW_DONE_IRQn      = BCM53158_A0_AVS_SW_DONE_INTR,
  AVS_PM_IRQn           = BCM53158_A0_AVS_PM_INTR,
  MAX_NUM_IRQn          = BCM53158_A0_MAX_INTR,
} IRQn_Type;

#endif /* BCM_53158_A0 */

#endif /* __IRQ_H */

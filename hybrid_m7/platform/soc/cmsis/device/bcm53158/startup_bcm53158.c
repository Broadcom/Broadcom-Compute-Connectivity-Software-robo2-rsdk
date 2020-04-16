/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 * $Id: $
 *
 * File:
 *     startup_bcm53158.c
 * Description:
 *     This file contains the exception vectors and boot code for the avenger m7 core
 *     This file has been written to comply with the ARM CMSIS model, although the filename is not compliant.
 */

#include <sal_types.h>
#include <sal_console.h>
#include <soc/robo2/bcm53158/irq.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/cpu_m7.h>
#include "sal/timer.h"

extern uint32 stext;
extern uint32 etext;
extern uint32 bss;
extern uint32 data;
extern uint32 __stack_start__;
extern uint32 __stack_end__;
extern uint32 __heap_start__;
extern uint32 __heap_end__;
extern uint32 __bss_start__;
extern uint32 __bss_end__;
extern uint32 __data_load__;
extern uint32 __data_start__;
extern uint32 __data_end__;

void common_handler(void);
void reset_handler(void);
void nmi_handler(void);
void hardfault_handler(void);
void memmanage_handler(void);
void busfault_handler(void);
void usagefault_handler(void);
void svc_handler(void);
void debugmon_handler(void);
void pendsv_handler(void);
void systick_handler(void);

#ifdef BCM_53158_A0
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
extern void avs_sw_done_handler(void);
extern void avs_pm_handler(void);
#endif /* BCM_53158_A0 */

/* Weak linkage for handlers */
#pragma weak nmi_handler = common_handler
#pragma weak busfault_handler = common_handler
#pragma weak memmanage_handler = common_handler
#pragma weak usagefault_handler = common_handler
#pragma weak svc_handler = common_handler
#pragma weak debugmon_handler = common_handler
#pragma weak pendsv_handler = common_handler
#pragma weak systick_handler = common_handler



#ifdef BCM_53158_A0
#pragma weak uc_to_m7_sw_handler = common_handler
#pragma weak uc_to_m7_mb_handler = common_handler
#pragma weak cpu_to_m7_sw_handler= common_handler
#pragma weak cpu_to_m7_mb_handler= common_handler
#pragma weak uc_to_cpu_sw_handler  = common_handler
#pragma weak uc_to_cpu_mb_handler  = common_handler
#pragma weak m7_to_cpu_sw_handler= common_handler
#pragma weak m7_to_cpu_mb_handler= common_handler
#pragma weak m7_to_uc_sw_handler   = common_handler
#pragma weak m7_to_uc_mb_handler   = common_handler
#pragma weak cpu_to_uc_sw_handler  = common_handler
#pragma weak cpu_to_uc_mb_handler  = common_handler
#pragma weak qspi_handler= common_handler
#pragma weak spi_handler = common_handler
#pragma weak gpio_handler= common_handler
#pragma weak uart_handler= common_handler
#pragma weak timer_handler   = common_handler
#pragma weak wdog_handler= common_handler
#pragma weak wdog_reset_handler  = common_handler
#pragma weak smbus_i2c_handler   = common_handler
#pragma weak miim_op_done_handler= common_handler
#pragma weak spi_master_handler  = common_handler
#pragma weak cb_idm_handler  = common_handler
#pragma weak hpa_handler = common_handler
#pragma weak npa_handler = common_handler
#pragma weak rsvd10_handler= common_handler
#pragma weak pm_handler  = common_handler
#pragma weak apb2pbus_handler= common_handler
#pragma weak gphy0_handler   = common_handler
#pragma weak gphy1_handler   = common_handler
#pragma weak gphy2_handler   = common_handler
#pragma weak gphy3_handler   = common_handler
#pragma weak gphy4_handler   = common_handler
#pragma weak gphy5_handler   = common_handler
#pragma weak gphy6_handler   = common_handler
#pragma weak gphy7_handler   = common_handler
#pragma weak unimac0_handler = common_handler
#pragma weak unimac1_handler = common_handler
#pragma weak unimac2_handler = common_handler
#pragma weak unimac3_handler = common_handler
#pragma weak unimac4_handler = common_handler
#pragma weak unimac5_handler = common_handler
#pragma weak unimac6_handler = common_handler
#pragma weak unimac7_handler = common_handler
#pragma weak unimac8_handler = common_handler
#pragma weak unimac9_handler = common_handler
#pragma weak unimac14_handler= common_handler
#pragma weak cb_handler  = common_handler
#pragma weak m7_reset_handler= common_handler
#pragma weak m7_lockup_handler   = common_handler
#pragma weak gphy03_enrgy_det_handler= common_handler
#pragma weak gphy47_enrgy_det_handler= common_handler
#pragma weak xfp0_handler= common_handler
#pragma weak xfp1_handler= common_handler
#pragma weak random_ng_handler   = common_handler
#pragma weak rimesync_handler= common_handler
#pragma weak nic_idm_handler = common_handler
#pragma weak avs_power_wdog_handler  = common_handler
#pragma weak avs_rocs_thresh1_handler= common_handler
#pragma weak avs_rocs_thresh2_handler= common_handler
#pragma weak avs_sw_done_handler = common_handler
#pragma weak avs_pm_handler  = common_handler
#endif /* BCM_53158_A0 */

#if !((OS == FREERTOS) | (OS == OPENRTOS))
static uint8  *__heap_ptr__ = NULL;
#endif


extern int main(void);
extern int SystemInit(void);
extern int SystemEarlyInit(void);

/* System Vector Table */

#ifdef BCM_53158_A0

__attribute__((section("vtable")))
const DeviceVectors_t bcm53158_a0_vectors = {

   .Stack_Base = (uint32)&__stack_start__,

   /* Cortex-M handlers */
   .Reset_Handler       =     reset_handler,           /* Reset Handler  */
   .NMI_Handler         =     nmi_handler,             /* NMI Handler  */
   .HardFault_Handler   =     hardfault_handler,       /* Hard Fault Handler  */
   .MemManage_Handler   =     memmanage_handler,       /* MPU Fault Handler  */
   .BusFault_Handler    =     busfault_handler,        /* Bus Fault Handler  */
   .UsageFault_Handler  =     usagefault_handler,      /* Usage Fault Handler  */
   .Rsvd0_Handler       =     0,                       /* Reserved  */
   .Rsvd1_Handler       =     0,                       /* Reserved  */
   .Rsvd2_Handler       =     0,                       /* Reserved  */
   .Rsvd3_Handler       =     0,                       /* Reserved  */
   .SVC_Handler         =     svc_handler,             /* SVCall Handler  */
   .DebugMon_Handler    =     debugmon_handler,        /* Debug Monitor Handler  */
   .Rsvd4_Handler       =     0,                       /* Reserved  */
   .PendSV_Handler      =     pendsv_handler,          /* PendSV Handler  */
#ifdef TIMER_SUPPORT
   .SysTick_Handler     =     systick_timer_isr,       /* SysTick Handler  */
#else
   .SysTick_Handler     =     systick_handler,         /* SysTick Handler  */
#endif

   /* BCM_53158_A0 Handlers */

   .Uc_to_m7_sw_Handler      =  uc_to_m7_sw_handler,       /*   8051 To M7 Software Interrupt Handler       */
   .Uc_to_m7_mb_Handler      =  uc_to_m7_mb_handler,       /*   8051 To M7 Mailbox Interrupt Handler        */
   .Cpu_to_m7_sw_Handler     =  cpu_to_m7_sw_handler,      /*   Cpu To M7 Software Interrupt Handler        */
   .Cpu_to_m7_mb_Handler     =  cpu_to_m7_mb_handler,      /*   Cpu To M7 Mailbox Interrupt Handler         */
   .Uc_to_cpu_sw_Handler     =  uc_to_cpu_sw_handler,      /*   8051 To Cpu Software Interrupt Handler      */
   .Uc_to_cpu_mb_Handler     =  uc_to_cpu_mb_handler,      /*   8051 To Cpu Mailbox Interrupt Handler       */
   .M7_to_cpu_sw_Handler     =  m7_to_cpu_sw_handler,      /*   M7 To Cpu Software Interrupt Handler        */
   .M7_to_cpu_mb_Handler     =  m7_to_cpu_mb_handler,      /*   M7 To Cpu Mailbox Interrupt Handler         */
   .M7_to_uc_sw_Handler      =  m7_to_uc_sw_handler,       /*   M7 To 8051 Software Interrupt Handler       */
   .M7_to_uc_mb_Handler      =  m7_to_uc_mb_handler,       /*   M7 To 8051 Mailbox Interrupt Handler        */
   .Cpu_to_uc_sw_Handler     =  cpu_to_uc_sw_handler,      /*   Cpu To 8051 Software Interrupt Handler      */
   .Cpu_to_uc_mb_Handler     =  cpu_to_uc_mb_handler,      /*   Cpu To 8051 Mailbox Interrupt Handler       */
   .Qspi_Handler             =  qspi_handler,              /*   Qspi Interrupt Handler                      */
   .Spi_Handler              =  spi_handler,               /*   Spi Interrupt Handler                       */
   .Gpio_Handler             =  gpio_handler,              /*   Gpio Interrupt Handler                      */
   .Uart_Handler             =  uart_handler,              /*   Uart Interrupt Handler                      */
   .Timer_Handler            =  timer_handler,             /*   Timer Interrupt Handler                     */
   .Wdog_Handler             =  wdog_handler,              /*   Wdog Interrupt Handler                      */
   .Wdog_reset_Handler       =  wdog_reset_handler,        /*   Wdog Reset Interrupt Handler                */
   .I2c_Handler              =  smbus_i2c_handler,         /*   Smbus I2c Interrupt Handler                 */
   .Miim_op_done_Handler     =  miim_op_done_handler,      /*   Miim Op Done Interrupt Handler              */
   .Spi_master_Handler       =  spi_master_handler,        /*   Spi Master Interrupt Handler                */
   .Cb_idm_Handler           =  cb_idm_handler,            /*   Cb Idm Interrupt Handler                    */
   .Hpa_Handler              =  hpa_handler,               /*   Hpa Interrupt Handler                       */
   .Npa_Handler              =  npa_handler,               /*   Npa Interrupt Handler                       */
   .Rsvd_Handler             =  rsvd10_handler,            /*   Rsvd Interrupt Handler                      */
   .Pm_Handler               =  pm_handler,                /*   Pm Interrupt Handler                        */
   .Apb2pbus_Handler         =  apb2pbus_handler,          /*   Apb2pbus Interrupt Handler                  */
   .Gphy0_Handler            =  gphy0_handler,             /*   Gphy0 Interrupt Handler                     */
   .Gphy1_Handler            =  gphy1_handler,             /*   Gphy1 Interrupt Handler                     */
   .Gphy2_Handler            =  gphy2_handler,             /*   Gphy2 Interrupt Handler                     */
   .Gphy3_Handler            =  gphy3_handler,             /*   Gphy3 Interrupt Handler                     */
   .Gphy4_Handler            =  gphy4_handler,             /*   Gphy4 Interrupt Handler                     */
   .Gphy5_Handler            =  gphy5_handler,             /*   Gphy5 Interrupt Handler                     */
   .Gphy6_Handler            =  gphy6_handler,             /*   Gphy6 Interrupt Handler                     */
   .Gphy7_Handler            =  gphy7_handler,             /*   Gphy7 Interrupt Handler                     */
   .Unimac0_Handler          =  unimac0_handler,           /*   Unimac0 Interrupt Handler                   */
   .Unimac1_Handler          =  unimac1_handler,           /*   Unimac1 Interrupt Handler                   */
   .Unimac2_Handler          =  unimac2_handler,           /*   Unimac2 Interrupt Handler                   */
   .Unimac3_Handler          =  unimac3_handler,           /*   Unimac3 Interrupt Handler                   */
   .Unimac4_Handler          =  unimac4_handler,           /*   Unimac4 Interrupt Handler                   */
   .Unimac5_Handler          =  unimac5_handler,           /*   Unimac5 Interrupt Handler                   */
   .Unimac6_Handler          =  unimac6_handler,           /*   Unimac6 Interrupt Handler                   */
   .Unimac7_Handler          =  unimac7_handler,           /*   Unimac7 Interrupt Handler                   */
   .Unimac8_Handler          =  unimac8_handler,           /*   Unimac8 Interrupt Handler                   */
   .Unimac9_Handler          =  unimac9_handler,           /*   Unimac9 Interrupt Handler                   */
   .Unimac14_Handler         =  unimac14_handler,          /*   Unimac14 Interrupt Handler                  */
   .Cb_Handler               =  cb_handler,                /*   Coronado Bridge Interrupt Handler           */
   .M7_reset_Handler         =  m7_reset_handler,          /*   M7 Reset Interrupt Handler                  */
   .M7_lockup_Handler        =  m7_lockup_handler,         /*   M7 Lockup Interrupt Handler                 */
   .Gphy03_enrgy_det_Handler =  gphy03_enrgy_det_handler,  /*   Gphy03 Energy Det Interrupt Handler         */
   .Gphy47_enrgy_det_Handler =  gphy47_enrgy_det_handler,  /*   Gphy47 Energy Det Interrupt Handler         */
   .Xfp0_Handler             =  xfp0_handler,              /*   Xfp0 Interrupt Handler                      */
   .Xfp1_Handler             =  xfp1_handler,              /*   Xfp1 Interrupt Handler                      */
   .Random_ng_Handler        =  random_ng_handler,         /*   Random Number Generator Interrupt Handler   */
   .Timesync_Handler         =  rimesync_handler,          /*   Timesync Interrupt Handler                  */
   .Nic_idm_Handler          =  nic_idm_handler,           /*   Nic Idm Interrupt Handler                   */
   .Avs_power_wdog_Handler   =  avs_power_wdog_handler,    /*   Avs Power Wdog Interrupt Handler            */
   .Avs_rocs_thresh1_Handler =  avs_rocs_thresh1_handler,  /*   Avs Rocs Thresh1 Interrupt Handler          */
   .Avs_rocs_thresh2_Handler =  avs_rocs_thresh2_handler,  /*   Avs Rocs Thresh2 Interrupt Handler          */
   .Avs_sw_done_Handler      =  avs_sw_done_handler,       /*   Avs Software Done Interrupt Handler         */
   .Avs_pm_Handler           =  avs_pm_handler,            /*   Avs Pm Interrupt Handler                    */
};
#endif /* BCM_53158_A0 */

/* Helpers and utilities */

#if !((OS == FREERTOS) | (OS == OPENRTOS))
uint8*
_sbrk (int size)
{
    uint8 *prev, *next, *sp;

    if (__heap_ptr__ == NULL) {
        __heap_ptr__ = (uint8*)&__heap_start__;
    }

    prev = __heap_ptr__;
    next = __heap_ptr__ + size;

    /* Sanitize */
    if (next > (uint8*)&__heap_end__) {
        return NULL;
    }
#ifdef CONFIG_STACK_CHECK
    asm volatile("mov %[spr], sp" : [spr] "=r" (sp) ::);
    if (((sp > (uint8*)__heap_start__) && (sp < (uint8*) &__heap_end__)) ||
        ((sp < (uint8*)__heap_start__) && (sp > (uint8*) &__heap_end__))) {
        assert(0);
    }
#endif

    __heap_ptr__ = next;
    return prev;
}
#else /* !((OS == FREERTOS) | (OS == OPENRTOS)) */

uint8*
_sbrk (int size)
{
    return NULL;
}

#endif /* !((OS == FREERTOS) | (OS == OPENRTOS)) */

void dump_frame(uint32* fault_stack)
{
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; 
    volatile uint32_t pc; 
    volatile uint32_t psr;

    r0 = fault_stack[0];
    r1 = fault_stack[1];
    r2 = fault_stack[2];
    r3 = fault_stack[3];
    r12 =fault_stack[4];
    lr = fault_stack[5];
    pc = fault_stack[6];
    psr =fault_stack[7];
    sysprintf("HardFault: pc: %08x lr: %08x psr: %08x r12: %08x\n", pc, lr, psr, r12);
    sysprintf("           r0: %08x r1: %08x  r2: %08x  r3: %08x\n", r0, r1, r2, r3);
    __DSB(); __ISB();
    sysprint("\n");
    /* Loop Forever */
    while(1);

}

/**
 * @brief Hardfault handler
 * This will handle hard fault
 * never returns 
 */
__attribute__((naked))
void hardfault_handler(void) 
{
    __asm volatile
    (
        " cpsid i                                                   \n"
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r2, dump_frame_const                                  \n"
        " bx r2                                                     \n"
        " dump_frame_const: .word dump_frame                        \n"
    );
}

/**
 * @brief common handler
 * This will notify of unhandled faults and ints
 * never returns 
 */
__attribute__((naked))
void common_handler(void) 
{
    __asm volatile
    (
        " cpsid i                                                   \n"
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, cb_const                                          \n"
        " bx r2                                                     \n"
        " cb_const: .word common_handler_callback                   \n"
    );
}

/**
 * @brief common_handler_callback
 * This routine will process unhandled interrupts and exceptions
 * never returns 
 */
void common_handler_callback(uint32 *fault_stack) 
{
    uint32 reg, ipsr;
    uint32 busfault, memfault, usefault;
    uint32 *cfsr = (uint32*)0xE000ED28L;
    uint32 *bfar = (uint32*)0xE000ED38L;
    uint32 *mmar = (uint32*)0xE000ED34L;
    //uint32 *abfsr = (uint32*)0xE000EFA8L;
    uint32 *icsr = (uint32*)0xE000ED04L;
  
    __ASM volatile ("MRS %0, ipsr" : "=r" (ipsr));
    sysprint("ERROR: Unhandled Intr/Exc Handler\n");
    sysprintf("IPSR:%08x ICSR:%08x CFSR:%08x \n", ipsr, *icsr, *cfsr);
  
    reg = *cfsr;
    busfault = (reg >> 8) & 0xFF;
    memfault = (reg >> 0) & 0xFF;
    usefault = (reg >> 16) & 0xFFFF;
    /* Bus Faults */
    if (busfault & 0x80) {
        sysprintf("BusFault Valid Set, BusFault Addr Reg %08x\n", *bfar);
    }
    if (busfault & 0x1) sysprint("Instruction Bus Error\n");
    if (busfault & 0x2) sysprint("Precise Data Bus Error\n");
    if (busfault & 0x4) sysprint("Imprecise Data Bus Error\n");
    if (busfault & 0x8) sysprint("Bus Error On Stacking\n");
    if (busfault & 0x10) sysprint("Bus Error On Unstacking\n");
  
    /* Memmanage Faults */
    if (memfault & 0x80) {
        sysprintf("MemMgmtFault Valid Set, MemMgmtFault Addr Reg %08x\n", *mmar);
    }
    if (memfault & 0x1) sysprint("Inst Fetch Violaton\n");
    if (memfault & 0x2) sysprint("Data Fetch Violaton\n");
    if (memfault & 0x8) sysprint("Fault on Unstacking \n");
    if (memfault & 0x10) sysprint("Fault on Stacking \n");

    /* Usage Faults */
    if (usefault & 0x1) sysprintf("Undefined Instr: Faulting Addr %08x\n", fault_stack[6]);
    if (usefault & 0x2) sysprintf("Invalid State: Faulting Addr %08x\n", fault_stack[6]);
    if (usefault & 0x4) sysprintf("Invalid PC: Faulting Addr %08x\n", fault_stack[6]);
    if (usefault & 0x8) sysprintf("No CoProcessor: Faulting Addr %08x\n", fault_stack[6]);
    if (usefault & 0x100) sysprintf("Unaligned Access: Faulting Addr %08x\n", fault_stack[6]);
    if (usefault & 0x200) sysprintf("Divide By Zero: Faulting Addr %08x\n", fault_stack[6]);

    sysprintf("pc: %08x lr: %08x psr: %08x r12: %08x\n", fault_stack[6], fault_stack[5], fault_stack[7], fault_stack[4]);
    sysprintf("r0: %08x r1: %08x  r2: %08x  r3: %08x\n", fault_stack[0], fault_stack[1], fault_stack[2], fault_stack[3]);
    sysprint("\n");
  
    /* No return from here */
    while(1);
}


/*
 * M7 Core System Reset Handler 
 */
__attribute__((naked))
void reset_handler(void)
{
    uint32 *p, *q;

    SystemEarlyInit();


    /* Relocate Data */
    if (&__data_load__ != &__data_start__) {
        for (p = &__data_start__, q = &__data_load__; p < &__data_end__; *p++ = *q++); 
    }

    /* Clear BSS */
    for (p = &__bss_start__; p < &__bss_end__; p++) { *p = 0; } 

    
    SystemInit();

    main();

    /* Never Return */
    while(1);
}


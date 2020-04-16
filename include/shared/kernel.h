/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 * File:
 *    kernel.h
 * Description
 *    kernel.h
 *
 */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#if 0 /* Uncoment when background task code is added to repository */
/* Tasking - Release CPU to other tasks */
extern void background_init(void) REENTRANT;
extern void background(void) REENTRANT;
#define POLL() background()

/* Tasking - Background task registration */
typedef void (*BACKGROUND_TASK_FUNC)(void *) REENTRANT;
extern void task_add(BACKGROUND_TASK_FUNC func, void *arg) REENTRANT;
extern void task_remove(BACKGROUND_TASK_FUNC func) REENTRANT;
extern void task_suspend(BACKGROUND_TASK_FUNC func) REENTRANT;
extern void task_resume(BACKGROUND_TASK_FUNC func) REENTRANT;

#endif

/* Timer registration/notification */
typedef void (*TIMER_FUNC)(void *) REENTRANT;
extern BOOL timer_add(TIMER_FUNC func, void *arg, uint32 usec) REENTRANT;
extern void timer_remove(TIMER_FUNC func) REENTRANT;

#if 0 /* Uncoment when link change/tx/rx tasks code is added to repository */
/* Link change registration/notification */
typedef void (*SYS_LINKCHANGE_FUNC)(uint16 port, BOOL link, void *arg) REENTRANT;
extern BOOL sys_register_linkchange(SYS_LINKCHANGE_FUNC func, void *arg) REENTRANT;
extern void sys_unregister_linkchange(SYS_LINKCHANGE_FUNC func) REENTRANT;

/* RX handler - prototype */
typedef enum {
    SYS_RX_INVALID,
    SYS_RX_NOT_HANDLED,
    SYS_RX_HANDLED,
    SYS_RX_HANDLED_AND_OWNED
} sys_rx_t;
typedef sys_rx_t (*SYS_RX_CBK_FUNC)(sys_pkt_t *pkt, void *cookie) REENTRANT;

/* RX handler - call to free packet buffer if SYS_RX_HANDLED_AND_OWNED */
extern void sys_rx_free_packet(sys_pkt_t *pkt) REENTRANT;

/* RX handler - registration
   Note: Do not register/unregister RX callbacks in RX callback function. */
#define SYS_RX_REGISTER_FLAG_ACCEPT_PKT_ERRORS      (1 << 0)
#define SYS_RX_REGISTER_FLAG_ACCEPT_TRUNCATED_PKT   (1 << 1)
extern sys_error_t sys_rx_register(
    SYS_RX_CBK_FUNC callback,
    uint8 priority,
    void *cookie,
    uint16 flags) REENTRANT;
extern sys_error_t sys_rx_unregister(SYS_RX_CBK_FUNC callback) REENTRANT;

/* Provide buffers to RX engine (at initialization phase) */
extern void sys_rx_add_buffer(uint8 *buffer, uint16 size) REENTRANT;

/* TX callback */
typedef void (*SYS_TX_CALLBACK)(sys_pkt_t *pkt, sys_error_t status) REENTRANT;

/* TX: if cbk is NULL, it will wait until completion */
extern sys_error_t sys_tx(sys_pkt_t *pkt, SYS_TX_CALLBACK cbk) REENTRANT;
#endif

#endif /* _KERNEL_H_ */

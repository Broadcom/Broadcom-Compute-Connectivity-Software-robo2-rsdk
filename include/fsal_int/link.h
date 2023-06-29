/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     link.h
 * * Purpose:
 * *     This file contains definitions internal to Link Scan module.
 */

#ifndef CBX_INT_LINK_H
#define CBX_INT_LINK_H

#include <fsal_int/types.h>
#include <sal_task.h>
#include <sal_sync.h>
#include <shared/types.h>
#include <fsal/link.h>

#define CBXI_LINKSCAN_INTERVAL_MIN   10000 /* 10 msec */
#define CBXI_LINKSCAN_DISABLE_WAIT   2 /* Delay 2 sec for link scan disable completion */

typedef struct cbx_ls_handler_s {
    struct cbx_ls_handler_s     *lh_next;
    cbx_link_scan_callback_t    lh_f;
    cbx_link_scan_info_t        lh_info;
} cbx_ls_handler_t;

/**
 * Link Scan Task Status
 */
typedef enum cbx_link_task_status_e {
    cbxLinkTaskStatusHalt,            /**< Link Scan task halted  */
    cbxLinkTaskStatusRunning,         /**< Link Scan task running */
} cbx_link_task_status_t;

typedef struct cbx_ls_cntl_s {
    char                init;    /* TRUE if Link scan module has been inited */
    cbx_link_task_status_t  task_status; /* Link Scan Task status */
    char                lc_taskname[16];
    sal_mutex_t         lc_lock;        /* Synchronization */
    sal_sem_t           lc_sema;        /* Link scan semaphore */
    pbmp_t              lc_pbm_sw[CBX_MAX_UNITS];/* Software link scan ports */
    pbmp_t              lc_pbm_link[CBX_MAX_UNITS];/* Ports currently up */
    cbx_ls_handler_t    *lc_handler;    /* List of handlers */
    int                 lc_us;          /* Time between scans (us) */
    sal_thread_t        lc_thread;      /* Link scan thread */
} cbx_ls_cntl_t;

int cbxi_robo2_link_get(int unit, cbx_port_t port, int *link);

int cbxi_link_scan_wakeup(void);

#endif /* !CBX_INT_LINK_H  */

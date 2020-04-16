/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     stat.h
 * * Purpose:
 * *     This file contains definitions internal to STAT module.
 */

#ifndef CBX_INT_STAT_H
#define CBX_INT_STAT_H

#include <fsal_int/types.h>
#include <fsal/stat.h>
#include <soc/types.h>

#define PORT_STAT_MAX  90
/* SNMP counters */
typedef enum counter_thread_e {
    COUNTER_THREAD_START,
    COUNTER_THREAD_STOP,
    COUNTER_THREAD_SYNC
} counter_thread_t;

extern int
counter_thread_set(int unit, counter_thread_t thread_op, uint32 flags,
                       int interval, soc_pbmp_t bmp);
extern int counter_reset(int unit);
extern int counter_get(int unit, uint32_t port,
                    cbx_port_stat_counter_t counter_type,
                    int sync_hw, uint64_t *val);


#endif


/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     tse.h
 * * Purpose:
 * *     This file contains definitions internal to Timesync Module.
 */

#ifndef CBX_INT_TSE_H
#define CBX_INT_TSE_H

#include "fsal_int/types.h"
#include "shared/types.h"
#include "fsal/ts.h"

extern int tse_init();
extern int tse_tod_get(cbx_ts_time_t *time, uint64_t* timestamp);
extern int tse_tod_set(cbx_ts_time_t *time, uint64_t timestamp);
extern void usb_serial_msg_write(char *msg);

#endif   //CBX_INT_TSE_H
/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     lin.h
 * * Purpose:
 * *     This file contains definitions internal to LIN module.
 */

#ifndef CBX_INT_LIN_H
#define CBX_INT_LIN_H

#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <fsal_int/types.h>

#define CBXI_LOGICAL_IF_MIN            1
#define CBXI_LOGICAL_IF_MAX            2048

extern int cbxi_lin_init( void );
extern int cbxi_lin_slot_get(int *linid );
extern int cbxi_lin_slot_release(int linid);
extern int cbxi_lin_dest_get(int unit, int linid, cbxi_pgid_t *lpgid);
extern int cbxi_lin_dest_set(int unit, int linid, cbxi_pgid_t lpgid);
extern int cbxi_ptp_lin_source_set(int unit, int linid,
                         cbxi_pgid_t lpgid, uint32_t vsiid, uint8_t slic_type);
extern int cbxi_ptp_lin_source_release(int unit, int linid, cbxi_pgid_t lpgid,
                                       uint32_t vsiid, uint8_t slic_type);
#endif /* CBX_INT_LIN_H */

/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     trap.h
 * * Purpose:
 * *     This file contains definitions internal to Trap module.
 */

#ifndef CBX_INT_TRAP_H
#define CBX_INT_TRAP_H

#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include "fsal/exception.h"
#include <fsal_int/types.h>

/* Trap group enum (last 16 entries in MTGT) */
typedef enum {
    TRAP_GROUP_NONE                 = 0,
    TRAP_GROUP_TRAP2CP              = 1, /* Trap to CPU */
    TRAP_GROUP_TRAP2CP_CSD          = 2, /* Trap to CPU on cascade */
    TRAP_GROUP_TRAP2CASC            = 3, /* Trap to Cascaed port*/
    TRAP_GROUP_TRAP2CP_TS           = 4, /* Trap to CPU with TS */
    TRAP_GROUP_TRAP2CP_CSD_TS       = 5, /* Trap to CPU on csd with TS */
    TRAP_GROUP_DROP                 = 6, /* Drop packets */
} cbxi_trap_group_t;

/* SLIC trap groups (maps to trapids 17 to 31)
 * Used to configure trap_group field in SRT*/
typedef enum cbxi_slic_trap_e {
    SLIC_TRAP_GROUP0           = 0,/* Trap disabled */
    SLIC_TRAP_GROUP1           = 1,/* SLIC_GROUP1 = 17 : Trap */
    SLIC_TRAP_GROUP2           = 2,/* SLIC_GROUP2 = 18 : Drop & Trap */
    SLIC_TRAP_GROUP3           = 3,/* SLIC_GROUP3 = 19 : Drop */
    SLIC_TRAP_GROUP4           = 4,/* SLIC_GROUP4 = 20 : Trap with TS */
    SLIC_TRAP_GROUP5           = 5,/* SLIC_GROUP5 = 21 : Drop & Trap with TS*/
    SLIC_TRAP_GROUP6           = 6,
    SLIC_TRAP_GROUP7           = 7,
    SLIC_TRAP_GROUP8           = 8,
    SLIC_TRAP_GROUP9           = 9,
    SLIC_TRAP_GROUP10          = 10,
    SLIC_TRAP_GROUP11          = 11,
    SLIC_TRAP_GROUP12          = 12,
    SLIC_TRAP_GROUP13          = 13,
    SLIC_TRAP_GROUP14          = 14,
    SLIC_TRAP_GROUP15          = 15,/* SLIC_GROUP15 = 31*/
} cbxi_slic_trap_t;

/* Trap id */
typedef cbx_trap_t cbxi_trapid_t;

/* Trap flags */
#define CBXI_TRAP_TO_CPU            0x1
#define CBXI_TRAP_TO_CASCADED_PORT  0x2
#define CBXI_TRAP_DROP              0x3
#define CBXI_TRAP_TO_CPU_TS         0x4

typedef struct cbxi_trap_ctrl_s {
    uint8_t  drop;
    uint8_t  term;
    uint16_t trap_flags;
} cbxi_trap_ctrl_t;

extern int cbxi_trap_init(void);
extern int cbxi_trap_control_get(int unit,
            cbxi_trapid_t trap_id, cbxi_trap_ctrl_t *trap_ctl);
extern int cbxi_trap_control_set(int unit,
            cbxi_trapid_t trap_id, cbxi_trap_ctrl_t *trap_ctl);
extern int cbxi_trap_dest_update(int unit, cbx_portid_t portid);
extern int cbxi_dest_cascade_set(int unit, cbx_portid_t portid);

#endif /* CBX_INT_TRAP_H */

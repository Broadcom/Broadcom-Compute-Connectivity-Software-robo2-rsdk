/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     auth.h
 * * Purpose:
 * *     This file contains definitions internal to Authentication module.
 */

#ifndef CBX_INT_AUTH_H
#define CBX_INT_AUTH_H

#include <fsal_int/types.h>
#include <shared/types.h>
#include <fsal/auth.h>

#define  CBXI_AUTH_PORT_ADD            0x00000001
#define  CBXI_AUTH_PORT_REMOVE         0x00000002

#define  CBXI_AUTH_SLIC_PGMAP_SET      0x00000001
#define  CBXI_AUTH_SLIC_PGMAP_CLEAR    0x00000002

/* TBD set Trap group for CPU trap */
#define  CBXI_TRAP_GROUP_CPU_PORT      10

typedef struct cbxi_auth_port_s {
    int         in_use;/* AUTH port entry use/free flag */
    uint32      flags; /* Store the current Auth. mode(ex. CBX_AUTH_PORT_XX) */
} cbxi_auth_port_t;

typedef struct cbx_auth_cntl_s {
    char                 init; /* TRUE if AUTH module inited */
    cbx_auth_params_t    auth_params;/* global config for AUTH */
    /* port auth info */
    cbxi_auth_port_t     port_auth[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT];
    pbmp_t               auth_pbmp[CBX_MAX_UNITS];/* Auth member port bit map for master/slave Avenger */
    uint32               trap2cp_enable[CBX_MAX_UNITS];
} cbx_auth_cntl_t;

#endif /* !CBX_INT_AUTH_H  */

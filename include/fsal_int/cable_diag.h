/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     cable_diag.h
 * * Purpose:
 * *     This file contains definitions internal to cable diag.
 */

#ifndef CBX_INT_CDIAG_H
#define CBX_INT_CDIAG_H

#include <shared/types.h>

#define PHY_REG_READ_WRITE_TIMEOUT_COUNT 100

/* Cable diag ECD values */
#define  ECD_INVALID          0x0
#define  ECD_NO_FAULT         0x1
#define  ECD_OPEN             0x2
#define  ECD_SHORT            0x3
#define  ECD_PIN_SHORT_OR_XT  0x4
#define  ECD_FORCED           0x9
#define  ECD_FAULT            (ECD_OPEN | ECD_SHORT | ECD_PIN_SHORT_OR_XT)

/* ECD_CTRL is 0x9400 to ensure cable diag executed immediately as per phy team
 * input */
#if 1
#define  ECD_CTRL             0x9400 /* Length in meters */
#else
#define  ECD_CTRL             0x9000 /* Length in centimeters */
#endif

/* Cable diag phy register definition */
#define PHY_DATA_REG            0x15
#define PHY_ADDR_REG            0x17

#if 0 /*Registers for Expansion register access method */
#define ECD_CONTROL_REG         0X0FC0
#define ECD_FAULT_REG           0X0FC1
#define ECD_PAIR_A_LEN_REG      0X0FC2
#define ECD_PAIR_B_LEN_REG      0X0FC3
#define ECD_PAIR_C_LEN_REG      0X0FC4
#define ECD_PAIR_D_LEN_REG      0X0FC5
#endif

/* Registers for RDB access method */
#define RDB_EN_REG              0x7E
#define RDB_DATA_REG            0x1F
#define RDB_ADDR_REG            0x1E
#define ECD_CONTROL_REG         0x2A0
#define ECD_FAULT_REG           0x2A1
#define ECD_PAIR_A_LEN_REG      0x2A2
#define ECD_PAIR_B_LEN_REG      0x2A3
#define ECD_PAIR_C_LEN_REG      0x2A4
#define ECD_PAIR_D_LEN_REG      0x2A5


extern int cbxi_cdiag_run_all (void);
#endif


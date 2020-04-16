/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename: 
 *    register.h
 * Description: 
 *    Contains the defintions of register
 */

#ifndef __REGISTER_H
#define __REGISTER_H

#include <soc/defs.h>
#include <soc/robo2/common/allenum.h>
#include <soc/field.h>

/*
 * Options for soc_reg_addr_get()
 *
 * WRITE          - Indicates this is a call for a write operation, which
 *                  may result in a different block number.
 * PRESERVE_PORT  - Preserve the value of the 'port' argument and
 *                  do not make any translation to the the index
 *                  (e.g. use 'port' value for physical port).
 */
#define SOC_REG_ADDR_OPTION_NONE             (0x0)
#define SOC_REG_ADDR_OPTION_WRITE            (1 << 0)
#define SOC_REG_ADDR_OPTION_PRESERVE_PORT    (1 << 1)
#define SOC_REG_BAD_ADDR                     (0xFFFFFFFF)

#define SOC_REG_FLAG_64_BITS (1<<0)     /* Register is 64 bits wide */
#define SOC_REG_FLAG_COUNTER (1<<1)     /* Register is a counter */
#define SOC_REG_FLAG_ARRAY   (1<<2)     /* Register is an array */
#define SOC_REG_FLAG_NO_DGNL (1<<3)     /* Array does not have diagonal els */
#define SOC_REG_FLAG_RO      (1<<4)     /* Register is write only */
#define SOC_REG_FLAG_WO      (1<<5)     /* Register is read only */
#define SOC_REG_FLAG_ED_CNTR (1<<6)     /* Counter of discard/error */
#define SOC_REG_FLAG_SPECIAL (1<<7)     /* Counter requires special
                                           processing */
#define SOC_REG_FLAG_EMULATION    (1<<8)    /* Available only in emulation */
#define SOC_REG_FLAG_VARIANT1    (1<<9)    /* Not available in chip variants  */
#define SOC_REG_FLAG_VARIANT2    (1<<10)    /* -- '' -- */
#define SOC_REG_FLAG_VARIANT3    (1<<11)    /* -- '' -- */
#define SOC_REG_FLAG_VARIANT4    (1<<12)    /* -- '' -- */
#define SOC_REG_FLAG_32_BITS    (1<<13) /* Register is 32 bits wide */
#define SOC_REG_FLAG_16_BITS    (1<<14) /* Register is 16 bits wide */
#define SOC_REG_FLAG_8_BITS     (1<<15) /* Register is 8 bits wide */
#define SOC_REG_FLAG_ARRAY2     (1<<16) /* Register is an array with stride 2*/
#define SOC_REG_FLAG_ARRAY4     (1<<17) /* Register is an array with stride 4*/
#define SOC_REG_FLAG_ABOVE_64_BITS (1<<20)     /* Register is above 64 bits wide */
#define SOC_REG_FLAG_REG_ARRAY  (1<<21)     /* This is a register array with indexed access */
#define SOC_REG_FLAG_INTERRUPT  (1<<22)     /* This is register is an interrupt */
#define SOC_REG_FLAG_GENERAL_COUNTER (1<<23) /* This is a general counter */
#define SOC_REG_FLAG_SIGNAL (1<<24) /* this is signal register */
#define SOC_REG_FLAG_IGNORE_DEFAULT (1<<25) /* this is register, default value of it should be ignored */
#define SOC_REG_FLAG_INDIRECT   (1<<26) /* Robo2 register is indirect */


/* argument to soc_reg_addr() */
#define    REG_PORT_ANY        (-10)

/* use bit 31 to indicate the port argument is an instance id */
#define SOC_REG_ADDR_INSTANCE_BP         (31)
#define SOC_REG_ADDR_INSTANCE_MASK       (1<<SOC_REG_ADDR_INSTANCE_BP)
#define SOC_REG_ADDR_SCHAN_ID_MASK       (1<<(SOC_REG_ADDR_INSTANCE_BP-1))
#define SOC_REG_ADDR_BLOCK_ID_MASK       (1<<(SOC_REG_ADDR_INSTANCE_BP-2))
#define SOC_REG_ADDR_PHY_ACC_MASK        (1<<(SOC_REG_ADDR_INSTANCE_BP-3))

#define _SOC_MAX_REGLIST        (2 * SOC_MAX_NUM_PORTS * SOC_MAX_NUM_COS)


typedef struct soc_reg_info_t {
    soc_block_types_t  block;
    soc_regtype_t      regtype;         /* Also indicates invalid */
    int                numels;          /* If array, num els in array. */
                                        /* Otherwise -1. */
    uint32             offset;          /* Includes 2-bit form field */

    uint32             flags;
    int                nFields;
    int                fidx;            /* Fields database index */
#if !defined(SOC_NO_RESET_VALS)
    uint32             rst_val_lo;
    uint32             rst_val_hi;      /* 64-bit regs only */
    uint32             rst_mask_lo;     /* 1 where resetVal is valid */
    uint32             rst_mask_hi;     /* 64-bit regs only */
#endif
} soc_reg_info_t;

#endif  /* __REGISTER_H */

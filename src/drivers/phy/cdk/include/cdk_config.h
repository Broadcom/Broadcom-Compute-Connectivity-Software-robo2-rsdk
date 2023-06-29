/*
 * $Id$
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 * 
 * $Copyright: Copyright 2017 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

/*
 * This config file defines all compilation-time specifications for the CDK.
 * 
 * Reasonable defaults are provided for all configuration options where
 * appropriate.
 * 
 * You need not edit this file directly to change your configuration, nor is
 * modifying this file advised -- so doing will require manually merging
 * whenever the CDK is upgraded.
 * 
 * You should provide your own configuration options or overrides through
 * a combination of:
 * 
 *      1. The compiler command line, such as -D{OPTION}={VALUE}
 *      2. Create your own custom configuration file:
 *         a) Create a file called 'cdk_custom_config.h'
 *         b) Define all custom settings, using this file as the reference
 *         c) Add -DCDK_INCLUDE_CUSTOM_CONFIG to your compilation
 *         d) Make sure the compilation include path includes 'cdk_custom_config.h'
 */
#ifndef __CDK_CONFIG_H__
#define __CDK_CONFIG_H__


/*
 * Include system config file if specified:
 */
#ifdef CDK_INCLUDE_CUSTOM_CONFIG
#include <cdk_custom_config.h>
#endif


/*
 * OPTIONAL configuration and feature values.
 * Defaults are provided for all non-specified values.
 */

/* Please see libc_assert.c and cdk_assert.h for details on providing a custom handler */
#ifndef CDK_CONFIG_INCLUDE_ASSERT
#define CDK_CONFIG_INCLUDE_ASSERT               1
#endif

/* Maximum number of chips supported */
#ifndef CDK_CONFIG_MAX_UNITS
#define CDK_CONFIG_MAX_UNITS                    8
#endif

/* Maximum number of ports per chip supported */
#ifndef CDK_CONFIG_MAX_PORTS
#define CDK_CONFIG_MAX_PORTS                    160
#endif

/* Maximum number of SCHAN polls */
#ifndef CDK_CONFIG_SCHAN_MAX_POLLS
#define CDK_CONFIG_SCHAN_MAX_POLLS              100000
#endif

/* Maximum number of MIIM polls */
#ifndef CDK_CONFIG_MIIM_MAX_POLLS
#define CDK_CONFIG_MIIM_MAX_POLLS               100000
#endif

/* Direct access to memory-mapped registers */
#ifndef CDK_CONFIG_MEMMAP_DIRECT
#define CDK_CONFIG_MEMMAP_DIRECT                0
#endif

/* Memory Barrier if necessary */
#ifndef CDK_CONFIG_MEMORY_BARRIER
#define CDK_CONFIG_MEMORY_BARRIER               ;
#endif

/* Logical to physical port mappings */
#ifndef CDK_CONFIG_INCLUDE_PORT_MAP
#define CDK_CONFIG_INCLUDE_PORT_MAP             0
#endif

/* Dynamic chip and port configuration */
#ifndef CDK_CONFIG_INCLUDE_DYN_CONFIG
#define CDK_CONFIG_INCLUDE_DYN_CONFIG           1
#endif

/* Enable debug messages */
#ifndef CDK_CONFIG_INCLUDE_DEBUG
#define CDK_CONFIG_INCLUDE_DEBUG                1
#endif

/* Enable Reset values */
#ifndef CDK_CONFIG_INCLUDE_RESET_VALUES
#define CDK_CONFIG_INCLUDE_RESET_VALUES                1
#endif

/*
 * Include chip symbol tables for the debug shell.
 * No symbolic debugging (register/memory names) will be available
 * without this defined.
 * 
 * You should enable at least these symbols if you can afford the space.
 * 
 * This define is required to get any symbols at all.
 * If you only wish to include symbols for a subset of chips in the system
 * (probably for code space reasons), you can define the following for each chip
 * whose symbols you wish to EXCLUDE:
 * 
 *       CDK_CONFIG_EXCLUDE_CHIP_SYMBOLS_<CHIP>
 * 
 */
#ifndef CDK_CONFIG_INCLUDE_CHIP_SYMBOLS
#define CDK_CONFIG_INCLUDE_CHIP_SYMBOLS         1
#endif

/*
 * Include register and memory field information for the debug shell.
 * This provides encoding, decoding, and displaying individual field values for
 * each register and memory.
 * 
 * Requires more code space than just the chip symbols alone.
 * The per-chip exclusion define (CDK_CONFIG_EXCLUDE_FIELD_INFO_<CHIP>) also applies.
 */
#ifndef CDK_CONFIG_INCLUDE_FIELD_INFO
#define CDK_CONFIG_INCLUDE_FIELD_INFO           1
#endif

/*
 * Use symbolic names for all of the fields in a register or memory when
 * encoding or decoding them.
 * 
 * This is the most powerful option, but increases the code size a little beyond
 * the basic field information (which deals only with the bit spans of the fields).
 * 
 * Definitely enable if you have space.
 * The per-chip exclusion define (CDK_CONFIG_EXCLUDE_FIELD_NAMES_<CHIP> also applies.
 */
#ifndef CDK_CONFIG_INCLUDE_FIELD_NAMES
#define CDK_CONFIG_INCLUDE_FIELD_NAMES          1
#endif

/*
 * Include alternative symbol names for registers and memories.
 * Mainly for internal Broadcom use, so you can safely leave this option off.
 */
#ifndef CDK_CONFIG_INCLUDE_ALIAS_NAMES
#define CDK_CONFIG_INCLUDE_ALIAS_NAMES          0
#endif

/* CDK DSYMs Configuration */

/*
 * Use the regenerated symbol tables produced by the DSYM program with the debug shell instead of the individual chip-based static tables.
 * When rebuilt according to your intended configuration the regenerated DSYM symbol tables will be smaller than the originals.
 */
#ifndef CDK_CONFIG_CHIP_SYMBOLS_USE_DSYMS
#define CDK_CONFIG_CHIP_SYMBOLS_USE_DSYMS       0
#endif

/* Use the DSYM regenerated symbol table for all dynamic chip programming structures instead of the individual definitions. */
#ifndef CDK_CONFIG_DSYMS_USE_SYMTAB
#define CDK_CONFIG_DSYMS_USE_SYMTAB             0
#endif

/* Include all DSYM generated structures in a single build. This is used for testing purposes only, and should remain disabled by default. */
#ifndef CDK_CONFIG_DSYMS_INCLUDE_ALL
#define CDK_CONFIG_DSYMS_INCLUDE_ALL            0
#endif

/* Include the name string for each DSYM object. Makes debugging easier at the cost of string space. */
#ifndef CDK_CONFIG_DSYMS_INCLUDE_NAMES
#define CDK_CONFIG_DSYMS_INCLUDE_NAMES          0
#endif

/* Redirect the static definitions in the chip headers to the DSYM versions. */
#ifndef CDK_CONFIG_CHIP_DEFS_USE_DSYMS
#define CDK_CONFIG_CHIP_DEFS_USE_DSYMS          0
#endif

/* CDK Debug Shell Configuration. */

/* Maximum shell input line length. */
#ifndef CDK_CONFIG_SHELL_IO_MAX_LINE
#define CDK_CONFIG_SHELL_IO_MAX_LINE            128
#endif

/* Maximum length of the shell prompt. */
#ifndef CDK_CONFIG_SHELL_IO_MAX_PROMPT
#define CDK_CONFIG_SHELL_IO_MAX_PROMPT          32
#endif

/* Maximum number of shell command arguments. */
#ifndef CDK_CONFIG_SHELL_MAX_ARGS
#define CDK_CONFIG_SHELL_MAX_ARGS               12
#endif

/* Prefix used when reporting errors during shell commands. */
#ifndef CDK_CONFIG_SHELL_ERROR_STR
#define CDK_CONFIG_SHELL_ERROR_STR              "*** error: "
#endif

/* Allow command prefix to select current unit. */
#ifndef CDK_CONFIG_SHELL_UNIT_PREFIX
#define CDK_CONFIG_SHELL_UNIT_PREFIX            1
#endif

/* Include the HELP Command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_HELP
#define CDK_CONFIG_SHELL_INCLUDE_HELP           1
#endif

/* Maximum number of command help lines. */
#ifndef CDK_CONFIG_SHELL_MAX_HELP_LINES
#define CDK_CONFIG_SHELL_MAX_HELP_LINES         4
#endif

/* This is the default setting for all unspecified shell commands. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#define CDK_CONFIG_SHELL_INCLUDE_DEFAULT        1
#endif

/* Include the GETI Command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_GETI
#define CDK_CONFIG_SHELL_INCLUDE_GETI           1
#endif

/* Include the SETI Command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_SETI
#define CDK_CONFIG_SHELL_INCLUDE_SETI           1
#endif

/* Include the GET Command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_GET
#define CDK_CONFIG_SHELL_INCLUDE_GET            CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Include the SET Command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_SET
#define CDK_CONFIG_SHELL_INCLUDE_SET            CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Include the CDK Command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_CDK
#define CDK_CONFIG_SHELL_INCLUDE_CDK            CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Include the UNIT command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_UNIT
#define CDK_CONFIG_SHELL_INCLUDE_UNIT           CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Include the LIST command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_LIST
#define CDK_CONFIG_SHELL_INCLUDE_LIST           CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Include the PID command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_PID
#define CDK_CONFIG_SHELL_INCLUDE_PID            CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Include the DEBUG command. */
#ifndef CDK_CONFIG_SHELL_INCLUDE_DEBUG
#define CDK_CONFIG_SHELL_INCLUDE_DEBUG          CDK_CONFIG_SHELL_INCLUDE_DEFAULT
#endif

/* Type uint8_t is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_UINT8_T
#define CDK_CONFIG_DEFINE_UINT8_T               1
#endif

/* Default type definition for uint8_t */
#ifndef CDK_CONFIG_TYPE_UINT8_T
#define CDK_CONFIG_TYPE_UINT8_T                 unsigned char
#endif

/* Type uint16_t is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_UINT16_T
#define CDK_CONFIG_DEFINE_UINT16_T              1
#endif

/* Default type definition for uint16_t */
#ifndef CDK_CONFIG_TYPE_UINT16_T
#define CDK_CONFIG_TYPE_UINT16_T                unsigned short
#endif

/* Type uint32_t is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_UINT32_T
#define CDK_CONFIG_DEFINE_UINT32_T              1
#endif

/* Default type definition for uint32_t */
#ifndef CDK_CONFIG_TYPE_UINT32_T
#define CDK_CONFIG_TYPE_UINT32_T                unsigned int
#endif

/* Type size_t is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_SIZE_T
#define CDK_CONFIG_DEFINE_SIZE_T                1
#endif

/* Default type definition for size_t */
#ifndef CDK_CONFIG_TYPE_SIZE_T
#define CDK_CONFIG_TYPE_SIZE_T                  unsigned int
#endif

/* Type dma_addr_t is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_DMA_ADDR_T
#define CDK_CONFIG_DEFINE_DMA_ADDR_T            1
#endif

/* Default type definition for dma_addr_t */
#ifndef CDK_CONFIG_TYPE_DMA_ADDR_T
#define CDK_CONFIG_TYPE_DMA_ADDR_T              unsigned int
#endif

/* Formatting macro CDK_PRIu32 is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_PRIu32
#define CDK_CONFIG_DEFINE_PRIu32                1
#endif

/* Default definition for formatting macro CDK_PRIu32 */
#ifndef CDK_CONFIG_MACRO_PRIu32
#define CDK_CONFIG_MACRO_PRIu32                 "u"
#endif

/* Formatting macro CDK_PRIx32 is not provided by the system */
#ifndef CDK_CONFIG_DEFINE_PRIx32
#define CDK_CONFIG_DEFINE_PRIx32                1
#endif

/* Default definition for formatting macro CDK_PRIx32 */
#ifndef CDK_CONFIG_MACRO_PRIx32
#define CDK_CONFIG_MACRO_PRIx32                 "x"
#endif

#endif /* __CDK_CONFIG_H__ */

#ifdef CONFIG_OPTION
#ifdef CDK_INCLUDE_CUSTOM_CONFIG
CONFIG_OPTION(CDK_INCLUDE_CUSTOM_CONFIG)
#endif
#ifdef CDK_CONFIG_INCLUDE_ASSERT
CONFIG_OPTION(CDK_CONFIG_INCLUDE_ASSERT)
#endif
#ifdef CDK_CONFIG_MAX_UNITS
CONFIG_OPTION(CDK_CONFIG_MAX_UNITS)
#endif
#ifdef CDK_CONFIG_MAX_PORTS
CONFIG_OPTION(CDK_CONFIG_MAX_PORTS)
#endif
#ifdef CDK_CONFIG_SCHAN_MAX_POLLS
CONFIG_OPTION(CDK_CONFIG_SCHAN_MAX_POLLS)
#endif
#ifdef CDK_CONFIG_MIIM_MAX_POLLS
CONFIG_OPTION(CDK_CONFIG_MIIM_MAX_POLLS)
#endif
#ifdef CDK_CONFIG_MEMMAP_DIRECT
CONFIG_OPTION(CDK_CONFIG_MEMMAP_DIRECT)
#endif
#ifdef CDK_CONFIG_INCLUDE_PORT_MAP
CONFIG_OPTION(CDK_CONFIG_INCLUDE_PORT_MAP)
#endif
#ifdef CDK_CONFIG_INCLUDE_DYN_CONFIG
CONFIG_OPTION(CDK_CONFIG_INCLUDE_DYN_CONFIG)
#endif
#ifdef CDK_CONFIG_INCLUDE_DEBUG
CONFIG_OPTION(CDK_CONFIG_INCLUDE_DEBUG)
#endif
#ifdef CDK_CONFIG_INCLUDE_CHIP_SYMBOLS
CONFIG_OPTION(CDK_CONFIG_INCLUDE_CHIP_SYMBOLS)
#endif
#ifdef CDK_CONFIG_INCLUDE_FIELD_INFO
CONFIG_OPTION(CDK_CONFIG_INCLUDE_FIELD_INFO)
#endif
#ifdef CDK_CONFIG_INCLUDE_FIELD_NAMES
CONFIG_OPTION(CDK_CONFIG_INCLUDE_FIELD_NAMES)
#endif
#ifdef CDK_CONFIG_INCLUDE_ALIAS_NAMES
CONFIG_OPTION(CDK_CONFIG_INCLUDE_ALIAS_NAMES)
#endif
#ifdef CDK_CONFIG_CHIP_SYMBOLS_USE_DSYMS
CONFIG_OPTION(CDK_CONFIG_CHIP_SYMBOLS_USE_DSYMS)
#endif
#ifdef CDK_CONFIG_DSYMS_USE_SYMTAB
CONFIG_OPTION(CDK_CONFIG_DSYMS_USE_SYMTAB)
#endif
#ifdef CDK_CONFIG_DSYMS_INCLUDE_ALL
CONFIG_OPTION(CDK_CONFIG_DSYMS_INCLUDE_ALL)
#endif
#ifdef CDK_CONFIG_DSYMS_INCLUDE_NAMES
CONFIG_OPTION(CDK_CONFIG_DSYMS_INCLUDE_NAMES)
#endif
#ifdef CDK_CONFIG_CHIP_DEFS_USE_DSYMS
CONFIG_OPTION(CDK_CONFIG_CHIP_DEFS_USE_DSYMS)
#endif
#ifdef CDK_CONFIG_SHELL_IO_MAX_LINE
CONFIG_OPTION(CDK_CONFIG_SHELL_IO_MAX_LINE)
#endif
#ifdef CDK_CONFIG_SHELL_IO_MAX_PROMPT
CONFIG_OPTION(CDK_CONFIG_SHELL_IO_MAX_PROMPT)
#endif
#ifdef CDK_CONFIG_SHELL_MAX_ARGS
CONFIG_OPTION(CDK_CONFIG_SHELL_MAX_ARGS)
#endif
#ifdef CDK_CONFIG_SHELL_UNIT_PREFIX
CONFIG_OPTION(CDK_CONFIG_SHELL_UNIT_PREFIX)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_HELP
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_HELP)
#endif
#ifdef CDK_CONFIG_SHELL_MAX_HELP_LINES
CONFIG_OPTION(CDK_CONFIG_SHELL_MAX_HELP_LINES)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_DEFAULT
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_DEFAULT)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_GETI
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_GETI)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_SETI
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_SETI)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_GET
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_GET)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_SET
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_SET)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_CDK
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_CDK)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_UNIT
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_UNIT)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_LIST
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_LIST)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_PID
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_PID)
#endif
#ifdef CDK_CONFIG_SHELL_INCLUDE_DEBUG
CONFIG_OPTION(CDK_CONFIG_SHELL_INCLUDE_DEBUG)
#endif
#ifdef CDK_CONFIG_DEFINE_UINT8_T
CONFIG_OPTION(CDK_CONFIG_DEFINE_UINT8_T)
#endif
#ifdef CDK_CONFIG_DEFINE_UINT16_T
CONFIG_OPTION(CDK_CONFIG_DEFINE_UINT16_T)
#endif
#ifdef CDK_CONFIG_DEFINE_UINT32_T
CONFIG_OPTION(CDK_CONFIG_DEFINE_UINT32_T)
#endif
#ifdef CDK_CONFIG_DEFINE_SIZE_T
CONFIG_OPTION(CDK_CONFIG_DEFINE_SIZE_T)
#endif
#ifdef CDK_CONFIG_DEFINE_DMA_ADDR_T
CONFIG_OPTION(CDK_CONFIG_DEFINE_DMA_ADDR_T)
#endif
#ifdef CDK_CONFIG_DEFINE_PRIu32
CONFIG_OPTION(CDK_CONFIG_DEFINE_PRIu32)
#endif
#ifdef CDK_CONFIG_DEFINE_PRIx32
CONFIG_OPTION(CDK_CONFIG_DEFINE_PRIx32)
#endif
#endif /* CONFIG_OPTION */
#include "cdk_config_chips.h"

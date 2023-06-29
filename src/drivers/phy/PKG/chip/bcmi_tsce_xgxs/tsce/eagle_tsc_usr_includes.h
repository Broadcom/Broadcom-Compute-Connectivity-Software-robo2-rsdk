/**************************************************************************************
 *  File Name     :  eagle_tsc_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :  $Id: eagle_tsc_usr_includes.h 898 2015-02-04 23:22:17Z eroes $ *
 *                                                                                    *
 *  $Copyright: (c) 2017 Broadcom Corporation                                         *
 *  All Rights Reserved$                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 *                                                                                    *
 **************************************************************************************
 **************************************************************************************/

/** @file eagle_tsc_usr_includes.h
 * Header file which includes all required std libraries and macros 
 */

/* The user is expected to replace the macro definitions with their required implementation */

#ifndef EAGLE_TSC_API_USR_INCLUDES_H
#define EAGLE_TSC_API_USR_INCLUDES_H

/* Standard libraries that can be replaced by your custom libraries */
#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif


#ifdef NON_SDK
#include <stdint.h>
#include <string.h>
#else

#ifndef __KERNEL__
#ifndef VXWORKS

#ifndef  int8_t
typedef signed char int8_t;
#endif
#ifndef  int16_t
typedef short int int16_t;
#endif
#ifndef  int32_t
typedef int int32_t;
#endif

#endif
#endif

#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

/* Redefine macros according your compiler requirements */
#ifdef NON_SDK
#define USR_PRINTF(stuff)             usr_logger_write stuff
#define USR_MEMSET(mem, val, num)     memset(mem, val, num)
#define USR_STRLEN(string)            strlen(string)
#define USR_STRCAT(str1, str2)        strcat(str1, str2)
#define USR_STRCPY(str1, str2)        strcpy(str1, str2)
#define USR_STRCMP(str1, str2)        strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num)  strncmp(str1, str2, num)
#define USR_STRNCAT(str1, str2, num)  strncat(str1, str2, num)

#ifdef SERDES_API_FLOATING_POINT 
#define USR_DOUBLE                    double 
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

/* Implementation specific macros below */
#define usr_logger_write(...) logger_write(-1,__VA_ARGS__)

#else
#define USR_PRINTF(stuff)             PHYMOD_DEBUG_ERROR(stuff)
#define USR_MEMSET(mem, val, num)     PHYMOD_MEMSET(mem, val, num)
#define USR_STRLEN(string)            PHYMOD_STRLEN(string)
#define USR_STRCAT(str1, str2)        PHYMOD_STRCAT(str1, str2)
#define USR_STRCPY(str1, str2)        PHYMOD_STRCPY(str1, str2)
#define USR_STRCMP(str1, str2)        PHYMOD_STRCMP(str1, str2)
#define USR_STRNCMP(str1, str2, num)  PHYMOD_STRNCMP(str1, str2, num)
#define USR_STRNCAT(str1, str2, num)  PHYMOD_STRNCAT(str1, str2, num)

#ifdef SERDES_API_FLOATING_POINT 
#define USR_DOUBLE                    double 
#else
#define USR_DOUBLE       int
#endif

#ifndef UINT16_MAX
#define UINT16_MAX                    0xFFFFU
#endif
#ifndef UINT32_MAX
#define UINT32_MAX                    0xFFFFFFFFU
#endif
#endif


#endif

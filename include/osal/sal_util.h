/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File: sal_endian.h
 * Purpose: Endianess operations
 */

#ifndef _SAL_UTIL_H
#define _SAL_UTIL_H

#include <stdlib.h>
#include <sys/types.h> 

extern unsigned short _sal_swap16(unsigned short val);
extern unsigned int _sal_swap32(unsigned int val);

#ifdef CONFIG_BE_MODE
#define BE_HOST /* BE_HOST */
#else
#define LE_HOST /* LE_HOST */
#endif /* CONFIG_BE_MODE */

#define sal_htonl(_l)   _sal_swap32(_l)
#define sal_htons(_s)   _sal_swap16(_s)
#define sal_ntohl(_l)   _sal_swap32(_l)
#define sal_ntohs(_s)   _sal_swap16(_s)

#if 0 /* BE_HOST */
#define sal_htonl(_l)   (_l)
#define sal_htons(_s)   (_s)
#define sal_ntohl(_l)   (_l)
#define sal_ntohs(_s)   (_s)

#endif /* BE_HOST */


#endif /* !_SAL_UTIL_H */

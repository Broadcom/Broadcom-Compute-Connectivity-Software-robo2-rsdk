/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_util.c
 * * Purpose:
 * *     Robo2 openrtos osal module. This provides abstraction for openrtos
 * *     endianess operations service.
 * *
 * */
#include "sal_util.h"

/*
 * Function:
 *    sal_swap16
 * Purpose:
 *    Convert unsigned short integer from host to network byte order and vice versa.
 * Parameters:
 *    val - value to be converted into another byte order
 * Returns:
 *    return value in converted byte order
 */
unsigned short
_sal_swap16(unsigned short val)
{
    return val << 8 | val >> 8;
}

/*
 * Function:
 *    sal_swap32
 * Purpose:
 *    Convert unsigned integer from host to network byte order and vice versa.
 * Parameters:
 *    val - value to be converted into another byte order
 * Returns:
 *    return value in converted byte order
 */
unsigned int
_sal_swap32(unsigned int val)
{
    val = (val << 16) | (val >> 16);

    return (val & 0xff00ffff) >> 8 | (val & 0xffff00ff) << 8;
}
